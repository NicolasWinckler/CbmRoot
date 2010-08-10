#ifndef ROC_ITERATOR_H
#define ROC_ITERATOR_H

#include "roc/Message.h"

#include "roc/Board.h"

namespace roc {

   class Iterator {
      protected:
         MessageFormat fFormat;
         void*         fBuffer;     // assigned buffer
         uint32_t      fBufferLen;  // length of assigned buffer
         uint32_t      fBufferPos;  // current position
         uint32_t      fMsgSize;    // size of single message
         Message       fMsg;        // current read message
         uint32_t      fEpoch;      // current epoch
         uint32_t      fEpoch2;     // current epoch2
         Board*        fBoard;      // board instance, which delivers buffers
         bool          fOwner;      // owner of board instance

      public:
         Iterator(MessageFormat fmt = formatNormal);

         Iterator(Board* brd, bool owner = false);

         Iterator(const Iterator& src);

         Iterator(const char* filename);

         virtual ~Iterator();

         void setFormat(MessageFormat fmt);
         MessageFormat getFormat() const { return fFormat; }

         void setRocNumber(uint16_t rocnum = 0);

         uint32_t getMsgSize() const { return fMsgSize; }

         bool assign(void* buf, uint32_t len);

         bool nextBuffer(Board* brd, double tmout = 1.);

         void resetEpochs();

         inline bool next(double tmout = 1.)
         {
            if ((fBuffer==0) || (fBufferPos>=fBufferLen))
               if (!nextBuffer(fBoard, tmout)) return false;
            if (fMsg.assign((uint8_t*) fBuffer + fBufferPos, fFormat)) {
               fBufferPos += fMsgSize;
               switch (fMsg.getMessageType()) {
                  case MSG_EPOCH:  fEpoch = fMsg.getEpochNumber(); break;
                  case MSG_EPOCH2: fEpoch2 = fMsg.getEpoch2Number(); break;
               }
               return true;
            }
            fBufferPos = fBufferLen;
            return false;
         }

         // can be used only inside buffer, not with board source
         inline bool last()
         {
            if ((fBuffer==0) || (fBufferLen<fMsgSize)) return false;

            fBufferPos = (fBufferLen - fMsgSize) / fMsgSize * fMsgSize;

            return next(0.);
         }

         // can be used only inside buffer, not with board source
         inline bool prev()
         {
            if ((fBuffer==0) || (fBufferPos<fMsgSize*2)) return false;

            fBufferPos -= fMsgSize*2;

            return next(0.);
         }

         uint32_t getLastEpoch() const { return fEpoch; }
         uint32_t getLastEpoch2() const { return fEpoch2; }
         uint32_t getProperLastEpoch() const
         {
            return (fMsg.getMessageType() == MSG_EPOCH2) || (fMsg.getMessageType() == MSG_GET4) ? getLastEpoch2() : getLastEpoch();
         }


         inline Message& msg() { return fMsg; }

         inline uint64_t getMsgFullTime() const
         {
            switch(fMsg.getMessageType()) {
               case MSG_EPOCH2:
               case MSG_GET4:
                  return fMsg.getMsgFullTime(fEpoch2);
            }

            return fMsg.getMsgFullTime(fEpoch);
         }

         inline double getMsgFullTimeD() const
         {
            switch(fMsg.getMessageType()) {
               case MSG_EPOCH2:
               case MSG_GET4:
                  return fMsg.getMsgFullTimeD(fEpoch2);
            }

            return fMsg.getMsgFullTimeD(fEpoch);
         }

         void printMessages(unsigned cnt = 100, unsigned kind = msg_print_Prefix | msg_print_Data);
   };
}


#endif
