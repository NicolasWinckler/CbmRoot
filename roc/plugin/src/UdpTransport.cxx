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

#include "roc/UdpTransport.h"

#include "roc/UdpDevice.h"
#include "roc/Commands.h"
#include "roc/defines_roc.h"
#include "roc/defines_udp.h"
#include "roc/Message.h"
#include "roc/Iterator.h"

#include "dabc/timing.h"
#include "dabc/Port.h"

#include <math.h>

const unsigned UDP_DATA_SIZE = ((sizeof(roc::UdpDataPacketFull) - sizeof(roc::UdpDataPacket)) / 6) * 6;

const char* roc::xmlFlashTimeout = "FlashTimeout";

roc::UdpDataSocket::UdpDataSocket(UdpDevice* dev, int fd) :
   dabc::SocketIOProcessor(fd),
   dabc::Transport(dev),
   dabc::MemoryPoolRequester(),
#if DABC_VERSION_CODE <= DABC_VERSION(1,0,2)
   dabc::CommandClientBase(),
#endif
   fDev(dev),
   fQueueMutex(),
   fQueue(1),
   fReadyBuffers(0),
   fTransportBuffers(0),
   fResend()
{
   // we will react on all input packets
   SetDoingInput(true);

   fTgtBuf = 0;
   fTgtBufIndx = 0;
   fTgtShift = 0;
   fTgtPtr = 0;
   fTgtCheckGap = false;

   fTgtNextId = 0;
   fTgtTailId = 0;

   fBufferSize = 0;
   fTransferWindow = 40;

   rocNumber = 0;
   daqState = daqInit;
   daqCheckStop = false;

   fTotalRecvPacket = 0;
   fTotalResubmPacket = 0;

   fFlashTimeout = .1;
   fLastDelivery = dabc::NullTimeStamp;

   fFormat = formatEth1;
}

roc::UdpDataSocket::~UdpDataSocket()
{
   DOUT3(("UdpDataSocket::~UdpDataSocket"));

   ResetDaq();

   if (fDev) fDev->fDataCh = 0;
   fDev = 0;
}

void roc::UdpDataSocket::ConfigureFor(dabc::Port* port)
{
   fQueue.Allocate(port->InputQueueCapacity());
   fBufferSize = port->GetCfgInt(dabc::xmlBufferSize, 16384);
   fTransferWindow = port->GetCfgInt(roc::xmlTransferWindow, fTransferWindow);
   fFlashTimeout = port->GetCfgDouble(roc::xmlFlashTimeout, fFlashTimeout);
   fPool = port->GetMemoryPool();
   fReadyBuffers = 0;

   fTgtCheckGap = false;

   // one cannot have too much resend requests
   fResend.Allocate(port->InputQueueCapacity() * fBufferSize / UDP_DATA_SIZE);

   DOUT2(("roc::UdpDataSocket:: Pool = %p buffer size = %u", fPool, fBufferSize));
}

void roc::UdpDataSocket::ProcessEvent(dabc::EventId evnt)
{
   switch (dabc::GetEventCode(evnt)) {
      case evntSocketRead: {
         void *tgt = fTgtPtr;
         if (tgt==0) tgt = fTempBuf;

         ssize_t len = DoRecvBufferHdr(&fTgtHdr, sizeof(UdpDataPacket),
                                        tgt, UDP_DATA_SIZE);
         if (len>0) {
            fTotalRecvPacket++;
//            DOUT0(("READ Packet %d len %d", ntohl(fTgtHdr.pktid), len));
            AddDataPacket(len, tgt);
         }

         break;
      }

      case evntStartTransport: {

         // no need to do anything when daq is already running
         if (daqActive()) {
            DOUT1(("Daq active - ignore start transport?"));
            return;
         }

         ResetDaq();

         daqState = daqStarting;

         fStartList.clear();

         fStartList.addPut(ROC_NX_FIFO_RESET, 1);
         fStartList.addPut(ROC_NX_FIFO_RESET, 0);
         fStartList.addPut(ROC_ETH_START_DAQ, 1);

         dabc::Command* cmd = new CmdNOper(&fStartList);

         fDevice->Submit(Assign(cmd));
         break;
      }

      case evntStopTransport: {
         // this is situation, when normal module stops its transports
         // here we do put(stop_daq) ourself and just waiting confirmation that command completed
         // there is no yet way so suspend daq from module

         if (!daqActive()) {
            DOUT1(("Daq is not active - no need to stop transport"));
            return;
         }

         daqState = daqStopping;

         fTgtBuf = 0;
         fTgtBufIndx = 0;
         fTgtShift = 0;
         fTgtPtr = 0;

         fLastDelivery = dabc::NullTimeStamp;

         dabc::Command* cmd = new CmdPut(ROC_ETH_STOP_DAQ, 1);

         fDevice->Submit(Assign(cmd));
         break;
      }

      case evntConfirmCmd: {
         if (dabc::GetEventArg(evnt) == 0) {
            ResetDaq();
            daqState = daqFails;
            ActivateTimeout(-1.);
         } else
         if (daqState == daqStarting) {
            daqState = daqRuns;
            fLastDelivery = TimeStamp();
            AddBuffersToQueue();
            ActivateTimeout(0.0001);
         } else
         if (daqState == daqStopping)  {
            DOUT3(("STOP DAQ command is ready - go in normal state"));
            ResetDaq();
         }
         break;
      }

      case evntFillBuffer:
         AddBuffersToQueue();
         break;

      default:
         dabc::SocketIOProcessor::ProcessEvent(evnt);
   }
}

#if DABC_VERSION_CODE <= DABC_VERSION(1,0,2)

bool roc::UdpDataSocket::_ProcessReply(dabc::Command* cmd)
{
   bool res = cmd->GetResult();

   DOUT3(("roc::UdpDataSocket::_ProcessReply cmd %s res = %s ", cmd->GetName(), DBOOL(res)));

   FireEvent(evntConfirmCmd, res ? 1 : 0);

   return false; // we do not need command object any longer
}

#else

bool roc::UdpDataSocket::ReplyCommand(dabc::Command* cmd)
{
   int res = cmd->GetResult();

   DOUT3(("roc::UdpDataSocket::ReplyCommand %s res = %s ", cmd->GetName(), DBOOL(res)));

   FireEvent(evntConfirmCmd, res==dabc::cmd_true ? 1 : 0);

   return true;
}


#endif

bool roc::UdpDataSocket::Recv(dabc::Buffer* &buf)
{
   buf = 0;
   {
      dabc::LockGuard lock(fQueueMutex);
      if (fReadyBuffers==0) return false;
      buf = fQueue.Pop();
      fReadyBuffers--;
   }
   FireEvent(evntFillBuffer);

   return buf!=0;
}

unsigned  roc::UdpDataSocket::RecvQueueSize() const
{
   dabc::LockGuard guard(fQueueMutex);

   return fReadyBuffers;
}

dabc::Buffer* roc::UdpDataSocket::RecvBuffer(unsigned indx) const
{
   dabc::LockGuard lock(fQueueMutex);

   if (indx>=fReadyBuffers) return 0;

   return fQueue.Item(indx);
}

bool roc::UdpDataSocket::ProcessPoolRequest()
{
   FireEvent(evntFillBuffer);
   return true;
}

void roc::UdpDataSocket::StartTransport()
{
   DOUT3(("Starting UDP transport "));

   FireEvent(evntStartTransport);
}

void roc::UdpDataSocket::StopTransport()
{
   DOUT3(("Stopping udp transport %ld", fTotalRecvPacket));

   FireEvent(evntStopTransport);
}

void roc::UdpDataSocket::AddBuffersToQueue(bool checkanyway)
{
   unsigned cnt = 0;

   {
      dabc::LockGuard lock(fQueueMutex);
      cnt = fQueue.Capacity() - fQueue.Size();
   }

   bool isanynew = false;

   while (cnt) {
      dabc::Buffer* buf = fPool->TakeBufferReq(this, fBufferSize);
      if (buf==0) break;

      fTransportBuffers++;
      if (fTgtBuf==0) {
         fTgtBuf = buf;
         fTgtShift = 0;
         fTgtPtr = (char*) buf->GetDataLocation();
      }

      isanynew = true;
      cnt--;
      dabc::LockGuard lock(fQueueMutex);
      buf->SetTypeId(roc::rbt_RawRocData + fFormat);
      fQueue.Push(buf);
   }

   if (isanynew || checkanyway) CheckNextRequest();
}

bool roc::UdpDataSocket::CheckNextRequest(bool check_retrans)
{
   UdpDataRequestFull req;
   double curr_tm = TimeStamp() * 1e-6;

   if (!daqActive()) return false;

   // send request each 0.2 sec,
   // if there is no replies on last request send it much faster - every 0.01 sec.
   bool dosend =
      fabs(curr_tm - lastRequestTm) > (lastRequestSeen ? 0.2 : 0.01);

   int can_send = 0;
   if (fTgtBuf) {
      can_send += (fBufferSize - fTgtShift) / UDP_DATA_SIZE;
      can_send += (fTransportBuffers - fTgtBufIndx - 1) * (fBufferSize / UDP_DATA_SIZE);
   }

   if (can_send > (int) fTransferWindow) can_send = fTransferWindow;

   if (fResend.Size() >= fTransferWindow) can_send = 0; else
   if (can_send + fResend.Size() > fTransferWindow)
      can_send = fTransferWindow - fResend.Size();

   req.frontpktid = fTgtNextId + can_send;

   // if newly calculated front id bigger than last
   if ((req.frontpktid - lastSendFrontId) < 0x80000000) {

     if ((req.frontpktid - lastSendFrontId) >= fTransferWindow / 3) dosend = true;

   } else
      req.frontpktid = lastSendFrontId;

   req.tailpktid = fTgtTailId;

   req.numresend = 0;

   if (can_send==0) dosend = false;

   if (!check_retrans && !dosend) return false;

   for (unsigned n=0; n<fResend.Size(); n++) {
      ResendInfo* entry = fResend.ItemPtr(n);

      if ((entry->numtry>0) && (fabs(curr_tm - entry->lasttm)) < 0.1) continue;

      entry->lasttm = curr_tm;
      entry->numtry++;
      if (entry->numtry < 8) {
         req.resend[req.numresend++] = entry->pktid;

         dosend = true;

         if (req.numresend >= sizeof(req.resend) / 4) {
            EOUT(("Number of resends more than one can pack in the retransmit packet"));
            break;
         }

      } else {
         EOUT(("Roc:%u Drop pkt %u\n", rocNumber, entry->pktid));

         fTgtCheckGap = true;

         memset(entry->ptr, 0, UDP_DATA_SIZE);

         roc::Message msg;

         msg.setRocNumber(rocNumber);
         msg.setMessageType(roc::MSG_SYS);
         msg.setSysMesType(roc::SYSMSG_PACKETLOST);
         msg.copyto(entry->ptr, fFormat);

         fResend.RemoveItem(n);
         n--;
      }

   }

   if (!dosend) return false;

   uint32_t pkt_size = sizeof(UdpDataRequest) + req.numresend * sizeof(uint32_t);

   // make request always 4 byte aligned
   while ((pkt_size < MAX_UDP_PAYLOAD) &&
          (pkt_size + UDP_PAYLOAD_OFFSET) % 4) pkt_size++;

   lastRequestTm = curr_tm;
   lastRequestSeen = false;
   lastSendFrontId = req.frontpktid;
   lastRequestId++;

//   DOUT0(("Send request id:%u  Range: 0x%04x - 0x%04x nresend:%d resend[0] = 0x%04x tgtbuf %p ptr %p tgtsize %u",
//         lastRequestId, req.tailpktid, req.frontpktid, req.numresend,
//         req.numresend > 0 ? req.resend[0] : 0, fTgtBuf, fTgtPtr, fTransportBuffers));

   req.password = htonl(ROC_PASSWORD);
   req.reqpktid = htonl(lastRequestId);
   req.frontpktid = htonl(req.frontpktid);
   req.tailpktid = htonl(req.tailpktid);
   for (uint32_t n=0; n < req.numresend; n++)
      req.resend[n] = htonl(req.resend[n]);
   req.numresend = htonl(req.numresend);

   DoSendBuffer(&req, pkt_size);

   fDev->fCtrlCh->SetLastSendTime();

   return true;
}

double roc::UdpDataSocket::ProcessTimeout(double)
{
   if (!daqActive()) return -1;

   if (fTgtBuf == 0)
      AddBuffersToQueue(true);
   else
      CheckNextRequest();

   // check if we should flush current buffer
   if (!dabc::IsNullTime(fLastDelivery) &&
       (dabc::TimeDistance(fLastDelivery, TimeStamp()) > fFlashTimeout)) {
          DOUT2(("Doing flush"));
          CheckReadyBuffers(true);
   }

//   DOUT0(("CALIBR tm1:%f tm2:%f %s", fLastCalibrAction, fLastCalibrStart, DBOOL(dabc::IsNullTime(fLastCalibrStart))));

   return 0.01;
}


void roc::UdpDataSocket::ResetDaq()
{
   daqCheckStop = false;
   daqState = daqInit;

   fTransportBuffers = 0;

   fTgtBuf = 0;
   fTgtBufIndx = 0;
   fTgtShift = 0;
   fTgtPtr = 0;

   fTgtNextId = 0;
   fTgtTailId = 0;
   fTgtCheckGap = false;

   lastRequestId = 0;
   lastSendFrontId = 0;
   lastRequestTm = 0.;
   lastRequestSeen = true;

   fResend.Reset();

   dabc::LockGuard lock(fQueueMutex);
   fQueue.Cleanup();
   fReadyBuffers = 0;

   fLastDelivery = dabc::NullTimeStamp;
}

void roc::UdpDataSocket::AddDataPacket(int len, void* tgt)
{
   uint32_t src_pktid = ntohl(fTgtHdr.pktid);

   if (tgt==0) {
      DOUT0(("Packet 0x%04x has no place buf %p bufindx %u queue %u ready %u", src_pktid, fTgtBuf, fTgtBufIndx, fQueue.Size(), fReadyBuffers));
      for (unsigned n=0;n < fResend.Size(); n++)
         DOUT0(("   Need resend 0x%04x retry %d", fResend.ItemPtr(n)->pktid, fResend.ItemPtr(n)->numtry));

      CheckNextRequest();

      return;
   }

   if (len <= (int) sizeof(UdpDataPacket)) {
      EOUT(("Too few data received %d", len));
      return;
   }

   if (ntohl(fTgtHdr.lastreqid) == lastRequestId) lastRequestSeen = true;

   int nummsgs = ntohl(fTgtHdr.nummsg);

   uint32_t gap = src_pktid - fTgtNextId;

   int data_len = nummsgs * 6;

//   if (gap!=0)
//      DOUT0(("Packet id:0x%04x Head:0x%04x NumMsgs:%d gap:%u", src_pktid, fTgtNextId, nummsgs, gap));

   bool packetaccepted = false;
   bool doflush = false;

   if ((fTgtPtr==tgt) && (gap < fBufferSize / UDP_DATA_SIZE * fTransportBuffers)) {

      if (gap>0) {
         // some packets are lost on the way, move pointer forward and
         // remember packets which should be resubmit
         void* src = fTgtPtr;

         while (fTgtNextId != src_pktid) {

            ResendInfo* info = fResend.PushEmpty();

//            DOUT0(("!!!! Lost packet 0x%04x", fTgtNextId));

            info->pktid = fTgtNextId;
            info->lasttm = 0.;
            info->numtry = 0;
            info->buf = fTgtBuf;
            info->bufindx = fTgtBufIndx;
            info->ptr = fTgtPtr;

            fTgtNextId++;
            fTgtShift += UDP_DATA_SIZE;
            fTgtPtr += UDP_DATA_SIZE;

            if (fTgtShift + UDP_DATA_SIZE > fTgtBuf->GetDataSize()) {
               fTgtBuf->SetDataSize(fTgtShift);
               fTgtBufIndx++;
               if (fTgtBufIndx >= fTransportBuffers) {
                  EOUT(("One get packet out of the available buffer spaces gap = %u indx %u numbufs %u !!!!", gap, fTgtBufIndx, fTransportBuffers));
                  return;
               }

               {
                  dabc::LockGuard lock(fQueueMutex);
                  fTgtBuf = fQueue.Item(fReadyBuffers + fTgtBufIndx);
               }

               fTgtPtr = (char*) fTgtBuf->GetDataLocation();
               fTgtShift = 0;
            }
         }

         // copy data which was received into the wrong place of the buffers
         memcpy(fTgtPtr, src, data_len);

//         DOUT1(("Copy pkt 0x%04x to buffer %p shift %u", src_pktid, fTgtBuf, fTgtShift));
      }

      // from here just normal situation when next packet is arrived

      if (fResend.Size()==0) fTgtTailId = fTgtNextId;

      fTgtNextId++;

      fTgtShift += data_len;
      fTgtPtr += data_len;

      if (fTgtBuf->GetDataSize() < fTgtShift + UDP_DATA_SIZE) {
         fTgtPtr = 0;
         fTgtBuf->SetDataSize(fTgtShift);
         fTgtShift = 0;
         fTgtBuf = 0;
         fTgtBufIndx++;
      }

      packetaccepted = true;

   } else {
      // this is retransmitted packet, may be received in temporary place
      for (unsigned n=0; n<fResend.Size(); n++) {
         ResendInfo* entry = fResend.ItemPtr(n);
         if (entry->pktid != src_pktid) continue;

         DOUT3(("Get retransmitted packet 0x%04x", src_pktid));

         fTotalResubmPacket++;

         memcpy(entry->ptr, tgt, data_len);
         if (data_len < (int) UDP_DATA_SIZE) {
            void* restptr = (char*) entry->ptr + data_len;
            memset(restptr, 0, UDP_DATA_SIZE - data_len);
            fTgtCheckGap = true;
         }

         fResend.RemoveItem(n);

         // if all packets retransmitted, one can allow to skip buffers on roc,
         // beside next packet, which is required
         if (fResend.Size()==0) fTgtTailId = fTgtNextId - 1;

         packetaccepted = true;

         break;
      }

   }

   if (!packetaccepted) {
      DOUT3(("ROC:%u Packet 0x%04x was not accepted - FLUSH???  ready = %u transport = %u tgtindx = %u buf %p", fDev->rocNumber(), src_pktid, fReadyBuffers, fTransportBuffers, fTgtBufIndx, fTgtBuf));
//      dabc::SetDebugLevel(1);
   }

   // check incoming data for stop/start messages
   if (packetaccepted && (data_len>0) && (tgt!=0) && daqCheckStop) {
//      DOUT0(("Search special kind of message !!!"));

      Iterator iter(fFormat);
      iter.assign(tgt, data_len);

      while (iter.next())
         if (iter.msg().isStopDaqMsg()) {
            DOUT2(("Find STOP_DAQ message"));
            doflush = true;
         }
   }

   CheckReadyBuffers(doflush);
}

void roc::UdpDataSocket::CompressBuffer(dabc::Buffer* buf)
{
   if (buf==0) return;

   Iterator iter(fFormat);
   if (!iter.assign(buf->GetDataLocation(), buf->GetDataSize())) return;

   uint8_t* tgt = (uint8_t*) buf->GetDataLocation();
   uint8_t* src = tgt;

   uint32_t rawsize = roc::Message::RawSize(fFormat);
   dabc::BufferSize_t tgtsize = 0;

   while (iter.next()) {
      if (iter.msg().isNopMsg()) {
         src += rawsize;
      } else {
         if (tgt!=src) memcpy(tgt, src, rawsize);
         src += rawsize;
         tgt += rawsize;
         tgtsize += rawsize;
      }
   }

   if (tgtsize==0)
      EOUT(("Zero size after compress !!!"));

   buf->SetDataSize(tgtsize);
}

void roc::UdpDataSocket::CheckReadyBuffers(bool doflush)
{
   if (doflush && (fTgtBuf!=0) && (fTgtShift>0) && (fResend.Size()==0)) {
      DOUT2(("Flash buffer when recv of size %u", fTgtShift));
      fTgtPtr = 0;
      fTgtBuf->SetDataSize(fTgtShift);
      fTgtShift = 0;
      fTgtBuf = 0;
      fTgtBufIndx++;
   }

   if (fTgtBufIndx>0) {
      unsigned minindx = fTgtBufIndx;

      for (unsigned n=0; n<fResend.Size(); n++) {
         unsigned indx = fResend.ItemPtr(n)->bufindx;
         if (indx < minindx) minindx = indx;
      }

//      DOUT0(("CheckReadyBuffers minindx = %u resend = %u", minindx, fResend.Size()));

      if (minindx>0) {

         fTransportBuffers -= minindx;
         fTgtBufIndx -= minindx;
         for (unsigned n=0; n<fResend.Size(); n++)
            fResend.ItemPtr(n)->bufindx -= minindx;

         {
            dabc::LockGuard lock(fQueueMutex);

            // check all buffers on gaps, if necessary
            if (fTgtCheckGap)
               for (unsigned n=0;n<minindx;n++)
                  CompressBuffer(fQueue.Item(fReadyBuffers + n));

            fReadyBuffers += minindx;
         }

         while (minindx--) FireInput();

         fLastDelivery = TimeStamp();
      }
   }

   if ((fTgtBuf==0) && (fTgtBufIndx<fTransportBuffers)) {
      dabc::LockGuard lock(fQueueMutex);
      fTgtBuf = fQueue.Item(fReadyBuffers + fTgtBufIndx);
      fTgtPtr = (char*) fTgtBuf->GetDataLocation();
      fTgtShift = 0;
      // one can disable checks once we have no data in queues at all
      if ((fTgtBufIndx==0) && (fResend.Size()==0)) {
//         if (fTgtCheckGap) DOUT0(("!!! DISABLE COMPRESS !!!"));
         fTgtCheckGap = false;
      }
   }

   if (fTgtBuf == 0)
      AddBuffersToQueue();
   else
      CheckNextRequest();
}

bool roc::UdpDataSocket::prepareForSuspend()
{
   if (!daqActive()) return false;
   daqCheckStop = true;
   return true;
}

int roc::UdpDataSocket::GetParameter(const char* name)
{
   if (strcmp(name, roc::xmlRocNumber)==0) return fDev->rocNumber();
   if (strcmp(name, roc::xmlMsgFormat)==0) return fFormat;
   if (strcmp(name, roc::xmlTransportKind)==0) return roc::kind_UDP;

   return dabc::Transport::GetParameter(name);
}
