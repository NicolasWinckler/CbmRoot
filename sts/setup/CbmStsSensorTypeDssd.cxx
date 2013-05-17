/** @file CbmStsSensorTypeDssd.cxx
 ** @author Volker Friese <v.friese@gsi.de>
 ** @date 02.05.2013
 **/


#include "TMath.h"

#include "FairLogger.h"

#include "CbmStsSensorPoint.h"
#include "CbmStsSensorTypeDssd.h"

#include <iomanip>

using namespace std;


// --- Energy for creation of an electron-hole pair in silicon [GeV]  ------
const double kPairEnergy = 3.6e-9;

// -----   Constructor   ---------------------------------------------------
CbmStsSensorTypeDssd::CbmStsSensorTypeDssd()
    : CbmStsSensorType(), 
      fDx(-1.), fDy(-1.), fDz(-1.),
      fPitch(), fStereo(), fIsSet(kFALSE),
      fNofStrips(), fCosStereo(), fSinStereo(), fStripShift()
{
}
// -------------------------------------------------------------------------



// -----   Print parameters   ----------------------------------------------
void CbmStsSensorTypeDssd::Print(Option_t* opt) const {

  LOG(INFO) << "Properties of sensor type " << GetName() << ": "
            << FairLogger::endl
            << "\t  Dimensions: (" << fixed << setprecision(4)
            << fDx << ", " << fDy << ", " << fDz
            << ") cm" << FairLogger::endl
            << setprecision(0)
            << "\t  Front side: pitch = " << fPitch[0]*1.e4 << " mum, "
            << fNofStrips[0] << " strips, stereo angle "
            << fStereo[0] << " degrees" << FairLogger::endl
            << "\t  Back side:  pitch = " << fPitch[1]*1.e4 << " mum, "
            << fNofStrips[1] << " strips, stereo angle "
            << fStereo[1] << " degrees" << FairLogger::endl;

}
// -------------------------------------------------------------------------



// -----   Process an MC Point  --------------------------------------------
void CbmStsSensorTypeDssd::ProcessPoint(CbmStsSensorPoint* point,
                                        const CbmStsSenzor* sensor) const {

  ProduceCharge(point, 0, sensor);
  ProduceCharge(point, 1, sensor);

}
// -------------------------------------------------------------------------



// -----   Produce charge on the strips   ----------------------------------
void CbmStsSensorTypeDssd::ProduceCharge(CbmStsSensorPoint* point,
                                         Int_t side,
                                         const CbmStsSenzor* sensor) const {

  // --- Protect against being called without parameters being set
  if ( ! fIsSet ) LOG(FATAL) << "Parameters of sensor " << fName
                             << " are not set!" << FairLogger::endl;

  // This implementation assumes a straight trajectory in the sensor
  // and a constant charge distribution along it.

  // Check for side qualifier
  if ( side < 0 || side > 1 )  {
    LOG(ERROR) << "Illegal side qualifier!" << FairLogger::endl;
    return;
  }

  // Total produced charge
  Double_t qtot = point->GetELoss() / kPairEnergy;

  // Stereo angle and strip pitch
  Double_t tanphi = fSinStereo[side] / fCosStereo[side];
  Double_t pitch  = fPitch[side];
  Int_t nStrips   = fNofStrips[side];


  // Project point coordinates (in / out) to readout (top) edge
  Double_t x1 = point->GetX1() - ( fDy - point->GetY1() ) * tanphi;
  Double_t x2 = point->GetX2() - ( fDy - point->GetY2() ) * tanphi;


  // Calculate corresponding strip numbers
  // This can be negative or larger than the number of channels if the
  // strip does not extend to the top edge.
  Int_t i1 = TMath::FloorNint( x1 / pitch );
  Int_t i2 = TMath::FloorNint( x2 / pitch );


  // --- If everything is in but one strip: register entire charge
  if ( i1 == i2 ) {
    RegisterCharge(sensor, side, i1, qtot, point->GetTime());
    return;
  }


  // --- More than one strip: sort strips
  if ( i1 > i2 ) {
    Int_t tempI = i1;
    i1 = i2;
    i2 = tempI;
    Double_t tempX = x1;
    x1 = x2;
    x2 = tempX;
  }


  // --- Loop over fired strips
  for (Int_t iStrip = i1; iStrip <= i2; iStrip++) {

    // --- Calculate charge in strip
    Double_t d = pitch;    // Width of strip covered by trajectory
    if ( iStrip == i1 ) d = Double_t(i1+1) * pitch - x1;
    else if ( iStrip == i2 ) d = x2 - Double_t(i2) * pitch;
    Double_t charge = d * qtot / ( x2 - x1 );

    // --- Account for cross-connection (double metal)
    Int_t jStrip = iStrip;
    if      ( jStrip < 0 ) jStrip += nStrips;
    else if ( jStrip >= nStrips ) jStrip -= nStrips;

    // --- Register charge to module
    RegisterCharge(sensor, side, jStrip, charge, point->GetTime());

  } // Loop over fired strips

}
// -------------------------------------------------------------------------



// -----   Register charge   -----------------------------------------------
void CbmStsSensorTypeDssd::RegisterCharge(const CbmStsSenzor* sensor,
                                          Int_t side, Int_t strip,
                                          Double_t charge,
                                          Double_t time) const {

  // Dummy implementation for the time being
  // TODO: Deliver charge to corresponding module.


  LOG(INFO) << "Registering charge: side " << side << ", strip " << strip
            << ", charge " << charge << ", time " << time
            << FairLogger::endl;

}
// -------------------------------------------------------------------------



// -----   Set the parameters   --------------------------------------------
void CbmStsSensorTypeDssd::SetParameters(Double_t dx, Double_t dy,
                                         Double_t dz, Double_t pitchF,
                                         Double_t pitchB, Double_t stereoF,
                                         Double_t stereoB) {

  // --- Check stereo angles
  if ( TMath::Abs(stereoF) > 85. )
    LOG(FATAL) << "Stereo angle for front side " << stereoF
               << "outside allowed limits! "
               << "Must be between -85 and 85 degrees."
               << FairLogger::endl;
  if ( TMath::Abs(stereoB) > 85. )
    LOG(FATAL) << "Stereo angle for back side " << stereoB
               << "outside allowed limits! "
               << "Must be between -85 and 85 degrees."
               << FairLogger::endl;

  // --- Set members
  fDx        = dx;
  fDy        = dy;
  fDz        = dz;
  fPitch[0]  = pitchF;
  fPitch[1]  = pitchB;
  fStereo[0] = stereoF;
  fStereo[1] = stereoB;

  // --- Calculate parameters for front and back
  for (Int_t side = 0; side < 2; side++) {
    fNofStrips[side] = TMath::CeilNint(fDx / fPitch[side]);
    fCosStereo[side] = TMath::Cos( fStereo[side] * TMath::DegToRad() );
    fSinStereo[side] = TMath::Sin( fStereo[side] * TMath::DegToRad() );
    Double_t tanPhi = fSinStereo[side] / fCosStereo[side];
    fStripShift[side] = TMath::Nint(fDy * tanPhi / fPitch[side]);
  }

  // --- Flag parameters to be set
  fIsSet = kTRUE;

}
// -------------------------------------------------------------------------



ClassImp(CbmStsSensorTypeDssd)
