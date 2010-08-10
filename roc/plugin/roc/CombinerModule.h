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

#ifndef ROC_COMBINERMODULE_H
#define ROC_COMBINERMODULE_H

#include "dabc/ModuleAsync.h"
#include "dabc/Pointer.h"

#include "roc/BoardsVector.h"

#include <list>

namespace roc {

   extern const char* xmlBnetMode;         // true if combiner works within bnet enviroment
   extern const char* xmlSyncScaleDown;    // identifies scale down of sync events
   extern const char* xmlSpillRoc;
   extern const char* xmlSpillAux;
   extern const char* xmlCalibrationPeriod;
   extern const char* xmlCalibrationLength;
   extern const char* xmlThrottleAux;

   class CombinerModule : public dabc::ModuleAsync {

      public:

         CombinerModule(const char* name, dabc::Command* cmd);
         virtual ~CombinerModule();

         virtual void ProcessInputEvent(dabc::Port* port);

         virtual void ProcessOutputEvent(dabc::Port* port);

         virtual void ProcessTimerEvent(dabc::Timer* timer);

      protected:
         struct SlaveRec {
            unsigned   rocid; // roc id number
            unsigned   slaveid; // sequence number of InputRec
            
            SlaveRec(unsigned _rocid, unsigned _slaveid) : rocid(_rocid), slaveid(_slaveid) {}

            SlaveRec(const SlaveRec& src) : rocid(src.rocid), slaveid(src.slaveid) {}
         };
         
         struct InputRec {
            bool         isslave; // indicate if it is slave record          
            unsigned      ninp; // actual input number, where data is taken
            unsigned      rocid;
            MessageFormat format;
            bool          isudp;  // indicate if optic or Ethernet transport are attached

            uint32_t   curr_epoch;
            bool       iscurrepoch;
            unsigned   curr_nbuf;
            unsigned   curr_indx;

            uint32_t   prev_epoch;  // previous epoch marker
            bool       isprev;
            unsigned   prev_nbuf;  // id of buffer where epoch is started
            unsigned   prev_indx;  // index inside buffer where epoch found
            uint32_t   prev_evnt;  // sync event number
            uint32_t   prev_stamp; // sync time stamp

            uint32_t   next_epoch;  // number of the next epoch
            bool       isnext;      //
            unsigned   next_nbuf;   // id of buffer where epoch is started
            unsigned   next_indx;  // index inside buffer where epoch found
            uint32_t   next_evnt;  // sync event number
            uint32_t   next_stamp; // sync time stamp
            
            unsigned   data_length; // length of data between prev and next event

            bool       isready;    // indicate if epoch and next are defined and ready for combining

            bool          use;     // use input for combining
            bool          data_err; // event numbers are wrong
            dabc::Pointer ptr;     // used in combining
            unsigned      nbuf;    // used in combining
            uint32_t      epoch;   // original epoch used in combining
            unsigned      stamp;   // corrected stamp value in combining
            unsigned      stamp_shift; // (shift to event begin)

            uint64_t             last_thottle_tm; // last time, when threshold state was changed
            bool                 last_throttle_state; // last state of throttle signal (on/off)
            dabc::RateParameter* throttle_rate;     // rate of throttle signal
            
            std::vector<SlaveRec> slaves; // list of extra codes 

            bool isrocid() const { return rocid!=0xffffffff; }
            
            bool IsDifferentRocId(unsigned id, bool& iserr)
            {
               iserr = false;
               if (rocid==id) return false;

               // in case of udp channel any different rocid is error
               if (isudp) { iserr = true; return true; }

               // if rec is slave we do not check any possible errors
               if (isslave) return true;

               // if master record has slave with such rocid, than it is ok
               for (unsigned n=0;n<slaves.size();n++)
                  if (slaves[n].rocid == id) return true;

               iserr = true;
               return true;
            }
            
            unsigned can_skip_buf() { return isprev ? prev_nbuf : 0; }
            
            void did_skip_buf(unsigned cnt) 
            { 
               curr_nbuf -= cnt;
               if (isprev) prev_nbuf -= cnt;
               if (isnext) next_nbuf -= cnt;
            }
            
            bool is_simple_rec() const {  return !isslave && (slaves.size()==0); }

            InputRec() :
               isslave(false), ninp(0), rocid(0xffffffff), format(formatEth1), isudp(true),
               curr_epoch(0), iscurrepoch(false), curr_nbuf(0), curr_indx(0),

               prev_epoch(0), isprev(false), prev_nbuf(0), prev_indx(0), prev_evnt(0), prev_stamp(0),
               next_epoch(0), isnext(false), next_nbuf(0), next_indx(0), next_evnt(0), next_stamp(0),
               data_length(0), isready(false), 
               use(false), data_err(false), ptr(), nbuf(0), epoch(0), stamp(0), stamp_shift(0),
               last_thottle_tm(0), last_throttle_state(false), throttle_rate(0),
               slaves() {}
         
             InputRec(const InputRec& r) :
               isslave(r.isslave), ninp(r.ninp), rocid(r.rocid), format(r.format), isudp(r.isudp),
               curr_epoch(r.curr_epoch), iscurrepoch(r.iscurrepoch), curr_nbuf(r.curr_nbuf), curr_indx(r.curr_indx),

               prev_epoch(r.prev_epoch), isprev(r.isprev), prev_nbuf(r.prev_nbuf), prev_indx(r.prev_indx), prev_evnt(r.prev_evnt), prev_stamp(r.prev_stamp),
               next_epoch(r.next_epoch), isnext(r.isnext), next_nbuf(r.next_nbuf), next_indx(r.next_indx), next_evnt(r.next_evnt), next_stamp(r.next_stamp),
               data_length(r.data_length), isready(r.isready), 
               use(r.use), data_err(r.data_err), ptr(r.ptr), nbuf(r.nbuf), epoch(r.epoch), stamp(r.stamp), stamp_shift(r.stamp_shift),
               last_thottle_tm(r.last_thottle_tm), last_throttle_state(r.last_throttle_state), throttle_rate(r.throttle_rate),
               slaves(r.slaves) {}


         };

         virtual int ExecuteCommand(dabc::Command* cmd);

         bool FindNextEvent(unsigned recid);
         bool SkipEvent(unsigned recid);

         void FillBuffer();
         bool FlushOutputBuffer();

         unsigned FillRawSubeventsBuffer(dabc::Pointer& outptr, roc::MessagesVector* extra = 0);
         void AddExtraMessagesToSubevent(roc::MessagesVector* extra, dabc::Pointer& outptr, unsigned& subeventsize, InputRec* rec);

         void DumpData(dabc::Buffer* buf);

         dabc::PoolHandle*    fInpPool;
         dabc::PoolHandle*    fOutPool;
         dabc::BufferSize_t   fBufferSize;

         bool                 fSimpleMode;
         bool                 fBnetMode;       // if true, format buffer to be accepted by other bnet modules
         int                  fSyncScaleDown;  // expected distance between two events ids, should be specified in config file or in command

         std::vector<InputRec> fInp;

         dabc::Buffer*        fOutBuf;
         dabc::Pointer        f_outptr;

         dabc::RateParameter* fCombinerRate;      // used to indicate frequency of bad events

         dabc::RateParameter* fEventsRate;     // produced event rate

         dabc::RateParameter* fErrorRate;      // used to indicate frequency of bad events
         bool                 fErrorOutput;

         int                  fSpillRoc;      // roc number, where spill on/off signal is connected
         int                  fSpillAux;      // number of Aux channel where spill on/off signal is measured
         bool                 fSpillState;    // current state of spill, known by module
         uint64_t             fLastSpillTime; // last time when spill signal was seen


         double               fCalibrationPeriod; // how often trigger mode should be started
         double               fCalibrationLength; // how long calibration should be switched on
         dabc::RateParameter* fSpillRate;
         dabc::TimeStamp_t    fLastCalibrationTime; // last time when trigger was caused

         int                  fThrottleAux;  // defines AUX signal, which analyzed at throttled

         std::list<MessagesVector*>  fExtraMessages;
   };
}

#endif
