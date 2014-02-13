// -----------------------------------------------------------------------------
// -----                                                                   -----
// -----                             CbmSourceLmd                          -----
// -----                    Created 13.12.2013 by V. Friese                 -----
// -----                                                                   -----
// -----------------------------------------------------------------------------

#include "CbmSourceLmd.h"
#include "CbmAuxDigi.h"
#include "CbmStsDigiLight.h"
#include "CbmMuchBeamTimeDigi.h"
#include "CbmDaqMap.h"
#include "CbmTbDaqBuffer.h"
#include "CbmTbEvent.h"
#include "much/CbmMuchAddress.h"
#include "fhodo/CbmFiberHodoAddress.h"
#include "fhodo/CbmFiberHodoDigi.h"

#include "FairLogger.h"
#include "FairRunOnline.h"
#include "FairEventHeader.h"

#include "TClonesArray.h"

#include <iostream>
#include <iomanip>

using std::cout;
using std::endl;
using std::map;

CbmSourceLmd::CbmSourceLmd()
  : FairSource(),
    fInputFileName(""),
    fReadInTimeStep(1e9),
    fPersistence(kTRUE),
    fEventTimeWindow(500.),
    fBuffer(CbmTbDaqBuffer::Instance()),
    fDaqMap(new CbmDaqMap()),
    fRocIter(NULL),
    fTimeStart(0),
    fTimeStop(0), 
    fCurrentEpochTime(0),
    fStartEpoch(0),
    fCurrentEpoch(),
    fCurrentBufferTime(0),
    fCurrentMessage(NULL),
    fTimeBufferFill(0),
    fTimeBufferOut(0.),
    fCurrentEvent(NULL),
    fCurrentDigi(NULL),
    fNofEvents(0),
    fNofMessages(0),
    fNofEpochs(0),
    fEvents(new TClonesArray("CbmTbEvent", 1)),
    fStsDigis(new TClonesArray("CbmStsDigiLight", 10)),
    fMuchDigis(new TClonesArray("CbmMuchBeamTimeDigi", 10)),
    fHodoDigis(new TClonesArray("CbmFiberHodoDigi", 10)),
    fAuxDigis(new TClonesArray("CbmAuxDigi", 10)),
    fNofMessType(),
    fNofMessRoc()
{
  // --- Initialise counters
  for (Int_t iType = 0; iType < 8; iType++) fNofMessType[iType] = 0;
  for (Int_t iRoc = 0; iRoc < 20; iRoc++)
  	for (Int_t iNx = 0; iNx < 5; iNx++ )
  		fNofMessRoc[iRoc][iNx] = 0;
}

CbmSourceLmd::CbmSourceLmd(const char * inFile)
  : FairSource(),
    fInputFileName(inFile),
    fReadInTimeStep(1e9),
    fPersistence(kTRUE),
    fEventTimeWindow(500.),
    fBuffer(CbmTbDaqBuffer::Instance()),
    fDaqMap(new CbmDaqMap()),
    fRocIter(NULL),
    fTimeStart(0),
    fTimeStop(0), 
    fCurrentEpochTime(0),
    fStartEpoch(0),
    fCurrentEpoch(),
    fCurrentBufferTime(0),
    fCurrentMessage(NULL),
    fTimeBufferFill(0),
    fTimeBufferOut(0.),
    fCurrentEvent(NULL),
    fCurrentDigi(NULL),
    fNofEvents(0),
    fNofMessages(0),
    fNofEpochs(0),
    fEvents(new TClonesArray("CbmTbEvent", 1)),
    fStsDigis(new TClonesArray("CbmStsDigiLight", 10)),
    fMuchDigis(new TClonesArray("CbmMuchBeamTimeDigi", 10)),
    fHodoDigis(new TClonesArray("CbmFiberHodoDigi", 10)),
    fAuxDigis(new TClonesArray("CbmAuxDigi", 10)),
    fNofMessType(),
    fNofMessRoc()
{
  // --- Initialise counters
  for (Int_t iType = 0; iType < 8; iType++) fNofMessType[iType] = 0;
  for (Int_t iRoc = 0; iRoc < 20; iRoc++)
  	for (Int_t iNx = 0; iNx < 5; iNx++ )
  		fNofMessRoc[iRoc][iNx] = 0;
}


CbmSourceLmd::CbmSourceLmd(const CbmSourceLmd& source)
  : FairSource(source)
{
}


CbmSourceLmd::~CbmSourceLmd()
{
  delete fDaqMap;
}


// -----   Fill the buffer up to a given time   -----------------------------
Bool_t CbmSourceLmd::FillBuffer(ULong_t time) {

	// --- Signal end of file if there are no more messages
	if ( ! fCurrentMessage ) {
		LOG(INFO) << GetName() << ": End of input file reached."
				      << FairLogger::endl;
		return kFALSE;
	}

	Int_t nMessages = 0;
	while ( fCurrentMessage ) {
		nMessages++;

		// --- Message type counters
		Int_t msgType = fCurrentMessage->getMessageType();
		if ( msgType < 0 || msgType > 7 ) {
			LOG(ERROR) << GetName() << ": Skipping mesage with unknwon type "
					       << msgType << FairLogger::endl;
			continue;
		}
		fNofMessType[msgType]++;

		// --- Treat epoch markers
		if ( fCurrentMessage->isEpochMsg() ) ProcessEpochMarker();


		// --- Treat AUX messages
		if ( fCurrentMessage->isAuxMsg() ) {

			// --- ROC Id and channel number
			Int_t rocId = fCurrentMessage->getRocNumber();
			Int_t channel = fCurrentMessage->getAuxChNum();
			fNofAux++;

			// --- Check for epoch marker for this ROC
			if ( fCurrentEpoch.find(rocId) == fCurrentEpoch.end() )
				LOG(FATAL) << GetName()
	    			       << ": Hit message without previous epoch marker for ROC "
	    			    	 << rocId << FairLogger::endl;

			// --- Get absolute time
			ULong_t auxTime = fCurrentMessage->getMsgFullTime(fCurrentEpoch[rocId]);

			// --- Jump out of loop of hit time is after time limit
			if (auxTime > time) break;

			// --- Create AuxDigi and send it to the buffer
			CbmAuxDigi* digi = new CbmAuxDigi(rocId, channel, auxTime);
			fBuffer->InsertData(digi);

		} //? AUX message


		// --- Treat hit messages
		if ( fCurrentMessage->isHitMsg() ) {

			// --- ROC and NXYTER number; increase counter
			Int_t rocId = fCurrentMessage->getRocNumber();
			Int_t nxyId = fCurrentMessage->getNxNumber();
			fNofMessRoc[rocId][nxyId]++;

			// --- Check for epoch marker for this ROC
			if ( fCurrentEpoch.find(rocId) == fCurrentEpoch.end() )
				LOG(FATAL) << GetName()
	    			       << ": Hit message without previous epoch marker for ROC "
	    			    	 << rocId << FairLogger::endl;

			// --- Get absolute time
			ULong_t hitTime = fCurrentMessage->getMsgFullTime(fCurrentEpoch[rocId]);

			// --- Jump out of loop of hit time is after time limit
			if (hitTime > time) break;

			// --- Process hit message (detector dependent)
			Int_t systemId = fDaqMap->GetSystemId(rocId);
			switch (systemId) {
				case kSTS:   ProcessStsMessage();  break;
				case kMUCH:  ProcessMuchMessage(); break;
				case kFHODO: ProcessHodoMessage(); break;
				default: break;
			}

	   }  //? Hit message

	  // --- Get next ROC message
		if ( fRocIter->next() ) fCurrentMessage = &fRocIter->msg();
		else fCurrentMessage = NULL;

	} //- message loop


	// --- Update buffer fill time
	fTimeBufferFill  = time;

	// --- Set buffer retrieval time to one time step before fill time.
	fTimeBufferOut  = Double_t(time - fReadInTimeStep);

	// --- End of input: retrieval time equals fill time
	if ( ! fCurrentMessage ) {
		LOG(INFO) << GetName() << ": End of input reached." << FairLogger::endl;
		fTimeBufferOut = Double_t(time);
	}

	// --- Status info
	LOG(INFO) << GetName() << ": Buffer fill time " << fixed
			      << setprecision(3) << Double_t(fTimeBufferFill) * 1.e-9
			      << " s, retrieval up to " << fTimeBufferOut * 1.e-9
			      << " s, " << nMessages << " messages processed"
			      << FairLogger::endl;
	fBuffer->PrintStatus();
	fNofMessages += nMessages;

	return kTRUE;
}
// --------------------------------------------------------------------------



// -----   Get next data   --------------------------------------------------
CbmDigi* CbmSourceLmd::GetNextData() {

	// --- Retrieve next data from the buffer.
	CbmDigi* digi = fBuffer->GetNextData(fTimeBufferOut);

	// --- If no data: fill the buffer with next time step
	// --- N.b.: fCurrentMessage = NULL means no more data in input and
	// ---       retrieval of buffer up to the fill time.
	while ( ( ! digi )  &&  fCurrentMessage ) {
		FillBuffer(fTimeBufferFill + fReadInTimeStep);
		digi = fBuffer->GetNextData(fTimeBufferOut);
	}

	// --- If the digi pointer is NULL; the input is exhausted and the buffer
	// --- is empty.
	return digi;
}
// --------------------------------------------------------------------------



// -----   Initialisation   -------------------------------------------------
Bool_t CbmSourceLmd::Init()
{

  cout << endl;
  LOG(INFO) << "====================================================="
  		      << FairLogger::endl;
  LOG(INFO) << GetName() << ": Initialising ..." << FairLogger::endl;

  // --- Register output branches
  FairRootManager* ioman = FairRootManager::Instance();
  ioman->Register("TBEvent", "Testbeam event", fEvents, fPersistence);
  ioman->Register("StsDigi", "STS raw data", fStsDigis, fPersistence);
  ioman->Register("MuchDigi", "MUCH raw data", fMuchDigis, fPersistence);
  ioman->Register("HodoDigi", "HODO raw data", fHodoDigis, fPersistence);
  ioman->Register("AuxDigi", "AUX data", fAuxDigis, fPersistence);

  // --- Open input file and get first message
  LOG(INFO) << GetName() << ": Opening file " << fInputFileName
  		      << FairLogger::endl;
  fRocIter = new roc::Iterator(fInputFileName.Data());
  if ( ! fRocIter->next() ) {
    LOG(ERROR) << GetName() << "::Init: input file does not exist "
	             << "or is empty!" << FairLogger::endl;
    return kFALSE;
  }
  fCurrentMessage = &fRocIter->msg();
  if ( fCurrentMessage->getMessageType() != roc::MSG_EPOCH ) {
  	LOG(ERROR) << GetName() << ": First message is not epoch marker!"
  			       << FairLogger::endl;
  	return kFALSE;
  }

  // --- Get start time and set initial buffer fill time
  Int_t epoch  = fCurrentMessage->getEpochNumber();
  ULong_t time = fCurrentMessage->getMsgFullTime(epoch);
  LOG(INFO) << GetName() << ": First message: epoch " << epoch << "  time "
  		      << setprecision(9) << Double_t(time) * 1.e-9 << " s"
  		      << FairLogger::endl;

  // --- Set initial buffer fill time (rounded to next read-in time step)
  fTimeBufferFill = ULong_t( Double_t(time) / Double_t(fReadInTimeStep) )
  		            * fReadInTimeStep;
  fTimeBufferOut  = fTimeBufferFill - fReadInTimeStep;

  // --- Read the first digi from the buffer
  fCurrentDigi = GetNextData();
  if ( ! fCurrentDigi ) {
  	LOG(ERROR) << GetName() << ": No hit data in input!" << FairLogger::endl;
  	return kFALSE;
  }

  LOG(INFO) << GetName() << ": Initialisation done. " << FairLogger::endl;
  LOG(INFO) << "====================================================="
  		      << FairLogger::endl;
  cout << endl;

  return kTRUE;
}
// --------------------------------------------------------------------------



// -----   Event loop   -----------------------------------------------------
// Action: Digis are retrieved from the buffer one by one.
// If their time is within the association window, they are added to the
// event. If not, the event loop is stopped.
Int_t CbmSourceLmd::ReadEvent()
{

  // --- Clear output arrays
  fEvents->Clear();
  fStsDigis->Clear();
  fHodoDigis->Clear();
  fMuchDigis->Clear();
  fAuxDigis->Clear();

  // --- Create new, empty event
  fCurrentEvent = new((*fEvents)[0]) CbmTbEvent();

  // Loop over digis
  while ( kTRUE ) {

    // --- If event is empty or time of digi matches to event, add digi to event.
    Double_t tDif = TMath::Abs(fCurrentDigi->GetTime() - fCurrentEvent->GetTime() );
    if ( fCurrentEvent->IsEmpty() || tDif < fEventTimeWindow ) {

      // --- Update event object
      fCurrentEvent->AddDigi(fCurrentDigi);

      // --- Copy digi to output array
      Int_t systemId = fCurrentDigi->GetSystemId();
      if ( systemId == kSTS ) {
        new( (*fStsDigis)[fStsDigis->GetEntriesFast()])
          CbmStsDigiLight(*(dynamic_cast<CbmStsDigiLight*>(fCurrentDigi)));
        fNofDigis[kSTS]++;
        LOG(DEBUG) << "STS digis " << fNofDigis[kMUCH] << FairLogger::endl;
      } //? STS digi
      else if ( systemId == kMUCH ) {
        new( (*fMuchDigis)[fMuchDigis->GetEntriesFast()])
          CbmMuchBeamTimeDigi(*(dynamic_cast<CbmMuchBeamTimeDigi*>(fCurrentDigi)));
        fNofDigis[kMUCH]++;
        LOG(DEBUG) << "MUCH digis " << fNofDigis[kMUCH] << FairLogger::endl;
      } //? MUCH digi
      else if ( systemId == kFHODO ) {
        new( (*fHodoDigis)[fHodoDigis->GetEntriesFast()])
          CbmFiberHodoDigi(*(dynamic_cast<CbmFiberHodoDigi*>(fCurrentDigi)));
        fNofDigis[kFHODO]++;
        LOG(DEBUG) << "HODO digis " << fNofDigis[kMUCH] << FairLogger::endl;
      } //? HODO digi
      else if ( systemId == 999) { // I know I should not hardcode numbers....
      	new( (*fAuxDigis)[fAuxDigis->GetEntriesFast()])
      	  CbmAuxDigi(*(dynamic_cast<CbmAuxDigi*>(fCurrentDigi)));

      }

      // --- Delete current digi from memory and get next one
      delete fCurrentDigi;
      fCurrentDigi = GetNextData();
      if ( ! fCurrentDigi ) {
        LOG(INFO) << "No more input data. End the run." << FairLogger::endl;
        return 1;  // no more data; trigger end of run
      } 
    }   //? Digi belongs to event?

    // --- If digi does not belong to current event: stop event loop.
    else {
      if ( FairLogger::GetLogger()->IsLogNeeded(DEBUG1) ) fCurrentEvent->Print();
      // Get Event Header to set the time
      FairRunOnline* ana = FairRunOnline::Instance();
      FairEventHeader* evtHead = ana->GetEventHeader();
      evtHead->SetEventTime(fCurrentEvent->GetTime());
      fNofEvents++;
      if (FairLogger::GetLogger()->IsLogNeeded(DEBUG)) fCurrentEvent->Print();
      return 0;
    }

  } //- event loop over digis

  LOG(INFO) << "===== Leaving Exec. " << FairLogger::endl;

  return 0;
}
// --------------------------------------------------------------------------




// -----   Process an epoch marker   ----------------------------------------
void CbmSourceLmd::ProcessEpochMarker() {

  Int_t rocId          = fCurrentMessage->getRocNumber();
  fCurrentEpoch[rocId] = fCurrentMessage->getEpochNumber();
  if ( rocId ) return;  // Further action only for ROC Id 0

  fCurrentEpochTime = fCurrentMessage->getMsgFullTime(fCurrentEpoch[rocId]);
  fNofEpochs++;
  LOG(DEBUG) << GetName() << ": Epoch message "
  		       << fNofEpochs << ", epoch " << Int_t(fCurrentEpoch[rocId])
  		       << ", time " << std::setprecision(9) << std::fixed
  		       << Double_t(fCurrentEpochTime) * 1.e-9 << " s"
  		       << FairLogger::endl;

  // --- Start and stop time
  if ( fTimeStart == 0  || fTimeStart > fCurrentEpochTime )
  		fTimeStart = fCurrentEpochTime;
  if ( fTimeStop < fCurrentEpochTime ) fTimeStop = fCurrentEpochTime;

}
// --------------------------------------------------------------------------



// -----   Process a HODO hit message   -------------------------------------
void CbmSourceLmd::ProcessHodoMessage()
{

	// --- Increment message counter
	fNofHitMsg[kFHODO]++;

  // --- Get absolute time, NXYTER and channel number
  Int_t rocId      = fCurrentMessage->getRocNumber();
  ULong_t hitTime  = fCurrentMessage->getMsgFullTime(fCurrentEpoch[rocId]);
  Int_t nxyterId   = fCurrentMessage->getNxNumber();
  Int_t nxChannel  = fCurrentMessage->getNxChNum();
  Int_t charge     = fCurrentMessage->getNxAdcValue();

  Int_t iStation;
  Int_t iSector; 
  Int_t iPlane;
  Int_t iFiber;

  fDaqMap->Map(rocId, nxyterId, nxChannel, iStation, iSector, iPlane, iFiber);
  Int_t address = CbmFiberHodoAddress::GetAddress(iStation, iPlane, iFiber);

	// --- Create a HODO digi and send it to the buffer
	CbmFiberHodoDigi* digi = new CbmFiberHodoDigi(address, charge, hitTime);
	fBuffer->InsertData(digi);

}
// --------------------------------------------------------------------------



// -----   Process a MUCH hit message   -------------------------------------
void CbmSourceLmd::ProcessMuchMessage() {

	// --- Increment message counter
	fNofHitMsg[kMUCH]++;

	// --- Get absolute time, NXYTER and channel number
	Int_t rocId      = fCurrentMessage->getRocNumber();
	ULong_t hitTime  = fCurrentMessage->getMsgFullTime(fCurrentEpoch[rocId]);
	Int_t nxyterId   = fCurrentMessage->getNxNumber();
	Int_t nxChannel  = fCurrentMessage->getNxChNum();
	Int_t charge     = fCurrentMessage->getNxAdcValue();

	LOG(DEBUG2) << "MUCH mssage at " << hitTime << FairLogger::endl;

	// --- Get detector element from DaqMap
	Int_t station = fDaqMap->GetMuchStation(rocId);
	Int_t layer   = 0;
	Int_t side    = 0;
	Int_t module  = 0;
	Int_t sector  = nxyterId;
	Int_t channel = nxChannel;

	// --- Construct unique address
	UInt_t address = CbmMuchAddress::GetAddress(station, layer, side,
			                                        module, sector, channel);

	// --- Create digi
	CbmMuchBeamTimeDigi* digi = new CbmMuchBeamTimeDigi(address, charge, hitTime);
	LOG(DEBUG2) << "MUCH digi at " << digi->GetTime() << FairLogger::endl;
	fBuffer->InsertData(digi);

}
// --------------------------------------------------------------------------



// -----   Process a STS hit message   --------------------------------------
void CbmSourceLmd::ProcessStsMessage() {

	// --- Increment message counter
	fNofHitMsg[kSTS]++;

	// --- Get absolute time, NXYTER and channel number
	Int_t rocId      = fCurrentMessage->getRocNumber();
	ULong_t hitTime  = fCurrentMessage->getMsgFullTime(fCurrentEpoch[rocId]);
	Int_t nxyterId   = fCurrentMessage->getNxNumber();
	Int_t nxChannel  = fCurrentMessage->getNxChNum();
	Int_t charge     = fCurrentMessage->getNxAdcValue();

	// --- Get detector element from DaqMap
	Int_t station = fDaqMap->GetStsStation(rocId);
	Int_t sector  = 0;
	Int_t side    = fDaqMap->GetStsSensorSide(rocId);
	Int_t channel = fDaqMap->GetStsChannel(rocId, nxyterId, nxChannel);

	// --- Create a STS digi and send it to the buffer
	CbmStsDigiLight* digi = new CbmStsDigiLight(station, sector, side, channel,
			                                  charge, hitTime);
	fBuffer->InsertData(digi);

}
// --------------------------------------------------------------------------


void CbmSourceLmd::Close()
{

  cout << endl;
  LOG(INFO) << "====================================================="
  		      << FairLogger::endl;
  fBuffer->PrintStatus();
  LOG(INFO) << GetName() << ": Run Summary" << FairLogger::endl;

  cout << endl;
  LOG(INFO) << "Messages in input: " << fNofMessages << FairLogger::endl;
  Double_t deltaT = (fTimeStop - fTimeStart) / 1.e9;
  LOG(INFO) << "Start time : " << fTimeStart * 1.e-9 << " s  ";
  LOG(INFO) << "Stop  time : " << fTimeStop  * 1.e-9 << " s  ";
  LOG(INFO) << "Duration   : " << deltaT  << " s" << FairLogger::endl;

  const char* names[8] = { "NOP", "HIT", "EPOCH", "SYNC", "AUX", "EPOCH2", "GET4", "SYS" };
  cout << endl;
  LOG(INFO) << "Messages per message type: " << FairLogger::endl;
  for (Int_t i=0; i<8; i++)
  	LOG(INFO) << setw(6) << names[i] << "  "
  	          << setw(10) << right << fNofMessType[i] << FairLogger::endl;

  cout << endl;
  LOG(INFO) << "Hit messages per ROC/NXYTER: " << FairLogger::endl;
  for (Int_t iRoc = 0; iRoc < 13; iRoc ++)
  	for (Int_t iNx = 0; iNx < 3; iNx +=2 ) {
  		LOG(INFO) << " ROC " << setw(2) << iRoc << "  NXYTER " << iNx << "  Messages "
  			      << setw(12) << right << fNofMessRoc[iRoc][iNx] << "  Rate "
  			      << setw(12) << setprecision(4) << fixed  << right
  			      << Double_t(fNofMessRoc[iRoc][iNx]) / deltaT
  			      << " /s " << FairLogger::endl;
  }

  cout << endl;
  LOG(INFO) << "Hit messages and digis per system: " << FairLogger::endl;
  for (Int_t iSys = 0; iSys < kNOFDETS; iSys++) {
  	TString sysName;
  	CbmDetectorList::GetSystemNameCaps(iSys, sysName);
  	LOG(INFO) << setw(5) << sysName << ": Messages " << fNofHitMsg[iSys]
  	          << ", Digis " << fNofDigis[iSys] << FairLogger::endl;
  }
  LOG(INFO) << "AUX  : Messages " << fNofMessType[roc::MSG_AUX] << ", Digis "
  		      << fNofAux << FairLogger::endl;


  delete fRocIter;
}

void CbmSourceLmd::Reset()
{
}

ClassImp(CbmSourceLmd)
