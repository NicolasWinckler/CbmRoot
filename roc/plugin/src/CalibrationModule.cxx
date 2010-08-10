/********************************************************************
 * The Data Acquisition Backbone Core (DABC)
 ********************************************************************
 * Copyright (C) 2009-
 * GSI Helmholtzzentrum fuer Schwerionenforschung GmbH
 * Planckstr. 1
 * 64291 Darmstadt
 * Germany
 * Contact:  http://dabc.gsi.de
 ********************************************************************
 * This software can be used under the GPL license agreements as stated
 * in LICENSE.txt file which is part of the distribution.
 ********************************************************************/
#include "roc/CalibrationModule.h"

#include <math.h>

#include "roc/Board.h"

#include "dabc/logging.h"
#include "dabc/PoolHandle.h"
#include "dabc/MemoryPool.h"
#include "dabc/Command.h"
#include "dabc/Port.h"
#include "dabc/Pointer.h"
#include "dabc/Manager.h"
#include "dabc/Application.h"

#include "roc/CombinerModule.h"

#define FullTimeStampRange 0x400000000000ULL

roc::CalibrationModule::CalibRec::CalibRec(unsigned id) :
   rocid(id),
   calibr_k(1.), calibr_b_e(0), calibr_b_f(0.), weight(0), sorter(0),
   evnt1_tm(0), evnt2_tm(0), evnt1_num(0), evnt2_num(0), evnt_len(0),
   src_data(0), src_length(0), src_format(formatNormal),
   data(0), numdata(0),
   front_epoch(0), front_shift(0.),
   last_epoch(0), is_last_epoch(false), last_stamp(0),
   stamp(0), stamp_copy(false)
{
}

void roc::CalibrationModule::CalibRec::RecalculateCalibr(double k, double b)
{
   double new_k(1.), new_b(0.);

   if ((weight==0) || !is_last_epoch) {
      new_k = k;
      new_b = b;
      weight = 1;

   } else {
      new_k = (k + calibr_k*weight) / (weight + 1.);

      while (b - calibr_b_e > 0x80000000) b -= 0x100000000LLU;
      while (calibr_b_e - b > 0x80000000) b += 0x100000000LLU;
      new_b = (b + (calibr_b_e + calibr_b_f) * weight) / (weight + 1.);

      if (weight<200) weight++;
   }

   calibr_k = new_k;

   while (new_b < 0.) new_b += 0x100000000LLU;
   while (new_b >= 0x100000000LLU) new_b -= 0x100000000LLU;

   calibr_b_e = uint32_t(new_b);
   calibr_b_f = new_b - calibr_b_e;
}



double roc::CalibrationModule::CalibRec::CalibrEpoch(uint32_t& epoch)
{
//   double calibr_epoch = epoch * calibr_k + calibr_b_e + calibr_b_f;

  double fraction = epoch * (calibr_k-1.) + calibr_b_f;

  epoch += calibr_b_e;

  double fl = floor(fraction);
  if (fl >= 1.) {
    fraction -= fl;
    epoch += uint32_t(fl);
  } else
  if (fl < 0) {
    fraction -= fl;
    epoch -= uint32_t(-fl);
  }

  return fraction;
}

/** For test purposes - make full recalculation of timestamp */
double roc::CalibrationModule::CalibRec::RecalculateTs(uint64_t tm)
{
   double res = (uint64_t) (calibr_k* tm + (calibr_b_e + calibr_b_f)*16384.);

   while (res >= FullTimeStampRange) res -= FullTimeStampRange;
   while (res < 0.) res += FullTimeStampRange;

   return res;
}


void roc::CalibrationModule::CalibRec::TimeStampOverflow()
{
// calibr_b += FullTimeStampRange * calibr_k;

   DOUT1(("Roc:%u Take into account overflow of epoch", rocid));

   double ep_shift = (calibr_k - 1.);
   if (ep_shift<0.) ep_shift+=1.;
   ep_shift*=0x100000000LL;

   calibr_b_e += uint32_t(floor(ep_shift));

   calibr_b_f += (ep_shift - floor(ep_shift));

   while (calibr_b_f >= 1.) {
      calibr_b_e++;
      calibr_b_f-=1.;
   }
}

roc::CalibrationModule::CalibrationModule(const char* name, dabc::Command* cmd) :
   dabc::ModuleAsync(name, cmd),
   fPool(0),
   fInp(),
   fOutBuf(0),
   f_outptr()
{
   fNumRocs = GetCfgInt(roc::xmlNumRocs, 2, cmd);
   fBufferSize = GetCfgInt(dabc::xmlBufferSize, 16384, cmd);
   int numinputs = GetCfgInt(dabc::xmlNumInputs, 1, cmd);
   int numoutputs = GetCfgInt(dabc::xmlNumOutputs, 2, cmd);

   DOUT1(("new roc::CalibrationModule %s buff %d", GetName(), fBufferSize));
   fPool = CreatePoolHandle(roc::xmlRocPool, fBufferSize, 1);

   dabc::RateParameter* rate = CreateRateParameter("CalibrRate", false, 3., "", "", "", 0., 10., true);

   for (int n=0; n<numinputs; n++) {
      CreateInput(FORMAT(("Input%d",n)), fPool, 10);
      Input(n)->SetInpRateMeter(rate);
      fInp.push_back(InputRec());
   }

   for(int n=0; n<numoutputs; n++)
      CreateOutput(FORMAT(("Output%d", n)), fPool, 10);

   fBuildEvents = 0;
   fSkippedEvents = 0;
   fErrorEvents = 0;

   fFrontOutEpoch = 0;
   fLastOutEpoch = 0;
   fIsOutEpoch = false;

   fOutFormat = formatOptic2;
   fOutRocNumber = 0;

   dabc::lgr()->SetErrorMask(dabc::Logger::lMessage);

}

roc::CalibrationModule::~CalibrationModule()
{
   for (unsigned n=0;n<fCalibr.size();n++) {
      delete fCalibr[n].sorter;
      fCalibr[n].sorter = 0;
   }

   dabc::Buffer::Release(fOutBuf); fOutBuf = 0;
}

void roc::CalibrationModule::ProcessInputEvent(dabc::Port* inport)
{
   DoCalibration();
}

void roc::CalibrationModule::ProcessOutputEvent(dabc::Port* inport)
{
   DoCalibration();
}

void roc::CalibrationModule::PrintEvent(void* evnt_p)
{
   mbs::EventHeader* evnt = (mbs::EventHeader*) evnt_p;
   if (evnt==0) return;

   DOUT1(("Event:%u NumSubevents:%u", evnt->EventNumber(), evnt->NumSubevents()));

   mbs::SubeventHeader* subevhdr = 0;

   while ((subevhdr = evnt->NextSubEvent(subevhdr)) != 0) {

      unsigned rocid = (unsigned) subevhdr->iSubcrate;

      roc::Iterator iter((MessageFormat)subevhdr->iControl);
      iter.setRocNumber(rocid);
      iter.assign(subevhdr->RawData(), subevhdr->RawDataSize());

      iter.next();
      if (!iter.msg().isEpochMsg()) {
         EOUT(("Should be epoch in the beginning, get %u", iter.msg().getMessageType()));
         continue;
      }
      uint32_t epoch1 = iter.msg().getEpochNumber();

      iter.next();
      if (!iter.msg().isSyncMsg()) {
         EOUT(("Should be sync event as second event"));
         continue;
      }
      uint32_t evnt1_num = iter.msg().getSyncData();

      iter.last();
      if (!iter.msg().isSyncMsg()) {
         EOUT(("Should be sync event in the end"));
         continue;
      }
      uint32_t evnt2_num = iter.msg().getSyncData();

      uint32_t epoch2(epoch1);
      while (iter.prev()) {
         if (iter.msg().isEpochMsg()) {
            epoch2 = iter.msg().getEpochNumber();
            break;
         }
      };

      DOUT1(("   Roc:%u Epoch1:%08x Event1:%06x Epoch2:%08x Event2:%06x Shift:%02x  %s",
            rocid, epoch1, evnt1_num, epoch2, evnt2_num, evnt2_num-evnt1_num,
            ((subevhdr->iProcId == roc::proc_ErrEvent) ? "Error" : "Ok")));
   }
}


#define NullStamp 0xffffffff
#define EndStamp  0xfffffffe

bool roc::CalibrationModule::DoCalibration()
{
   bool doagain = true;

/*
   if (fBuildSize > 22000000) {

      DOUT0(("Build: %ld  Skip: %ld events %5.1f",
            fBuildEvents, fSkippedEvents,
           (fBuildEvents > 0 ? 100. * fSkippedEvents / fBuildEvents : 0.)));

      dabc::mgr()->RaiseCtrlCSignal();

      return false;
   }
*/
//   uint64_t fLastOutTm = 0;

   while (doagain) {

      // first check that we can send data to output
      if (!CanSendToAllOutputs()) return false;

      // second check if we have something in all used inputs
      unsigned ninp(0), nmin(0), nmax(0);
      unsigned num_in_use = 0;

      while (ninp<fInp.size()) {
         InputRec* inp = &(fInp[ninp]);

         // do not consider here unused (closed) inputs
         if (!inp->inuse) { ninp++; continue; }

         if (!Input(ninp)->CanRecv()) return false;

         // skip input buffer if it has no data
         if ((inp->ptr.buf()!=0) && (inp->ptr.fullsize() <= sizeof(mbs::EventHeader))) {
            inp->ptr.reset();
            Input(ninp)->SkipInputBuffers(1);
            continue; // analyse this record again while we do not see any data in the input
         }

         // assign pointer on the beginning of the first buffer
         if (inp->ptr.fullsize()==0) {
            dabc::Buffer* buf = Input(ninp)->InputBuffer(0);
            if (buf->GetTypeId() == dabc::mbt_EOF) {
               Input(ninp)->SkipInputBuffers(1);
               inp->inuse = false;
               ninp++;
               continue;
            }
            inp->ptr.reset(buf);
         }

         inp->evhdr = (mbs::EventHeader*) inp->ptr();

         if (inp->ptr.fullsize() < inp->evhdr->FullSize()) {
            EOUT(("Mismatch in beginning !!!!!!! rest: %u  evnt: %u  buf:%u",
               inp->ptr.fullsize(), inp->evhdr->FullSize(), Input(ninp)->InputBuffer(0)->GetDataSize()));
            inp->ptr.reset();
            Input(ninp)->SkipInputBuffers(1);
            fSkippedEvents++;
            // repeat loop for this input while we have no data
            continue;
         }

         // immediately found inputs with minimum and maximum events
         if (num_in_use==0) {
            nmin = ninp;
            nmax = ninp;
         } else {
            if (inp->evhdr->EventNumber() < fInp[nmin].evhdr->EventNumber()) nmin = ninp; else
            if (inp->evhdr->EventNumber() > fInp[nmax].evhdr->EventNumber()) nmax = ninp;
         }

//         DOUT0(("Input %u event 0x%06x", ninp, inp->evhdr->EventNumber()));

         ninp++;
         num_in_use++;
      }

      if (num_in_use==0) {
         // if we send to output, check again that output not blocked
         if (FlushOutputBuffer()) continue;

         DOUT0(("Stop CALIBRATION Build: %ld  Skip: %ld Error: %ld",
             fBuildEvents, fSkippedEvents, fErrorEvents));

         for(unsigned n=0;n<NumOutputs();n++) {
            dabc::Buffer* eof_buf = fPool->TakeEmptyBuffer();
            eof_buf->SetTypeId(dabc::mbt_EOF);
            Output(n)->Send(eof_buf);
         }

         // stop module execution - application will react later on
         Stop();

         dabc::mgr()->GetApp()->InvokeCheckModulesCmd();

         return false;
      }

      if (fOutBuf==0) {
         fOutBuf = fPool->TakeBuffer(fBufferSize);
         if (fOutBuf==0) return false;
         fOutBuf->SetDataSize(fBufferSize);
         f_outptr.reset(fOutBuf);
         fIsOutEpoch = false; // always put epoch in begin of output buffer
      }


      // here we select minimum event
      // we should take into account that due to event number overflow minimum
      // in reality can means maximum

      if (num_in_use>1)
         if ((fInp[nmax].evhdr->EventNumber() - fInp[nmin].evhdr->EventNumber()) >
            (fInp[nmin].evhdr->EventNumber() + 0x1000000 - fInp[nmax].evhdr->EventNumber())) {

            DOUT0(("Overflow detected min = %d max = %d",
                  fInp[nmin].evhdr->EventNumber(), fInp[nmax].evhdr->EventNumber()));

            nmin = nmax;
         }


      mbs::EventHeader* inpevhdr = fInp[nmin].evhdr;


//      if (fSkippedEvents + fBuildEvents > 5500) exit(1);

//      DOUT1(("CALIBR: Analyze event %u of full size %u", inpevhdr->EventNumber(), inpevhdr->FullSize()));

      if (inpevhdr->FullSize() > f_outptr.fullsize()) {
         if (FlushOutputBuffer())
            continue;
         else {
            EOUT(("Event in input buffer too big %u, skip it", inpevhdr->FullSize()));
            Input(nmin)->SkipInputBuffers(1);
            fInp[nmin].ptr.reset();
            fSkippedEvents++;
            continue;
         }
      }
      // check input event - if it is ok

      // PrintEvent(inpevhdr);

      mbs::SubeventHeader* subevhdr = 0;
      bool iserrdata(false);
      unsigned subevntcnt = 0;
      CalibRec* rec0 = 0;

      while ((subevhdr = inpevhdr->NextSubEvent(subevhdr)) != 0) {
         subevntcnt++;
         if (subevhdr->iProcId == roc::proc_ErrEvent) {
            fErrorEvents++;
            iserrdata = true;
            break;
         }
         if (subevhdr->RawDataSize() < 3*roc::Message::RawSize((MessageFormat)subevhdr->iControl)) {
            EOUT(("To small raw data size %u in event, skip", subevhdr->RawDataSize()));
            iserrdata = true;
            break;
         }
      }
      if (subevntcnt != fNumRocs) iserrdata = true;

      subevhdr = 0;
      subevntcnt = 0;
      if (!iserrdata)
      while ((subevhdr = inpevhdr->NextSubEvent(subevhdr)) != 0) {

         subevntcnt++;

         if (subevntcnt > fCalibr.size()) {
            DOUT0(("Add subcrate %u format %u", subevhdr->iSubcrate, subevhdr->iControl));

            if (fCalibr.size()==0) {
               // fOutFormat = (MessageFormat) subevhdr->iControl;
               fOutRocNumber = subevhdr->iSubcrate;
            }

            fCalibr.push_back(CalibRec(subevhdr->iSubcrate));
            unsigned msize = sizeof(roc::Message);
            fCalibr[subevntcnt-1].sorter = new roc::Sorter(fBufferSize / msize * 2, fBufferSize / 6/*msize*/ * 2 , roc::formatNormal, fBufferSize / msize);
         }

         rec0 = &(fCalibr[0]);
         CalibRec *rec = &(fCalibr[subevntcnt-1]);
         if (rec->rocid != (unsigned) subevhdr->iSubcrate) {
            EOUT(("Mismatch in ROCs ids cnt:%u %u %u",  subevntcnt, rec->rocid, subevhdr->iSubcrate));
            iserrdata = true;
            continue;
         }

         // from this point we can switch to next subevent in case of error
         if (iserrdata) continue;

         roc::Iterator iter((MessageFormat) subevhdr->iControl);
         iter.assign(subevhdr->RawData(), subevhdr->RawDataSize());

         iter.next();
         if (!iter.msg().isEpochMsg()) {
            EOUT(("Should be epoch in the beginning, get %u", iter.msg().getMessageType()));
            iserrdata = true;
            continue;
         }

         uint32_t epoch1 = iter.msg().getEpochNumber();
         iter.next();

         if (!iter.msg().isSyncMsg()) {
            EOUT(("Should be sync event as second message"));
            iserrdata = true;
            continue;
         }

         rec->evnt1_tm = iter.msg().getMsgFullTime(epoch1);
         rec->evnt1_num = iter.msg().getSyncData();

         if (!rec->is_last_epoch) {
            rec->is_last_epoch = true;
            rec->last_epoch = epoch1 - 8;
            if (rec->last_epoch > epoch1) {
               EOUT(("Roc%u Overflows by first last_epoch assignment, set to 0", rec->rocid));
               rec->last_epoch = 0;
            }
         }

         iter.last();
         if (!iter.msg().isSyncMsg()) {
            EOUT(("Should be sync event in the end"));
            iserrdata = true;
            continue;
         }
         roc::Message last_sync = iter.msg();

//         DOUT0(("First 0x%06x Last 0x%06x", rec->evnt1_num, last_sync.getSyncData()));

         iter.prev();
         bool is_extra_last_epoch = iter.msg().isEpochMsg();

         uint32_t epoch2(epoch1);

         do {
            if (iter.msg().isEpochMsg()) {
               epoch2 = iter.msg().getEpochNumber();
               break;
            }
         } while (iter.prev());

         rec->evnt2_tm = last_sync.getMsgFullTime(epoch2);
         rec->evnt2_num = last_sync.getSyncData();

//         EOUT(("   Roc:%u Epoch1:%08x Event1:%06x Epoch2:%08x Event2:%06x Shift:%02x  K:%9.7f",
//               rec->rocid, epoch1, rec->evnt1_num, epoch2, rec->evnt2_num, rec->evnt2_num-rec->evnt1_num, rec->calibr_k));

         if (epoch2 - rec->last_epoch > 0x20000) {
            EOUT(("Roc:%u Too far epoch jump last:%08x evnt1:%08x evnt2:%08x",
                   rec->rocid, rec->last_epoch, epoch1, epoch2));
            iserrdata = true;
            continue;
         }

         if (epoch2==epoch1) {
//            EOUT(("Roc:%u Sync markers cannot be with the same epoch number evnt1:%08x evnt2:%08x",
//                   rec->rocid, epoch1, epoch2));
            iserrdata = true;
            continue;
         }

//         EOUT(("    TM1:%llu  TM2:%llu", rec->evnt1_tm, rec->evnt2_tm));

         rec->evnt_len = roc::Message::CalcDistance(rec->evnt1_tm, rec->evnt2_tm);

         if (rec->evnt_len == 0) {
//            EOUT(("Roc:%u Zero event length!!!", rec->rocid));
            iserrdata = true;
            continue;
         }

/*
         if (rec->rocid==0)
            if ((rec->evnt1_num != (epoch1 & 0xffffff)) ||
                (rec->evnt2_num != (epoch2 & 0xffffff))) {
//                    EOUT(("Sync number error for master roc %06x != %08x || %06x != %08x",
//                        rec->evnt1_num, epoch1 & 0xffffff, rec->evnt2_num, epoch2 & 0xffffff));
                    iserrdata = true;
                    continue;
                }
*/

//         DOUT1(("Roc:%u %s Evnt1:%06x ep1 %08x Evnt2:%06x ep2 %08x len %06x",
//                 rec->rocid, DBOOL(rec!=rec0),
//                 rec->evnt1_num, epoch1, rec->evnt2_num, epoch2, rec->evnt_len));

         if (rec!=rec0) {

            if ((rec->evnt1_num != rec0->evnt1_num) || (rec->evnt2_num != rec0->evnt2_num)) {
               EOUT(("Events numbers mismatch !!!"));
               iserrdata = true;
               continue;
            }

            double k = 1. / rec->evnt_len * rec0->evnt_len;

//            EOUT(("Evnt:%llu Evnt0:%llu k:%10.8f", rec->evnt_len, rec0->evnt_len, k));

            if ((k<0.99) || (k>1.01)) {
               EOUT(("Coefficient K:%5.3f too far from 1.0, error", k));
               iserrdata = true;
               continue;
            }

            // b calculated in epoch scale
            double b = rec0->evnt1_tm / 16384. - rec->evnt1_tm / 16384. * k;

            rec->RecalculateCalibr(k, b);

//            DOUT0(("k = %10.8f len0 = %llu rec->k %10.8f rec->b_e %08x rec->b_f = %10.8f diff1 = %5.1f diff2 = %5.1f",
//                  k, rec0->evnt_len, rec->calibr_k, rec->calibr_b_e, rec->calibr_b_f,
//                  rec->RecalculateTs(rec->evnt1_tm) - rec0->evnt1_tm, rec->RecalculateTs(rec->evnt2_tm) - rec0->evnt2_tm ));
         }

         rec->src_format = iter.getFormat();
         rec->src_data = subevhdr->RawData();
         rec->src_length = subevhdr->RawDataSize() - iter.getMsgSize();
         if (is_extra_last_epoch) rec->src_length -= iter.getMsgSize();
      }

      // now try to adjust front output epoch on all channels
      if (!iserrdata) {

         uint32_t min_cal_epoch = 0xFFFFFFFF;
//         0x100000000LL;

         // such method to define minimum epoch is not safe, while
         // in case of overflow epoch=1 in time not before epoch=0xffffffff,
         // but rather wise-versa. The only chance to avoid this
         // subtract some constant (here 8) from mathematical minimum and
         // believe that calibrated epochs not much far from one other as this constant, divided by two

         for(unsigned n=0;n<fCalibr.size();n++) {
            CalibRec *rec = &(fCalibr[n]);

            rec->front_epoch = rec->last_epoch;

            uint32_t cal_epoch = rec->front_epoch;

            // here one not interesting in fraction
            rec->CalibrEpoch(cal_epoch);

//            DOUT1(("Roc%u front epoch %08x calepoch %08x k = %7.5f  b_e = %08x  b_f = %5.2f",
//                    rec->rocid, rec->front_epoch, cal_epoch, rec->calibr_k, rec->calibr_b_e, rec->calibr_b_f));

            if (cal_epoch < min_cal_epoch) min_cal_epoch = cal_epoch;
         }

         fFrontOutEpoch = min_cal_epoch - 8; // -8 to protect us again floating round problem

//         DOUT1(("Let's try front epoch as %x", fFrontOutEpoch));

         // now calculate front_shift values, which are correspond to selected front epochs

         for(unsigned n=0;n<fCalibr.size();n++) {
            CalibRec *rec = &(fCalibr[n]);

            uint32_t cal_epoch = rec->front_epoch;

            double fraction = rec->CalibrEpoch(cal_epoch);

            if (cal_epoch - fFrontOutEpoch < 8 / 2) {
               EOUT(("Rounding problem !!!!!!!!!!!!!!!!!!"));
               exit(1);
            }

//            DOUT1(("For roc:%u diff to front is %u %5.3f", rec->rocid, cal_epoch - fFrontOutEpoch, fraction));

            if (cal_epoch - fFrontOutEpoch > 0x20000) {
               EOUT(("Roc:%u Distances between epochs cal:%08X out:%08X tooooooo far = %08X",
                  rec->rocid, cal_epoch, fFrontOutEpoch, cal_epoch - fFrontOutEpoch));
               iserrdata = true;
               break;
            }

//            calibr_fr_epoch -= fFrontOutEpoch;
//            while (calibr_fr_epoch > 0x100000000LLU) calibr_fr_epoch -= 0x100000000LLU;
//            rec->front_shift = calibr_fr_epoch  * 0x4000;

            cal_epoch -= fFrontOutEpoch;

            rec->front_shift = (cal_epoch + fraction) * 16384.;

            if ((rec->front_shift < 0.) || (rec->front_shift > 0x80000000)) {
               EOUT(("Something completely wrong with front_shift = %9.1f!!!", rec->front_shift));
               iserrdata = true;
               break;
            }
         }
      }

      if (!iserrdata)
         for(unsigned n=0;n<fCalibr.size();n++) {

            CalibRec *rec = &(fCalibr[n]);

//            DOUT0(("Perform sorting Roc:%u length:%u Format:%u", rec->rocid, rec->src_length, rec->src_format));

//            EOUT(("ADD Rec:%u length:%u", n, rec->src_length/6 ));

            if (!rec->sorter->addData(rec->src_data, rec->src_length, rec->src_format)) {
                EOUT(("Cannot sort out data on roc %u size:%u", n, rec->numdata));
                iserrdata = true;
                break;
            }

            /** uncomment next line to get all messages from sorter, only for debug purposes */
            // rec->sorter->flush();

            rec->data = (roc::Message*) rec->sorter->filledBuf();
            rec->numdata = rec->sorter->sizeFilled();

//            DOUT0(("After sorting Roc:%u has %u data last:%08x", rec->rocid, rec->numdata, rec->last_epoch));

            rec->stamp = NullStamp;
         }

      if (iserrdata) {
         fSkippedEvents++;

//         EOUT(("CALIBR: Skip event id = %u sz = %u from input iserr %s numrecs %u",
//            inpevhdr->EventNumber(), inpevhdr->FullSize(), DBOOL(iserrdata), recs.size()));

         if (fInp[nmin].ptr.fullsize() < inpevhdr->FullSize()) {
            EOUT(("Corrupted MBS data rest: %u  evnt: %u", fInp[nmin].ptr.fullsize(), inpevhdr->FullSize()));
            fInp[nmin].ptr.reset();
         } else
            fInp[nmin].ptr.shift(inpevhdr->FullSize());

         // in any case, remove all previous messages - bad lack for them
         for(unsigned n=0;n<fCalibr.size();n++) {
            fCalibr[n].sorter->cleanBuffers();
            fCalibr[n].is_last_epoch = false;
         }

         continue;
      }

//      DOUT0(("Combine sync event %06x", fCalibr[0].evnt1_num));

//      fLastOutTm = 0;

      dabc::Pointer out_begin(f_outptr);

      mbs::EventHeader* evhdr = (mbs::EventHeader*) f_outptr();
      evhdr->Init(inpevhdr->EventNumber());
      f_outptr.shift(sizeof(mbs::EventHeader));

      mbs::SubeventHeader* subhdr = (mbs::SubeventHeader*) f_outptr();
      subhdr->Init();
      subhdr->iProcId = roc::proc_MergedEvent;
      subhdr->iSubcrate = fOutRocNumber;
      subhdr->iControl = fOutFormat;
      f_outptr.shift(sizeof(mbs::SubeventHeader));

      dabc::Pointer data_begin(f_outptr);

      CalibRec *minrec(0);

      bool far_epoch_jump = false;

      // here one should fill

      do {

         minrec = 0;

         bool is_end_of_data = false;

         for (unsigned n = 0; n<fCalibr.size(); n++) {
            CalibRec *rec = &(fCalibr[n]);

            while (rec->stamp==NullStamp) {

               if (rec->numdata==0) {
                  rec->stamp = EndStamp;
                  is_end_of_data = true;
                  break;
               }

//               DOUT1(("Get msg %u in rocid %u", data->getMessageType(), rec->rocid));

               if (rec->data->isHitMsg()) {
                  rec->last_stamp = rec->data->getNxTs();
                  rec->stamp = uint32_t(rec->data->getNxTs() * rec->calibr_k + rec->front_shift);
                  rec->stamp_copy = true;
//                  DOUT1(("Roc:%u Hit stamp:%x", rec->rocid, rec->stamp));
               } else
               if (rec->data->isAuxMsg()) {
                  rec->last_stamp = rec->data->getAuxTs();
                  rec->stamp = uint32_t(rec->data->getAuxTs() * rec->calibr_k + rec->front_shift) & 0xfffffffe;
                  rec->stamp_copy = true;
//                  DOUT1(("Roc:%u Aux stamp:%x", rec->rocid, rec->stamp));
               } else
               if (rec->data->isSyncMsg()) {
                  rec->last_stamp = rec->data->getSyncTs();
                  rec->stamp = uint32_t(rec->data->getSyncTs() * rec->calibr_k + rec->front_shift) & 0xfffffffe;
                  rec->stamp_copy = false;
//                  DOUT1(("Roc:%u Sync stamp:%x", rec->rocid, rec->stamp));
               } else
               if (rec->data->isEpochMsg()) {

                  uint32_t epoch = rec->data->getEpochNumber();

                  if (rec->is_last_epoch && (epoch < rec->last_epoch))
                     rec->TimeStampOverflow();

                  unsigned diff = epoch - rec->last_epoch;
                  if (!rec->is_last_epoch) diff = epoch - rec->front_epoch;

                  rec->last_epoch = epoch;
                  rec->last_stamp = 0;
                  rec->is_last_epoch = true;

                  rec->front_shift += diff * rec->calibr_k * 0x4000;

                  if (rec->last_epoch - rec->front_epoch > 0x20000) {
                     EOUT(("Roc:%u Epochs front:%x epoch:%x diff:%x - should not happen !!!",
                         rec->rocid, rec->front_epoch, rec->last_epoch, rec->last_epoch - rec->front_epoch));
                     far_epoch_jump = true;
                     rec->stamp = EndStamp;
                     iserrdata = true;
                  } else
                     rec->stamp = uint32_t(rec->front_shift);

                  rec->stamp_copy = false;
               } else {
                  // skip all other messages
                  rec->data++;
                  rec->numdata--;
               }
            }

            // no any data can be taken from that stream
            if (rec->stamp==EndStamp) continue;

            if ((minrec==0) || (rec->stamp < minrec->stamp)) minrec = rec;
         }

         if (is_end_of_data) minrec = 0;

         if (minrec != 0) {

//            DOUT1(("Minimum rec has ROC:%u stamp:%x DoCopy:%d", minrec->rocid, minrec->stamp, minrec->stamp_copy));
/*
            if (minrec->stamp_copy) {
               uint64_t fulltm = (uint64_t(fFrontOutEpoch) << 14) | minrec->stamp;

               DOUT1(("Now:%llu  Prev:%llu Diff to prev:%llu %llu",  fulltm, fLastOutTm, fulltm - fLastOutTm, fLastOutTm - fulltm));

               if (fLastOutTm!=0)
                  if (fLastOutTm > fulltm) {
                     if (roc::Message::CalcDistance(fLastOutTm, fulltm) > 16 * 0x4000) minrec->stamp_copy = false;

                     EOUT(("Missmatch in time order iscorrect = %s!!!", DBOOL(minrec->stamp_copy)));
                  }

               if (minrec->stamp_copy)
                  fLastOutTm = fulltm;
            }
*/

            if (minrec->stamp_copy) {

//               DOUT1(("Roc:%u Add hit with stamp %x to output", minrec->rocid, minrec->stamp));

               uint32_t next_epoch = fFrontOutEpoch + (minrec->stamp >> 14);

               if (!fIsOutEpoch || (fLastOutEpoch != next_epoch)) {

                  if (next_epoch < fLastOutEpoch) DOUT1(("Epoch missmatch prev:%x next:%x", fLastOutEpoch, next_epoch));

                  // at that place we need at least 12 bytes - for epoch and message
                  if (f_outptr.fullsize() < 12) {
//                     EOUT(("Not enougth place in output buffer for epoch - AAAAA"));
//                     iserrdata = true;
                     is_end_of_data = true;
                     minrec = 0;
                     break;
                  }

                  roc::Message msg;

                  msg.setMessageType(roc::MSG_EPOCH);
                  msg.setRocNumber(fOutRocNumber);
                  msg.setEpochNumber(next_epoch);
                  msg.setEpochMissed(0);

//                  msg.printData(7);

                  msg.copyto(f_outptr(), fOutFormat);
                  f_outptr.shift(roc::Message::RawSize(fOutFormat));
//                  if (fSkippedEvents + fBuildEvents > 5100)
//                     data->printData(3);
                  fLastOutEpoch = next_epoch;
                  fIsOutEpoch = true;
               }

               // if one have no enough space in buffer, just stop event filling
               if (f_outptr.fullsize() < roc::Message::RawSize(fOutFormat)) {
                  is_end_of_data = true;
                  minrec = 0;
//                  EOUT(("Not enougth place in output buffer - AAAAA"));
//                  iserrdata = true;
                  break;
               }

               roc::Message msg(*(minrec->data));
               msg.setRocNumber(minrec->rocid);
               if (msg.isHitMsg()) {
                  msg.setNxLastEpoch(0);
                  msg.setNxTs(minrec->stamp & 0x3fff);
               } else
               if (msg.isAuxMsg()) {
                  msg.setAuxTs(minrec->stamp & 0x3fff);
                  msg.setAuxEpochLSB(fLastOutEpoch & 0x1);
               } else
               if (msg.isSyncMsg()) {
                  msg.setSyncTs(minrec->stamp & 0x3fff);
                  msg.setSyncEpochLSB(fLastOutEpoch & 0x1);
               }

//               msg.printData(7);
//               static int cnt = 0;
//               if (cnt++> 1000000) exit(1);


//               msg.printData(std::cerr, 3);

               msg.copyto(f_outptr(), fOutFormat);
               f_outptr.shift(roc::Message::RawSize(fOutFormat));
            }

            minrec->data++;
            minrec->numdata--;
            minrec->stamp = NullStamp;
         }

      } while (minrec!=0);

      // skip processed data from sorter at the end
      for(unsigned n=0;n<fCalibr.size();n++) {
         CalibRec *rec = &(fCalibr[n]);
//         if (rec->sorter->sizeFilled() - rec->numdata)
//            DOUT1(("Roc:%u Shift %u data from sorter", rec->rocid, rec->sorter->sizeFilled() - rec->numdata));

         if (far_epoch_jump) {
            rec->sorter->cleanBuffers();
            rec->is_last_epoch = false;
            EOUT(("Remove everything from rec %u", rec->rocid));
         } else
            rec->sorter->shiftFilledData(rec->sorter->sizeFilled() - rec->numdata);
      }


      fBuildEvents++;

      subhdr->SetRawDataSize(data_begin.distance_to(f_outptr));
      evhdr->SetSubEventsSize(subhdr->FullSize());

//      EOUT(("Event %u Size %u", evhdr->EventNumber(), evhdr->SubEventsSize()));
//      if (evhdr->EventNumber()==13600 + 32000) exit(1);

      if (iserrdata) {
         EOUT(("Hard error - data was filled partially, therefore they will be discarded completely"));
         f_outptr.reset(out_begin);
      } else
         fBuildSize+=evhdr->FullSize();

      fInp[nmin].ptr.shift(inpevhdr->FullSize());

//      DOUT1(("Fill output event of total size %u", evhdr->FullSize()));
   }

   return true;
}


bool roc::CalibrationModule::FlushOutputBuffer()
{
   if (fOutBuf==0) return false;

   if (f_outptr.fullsize() == fOutBuf->GetDataSize()) return false;

   // send output buffer to all outputs
   fOutBuf->SetDataSize(fOutBuf->GetDataSize() - f_outptr.fullsize());
   fOutBuf->SetTypeId(mbs::mbt_MbsEvents);

   SendToAllOutputs(fOutBuf);
   fOutBuf = 0;
   f_outptr.reset();

   return true;
}
