/**@file CbmDaqMap.cxx
 **@date 11.12.2013
 **@author V. Friese <v.friese@gsi.de>
 **/


#include "FairLogger.h"
#include "CbmDaqMap.h"
#include "CbmDetectorList.h"

#include <iostream>


using std::cout;
using std::endl;



// -----   Default Constructor  ---------------------------------------------- 
CbmDaqMap::CbmDaqMap() 
  : TObject(),
    fRun(-1),
    fFiberHodoFiber(),
    fFiberHodoPlane(),
    fFiberHodoPixel()
{
  InitializeFiberHodoMapping();
}
// ---------------------------------------------------------------------------



// -----   Standard Constructor  --------------------------------------------- 
CbmDaqMap::CbmDaqMap(Int_t iRun) 
  : TObject(),
    fRun(iRun),
    fFiberHodoFiber(),
    fFiberHodoPlane(),
    fFiberHodoPixel()
{
  InitializeFiberHodoMapping();
}
// ---------------------------------------------------------------------------



// -----   Destructor   ------------------------------------------------------
CbmDaqMap::~CbmDaqMap() { }
// ---------------------------------------------------------------------------


// -----   Get System   ------------------------------------------------------
Int_t CbmDaqMap::GetSystemId(Int_t rocId) {
	Int_t systemId = -1;
	if ( rocId >= 0  && rocId <= 4 ) systemId = kMUCH;
	else if ( rocId == 5  || rocId == 6 ) systemId = kFHODO;
	else if ( rocId >= 7  || rocId <= 12 ) systemId = kSTS;
	else LOG(WARNING) << GetName() << ": Unknown ROC id " << rocId
			              << FairLogger::endl;
	return systemId;
}
// ---------------------------------------------------------------------------


// -----   Get STS station number   ------------------------------------------
Int_t CbmDaqMap::GetStsStation(Int_t rocId) {
	if ( rocId < 7 || rocId > 12 ) {
		LOG(ERROR) << GetName() << ": Illegal STS ROC Id " << rocId
				       << FairLogger::endl;
		return -1;
	}
	return ( (rocId-0.1) / 2 - 3 );
}
// ---------------------------------------------------------------------------


// -----   Get MUCH station number  ------------------------------------------
Int_t CbmDaqMap::GetMuchStation(Int_t rocId) {
	Int_t station = -1;
	if ( rocId == 0 || rocId == 1 ) station = 0;
	else if ( rocId == 2 ) station = 1;
	else if ( rocId == 3 || rocId == 4 ) station = 2;
	else LOG(ERROR) << GetName() << ": Illegal MUCH ROC Id " << rocId
			            << FairLogger::endl;
	return station;
}
// ---------------------------------------------------------------------------

// -----   Get Fiber Hodoscope station number   ------------------------------
Int_t CbmDaqMap::GetFiberHodoStation(Int_t rocId) {
	if ( rocId < 5 || rocId > 6 ) {
		LOG(ERROR) << GetName() << ": Illegal Fiber Hodoscope ROC Id " << rocId
				       << FairLogger::endl;
		return -1;
	}
	return ( rocId - 5 );
}
// ---------------------------------------------------------------------------



// ----  Get STS sensor side   -----------------------------------------------
Int_t CbmDaqMap::GetStsSensorSide(Int_t rocId) {
	return ( TMath::Even(rocId) ? 0 : 1 );
}
// ---------------------------------------------------------------------------



// -----   Get STS channel number   ------------------------------------------
Int_t CbmDaqMap::GetStsChannel(Int_t rocId, Int_t nxId, Int_t nxChannel) {
	Int_t channel = -1;
	if ( rocId >= 7 && rocId <= 10 ) channel = nxChannel + nxId / 2 * 127;
	else channel = nxChannel;
	return channel;
}
// ---------------------------------------------------------------------------



// -----   Get STS channel number   ------------------------------------------
Int_t CbmDaqMap::GetFiberHodoChannel(Int_t rocId, Int_t nxId, Int_t nxChannel) {
	Int_t channel = -1;
	return channel;
}
// ---------------------------------------------------------------------------



// -----   Mapping   ---------------------------------------------------------
Bool_t CbmDaqMap::Map(Int_t iRoc, Int_t iNx, Int_t iId,
			Int_t& iStation, Int_t& iSector, 
			Int_t& iSide, Int_t& iChannel) {

	// --- Valid ROC Id
	if ( iRoc < 0 || iRoc > 12 )
		LOG(FATAL) << GetName() << ": UInvalid ROC Id " << iRoc
		           << FairLogger::endl;


	// --- ROC 0 to 4 are MUCH
	// --- Not implemented yet
	if ( iRoc <= 4 ) {
		iStation = -1;
		iSector  = 0;
		iSide    = 0;

		return kTRUE;
	}


	// --- ROC 5 to 6: Hodoscope
	else if ( iRoc <= 6 ) {
          if (iNx == 0) {
	    iStation = iRoc -5;
	    iSector = 0;
	    iSide = fFiberHodoPlane[iId];
	    iChannel = fFiberHodoFiber[iId];
	    return kTRUE;
	  } else {
	    LOG(FATAL) << GetName() << ": Unknown Nxyter Id " << iNx
		       << " for Roc Id " << iRoc << FairLogger::endl;
	  }
	}

	// --- ROC 7 to 12: STS
	else {

		// --- STS station 0 (reference)
		if ( iRoc == 7 || iRoc == 8 ) {
			iStation = 0;
			iSector  = 0;
			iSide    = 8 - iRoc;
			iChannel = iId + iNx / 2 * 127;
		}

		// --- STS station 1 (reference)
		else if ( iRoc == 9 || iRoc == 10 ) {
			iStation = 1;
			iSector  = 0;
			iSide    = 10 - iRoc;
			iChannel = iId + iNx / 2 * 127;
		}

		// --- STS station 2 (OUT)
		else if ( iRoc == 11 || iRoc == 12 ) {
			iStation = 2;
			iSector  = 0;
			iSide    = 12 - iRoc;
			iChannel = iId;
		}

		else LOG(FATAL) << GetName() << ": Unknown ROC Id " << iRoc
				            << FairLogger::endl;

	}  // --- ROC 7 to 12


  return kTRUE;
}
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
void CbmDaqMap::InitializeFiberHodoMapping()
{
  // This code was copied from the Go4 analysis used for previous beamtimes
  for (Int_t i=0; i<128; i++) {
    fFiberHodoFiber[i] = -1;
    fFiberHodoPlane[i] = -1;
    fFiberHodoPixel[i] = -1;
  }
  
  for (Int_t ifiber=1; ifiber<=64; ifiber++) {
    // Calculate fiber number [1..64] from feb channel
    // lcn: linearconnectornumber, is the wire number on one of the
    // flat cables. [1..16]
    // each 16 fibers go to one connector.
    // fibersubnr[0..15] linear fiber counter in groups of 16

    Int_t fibersubnr=(ifiber-1)%16;

    Int_t lcn=15-fibersubnr*2;
    if (fibersubnr>=8) lcn=(fibersubnr-7)*2;
    
    Int_t channel=-1;
    Int_t cable=(ifiber-1)/16+1;
    Int_t pixel= ((lcn-1)/2)*8 +((lcn-1)%2);
    if (cable==1) {
      channel=(lcn-1)*4+0;
      pixel=pixel+1;
    }
    if (cable==2) {
      channel=(lcn-1)*4+2;
      pixel=pixel+3;
    }
    if (cable==3) {
      channel=(lcn-1)*4+1;
      pixel=pixel+5;
    }
    if (cable==4) {
      channel=(lcn-1)*4+3;
      pixel=pixel+7;
    }
    
    // new code to resolve cabling problem during cern-oct12
    int ifiber_bis = ifiber;
    if (ifiber <= 8 )  ifiber_bis = ifiber + 56; else
      if (ifiber <= 16 ) ifiber_bis = ifiber + 40; else
	if (ifiber <= 24 ) ifiber_bis = ifiber + 24; else
	  if (ifiber <= 32 ) ifiber_bis = ifiber + 8; else 
	    if (ifiber <= 40 ) ifiber_bis = ifiber - 8; else 
	      if (ifiber <= 48 ) ifiber_bis = ifiber - 24; else
		if (ifiber <= 56 ) ifiber_bis = ifiber - 40; else
		  if (ifiber <= 64 ) ifiber_bis = ifiber - 56;
    
    // and swap at the end
    ifiber_bis = 65 - ifiber_bis;

    fFiberHodoFiber[channel] = ifiber_bis - 1;
    fFiberHodoPlane[channel] = 0;
    fFiberHodoPixel[channel] = pixel;

    fFiberHodoFiber[channel+64] = ifiber_bis - 1;
    fFiberHodoPlane[channel+64] = 1;
    fFiberHodoPixel[channel+64] = pixel;

  }

  for (Int_t i=0; i<128; i++) {
    LOG(DEBUG) << "Channel[" << i << "]: " << fFiberHodoFiber[i] << ", " 
              << fFiberHodoPlane[i] << ", " << fFiberHodoPixel[i] 
              << FairLogger::endl;
  }

}

ClassImp(CbmDaqMap)

