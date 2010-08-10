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

#ifndef ROC_CALIBRATIONMODULE_H
#define ROC_CALIBRATIONMODULE_H

#include "dabc/ModuleAsync.h"
#include "dabc/Pointer.h"

#include "mbs/LmdTypeDefs.h"
#include "mbs/MbsTypeDefs.h"


#include "roc/Message.h"
#include "roc/Iterator.h"
#include "roc/Sorter.h"

#include <vector>

namespace roc {

   class CalibrationModule : public dabc::ModuleAsync {

      public:

         CalibrationModule(const char* name, dabc::Command* cmd);
         virtual ~CalibrationModule();

         virtual void ProcessInputEvent(dabc::Port* port);

         virtual void ProcessOutputEvent(dabc::Port* port);

      protected:

         struct InputRec {
            dabc::Pointer     ptr;    // pointer on current event in input buffer
            bool              inuse;  // indicate if some more buffers expected from input
            mbs::EventHeader* evhdr;  // current event header
            InputRec() : ptr(), inuse(true), evhdr(0) {}
            InputRec(const InputRec& src) : ptr(src.ptr), inuse(src.inuse), evhdr(src.evhdr) {}
         };

         struct CalibRec {
            unsigned       rocid;      // rocid
            double         calibr_k;   // calibration coeff K
            uint32_t       calibr_b_e; // calibration coeff B, integer part (in epochs size)
            double         calibr_b_f; // calibration coeff B, float path (in epoch fraction)
            unsigned       weight;     // weight of coefficients
            roc::Sorter*   sorter;

            uint64_t       evnt1_tm;    // time of event in the beginning
            uint64_t       evnt2_tm;    // time of event in the end
            uint32_t       evnt1_num;   // number of event in the beginning
            uint32_t       evnt2_num;   // number of event in the end
            uint64_t       evnt_len;    // time length of the event

            void*          src_data;    // original data
            unsigned       src_length;  // data length in source
            MessageFormat  src_format;  // format of source data

            roc::Message*  data;        // sorted data
            unsigned       numdata;     // number of data sill to process

            uint32_t       front_epoch; // value of front epoch, relative to which delta t is calculating
            double         front_shift; // shift to calibrated time to adjust to output front epoch
            uint32_t       last_epoch;  // last epoch, taken from stream
            bool           is_last_epoch; // do we get at all any epoch
            uint32_t       last_stamp;    // last stamp, detected on the system

            uint32_t       stamp;       // current timestamp in stream (calibrated, relative to FrontOutEpoch)
            bool           stamp_copy;  // indicate that message should be copied to output

            CalibRec(unsigned id = 0);
            void RecalculateCalibr(double k, double b);
            double CalibrEpoch(uint32_t& epoch);
            double RecalculateTs(uint64_t tm);
            void TimeStampOverflow();
         };

         bool DoCalibration();

         bool FlushOutputBuffer();

         void PrintEvent(void* evnt);

         dabc::PoolHandle*    fPool;
         unsigned             fNumRocs;
         dabc::BufferSize_t   fBufferSize;

         std::vector<InputRec> fInp;

         dabc::Buffer*        fOutBuf;
         dabc::Pointer        f_outptr; // pointer on free space in output buffer

         long                 fBuildEvents;
         long                 fSkippedEvents;
         long                 fErrorEvents;
         long                 fBuildSize;

         std::vector<CalibRec> fCalibr;

         uint32_t             fFrontOutEpoch; // epoch, relative to which all time stamps are calculating
         uint32_t             fLastOutEpoch;  // last epoch, delivered to output
         bool                 fIsOutEpoch;    // indicates if there was epoch delivered to output

         MessageFormat        fOutFormat;
         uint16_t             fOutRocNumber; // roc number, used for epoch markers
   };
}

#endif
