/** @file CbmStsModule.cxx
 ** @author Volker Friese <v.friese@gsi.de>
 ** @date 14.05.2013
 **/


#include "FairLogger.h"
#include "setup/CbmStsModule.h"
#include "setup/CbmStsSensorType.h"


// -----   Default constructor   -------------------------------------------
CbmStsModule::CbmStsModule() : fAddress(0), fNode(NULL) {
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



}


// -----   Add a signal to the buffer   ------------------------------------
void CbmStsModule::AddSignal(CbmStsSenzor* sensor, Int_t side, Int_t strip,
                             Double_t time, Double_t charge) {



}

ClassImp(CbmStsModule)
