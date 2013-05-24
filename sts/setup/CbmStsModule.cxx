/** @file CbmStsModule.cxx
 ** @author Volker Friese <v.friese@gsi.de>
 ** @date 14.05.2013
 **/


#include "FairLogger.h"
#include "setup/CbmStsAddress.h"
#include "setup/CbmStsModule.h"
#include "setup/CbmStsSensorType.h"


// -----   Default constructor   -------------------------------------------
CbmStsModule::CbmStsModule() 
 : TNamed(),
   fAddress(0), 
   fSensors(),
   fNode(NULL),
   fBuffer()
{
}
// -------------------------------------------------------------------------



// --- Destructor   --------------------------------------------------------
CbmStsModule::~CbmStsModule() {
}
// -------------------------------------------------------------------------



// ----- Add a sensor  -----------------------------------------------------
void CbmStsModule::AddSensor(CbmStsSenzor* sensor) {

  // Check compatibility with already present sensors
  if ( ! fSensors.empty() ) {
    if ( sensor->GetType() != fSensors[0]->GetType() ) {
      LOG(ERROR) << "Sensor type " << sensor->GetType()->GetTypeId()
                 << " incompatible with sensor type "
                 << fSensors[0]->GetType()->GetTypeId() << FairLogger::endl;
      return;
    }
  }

  // Add sensor to the array
  fSensors.push_back(sensor);

  // Set sensor address and pointer to module
  Int_t sensorId = fSensors.size() - 1;
  UInt_t address = CbmStsAddress::SetElementId(fAddress, kStsSensor,
                                               sensorId);
  sensor->SetAddress(address);
  sensor->SetModule(this);

  return;
}
// -------------------------------------------------------------------------


// -----   Add a signal to the buffer   ------------------------------------
void CbmStsModule::AddSignal(Int_t channel, Double_t time,
                             Double_t charge) {



}

ClassImp(CbmStsModule)
