/** @file CbmStsModule.cxx
 ** @author Volker Friese <v.friese@gsi.de>
 ** @date 14.05.2013
 **/


#include "FairLogger.h"
#include "CbmStsAddress.h"
#include "CbmStsDigi.h"
#include "setup/CbmStsModule.h"
#include "setup/CbmStsSensorType.h"


// -----   Default constructor   -------------------------------------------
CbmStsModule::CbmStsModule() : CbmStsElement(), fSensors(), fBuffer()
{
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
CbmStsModule::CbmStsModule(const char* name, const char* title,
                           TGeoPhysicalNode* node) :
                           CbmStsElement(name, title, kStsModule, node),
                           fSensors(), fBuffer()
{
}
// -------------------------------------------------------------------------



// --- Destructor   --------------------------------------------------------
CbmStsModule::~CbmStsModule() {
}
// -------------------------------------------------------------------------



// ----- Add a sensor  -----------------------------------------------------
void CbmStsModule::AddSensor(CbmStsSenzor* sensor) {

  // TODO: Unify adding elements from base class and adding sensors
  // If AddDaughter is used, compatibility of sensor types is not assured.
  // Still unclear how sensor types are assigned when setup is read from
  // geometry.

  // Check compatibility with already present sensors
  if ( ! fSensors.empty() ) {
    if ( sensor->GetType() != fSensors[0]->GetType() ) {
      LOG(ERROR) << "Sensor type " << sensor->GetType()->GetTypeId()
                 << " incompatible with sensor type "
                 << fSensors[0]->GetType()->GetTypeId()
                 << FairLogger::endl;
      return;
    }
  }

  // Add sensor to the array
  fSensors.push_back(sensor);

  // Set sensor address and pointer to module
  Int_t sensorId = fDaughters.size() - 1;
  UInt_t address = CbmStsAddress::SetElementId(fAddress, kStsSensor,
                                               sensorId);
  sensor->SetAddress(address);

  return;
}
// -------------------------------------------------------------------------



// -----   Add a signal to the buffer   ------------------------------------
void CbmStsModule::AddSignal(Int_t channel, Double_t time,
                             Double_t charge) {

	// --- If there is no signal in the buffer for this channel:
	// --- write the signal into the buffer
	if ( fBuffer.find(channel) == fBuffer.end() )
		fBuffer[channel] = pair<Double_t, Double_t>(charge, time);

	// --- If there is already a signal, compare the time of the two
	else {
		Double_t chargeOld = (fBuffer.find(channel)->second).first;
		Double_t timeOld   = (fBuffer.find(channel)->second).second;

		// --- Time separation large; no interference. Create a digi from
		// --- the old signal, send it to the DAQ and write the new signal
		// --- into the buffer.
		if ( time - timeOld > 100. ) {  // 5 Mark in die Kasse für hardcoded numbers
			CreateDigi(channel, chargeOld, timeOld);
			fBuffer[channel] = pair<Double_t, Double_t>(charge, time);
		}

		// --- Time separation small; interference.
		// --- Current implementation: Add up charge in the buffer.
		// --- Time is the one of the first signal.
		else {
			fBuffer[channel] = pair<Double_t, Double_t>(chargeOld + charge, timeOld);
		}

	}

}
// -------------------------------------------------------------------------


// -----   Create a digi   -------------------------------------------------
CbmStsDigi* CbmStsModule::CreateDigi(Int_t channel, Double_t charge,
																		Double_t time) {

	// --- Construct channel address from module address and channel number
	UInt_t address = CbmStsAddress::SetElementId(GetAddress(),
			                                         kStsChannel, channel);

	// --- Digitise charge
	UShort_t adc = 0;
	if ( charge > fThreshold) {
		if ( charge > fDynRange ) adc = fNofAdcChannels;
		else adc = UShort_t( (charge - fThreshold) / (fDynRange - fThreshold)
				                    * Double_t(fNofAdcChannels) );
	}

	// --- Digitise time
	// TODO: Add Gaussian time resolution
	ULong64_t dTime = ULong64_t(time);

	// --- Instantiate a digi
	CbmStsDigi* digi = new CbmStsDigi(address, dTime, adc);

	// TODO: Send digi to DAQ

	return digi;
}
// -------------------------------------------------------------------------

ClassImp(CbmStsModule)
