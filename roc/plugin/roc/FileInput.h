#ifndef ROC_FILEINPUT_H
#define ROC_FILEINPUT_H

#include "roc/UdpBoard.h"

#ifndef MBS_LmdInput
#include "mbs/LmdInput.h"
#endif

#ifndef MBS_MbsTypeDefs
#include "mbs/MbsTypeDefs.h"
#endif

namespace roc {

   class FileInput : public Board {
      protected:
         mbs::LmdInput* fInput;

         mbs::EventHeader* fHdr;
         mbs::SubeventHeader* fSubev;

         int fSelectRoc;

      public:
         FileInput(mbs::LmdInput* inp, int selectroc = -1);

         virtual ~FileInput();

         virtual int getTransportKind() const;
         
         virtual void setFlushTimeout(double) {}

         virtual int operGen(OperList&, double = 0.) { return 1; }

         // daq interface part of roc::Board
         virtual bool startDaq() { return false; }
         virtual bool suspendDaq()  { return false; }
         virtual bool stopDaq()  { return false; }

         virtual bool getNextBuffer(void* &buf, unsigned& len, double tmout = 1.);

         virtual MessageFormat getMsgFormat() const;
   };
}


#endif
