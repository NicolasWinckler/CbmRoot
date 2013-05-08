/** @file CbmStsSensorTypeDssd.cxx
 ** @author Volker Friese <v.friese@gsi.de>
 ** @date 02.05.2013
 **/


#include "TMath.h"

#include "FairLogger.h"

#include "CbmStsSensorPoint.h"
#include "CbmStsSensorTypeDssd.h"


using namespace std;


// --- Energy for creation of an electron-hole pair in silicon [GeV]  ------
const double kPairEnergy = 3.6e-9;



// -----   Constructor   ---------------------------------------------------
CbmStsSensorTypeDssd::CbmStsSensorTypeDssd()
    : CbmStsSensorType(), fDx(-1.), fDy(-1.), fDz(-1.),
      fPitchF(-1.), fPitchB(-1.), fStereoF(0.), fStereoB(0.),
      fIsSet(kFALSE) {
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
            << "\t  Front side: pitch = " << fPitchF*1.e4 << " mum, "
            << fNofStripsF << " strips, stereo angle "
            << fStereoF << " degrees" << FairLogger::endl
            << "\t  Back side:  pitch = " << fPitchB*1.e4 << " mum, "
            << fNofStripsB << " strips, stereo angle "
            << fStereoB << " degrees" << FairLogger::endl;

}
// -------------------------------------------------------------------------



// -----   Process an MC Point  --------------------------------------------
void CbmStsSensorTypeDssd::ProcessPoint(CbmStsSensorPoint* point) {

  ProduceCharge(point, 0);
  ProduceCharge(point, 1);

}
// -------------------------------------------------------------------------



// -----   Produce charge on the strips   ----------------------------------
void CbmStsSensorTypeDssd::ProduceCharge(CbmStsSensorPoint* point,
                                         Int_t side) const {

  // --- Protect against being called without parameters being set
  if ( ! fIsSet ) LOG(FATAL) << "Parameters of sensor " << fName
                             << " are not set!" << FairLogger::endl;

  // This implementation assumes a straight trajectory in the sensor
  // and a constant charge distribution along it.

  // Total produced charge
  Double_t qtot = point->GetELoss() / kPairEnergy;

  // Stereo angle and strip pitch
  Double_t tanphi = 0.;     // tangent of stereo angle
  Double_t pitch  = 0.;     // strip pitch
  Int_t nStrips;            // number of strips
  if ( side == 0 ) {        // front side
    tanphi  = fSinStereoF / fCosStereoF;
    pitch   = fPitchF;
    nStrips = fNofStripsF;
  }
  else if ( side == 1 ) {   // back side
    tanphi  = fSinStereoB / fCosStereoB;
    pitch   = fPitchB;
    nStrips = fNofStripsB;
  }
  else {
    LOG(ERROR) << "Illegal side qualifier!" << FairLogger::endl;
    return;
  }


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
    RegisterCharge(side, i1, qtot, point->GetTime());
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
    RegisterCharge(side, jStrip, charge, point->GetTime());

  } // Loop over fired strips

}
// -------------------------------------------------------------------------



// -----   Register charge   -----------------------------------------------
void CbmStsSensorTypeDssd::RegisterCharge(Int_t side, Int_t strip,
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
  fDx      = dx;
  fDy      = dy;
  fDz      = dz;
  fPitchF  = pitchF;
  fPitchB  = pitchB;
  fStereoF = stereoF;
  fStereoB = stereoB;

  // --- Calculate number of strips
  fNofStripsF = TMath::CeilNint(fDx / fPitchF);
  fNofStripsB = TMath::CeilNint(fDx / fPitchB);

  // --- Calculate cos and sin of stereo angles
  fCosStereoF = TMath::Cos( fStereoF * TMath::DegToRad() );
  fSinStereoF = TMath::Sin( fStereoF * TMath::DegToRad() );
  fCosStereoB = TMath::Cos( fStereoB * TMath::DegToRad() );
  fSinStereoB = TMath::Sin( fStereoB * TMath::DegToRad() );

  fIsSet = kTRUE;

}
// -------------------------------------------------------------------------



ClassImp(CbmStsSensorTypeDssd)
