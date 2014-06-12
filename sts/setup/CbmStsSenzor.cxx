/** @file CbmStsSenzor.cxx
 ** @author Volker Friese <v.friese@gsi.de>
 ** @date 03.05.2013
 **/


// Include class header
#include "CbmStsSenzor.h"

// Includes from ROOT
#include "TGeoMatrix.h"

// Includes from CbmRoot
#include "CbmStsPoint.h"

// Includes from STS
#include "setup/CbmStsModule.h"
#include "setup/CbmStsSensorPoint.h"
#include "setup/CbmStsSetup.h"



// -----   Constructor   ---------------------------------------------------
CbmStsSenzor::CbmStsSenzor() : CbmStsElement(), fType(NULL)
{
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
CbmStsSenzor::CbmStsSenzor(const char* name, const char* title,
                           TGeoPhysicalNode* node) :
                           CbmStsElement(name, title, kStsSensor, node),
                           fType(NULL)
{
}
// -------------------------------------------------------------------------



// -----  Get the mother module   ------------------------------------------
CbmStsModule* CbmStsSenzor::GetModule() const {
	 return dynamic_cast<CbmStsModule*> ( GetMother() );
}
// -------------------------------------------------------------------------



// -----   Process a CbmStsPoint  ------------------------------------------
Int_t CbmStsSenzor::ProcessPoint(CbmStsPoint* point) const {

	// Check whether type is assigned
	if ( ! fType ) {
		LOG(FATAL) << GetName() << ": No sensor type assigned!"
				       << FairLogger::endl;
		return -1;
	}

  Double_t global[3];
  Double_t local[3];

  // --- Debug output of transformation matrix
  if ( FairLogger::GetLogger()->IsLogNeeded(DEBUG4) ) {
  	LOG(DEBUG4) << GetName() << ": Transformation matrix is:"
  			        << FairLogger::endl;
  	fNode->GetMatrix()->Print();
  }

  // --- Transform entry coordinates into local C.S.
  global[0] = point->GetXIn();
  global[1] = point->GetYIn();
  global[2] = point->GetZIn();
  fNode->GetMatrix()->MasterToLocal(global, local);
  Double_t x1 = local[0];
  Double_t y1 = local[1];

  // --- Transform exit coordinates into local C.S.
  global[0] = point->GetXOut();
  global[1] = point->GetYOut();
  global[2] = point->GetZOut();
  fNode->GetMatrix()->MasterToLocal(global, local);
  Double_t x2 = local[0];
  Double_t y2 = local[1];

  // Create SensorPoint
  CbmStsSensorPoint* sPoint = new CbmStsSensorPoint(x1, y1, x2, y2,
                                                    point->GetEnergyLoss(),
                                                    point->GetTime());
  LOG(DEBUG2) << GetName() << ": Local point coordinates are (" << x1
  		        << ", " << y1 << "), (" << x2 << ", " << y2 << ")"
  		        << FairLogger::endl;
  LOG(DEBUG2) << GetName() << ": Sensor type is " << fType->GetName()
  		        << " " << fType->GetTitle() << FairLogger::endl;

  // --- Call ProcessPoint method from sensor type

  return fType->ProcessPoint(sPoint, this);
}
// -------------------------------------------------------------------------


ClassImp(CbmStsSenzor)
