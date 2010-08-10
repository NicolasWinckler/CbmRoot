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

#ifndef ROC_UDPTRANSPORT_H
#define ROC_UDPTRANSPORT_H

#ifndef DABC_SocketThread
#include "dabc/SocketThread.h"
#endif

#ifndef DABC_Transport
#include "dabc/Transport.h"
#endif

#ifndef DABC_collections
#include "dabc/collections.h"
#endif

#ifndef DABC_MemoryPool
#include "dabc/MemoryPool.h"
#endif

#ifndef ROC_UdpBoard
#include "roc/UdpBoard.h"
#endif

#ifndef DABC_version
#include "dabc/version.h"
#endif

#if DABC_VERSION_CODE <= DABC_VERSION(1,0,2)
#include "dabc/CommandClient.h"
#endif



#include <list>

namespace roc {

   extern const char* xmlFlashTimeout;

   class UdpDevice;


   class UdpDataSocket : public dabc::SocketIOProcessor,
                         public dabc::Transport,
                         protected dabc::MemoryPoolRequester
#if DABC_VERSION_CODE <= DABC_VERSION(1,0,2)
                         , protected dabc::CommandClientBase
#endif
                         {
      friend class UdpDevice;
      protected:
         enum EUdpEvents { evntStartTransport = evntSocketLast + 1,
                           evntStopTransport,
                           evntConfirmCmd,
                           evntFillBuffer };

         enum EDaqState { daqInit,        // initial state
                          daqStarting,    // daq runs, but no any start daq message was seen
                          daqRuns,        // normal working mode
                          daqStopping,    // we did poke(stop_daq) and just waiting that command is performed
                          daqFails        // error state
                        };

         struct ResendInfo {

            uint32_t      pktid;    // id of packet
            double        lasttm;   // when request was send last time
            int           numtry;   // how many requests was send
            dabc::Buffer* buf;      // buffer pointer
            unsigned      bufindx;  // index of buffer in the queue
            char*         ptr;      // target location

            ResendInfo(unsigned id = 0) :
               pktid(id),
               lasttm(0.),
               numtry(0),
               buf(0),
               bufindx(0),
               ptr(0) {}

            ResendInfo(const ResendInfo& src) :
               pktid(src.pktid),
               lasttm(src.lasttm),
               numtry(src.numtry),
               buf(src.buf),
               bufindx(src.bufindx),
               ptr(src.ptr) {}

         };


         UdpDevice*         fDev;
         dabc::Mutex        fQueueMutex;
         dabc::BuffersQueue fQueue;

         unsigned           fReadyBuffers; // how many buffers in queue can be provided to user
         unsigned           fTransportBuffers; // how many buffers can be used for transport

         dabc::Buffer*      fTgtBuf;   // pointer on buffer, where next portion can be received
         unsigned           fTgtBufIndx; // queue index of target buffer - use fQueue.Item(fReadyBuffers + fTgtBufIndx)
         unsigned           fTgtShift; // current shift in the buffer
         UdpDataPacket      fTgtHdr;   // place where data header should be received
         char*              fTgtPtr;   // location where next data should be received
         uint32_t           fTgtNextId; // expected id of next packet
         uint32_t           fTgtTailId; // first id of packet which can not be dropped on ROC side
         bool               fTgtCheckGap;  // true if one should check gaps

         char               fTempBuf[2000]; // buffer to recv packet when no regular place is available

         dabc::Queue<ResendInfo>  fResend;

         EDaqState          daqState;
         bool               daqCheckStop;

         UdpDataPacketFull  fRecvBuf;

         unsigned           fBufferSize;
         unsigned           fTransferWindow;

         dabc::MemoryPool  *fPool;

         double   lastRequestTm;   // time when last request was send
         bool     lastRequestSeen; // indicate if we seen already reply on last request
         uint32_t lastRequestId;   // last request id
         uint32_t lastSendFrontId; // last send id of front packet

         unsigned rocNumber;

         uint64_t fTotalRecvPacket;
         uint64_t fTotalResubmPacket;

         double             fFlashTimeout;  // after such timeout partially filled packed will be delivered
         dabc::TimeStamp_t  fLastDelivery;  // time of last delivery

         OperList           fStartList;

         MessageFormat      fFormat;

         bool daqActive() const { return daqState == daqRuns; }

#if DABC_VERSION_CODE <= DABC_VERSION(1,0,2)
         virtual bool _ProcessReply(dabc::Command* cmd);
#else
         virtual bool ReplyCommand(dabc::Command* cmd);
#endif
         virtual void ProcessPoolChanged(dabc::MemoryPool* pool) {}

         virtual bool ProcessPoolRequest();

         virtual double ProcessTimeout(double last_diff);

         void ConfigureFor(dabc::Port* port);

         void AddBuffersToQueue(bool checkanyway = true);
         bool CheckNextRequest(bool check_retrans = true);

         void AddDataPacket(int len, void* tgt);
         void CompressBuffer(dabc::Buffer* buf);
         void CheckReadyBuffers(bool doflush = false);

         void ResetDaq();

         bool prepareForSuspend();

         virtual int GetParameter(const char* name);

         void setFlushTimeout(double tmout) { fFlashTimeout = tmout; }
         double getFlushTimeout() const {  return fFlashTimeout; }

      public:
         UdpDataSocket(UdpDevice* dev, int fd);
         virtual ~UdpDataSocket();

         virtual void ProcessEvent(dabc::EventId evnt);

         virtual bool ProvidesInput() { return true; }
         virtual bool ProvidesOutput() { return false; }

         virtual bool Recv(dabc::Buffer* &buf);
         virtual unsigned  RecvQueueSize() const;
         virtual dabc::Buffer* RecvBuffer(unsigned indx) const;
         virtual bool Send(dabc::Buffer* buf) { return false; }
         virtual unsigned SendQueueSize() { return 0; }
         virtual unsigned MaxSendSegments() { return 0; }

         virtual void StartTransport();
         virtual void StopTransport();
   };

}

#endif
