/**@file CbmTbEvent.cxx
 **@date 26.06.2009
 **@author V. Friese <v.friese@gsi.de>
 **/


#include "CbmTbEvent.h"

#include <iostream>
#include <iomanip>

#include "CbmDigi.h"
#include "much/CbmMuchDigi.h"
#include "sts/CbmStsDigiLight.h"


using namespace std;


// -----   Constructor  ------------------------------------------------------ 
CbmTbEvent::CbmTbEvent()
  : TObject(),
  fTime(0.),               
  fTimeStart(0.),          
  fTimeStop(0.),           
  fEpochTime(0.),          
  fNofData(0),            
  fTimeBc(),
  fNofDigis(),
  fNofAux(0),
  fTimeSum(0.)
{
  Clear();
}
// ---------------------------------------------------------------------------


// -----   Add digi to event   ----------------------------------------------
void CbmTbEvent::AddDigi(CbmDigi* digi) {

	// --- Set event start and stop times for empty event.
	if ( IsEmpty() ) {
		fTimeStart = digi->GetTime();
		fTimeStop  = digi->GetTime();
	}

	// --- For non-empty events: check time ordering
	else {
		if ( digi->GetTime() < fTimeStop ) {
			LOG(FATAL) << GetName() << ": time disorder. Event stop is "
			           << fTimeStop << ", digi time is " << digi->GetTime()
			           << FairLogger::endl;
			return;
		}
	}

  // --- Set event stop time
	fTimeStop = digi->GetTime();

	// --- Calculate average event time
	fNofData++;
	fTimeSum += digi->GetTime();
	fTime = fTimeSum / Double_t(fNofData);

	// --- Increase digi counter
	if ( digi->GetSystemId() == 999 ) fNofAux++;
	else fNofDigis[digi->GetSystemId()]++;

}
// --------------------------------------------------------------------------








// -----   Reset event   -----------------------------------------------------
void CbmTbEvent::Clear() {
  fTime          = 0.;
  fTimeStart     = 0.;
  fTimeStop      = 0.;
  fEpochTime     = 0.;
  fNofData       = 0;
  fNofAux        = 0;
  for (Int_t iSys = 0; iSys < kNOFDETS; iSys++) fNofDigis[iSys] = 0;
  fTimeBc.clear();
  fTimeSum       = 0.;
}
// ---------------------------------------------------------------------------



// -----   Print   -----------------------------------------------------------
void CbmTbEvent::Print(Option_t* opt) { 
  cout.setf(ios::fixed, ios::floatfield);
  cout << "-I- Cbm Testbeam Event: " << setprecision(9) << fixed << "Event time " 
       << fTime << " s, duration " << setprecision(0) << GetDuration()
       << " ns, Beam " << fTimeBc.size();
  for (Int_t iSys = 0; iSys < kNOFDETS; iSys++) {
  	if ( fNofDigis[iSys] ) {
  		TString systemName;
  		CbmDetectorList::GetSystemNameCaps(iSys, systemName);
  		cout << ", " << systemName << " " << fNofDigis[iSys];
  	}
  }
  cout << ", AUX " << fNofAux << endl;
}
// ---------------------------------------------------------------------------


ClassImp(CbmTbEvent)







