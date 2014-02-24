/** @file CbmStsSenzor.cxx
 ** @author Volker Friese <v.friese@gsi.de>
 ** @date 03.05.2013
 **/


#include "TGeoMatrix.h"
#include "TGeoPhysicalNode.h"
#include "CbmStsPoint.h"
#include "CbmStsSenzor.h"
#include "CbmStsAddress.h"
#include "CbmStsModule.h"
#include "CbmStsSensorPoint.h"
#include "CbmStsSensorType.h"
#include "CbmStsSensorTypeDssd.h"


// -----   Constructor   ---------------------------------------------------
CbmStsSenzor::CbmStsSenzor() 
 : CbmStsElement(),
   fType(NULL)
{
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
CbmStsSenzor::CbmStsSenzor(const char* name, const char* title,
                           TGeoPhysicalNode* node) :
                           CbmStsElement(name, title, kStsSensor, node),
                           fType(NULL)
{
	// TODO: Currently, the sensor type is hard-coded. Should be eventually
	// replaced by initialisation from parameters / database
	fType = new CbmStsSensorTypeDssd();
	Double_t dx = 5.9392;   // sensor size in x (across strips)
	Double_t dy = 5.9600;   // sensor size in y (along strips)
	Double_t dz = 0.0300;   // sensor thickness
	Int_t nStripsF = 1024;  // number of strips on front side
	Int_t nStripsB = 1024;  // number of strips on back side
	Double_t stereoF = 0.;  // stereo angle on front side
	Double_t stereoB = 7.5; // stereo angle on back side [degrees]
	dynamic_cast<CbmStsSensorTypeDssd*>(fType)->SetParameters(dx, dy, dz,
																					nStripsF, nStripsB,
																					stereoF, stereoB);
}
// -------------------------------------------------------------------------



// -----  Get the mother module   ------------------------------------------
CbmStsModule* CbmStsSenzor::GetModule() const {
	 return dynamic_cast<CbmStsModule*>
	    ( CbmStsSetup::Instance()->GetElement(fAddress, kStsModule) );
}
// -------------------------------------------------------------------------



// -----   Process a CbmStsPoint  ------------------------------------------
void CbmStsSenzor::ProcessPoint(CbmStsPoint* point) const {

  Double_t global[3];
  Double_t local[3];

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

  LOG(INFO) << "Module of sensor is " << GetModule() << FairLogger::endl;

  // --- Call ProcessPoint method from sensor type
  fType->ProcessPoint(sPoint, this);

}
// -------------------------------------------------------------------------










ClassImp(CbmStsSenzor)
