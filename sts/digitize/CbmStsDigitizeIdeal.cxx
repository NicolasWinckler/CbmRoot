/** @file CbmStsDigitizeIdeal.cxx
 ** @author Volker Friese <v.friese@gsi.de>
 ** @date 23.05.2014
 **/

// Include class header
#include "CbmStsDigitizeIdeal.h"

// Includes from C++
#include <iomanip>
#include <iostream>

// Includes from ROOT
#include "TClonesArray.h"
#include "TGeoBBox.h"
#include "TGeoPhysicalNode.h"
#include "TGeoVolume.h"

// Includes from FairRoot
#include "FairEventHeader.h"
#include "FairLogger.h"
#include "FairRunAna.h"

// Includes from CbmRoot
#include "CbmStsDigi.h"
#include "CbmStsPoint.h"

// Include from STS
#include "digitize/CbmStsSensorTypeDssdIdeal.h"
#include "setup/CbmStsModule.h"
#include "setup/CbmStsSetup.h"
#include "setup/CbmStsSensorTypeDssd.h"



// -----   Standard constructor   ------------------------------------------
CbmStsDigitizeIdeal::CbmStsDigitizeIdeal(Int_t digiModel)
  : FairTask("StsDigitize"),
  fDigiModel(digiModel),
  fPoints(NULL),
  fDigis(NULL),
  fNofPoints(0),
  fNofSignalsF(0),
  fNofSignalsB(0),
  fNofDigis(0),
  fNofEvents(0),
  fNofPointsTot(0),
  fNofSignalsFTot(0),
  fNofSignalsBTot(0),
  fNofDigisTot(0),
  fTimer()
{ 
  Reset();
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
CbmStsDigitizeIdeal::~CbmStsDigitizeIdeal() {
 if ( fDigis ) {
    fDigis->Delete();
    delete fDigis;
  }
 Reset();
}
// -------------------------------------------------------------------------



// -----   Create a digi object   ------------------------------------------
void CbmStsDigitizeIdeal::CreateDigi(UInt_t address,
		              									 ULong64_t time,
		              									 UShort_t adc) {

	// Current implementation is for event-based only. Digi is created
	// by placement in the respective TClonesArray
	if ( ! fDigis ) {
		LOG(FATAL) << GetName() << ": No output TClonesArray for StsDigis!"
				       << FairLogger::endl;
		return;
	}
	Int_t nDigis = fDigis->GetEntriesFast();
	new ( (*fDigis)[nDigis] ) CbmStsDigi(address, time, adc);
	fNofDigis++;
	LOG(DEBUG3) << GetName() << ": created digi at " << time
			        << " ns with ADC " << adc << " at address " << address
			        << " (module "
			        << fSetup->GetElement(address, kStsModule)->GetName()
			        << ", channel "
			        << CbmStsAddress::GetElementId(address, kStsChannel)
	            << ")" << FairLogger::endl;

}
// -------------------------------------------------------------------------



// -----   Task execution   ------------------------------------------------
void CbmStsDigitizeIdeal::Exec(Option_t* opt) {

	// --- Start timer and reset counters
	fTimer.Start();
	Reset();

	// --- MC Event number
	Int_t iEvent =
			FairRunAna::Instance()->GetEventHeader()->GetMCEntryNumber();

	// --- Check for input array. Should be caught in Init method.
	if ( ! fPoints ) {
		LOG(FATAL) << GetName() << ": No input array of CbmStsPoint!"
				       << FairLogger::endl;
		return;
	}
	if ( FairLogger::GetLogger()->IsLogNeeded(DEBUG) ) std::cout << std::endl;
	LOG(DEBUG) << GetName() << ": processing event " << iEvent << " with " << fPoints->GetEntriesFast()
			       << " StsPoints " << FairLogger::endl;


  // --- Loop over all StsPoints and execute the ProcessPoint method
  for (Int_t iPoint=0; iPoint<fPoints->GetEntriesFast(); iPoint++) {
   CbmStsPoint* point = (CbmStsPoint*) fPoints->At(iPoint);
   ProcessPoint(point);
    fNofPoints++;
  }  // StsPoint loop

  // --- Clean buffers of all modules
  for (Int_t iModule = 0; iModule < fSetup->GetNofModules(); iModule++)
  	fSetup->GetModule(iModule)->CleanBuffer(-1.);

  // --- Counters
  fTimer.Stop();
  fNofEvents++;
  fNofPointsTot   += fNofPoints;
  fNofSignalsFTot += fNofSignalsF;
  fNofSignalsBTot += fNofSignalsB;
  fNofDigisTot    += fNofDigis;
  fTimeTot        += fTimer.RealTime();

  LOG(INFO) << "+ " << GetName() << ": time "  << fTimer.RealTime()
  		      << "s, event " << iEvent << ", points: " << fNofPoints
  		      << ", signals: " << fNofSignalsF << " / " << fNofSignalsB
  		      << ", digis: " << fNofDigis << FairLogger::endl;
}
// -------------------------------------------------------------------------



// -----   Finish run   ----------------------------------------------------
void CbmStsDigitizeIdeal::Finish() {
	std::cout << std::endl;
	LOG(INFO) << "=====================================" << FairLogger::endl;
	LOG(INFO) << GetName() << ": Run summary" << FairLogger::endl;
	LOG(INFO) << "Events processed : " << fNofEvents << FairLogger::endl;
	LOG(INFO) << "StsPoint / event : " << fNofPointsTot / Double_t(fNofEvents)
			      << FairLogger::endl;
	LOG(INFO) << "Signals / event  : "
			      << fNofSignalsFTot / Double_t(fNofEvents)
			      << " / " << fNofSignalsBTot / Double_t(fNofEvents)
			      << FairLogger::endl;
	LOG(INFO) << "StsDigi / event  : " << fNofDigisTot  / Double_t(fNofEvents)
			      << FairLogger::endl;
	LOG(INFO) << "Digis per point  : " << fNofDigisTot / fNofPointsTot
			      << FairLogger::endl;
	LOG(INFO) << "Digis per signal : "
			      << fNofDigisTot / ( fNofSignalsFTot + fNofSignalsBTot )
			      << FairLogger::endl;
	LOG(INFO) << "Time per event   : " << fTimeTot / Double_t(fNofEvents)
			      << FairLogger::endl;
	LOG(INFO) << "=====================================" << FairLogger::endl;
}
// -------------------------------------------------------------------------



// -----  Process a StsPoint   ---------------------------------------------
void CbmStsDigitizeIdeal::ProcessPoint(CbmStsPoint* point) {

	// Debug
	if ( FairLogger::GetLogger()->IsLogNeeded(DEBUG2) ) point->Print();
	LOG(DEBUG2) << GetName() << ": Point coordinates: in (" << point->GetXIn()
			        << ", " << point->GetYIn() << ", " << point->GetZIn() << ")"
			        << ", out (" << point->GetXOut() << ", " << point->GetYOut()
			        << ", " << point->GetZOut() << ")" << FairLogger::endl;

	// --- Get the sensor the point is in
	UInt_t address = point->GetDetectorID();
	CbmStsSenzor* sensor = dynamic_cast<CbmStsSenzor*>
					(fSetup->GetElement(address, kStsSensor));
	if ( ! sensor ) LOG(ERROR) << GetName() << ": Sensor of StsPoint not found!"
			                       << FairLogger::endl;
	LOG(DEBUG2) << GetName() << ": Sending point to sensor "
			        << sensor->GetName() << " ( " << sensor->GetAddress()
			        << " ) " << FairLogger::endl;

	// --- Process the point on the sensor
	Int_t status = sensor->ProcessPoint(point);

	// --- Statistics
	Int_t nSignalsF = status / 1000;
	Int_t nSignalsB = status - 1000 * nSignalsF;
	LOG(DEBUG2) << GetName() << ": Produced signals: " << nSignalsF << " / "
			        << nSignalsB << FairLogger::endl;
	fNofSignalsF += nSignalsF;
	fNofSignalsB += nSignalsB;

}
// -------------------------------------------------------------------------



// -----   Initialisation   ------------------------------------------------
InitStatus CbmStsDigitizeIdeal::Init() {

  // Get input array (CbmStsPoint)
  FairRootManager* ioman = FairRootManager::Instance();
  if ( ! ioman ) Fatal("Init", "No FairRootManager");
  fPoints = (TClonesArray*) ioman->GetObject("StsPoint");

  // Register output array (CbmStsDigi)
  fDigis = new TClonesArray("CbmStsDigi",1000);
  ioman->Register("StsDigi", "Digital response in STS", fDigis, kTRUE);

  // Get STS setup interface
  fSetup = CbmStsSetup::Instance();

  // Assign types to the sensors in the setup
  SetSensorTypes();

  // Set the digitisation parameters of the modules
  SetModuleParameters();

  // Register this task to the setup
  fSetup->SetDigitizer(this);

  LOG(INFO) << GetName() << ": Initialisation successful" << FairLogger::endl;
  return kSUCCESS;

}
// -------------------------------------------------------------------------



// -----   Private method ReInit   -----------------------------------------
InitStatus CbmStsDigitizeIdeal::ReInit() {

  fSetup = CbmStsSetup::Instance();

  return kERROR;

}
// -------------------------------------------------------------------------



// -----   Private method Reset   ------------------------------------------
void CbmStsDigitizeIdeal::Reset() {
  fNofPoints = fNofSignalsF = fNofSignalsB = fNofDigis = 0;
  if ( fDigis ) fDigis->Delete();
}
// -------------------------------------------------------------------------



// -----   Set the digitisation parameters for the modules   ---------------
// TODO: Currently, all modules have the same parameters. In future,
// more flexible schemes must be used, in particular for the thersholds.
void CbmStsDigitizeIdeal::SetModuleParameters() {
	Int_t nModules = fSetup->GetNofModules();
	for (Int_t iModule = 0; iModule < nModules; iModule++) {
		fSetup->GetModule(iModule)->SetParameters(fDynRange, fThreshold,
				                                      fNofAdcChannels);
	}
}
// -------------------------------------------------------------------------



// --- Set the types for the sensors in the setup --------------------------
// TODO: Currently hard-coded to be SensorTypeDssd. Should be made more
// flexible in the future.
void CbmStsDigitizeIdeal::SetSensorTypes() {

	Int_t nSensors = fSetup->GetNofSensors();
	Int_t nTypes[6] = {0, 0, 0, 0, 0, 0};

	for (Int_t iSensor = 0; iSensor < fSetup->GetNofSensors(); iSensor++) {
		CbmStsSenzor* sensor = fSetup->GetSensor(iSensor);
		TGeoBBox* shape = dynamic_cast<TGeoBBox*>
											(sensor->GetPnode()->GetVolume()->GetShape());
		Double_t volX = 2. * shape->GetDX();
		Double_t volY = 2. * shape->GetDY();

		// --- Instantiate sensor type object
		CbmStsSensorTypeDssd* type = NULL;
		switch (fDigiModel) {
			case 0:  type = new CbmStsSensorTypeDssdIdeal();
			         type->SetTitle("DssdIdeal");
			         break;
			case 1:  type = new CbmStsSensorTypeDssd();
			         type->SetTitle("Dssd");
			         break;
			default: type = NULL;
							 break;
		}
		if ( ! type ) {
			LOG(FATAL) << GetName() << ": Unknown sensor response model "
					       << fDigiModel << FairLogger::endl;
			continue;
		}
		sensor->SetType(type);

		// Sensor01: 4 x 2.2
		if ( volX > 3.99 && volX < 4.01 ) {
			Double_t lx = 3.7584;   // active size in x
			Double_t ly = 1.96;     // active size in y
			Double_t lz = 0.03;     // active thickness
			Int_t nStripsF = 648;   // number of strips front side (58 mum)
			Int_t nStripsB = 648;   // number of strips back side  (58 mum)
			Double_t stereoF = 0.;  // stereo angle front side
			Double_t stereoB = 7.5; // stereo angle back side
			type->SetParameters(lx, ly, lz, nStripsF, nStripsB, stereoF, stereoB);
			type->SetName("Sensor01");
			nTypes[0]++;
		} //? Sensor01

		// Sensor02: 6.2 x 2.2
		else if ( volX > 6.19 && volX < 6.21 && volY > 2.19 && volY < 2.21) {
			Double_t lx = 5.9392;   // active size in x
			Double_t ly = 1.96;     // active size in y
			Double_t lz = 0.03;     // active thickness
			Int_t nStripsF = 1024;  // number of strips front side (58 mum)
			Int_t nStripsB = 1024;  // number of strips back side  (58 mum)
			Double_t stereoF = 0.;  // stereo angle front side
			Double_t stereoB = 7.5; // stereo angle back side
			type->SetParameters(lx, ly, lz, nStripsF, nStripsB, stereoF, stereoB);
			type->SetName("Sensor02");
			nTypes[1]++;
		} //? Sensor02

		// Sensor03: 6.2 x 4.2
		else if ( volX > 6.19 && volX < 6.21 && volY > 4.19 && volY < 4.21) {
			Double_t lx = 5.9392;   // active size in x
			Double_t ly = 3.96;     // active size in y
			Double_t lz = 0.03;     // active thickness
			Int_t nStripsF = 1024;  // number of strips front side (58 mum)
			Int_t nStripsB = 1024;  // number of strips back side  (58 mum)
			Double_t stereoF = 0.;  // stereo angle front side
			Double_t stereoB = 7.5; // stereo angle back side
			type->SetParameters(lx, ly, lz, nStripsF, nStripsB, stereoF, stereoB);
			type->SetName("Sensor03");
			nTypes[2]++;
		} //? Sensor03

		// Sensor04: 6.2 x 6.2
		else if ( volX > 6.19 && volX < 6.21 && volY > 6.19 && volY < 6.21) {
			Double_t lx = 5.9392;   // active size in x
			Double_t ly = 5.96;     // active size in y
			Double_t lz = 0.03;     // active thickness
			Int_t nStripsF = 1024;  // number of strips front side (58 mum)
			Int_t nStripsB = 1024;  // number of strips back side  (58 mum)
			Double_t stereoF = 0.;  // stereo angle front side
			Double_t stereoB = 7.5; // stereo angle back side
			type->SetParameters(lx, ly, lz, nStripsF, nStripsB, stereoF, stereoB);
			type->SetName("Sensor04");
			nTypes[3]++;
		} //? Sensor04

		// Sensor05: 3.1 x 3.1
		else if ( volX > 3.09 && volX < 3.11 && volY > 3.09 && volY < 3.11) {
			Double_t lx = 2.8594;   // active size in x
			Double_t ly = 2.86;     // active size in y
			Double_t lz = 0.03;     // active thickness
			Int_t nStripsF = 493;   // number of strips front side (58 mum)
			Int_t nStripsB = 493;   // number of strips back side  (58 mum)
			Double_t stereoF = 0.;  // stereo angle front side
			Double_t stereoB = 7.5; // stereo angle back side
			type->SetParameters(lx, ly, lz, nStripsF, nStripsB, stereoF, stereoB);
			type->SetName("Sensor05");
			nTypes[4]++;
		} //? Sensor05

		// Sensor06: 1.5 x 4.2
		else if ( volX > 1.49 && volX < 1.51 && volY > 4.19 && volY < 4.21) {
			Double_t lx = 1.2586;   // active size in x
			Double_t ly = 3.96;     // active size in y
			Double_t lz = 0.03;     // active thickness
			Int_t nStripsF = 217;   // number of strips front side (58 mum)
			Int_t nStripsB = 217;   // number of strips back side  (58 mum)
			Double_t stereoF = 0.;  // stereo angle front side
			Double_t stereoB = 7.5; // stereo angle back side
			type->SetParameters(lx, ly, lz, nStripsF, nStripsB, stereoF, stereoB);
			type->SetName("Sensor06");
			nTypes[5]++;
		} //? Sensor06

		// Unknown sensor type
		else {
			LOG(FATAL) << GetName() << ": Unknown sensor type with x = " << volX
					       << ", y = " << volY << FairLogger::endl;
			continue;
		}  //? unknown type

		sensor->SetType(type);
	} //? Loop over sensors

	LOG(INFO) << GetName() << ": Set types for " << nSensors
			      << " sensors ( " << nTypes[0];
	for (Int_t iType = 0; iType < 6; iType++)
		LOG(INFO) << " / " << nTypes[iType];

	LOG(INFO) << " ) " << FairLogger::endl;

}
// -------------------------------------------------------------------------


ClassImp(CbmStsDigitizeIdeal)

