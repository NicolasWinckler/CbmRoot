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

#include "roc/CombinerModule.h"

#include "roc/Board.h"
#include "roc/Iterator.h"
#include "roc/Commands.h"

#include "dabc/logging.h"
#include "dabc/PoolHandle.h"
#include "dabc/MemoryPool.h"
#include "dabc/Command.h"
#include "dabc/Port.h"
#include "dabc/Pointer.h"
#include "dabc/Manager.h"
#include "dabc/Application.h"

#include "mbs/LmdTypeDefs.h"
#include "mbs/MbsTypeDefs.h"

#include "bnet/common.h"

const char* roc::xmlBnetMode          = "BnetMode";
const char* roc::xmlSyncScaleDown     = "SyncScaleDown";
const char* roc::xmlSpillRoc          = "SpillRoc";
const char* roc::xmlSpillAux          = "SpillAux";
const char* roc::xmlCalibrationPeriod = "CalibrationPeriod";
const char* roc::xmlCalibrationLength = "CalibrationLength";
const char* roc::xmlThrottleAux       = "ThrottleAux";


bool InitIterator(roc::Iterator& iter, dabc::Buffer* buf, unsigned shift = 0, bool isudp = false, unsigned rocid = 0)
{
   if (buf==0) return false;

   if ((buf->GetTypeId() < roc::rbt_RawRocData) ||
       (buf->GetTypeId() > roc::rbt_RawRocData + roc::formatNormal)) return false;

   roc::MessageFormat fmt = (roc::MessageFormat) (buf->GetTypeId() - roc::rbt_RawRocData);

   iter.setFormat(fmt);
   if (isudp) iter.setRocNumber(rocid);

   void* ptr = buf->GetDataLocation();
   unsigned size = buf->GetDataSize();

   if (shift>=size) return false;

   iter.assign((uint8_t*) ptr + shift, size - shift);

   return true;
}


roc::CombinerModule::CombinerModule(const char* name, dabc::Command* cmd) :
   dabc::ModuleAsync(name, cmd),
   fInpPool(0),
   fOutPool(0),
   fBufferSize(0),
   fSimpleMode(false),
   fBnetMode(false),
   fOutBuf(0),
   f_outptr(),
   fSpillRoc(-1),
   fSpillAux(-1),
   fSpillState(false),
   fCalibrationPeriod(-1.),
   fCalibrationLength(1.),
   fSpillRate(0),
   fLastCalibrationTime(),
   fExtraMessages()
{
//   dabc::SetDebugLevel(5);

   fBnetMode = GetCfgBool(roc::xmlBnetMode, false, cmd);
   fSyncScaleDown = GetCfgInt(roc::xmlSyncScaleDown, -1, cmd);
   if ((fSyncScaleDown>=0) && (fSyncScaleDown<32))
      fSyncScaleDown = 1 << fSyncScaleDown;
   else {
      EOUT(("Sync scale down factor not specified. Use 1 as default"));
      fSyncScaleDown = -1;
   }

   int numinputs = GetCfgInt(dabc::xmlNumInputs, 1, cmd);
   fBufferSize = GetCfgInt(dabc::xmlBufferSize, 16384, cmd);
   int numoutputs = GetCfgInt(dabc::xmlNumOutputs, 2, cmd);
   fSpillRoc = GetCfgInt(roc::xmlSpillRoc, -1, cmd);
   fSpillAux = GetCfgInt(roc::xmlSpillAux, -1, cmd);
   fCalibrationPeriod = GetCfgDouble(roc::xmlCalibrationPeriod, -1., cmd);
   fCalibrationLength = GetCfgDouble(roc::xmlCalibrationLength, 0.5, cmd);

   DOUT1(("CombinerModule numinp = %d SyncScaleDown = %d BnetMode = %s", numinputs, fSyncScaleDown, DBOOL(fBnetMode)));


   
   fLastCalibrationTime = dabc::NullTimeStamp;

   fLastSpillTime = 0;

   CreateParInt("SpillState", -1);

   if ((fSpillRoc>=0) && (fSpillAux>=0)) {
      DOUT1(("CombinerModule spill detection ROC:%d aux:%d period:%4.1f length:%3.1f", fSpillRoc, fSpillAux, fCalibrationPeriod, fCalibrationLength));

      fSpillRate = CreateRateParameter("SpillRate", false, 10., "", "", "1/s", 0., 5.);

      // if (!fBnetMode) fSpillRate->SetDebugOutput(true);
      CreateTimer("CalibrTimer", -1., false);
   }

   fCombinerRate = CreateRateParameter("CombinerRate", false, 3., "", "", "MB/s", 0., 20., fBnetMode ? 0 : 4);

   fEventsRate = CreateRateParameter("EventsRate", false, 3., "", "", "1/s", 0., 2500.);

   fErrorRate = CreateRateParameter("ErrorRate", false, 3., "", "", "1/s", 0., 10.);

   fThrottleAux = GetCfgInt(roc::xmlThrottleAux, -1, cmd);

   std::string pname =  GetCfgStr(dabc::xmlInputPoolName, roc::xmlRocPool, cmd);

   fInpPool = CreatePoolHandle(GetCfgStr(dabc::xmlInputPoolName, roc::xmlRocPool, cmd).c_str(), fBufferSize, 1);

   for(unsigned num=0; num < (unsigned) numinputs; num++) {
      CreateInput(FORMAT(("Input%u", num)), fInpPool, GetCfgInt(dabc::xmlInputQueueSize, 10, cmd));
      Input(num)->SetInpRateMeter(fCombinerRate);
      fInp.push_back(InputRec());
      fInp[num].ninp = num;
      fInp[num].use = false;
      if (fThrottleAux>=0)
         fInp[num].throttle_rate = CreateRateParameter(FORMAT(("Throttle%u", num)), false, 3., "", "", "%%", 0., 100.);
   }

   fOutPool = CreatePoolHandle(GetCfgStr(dabc::xmlOutputPoolName, roc::xmlRocPool, cmd).c_str(), fBufferSize, 1);

   for(int n=0; n<numoutputs; n++)
      CreateOutput(FORMAT(("Output%d", n)), fOutPool, GetCfgInt(dabc::xmlOutputQueueSize, 10, cmd),
                      fBnetMode ? sizeof(bnet::EventId) : 0);

   fErrorOutput = false;

   DOUT3(("Did CombinerModule creation"));
}

roc::CombinerModule::~CombinerModule()
{
   while (fExtraMessages.size()>0) {
      delete fExtraMessages.front();
      fExtraMessages.pop_front();
   }

   dabc::Buffer::Release(fOutBuf); fOutBuf = 0;
}

void roc::CombinerModule::ProcessInputEvent(dabc::Port* inport)
{
   unsigned inpid = InputNumber(inport);

   if (!inport->CanRecv()) {
      EOUT(("Something wrong with input %u %s", inpid, inport->GetName()));
      return;
   }

   if (fSimpleMode) {

      dabc::Buffer* buf = inport->Recv();

      if (Output(0)->CanSend())
         Output(0)->Send(buf);
      else
         dabc::Buffer::Release(buf);

      return;
   }

//   DOUT3(("Get new buffer in input %u ready %s !!!", inpid, DBOOL(fInp[inpid].isready)));

   // check events in the buffers queues
   FindNextEvent(inpid);
   
   // also check events for all slaves
   for (unsigned nslave=0; nslave<fInp[inpid].slaves.size(); nslave++)
      FindNextEvent(fInp[inpid].slaves[nslave].slaveid);

   FillBuffer();
}


void roc::CombinerModule::ProcessTimerEvent(dabc::Timer* timer)
{
   dabc::mgr()->GetApp()->Submit(new roc::CmdCalibration(false));
}

bool roc::CombinerModule::FindNextEvent(unsigned recid)
{
   if (recid>=fInp.size()) return false;

   InputRec* rec = &(fInp[recid]);

   dabc::Port* port = Input(rec->ninp);
   
   dabc::Buffer* buf = 0;

   if (!rec->isrocid() && !rec->isslave) {
      rec->isudp = (port->GetTransportParameter(roc::xmlTransportKind) == kind_UDP);
      rec->rocid = port->GetTransportParameter(roc::xmlRocNumber);
      rec->format = (roc::MessageFormat) port->GetTransportParameter(roc::xmlMsgFormat);

      for (unsigned sl=0;sl<rec->slaves.size();sl++)
         fInp[rec->slaves[sl].slaveid].format = rec->format;

      DOUT0(("Detect on input %u ROC:%d Kind:%s Slaves:%u", rec->ninp, rec->rocid, (rec->isudp ? "UDP" : "Optic"), rec->slaves.size()));
   }

//   DOUT5(("FindNextEvent REC:%p ROCID:%d KIND:%d", rec, rec->rocid, rec->isudp));

   // if one already found events for specified roc, try to find them for all slaves too
   if (rec->isready) return true;

   while ((buf = port->InputBuffer(rec->curr_nbuf)) != 0) {
   
      DOUT5(("Process buffer %p", buf));

      if (rec->curr_indx >= buf->GetTotalSize()) {
         if (rec->isprev || rec->isnext || !rec->is_simple_rec())
            rec->curr_nbuf++;
         else {
            // no need to keep this buffer in place if no epoch was found
            port->SkipInputBuffers(rec->curr_nbuf+1);
            rec->curr_nbuf = 0;
         }
         rec->curr_indx = 0;
         continue;
      }

//      bool dodump = (rec->curr_indx == 0);

      roc::Iterator iter;

      InitIterator(iter, buf, rec->curr_indx, rec->isudp, rec->rocid);

      bool iserr = false;
      roc::Message* data = 0;

      while (iter.next()) {

         data = & iter.msg();

         if (rec->IsDifferentRocId(data->getRocNumber(), iserr)) {
            if (iserr)
               EOUT(("Input:%u Kind:%s Mismatch in ROC numbers %u %u", rec->ninp, (rec->isudp ? "UDP" : "Optic"), rec->rocid, data->getRocNumber()));
            rec->curr_indx += iter.getMsgSize();
            continue;
         }
         
         if (data->isEpochMsg()) { // epoch marker
            rec->curr_epoch = data->getEpochNumber();
            rec->iscurrepoch = true;
         } else
         if (data->isSyncMsg() && (data->getSyncChNum()==0)) {
            if (!rec->iscurrepoch) {
               DOUT0(("Found SYNC marker %6x without epoch", data->getSyncData()));
            } else
            if ((fSyncScaleDown>0) && (data->getSyncData() % fSyncScaleDown != 0)) {
               DOUT0(("Roc%u SYNC marker %06x not in expected sync step %02x",
                     rec->rocid, data->getSyncData(), fSyncScaleDown));
            } else {
               if (!rec->isprev) {
                  rec->prev_epoch = rec->curr_epoch;
                  rec->isprev = true;
                  rec->prev_nbuf = rec->curr_nbuf;
                  rec->prev_indx = rec->curr_indx;
                  rec->prev_evnt = data->getSyncData();
                  rec->prev_stamp = data->getSyncTs();
                  rec->data_length = 0;
               } else {
                  rec->next_epoch = rec->curr_epoch;
                  rec->isnext = true;
                  rec->next_nbuf = rec->curr_nbuf;
                  rec->next_indx = rec->curr_indx;
                  rec->next_evnt = data->getSyncData();
                  rec->next_stamp = data->getSyncTs();
               }
            } // iscurrepoch
         } else
         if (data->isAuxMsg()) {

            if ((fThrottleAux>=0) &&  (data->getAuxChNum()==(unsigned)fThrottleAux)) {
               uint64_t tm = data->getMsgFullTime(rec->curr_epoch);
               bool state = data->getAuxFalling() == 0;

               if ((rec->last_thottle_tm != 0) && (rec->last_throttle_state != state)) {
                  uint64_t dist = roc::Message::CalcDistance(rec->last_thottle_tm, tm);

                  if (rec->last_throttle_state && rec->throttle_rate)
                     rec->throttle_rate->AccountValue(dist * 1e-7); // maximum is 100 % can be
               }

               rec->last_thottle_tm = tm;
               rec->last_throttle_state = state;
            } else
            if ((fSpillRoc>=0) && ((unsigned)fSpillRoc==data->getRocNumber())
                 && ((unsigned)fSpillAux == data->getAuxChNum())) {

               uint64_t tm = data->getMsgFullTime(rec->curr_epoch);

               bool faraway = (roc::Message::CalcDistance(fLastSpillTime, tm) > 1000000);

               bool changed = false;

               if (data->getAuxFalling() && fSpillState && faraway) {
                  fSpillState = false;
                  fLastSpillTime = tm;
                  changed = true;
               } else
               if (!data->getAuxFalling() && !fSpillState && faraway) {
                  fSpillState = true;
                  fLastSpillTime = tm;
                  changed = true;
                  if (fSpillRate) fSpillRate->AccountValue(1.);
               }

               if (changed) SetParInt("SpillState", fSpillState ? 1 : 0);

               // if spill is off and calibration period is specified, try to start calibration
               if ((fCalibrationPeriod>0) && changed && !fSpillState) {
                  dabc::TimeStamp_t now = TimeStamp();
                  if (dabc::IsNullTime(fLastCalibrationTime)) fLastCalibrationTime = now;

                  if ((dabc::TimeDistance(fLastCalibrationTime, now) > fCalibrationPeriod)) {
                     fLastCalibrationTime = now;
                     // DOUT0(("Do trigger here"));

                     dabc::mgr()->GetApp()->Submit(new roc::CmdCalibration(true));
                     ShootTimer("CalibrTimer", fCalibrationLength);
                  }
               }

            }
         } else
         if (data->isSysMsg()) {
            if  (data->getSysMesType() == roc::SYSMSG_SYNC_PARITY)
               DOUT0(("Roc%u Sync parity", rec->rocid));
            else
               DOUT2(("Roc%u SysMsg type = %u", rec->rocid, data->getSysMesType()));
         }

         rec->curr_indx += iter.getMsgSize();

         if (rec->isprev) rec->data_length += iter.getMsgSize();

         if (rec->isprev && rec->isnext) {
	   DOUT5(("ROCID:%u Find sync events %u - %u fmt:%u", rec->rocid, rec->prev_evnt, rec->next_evnt, rec->format));

//            DOUT1(("Tm: %7.5f Rocid:%u Find sync events %u - %u between %u:%u and %u:%u",
//                    TimeStamp()*1e-6, rec->rocid, rec->prev_evnt, rec->next_evnt,
//                    rec->prev_nbuf, rec->prev_indx, rec->next_nbuf, rec->next_indx));

            rec->isready = true;
            return true;
         }
      }
   }

   return false;
}


void roc::CombinerModule::ProcessOutputEvent(dabc::Port* inport)
{
   if (fSimpleMode) return;

   FillBuffer();
}


bool roc::CombinerModule::SkipEvent(unsigned recid)
{
   if (recid>=fInp.size()) return false;

   InputRec* rec = &(fInp[recid]);

   if ((rec==0) || !rec->isprev || !rec->isnext) return false;

   rec->isready     = false;
   rec->isprev      = true;
   rec->prev_epoch  = rec->next_epoch;
   rec->prev_nbuf   = rec->next_nbuf;
   rec->prev_indx   = rec->next_indx;
   rec->prev_evnt   = rec->next_evnt;
   rec->prev_stamp  = rec->next_stamp;
   rec->data_length = 0;

   rec->isnext     = false;
   
   InputRec* master = rec;
   if (rec->isslave) master = & fInp[rec->ninp];

   unsigned master_skip = master->can_skip_buf();
   
   for (unsigned n=0;n<master->slaves.size();n++) {
      InputRec* slave = & fInp[master->slaves[n].slaveid];
      unsigned slave_skip = slave->can_skip_buf();
      if (slave_skip<master_skip) master_skip = slave_skip; 
   }
   
   if (master_skip>0) {
//      DOUT0(("On input %u Skip buffers %u", rec->ninp, master_skip));

      Input(rec->ninp)->SkipInputBuffers(master_skip);
   
      master->did_skip_buf(master_skip);

      for (unsigned n=0;n<master->slaves.size();n++) {
         InputRec* slave = & fInp[master->slaves[n].slaveid];
         slave->did_skip_buf(master_skip);
      }
   }

//   DOUT4(("Skip event done, search next from %u:%u",
//            rec->curr_nbuf, rec->curr_indx));

   return FindNextEvent(recid);
}

uint32_t CalcEventDistance(uint32_t prev_ev, uint32_t next_ev)
{
   return (next_ev>=prev_ev) ? next_ev - prev_ev : next_ev + 0x1000000 - prev_ev;
}

uint32_t CalcAbsEventDistance(uint32_t ev1, uint32_t ev2)
{
   uint32_t diff = ev1>ev2 ? ev1-ev2 : ev2-ev1;
   if (diff > 0x800000) diff = 0x1000000 - diff;
   return diff;
}


void roc::CombinerModule::FillBuffer()
{
   // method fill output buffer with complete sync event from all available sources

   bool doagain = true;
   uint32_t evnt_cut = 0;

   while (doagain) {

      doagain = false;

      bool is_data_selected = false;

      // first check that all inputs has at least first event
      for (unsigned ninp = 0; ninp < fInp.size(); ninp++) {
         InputRec* inp = &(fInp[ninp]);
         if (!inp->isprev) return;
         inp->use = false;
         inp->data_err = false;
      }

      // than check that if two event are defined, not too big difference between them
      for (unsigned ninp = 0; ninp < fInp.size(); ninp++) {
         InputRec* inp = &(fInp[ninp]);

         if (!inp->isready) continue;

         uint32_t diff = CalcEventDistance(inp->prev_evnt, inp->next_evnt);
         if (fSyncScaleDown>0) {
            if (diff != (unsigned) fSyncScaleDown) {
               DOUT0(("ROC%u: Events shift %06x -> %06x = %02x, expected %02x, ignore", inp->rocid, inp->prev_evnt, inp->next_evnt, diff, fSyncScaleDown));
               inp->data_err = true;
            }
         } else {
            if (diff > 0x10000) {
               inp->data_err = true;
               EOUT(("Too large event shift %06x -> %06x = %06x on ROC%u", inp->prev_evnt, inp->next_evnt, diff, inp->rocid));
            }
         }

/*         if ((!inp->data_err) && (ninp==0)) {
            if  (inp->prev_evnt != (inp->prev_epoch & 0xffffff)) {
                EOUT(("Mismatch in epoch %8x and event number %6x for ROC%u", inp->prev_epoch, inp->prev_evnt, inp->rocid));
                inp->data_err = true;
            }

            if  (inp->next_evnt != (inp->next_epoch & 0xffffff)) {
                EOUT(("Mismatch in epoch %8x and event number %6x for ROC%u", inp->next_epoch, inp->next_evnt, inp->rocid));
                inp->data_err = true;
            }
         }
*/
         if (inp->data_err) {
            inp->use = true;
            is_data_selected = true;
            break;
         }
      }

      uint32_t min_evnt(0xffffff), max_evnt(0); // 24 bits

      if (!is_data_selected) {

         // try select data on the basis of the event number

         for (unsigned ninp = 0; ninp < fInp.size(); ninp++) {
            // one should have at least epoch mark define
            InputRec* inp = &(fInp[ninp]);

            if (!inp->isprev) return;

            if (inp->prev_evnt < evnt_cut) continue;

            if (inp->prev_evnt < min_evnt) min_evnt = inp->prev_evnt;
            if (inp->prev_evnt > max_evnt) max_evnt = inp->prev_evnt;
         }

         if (min_evnt > max_evnt) {
            EOUT(("Nothing found!!!"));
            return;
         }

         // try to detect case of upper border crossing
         if (min_evnt != max_evnt)
            if ((min_evnt < 0x10000) && (max_evnt > 0xff0000L)) {
               evnt_cut = 0x800000;
               doagain = true;
               continue;
            }

         evnt_cut = 0;

         for (unsigned ninp = 0; ninp < fInp.size(); ninp++) {

            InputRec* inp = &(fInp[ninp]);

            inp->use = (inp->prev_evnt == min_evnt);
            inp->data_err = false;

            if (!inp->use) continue;

            // one should have next event completed to use it for combining
            if (!inp->isready) return;

            is_data_selected = true;

            if (ninp==0) continue;

            if (!fInp[0].use) // if first roc is not used, anyway error data
               inp->data_err = true;
            else
            if (inp->next_evnt != fInp[0].next_evnt) {
               inp->data_err = true;
               uint32_t diff = CalcAbsEventDistance(inp->next_evnt , fInp[0].next_evnt);

               if (diff > 0x1000)
                  EOUT(("Next event mismatch between ROC%u:%06x and ROC%u:%06x  diff = %06x",
                         fInp[0].rocid, fInp[0].next_evnt, inp->rocid, inp->next_evnt, diff));
            }
         }

      }

      if (!is_data_selected) {
         EOUT(("Data not selected here - why"));
         return;
      }

      dabc::BufferSize_t totalsize = sizeof(mbs::EventHeader);
      unsigned numused = 0;

//      DOUT3(("Try to prepare for event %u", min_evnt));

      for (unsigned ninp = 0; ninp < fInp.size(); ninp++) {

         InputRec* inp = &(fInp[ninp]);

         if (!inp->use) continue;

         totalsize += sizeof(mbs::SubeventHeader);

         // one need always epoch for previous event
         totalsize += 2*roc::Message::RawSize(inp->format);

         totalsize += inp->data_length; 

         numused++;
      }

      bool skip_evnt = false;
      unsigned extra_size = 0;

      // check that event id corresponds with sync scale down factor
      if (fBnetMode && (min_evnt<=max_evnt) && (fSyncScaleDown > 0) && (min_evnt % fSyncScaleDown != 0)) {
         EOUT(("Event number %u not in agreement with scale down factor %u - discard event", min_evnt, fSyncScaleDown));
         skip_evnt = true;
      }

      // if not all inputs are selected in bnet mode, skip data while we need complete events
      if (fBnetMode && (numused<fInp.size())) {
         EOUT(("Skip while too few events"));
         skip_evnt = true;
      }

      // if we producing normal event, try to add extra messages if we have them
      if (!skip_evnt && (numused==fInp.size()) && fExtraMessages.size()>0)
         extra_size = fExtraMessages.front()->size() * roc::Message::RawSize(roc::formatNormal);

      // check if we should send previously filled data - reason that new event does not pass into rest of buffer
      if (fBnetMode || (!skip_evnt && ((totalsize + extra_size) > f_outptr.fullsize())))
         if (!FlushOutputBuffer()) return;

      // take new buffer if required
      if (!skip_evnt && (fOutBuf==0)) {
         fOutBuf = fOutPool->TakeBuffer(fBufferSize);
         if (fOutBuf==0) {
            EOUT(("Cannot get buffer from pool"));
            return;
         }
         fOutBuf->SetDataSize(fBufferSize);

         // if we cannot put extra messages, try to skip them at this event
         if (fOutBuf->GetDataSize() < (totalsize + extra_size)) extra_size = 0;

         // if anyway size too big, skip event totally
         if (fOutBuf->GetDataSize() < (totalsize + extra_size)) {
            EOUT(("Cannot put event (sz:%u) in buffer (sz:%u) - skip event", totalsize, fOutBuf->GetDataSize()));
            skip_evnt = true;
         }
         f_outptr.reset(fOutBuf);
      }

      if (!skip_evnt) {
         dabc::Pointer old(f_outptr);

//         DOUT5(("Start MBS event at pos %u", dabc::Pointer(fOutBuf).distance_to(f_outptr)));

         mbs::EventHeader* evhdr = (mbs::EventHeader*) f_outptr();
         evhdr->Init(min_evnt);
         f_outptr.shift(sizeof(mbs::EventHeader));

//         DOUT1(("Fill event %7u  ~size %6u inp0 %ld outsize:%u",
//            min_evnt, totalsize, Input(0)->InputPending(), f_outptr.fullsize()));

//         DOUT1(("Distance to data %u", dabc::Pointer(fOutBuf).distance_to(f_dataptr)));

         MessagesVector* extra = 0;

         if (extra_size>0) {
            extra = fExtraMessages.front();
            fExtraMessages.pop_front();
         }

         unsigned filled_sz = FillRawSubeventsBuffer(f_outptr, extra);

         //DOUT1(("Build event %u", filled_sz));

         if (filled_sz == 0) {
             EOUT(("Event data not filled - event skipped!!!"));
             f_outptr.reset(old);
         } else {
            
//            DOUT0(("Calculated size = %u  filled size = %u", totalsize, filled_sz + sizeof(mbs::EventHeader)));
            
            evhdr->SetSubEventsSize(filled_sz);

            if (fBnetMode) {
               fOutBuf->SetHeaderSize(sizeof(bnet::EventId));
               uint32_t ev = min_evnt;
               if (fSyncScaleDown>0) ev = ev / fSyncScaleDown;
               *((bnet::EventId*) fOutBuf->GetHeader()) =  ev;
               // DOUT0(("Produce event %u", ev));
               FlushOutputBuffer();
            }
         }
      }

      doagain = true;
      for (unsigned ninp = 0; ninp<fInp.size(); ninp++)
         if (fInp[ninp].use)
            if (!SkipEvent(ninp)) doagain = false;
   }
}

/*! send to output all data, which is now filled into output buffer
 */

bool roc::CombinerModule::FlushOutputBuffer()
{
   // if no buffer, nothing to flush - exit normally
   if (fOutBuf==0) return true;

   dabc::BufferSize_t usedsize = fOutBuf->GetDataSize() - f_outptr.fullsize();
   if (usedsize==0) return true;

   // all outputs must be able to get buffer for sending
   if (!CanSendToAllOutputs()) return false;

//   DOUT1(("Filled size = %u", usedsize));

   fOutBuf->SetDataSize(usedsize);

   fOutBuf->SetTypeId(mbs::mbt_MbsEvents);

   SendToAllOutputs(fOutBuf);

   fOutBuf = 0;
   f_outptr.reset();

   return true;
}

unsigned roc::CombinerModule::FillRawSubeventsBuffer(dabc::Pointer& outptr, roc::MessagesVector* extra)
{
   unsigned filled_size = 0;

   bool iserr = false;

   for (unsigned ninp = 0; ninp < fInp.size(); ninp++) {
      InputRec* rec = &(fInp[ninp]);
      if (!rec->use) continue;
 
      mbs::SubeventHeader* subhdr = (mbs::SubeventHeader*) outptr();
      subhdr->Init();
      subhdr->iProcId = rec->data_err ? roc::proc_ErrEvent : roc::proc_RocEvent;
      subhdr->iSubcrate = rec->rocid;
      subhdr->iControl = rec->format; // roc::formatEth1 == 0, compatible with older lmd files

      if (rec->data_err) iserr = true;

      outptr.shift(sizeof(mbs::SubeventHeader));
      filled_size+=sizeof(mbs::SubeventHeader);

      unsigned msg_size = roc::Message::RawSize(rec->format);

      unsigned subeventsize = msg_size;

      roc::Message msg;
      msg.setMessageType(roc::MSG_EPOCH);
      msg.setRocNumber(rec->rocid);
      msg.setEpochNumber(rec->prev_epoch);
      msg.setEpochMissed(0);
      msg.copyto(outptr(), rec->format);

      outptr.shift(msg_size);

      unsigned nbuf = rec->prev_nbuf;

      bool firstmsg = true;

      while (nbuf<=rec->next_nbuf) {
         dabc::Buffer* buf = Input(rec->ninp)->InputBuffer(nbuf);
         if (buf==0) {
            EOUT(("Internal error"));
            return 0;
         }

         // if next epoch in the same buffer, limit its size by next_indx + 6 (with next event)
         dabc::Pointer ptr(buf);
         if (nbuf == rec->next_nbuf) ptr.setfullsize(rec->next_indx + msg_size);
         if (nbuf == rec->prev_nbuf) ptr.shift(rec->prev_indx);

         if (rec->is_simple_rec()) {
            // DOUT0(("Copy to output %u has %u", ptr.fullsize(), outptr.fullsize()));
            if (firstmsg && extra) {
               // if we have extra messages, put them right after first sync message
               outptr.copyfrom(ptr, msg_size);
               outptr.shift(msg_size);
               subeventsize += msg_size;
               ptr.shift(msg_size);
               AddExtraMessagesToSubevent(extra, outptr, subeventsize, rec);
            }
            // copy rest of the data
            outptr.copyfrom(ptr, ptr.fullsize());
            outptr.shift(ptr.fullsize());
            subeventsize += ptr.fullsize();
            firstmsg = false;
         } else {
            roc::Iterator iter(rec->format);
            iter.assign(ptr(), ptr.fullsize());
            while (iter.next())
               if (iter.msg().getRocNumber()==rec->rocid) {
                  iter.msg().copyto(outptr(), rec->format);
                  outptr.shift(msg_size);
                  subeventsize += msg_size;
                  if (firstmsg && extra)
                     AddExtraMessagesToSubevent(extra, outptr, subeventsize, rec);

                  firstmsg = false;
               }

            DOUT5(("Write event for ROC%u sz %u fullsz %u nbuf %u fmt1: %u fmt2: %u", 
               rec->rocid, subeventsize, ptr.fullsize(), nbuf, rec->format, buf->GetTypeId() - roc::rbt_RawRocData));

         }
         nbuf++;
      }

      filled_size += subeventsize;
      subhdr->SetRawDataSize(subeventsize);
   }

   if (fEventsRate) fEventsRate->AccountValue(1.);

   if (iserr && fErrorRate) fErrorRate->AccountValue(1.);

   if (extra!=0) delete extra;

   return filled_size;
}

void roc::CombinerModule::AddExtraMessagesToSubevent(roc::MessagesVector* extra, dabc::Pointer& outptr, unsigned& subeventsize, InputRec* rec)
{
   unsigned msg_size = roc::Message::RawSize(rec->format);

//   int cnt = 0;

   for (unsigned n=0;n<extra->size();n++)
      if (extra->at(n).getRocNumber() == rec->rocid) {
         extra->at(n).copyto(outptr(), rec->format);
         outptr.shift(msg_size);
         subeventsize += msg_size;
//         cnt++;
      }

//   if (cnt>0) DOUT0(("Add %d extra messages for ROC%u", cnt, rec->rocid));
}

void roc::CombinerModule::DumpData(dabc::Buffer* buf)
{
   roc::Iterator iter;
   if (!InitIterator(iter, buf)) return;

   while (iter.next())
      iter.msg().printData(3);
}

int roc::CombinerModule::ExecuteCommand(dabc::Command* cmd)
{
   if (cmd->IsName(CmdAddSlave::CmdName())) {
      unsigned masterid = cmd->GetUInt(CmdAddSlave::Master(), 0);
      unsigned rocid = cmd->GetUInt(CmdAddSlave::RocId(), 0);

      if (masterid>=fInp.size()) return dabc::cmd_false;

      unsigned slaveid = fInp.size();

      fInp.push_back(InputRec());

      InputRec* master = &(fInp[masterid]);
      InputRec* slave = &(fInp[slaveid]);

      slave->isslave = true;
      slave->ninp = master->ninp;
      slave->rocid = rocid;
      slave->isudp = false;
      slave->format = roc::formatOptic2;

      master->slaves.push_back(SlaveRec(rocid, slaveid));

      return dabc::cmd_true;

   } else

   if (cmd->IsName(CmdMessagesVector::CmdName())) {
      MessagesVector* vect = (MessagesVector*) cmd->GetPtr(CmdMessagesVector::Vector());

      if (vect!=0)
         fExtraMessages.push_back(vect);
      else
         EOUT(("Zero vector with extra messages"));

      if (fExtraMessages.size()>10) {
         EOUT(("Too many extra messages, remove part of them"));
         delete fExtraMessages.front();
         fExtraMessages.pop_front();
      }

      return dabc::cmd_true;
   }

   return dabc::ModuleAsync::ExecuteCommand(cmd);
}


