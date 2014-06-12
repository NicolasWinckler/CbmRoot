/** @file CbmStsSensorTypeDssd.cxx
 ** @author Volker Friese <v.friese@gsi.de>
 ** @date 02.05.2013
 **/

#include "setup/CbmStsSensorTypeDssd.h"

#include <iomanip>

#include "TMath.h"

#include "FairLogger.h"


#include "setup/CbmStsModule.h"
#include "setup/CbmStsSensorPoint.h"
#include "setup/CbmStsSenzor.h"


using std::fixed;
using std::setprecision;



// --- Energy for creation of an electron-hole pair in silicon [GeV]  ------
const double kPairEnergy = 3.57142e-9;



// -----   Constructor   ---------------------------------------------------
CbmStsSensorTypeDssd::CbmStsSensorTypeDssd()
    : CbmStsSensorType(), 
      fDx(-1.), fDy(-1.), fDz(-1.),
      fPitch(), fStereo(), fIsSet(kFALSE),
      fNofStrips(), fCosStereo(), fSinStereo(), fStripShift()
{
}
// -------------------------------------------------------------------------



// -----   Get channel number in module   ----------------------------------
Int_t CbmStsSensorTypeDssd::GetModuleChannel(Int_t strip, Int_t side,
                                             Int_t sensorId) const {

  // --- Check side
  if ( side < 0 || side > 1 ) {
    LOG(ERROR) << "Illegal side qualifier " << side << FairLogger::endl;
    return -1;
  }

  // --- Account for offset due to stereo angle
  Int_t channel = strip - sensorId * fStripShift[side];

  // --- Account for horizontal cross-connection of strips
  while ( channel < 0 ) channel += fNofStrips[side];
  while ( channel >= fNofStrips[side] ) channel -= fNofStrips[side];

  // --- Account for front or back side
  if ( side ) channel += fNofStrips[0];

  return channel;
}
// -------------------------------------------------------------------------



// -----   Get strip number from coordinates   -----------------------------
Int_t CbmStsSensorTypeDssd::GetStripNumber(Double_t x, Double_t y,
		                                       Int_t side) const {

	// Cave: This implementation assumes that the centre of the sensor volume
	// is also the centre of the active area, i.e. that the inactive borders
	// (guard ring) are symmetric both and x and y (not necessarily the same
	// in y and y).

	// Check side
	if ( side < 0 || side > 1) {
		LOG(ERROR) << "Illegal side qualifier " << side << FairLogger::endl;
		return -1;
	}

	// Check whether in active area (should have been caught before)
	if ( TMath::Abs(x) > fDx / 2. ) {
		LOG(ERROR) << GetName() << ": Outside active area : x = "
				       << x << " cm"<< FairLogger::endl;
    return -1;
	}
	if ( TMath::Abs(y) > fDy / 2. ) {
		LOG(ERROR) << GetName() << ": Outside active area : y = "
			         << y << " cm"<< FairLogger::endl;
    return -1;
	}

  // Stereo angle and strip pitch
  Double_t tanphi = fSinStereo[side] / fCosStereo[side];
  Double_t pitch  = fPitch[side];
  Int_t nStrips   = fNofStrips[side];

  // Calculate distance from lower left corner of the active area.
  // Note: the coordinates are given w.r.t. the centre of the volume.
  Double_t xdist = x + 0.5 * fDx;
  Double_t ydist = y + 0.5 * fDy;

  // Project coordinates to readout (top) edge
  Double_t xro = xdist - ( fDy - ydist ) * tanphi;

  // Calculate corresponding strip number
  Int_t iStrip = TMath::FloorNint( xro / pitch );

  // Account for horizontal cross-connection of strips
  // not extending to the top edge
  while ( iStrip < 0 )        iStrip += nStrips;
  while ( iStrip >= nStrips ) iStrip -= nStrips;

  if ( FairLogger::GetLogger()->IsLogNeeded(DEBUG4)) Print();
  LOG(DEBUG3) << setprecision(6) << GetName() << ": (x,y) = (" << x << ", " << y
  		        << ") cm, x(ro) = " << xro << " cm, strip "
  		        << iStrip << FairLogger::endl;
  return iStrip;
}
// -------------------------------------------------------------------------



// -----   Get strip and side from channel number   ------------------------
void CbmStsSensorTypeDssd::GetStrip(Int_t channel, Int_t sensorId,
                                    Int_t& strip, Int_t& side) {

  Int_t stripNr = -1;
  Int_t sideNr  = -1;


  // --- Determine front or back side
  if ( channel < fNofStrips[0] ) {          // front side
    sideNr = 0;
    stripNr = channel;
  }
  else {
    sideNr = 1;
    stripNr = channel - fNofStrips[0];      // back side
  }
  side = sideNr;

  // --- Offset due to stereo angle
  stripNr += sensorId * fStripShift[side];

  // --- Horizontal cross-connection
  while ( stripNr < 0 ) stripNr += fNofStrips[side];
  while ( stripNr >= fNofStrips[side] ) stripNr -= fNofStrips[side];

  strip = stripNr;
  return;
}
// -------------------------------------------------------------------------




// -----   Print parameters   ----------------------------------------------
void CbmStsSensorTypeDssd::Print(Option_t* opt) const {

  LOG(INFO) << "Properties of sensor type " << GetName() << ": "
            << FairLogger::endl
            << "\t  Dimensions: (" << fixed << setprecision(4)
            << fDx << ", " << fDy << ", " << fDz << ") cm"
            << FairLogger::endl
            << "\t  Front side: pitch = "
            << setprecision(0) << fPitch[0]*1.e4 << " mum, "
            << fNofStrips[0] << " strips, stereo angle "
            << setprecision(1) << fStereo[0] << " degrees"
            << FairLogger::endl
            << "\t  Back side:  pitch = "
            << setprecision(0) << fPitch[1]*1.e4 << " mum, "
            << fNofStrips[1] << " strips, stereo angle "
            << setprecision(1) << fStereo[1] << " degrees"
            << FairLogger::endl;

}
// -------------------------------------------------------------------------



// -----   Process an MC Point  --------------------------------------------
Int_t CbmStsSensorTypeDssd::ProcessPoint(CbmStsSensorPoint* point,
                                         const CbmStsSenzor* sensor) const {

  // --- Catch if parameters are not set
  if ( ! fIsSet ) {
    LOG(FATAL) << fName << ": parameters are not set!"
               << FairLogger::endl;
    return -1;
  }

  // --- Created charge signals
  Int_t nSignals = 0;

  // --- Produce charge on front and back side
  nSignals += 1000 * ProduceCharge(point, 0, sensor);
 	nSignals += ProduceCharge(point, 1, sensor);

  return nSignals;
}
// -------------------------------------------------------------------------



// -----   Produce charge on the strips   ----------------------------------
Int_t CbmStsSensorTypeDssd::ProduceCharge(CbmStsSensorPoint* point,
                                          Int_t side,
                                          const CbmStsSenzor* sensor)
                                          const {

  // --- Protect against being called without parameters being set
  if ( ! fIsSet ) LOG(FATAL) << "Parameters of sensor " << fName
                             << " are not set!" << FairLogger::endl;

  // This implementation assumes a straight trajectory in the sensor
  // and a constant charge distribution along it.

  // Check for side qualifier
  if ( side < 0 || side > 1 )  {
    LOG(ERROR) << "Illegal side qualifier!" << FairLogger::endl;
    return -1;
  }

  // Total produced charge
  Double_t qtot = point->GetELoss() / kPairEnergy;

  // Stereo angle and strip pitch
  Double_t tanphi = fSinStereo[side] / fCosStereo[side];
  Double_t pitch  = fPitch[side];
  Int_t nStrips   = fNofStrips[side];

  // Debug output
  LOG(DEBUG4) << GetName() << ": Side " << side << ", dx = " << fDx
  		        << " cm, dy = " << fDy << " cm, stereo " << fStereo[side]
  		        << " degrees, strips " << fNofStrips[side] << ", pitch "
  		        << pitch << " mum" << FairLogger::endl;

  // Project point coordinates (in / out) along strips to readout (top) edge
  // Keep in mind that the SensorPoint gives coordinates with
  // respect to the centre of the active area.
  Double_t x1 = point->GetX1() + 0.5 * fDx
                - ( 0.5 * fDy - point->GetY1() ) * tanphi;
  Double_t x2 = point->GetX2() + 0.5 * fDx
                - ( 0.5 * fDy - point->GetY2() ) * tanphi;
  LOG(DEBUG4) << GetName() << ": R/O x coordinates are " << x1 << " "
  		        << x2 << FairLogger::endl;


  // Calculate corresponding strip numbers
  // This can be negative or larger than the number of channels if the
  // strip does not extend to the top edge.
  Int_t i1 = TMath::FloorNint( x1 / pitch );
  Int_t i2 = TMath::FloorNint( x2 / pitch );


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
  Int_t nSignals = 0;
  for (Int_t iStrip = i1; iStrip <= i2; iStrip++) {

    // --- Calculate charge in strip
    Double_t y1 = TMath::Max(x1, Double_t(iStrip) * pitch);  // start in strip
    Double_t y2 = TMath::Min(x2, Double_t(iStrip+1) * pitch); // stop in strip
    Double_t charge = (y2 - y1) * qtot / ( x2 - x1 );

    // --- Register charge to module
    RegisterCharge(sensor, side, iStrip, charge, point->GetTime());
    nSignals++;

  } // Loop over fired strips

  return nSignals;
}
// -------------------------------------------------------------------------



// -----   Register charge   -----------------------------------------------
void CbmStsSensorTypeDssd::RegisterCharge(const CbmStsSenzor* sensor,
                                          Int_t side, Int_t strip,
                                          Double_t charge,
                                          Double_t time) const {

  // --- Determine module channel for given sensor strip
  Int_t channel = GetModuleChannel(strip, side, sensor->GetSensorId() );

  // --- Debug output
  LOG(DEBUG4) << fName << ": Registering charge: side " << side
              << ", strip " << strip << ", time " << time
              << ", charge " << charge
              << " to channel " << channel
              << " of module " << sensor->GetModule()->GetName()
              << FairLogger::endl;

  // --- Send signal to module
  sensor->GetModule()->AddSignal(channel, time, charge);


}
// -------------------------------------------------------------------------



// -----   Self test   -----------------------------------------------------
Bool_t CbmStsSensorTypeDssd::SelfTest() {

  for (Int_t sensorId = 0; sensorId < 3; sensorId++ ) {
    for (Int_t side = 0; side < 2; side ++ ) {
      for (Int_t strip = 0; strip < fNofStrips[side]; strip++ ) {
        Int_t channel = GetModuleChannel(strip, side, sensorId);
        Int_t testStrip, testSide;
        GetStrip(channel, sensorId, testStrip, testSide);
        if ( testStrip != strip || testSide != side ) {
          LOG(ERROR) << fName << "Self test failed! Sensor " << sensorId
                     << " side " << side << " strip " << strip
                     << " gives channel " << channel << " gives strip "
                     << testStrip << " side " << testSide
                     << FairLogger::endl;
          return kFALSE;
        }
      } // strip loop
    } // side loop
  } // sensor loop

  //LOG(DEBUG2) << fName << ": self test passed" << FairLogger::endl;
  return kTRUE;
}
// -------------------------------------------------------------------------



// -----   Set the parameters   --------------------------------------------
void CbmStsSensorTypeDssd::SetParameters(Double_t dx, Double_t dy,
                                         Double_t dz, Int_t nStripsF,
                                         Int_t nStripsB, Double_t stereoF,
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
  fDx           = dx;
  fDy           = dy;
  fDz           = dz;
  fNofStrips[0] = nStripsF;
  fNofStrips[1] = nStripsB;
  fStereo[0]    = stereoF;
  fStereo[1]    = stereoB;

  // --- Calculate parameters for front and back
  for (Int_t side = 0; side < 2; side++) {
    fPitch[side] = fDx / Double_t(fNofStrips[side]);
    fCosStereo[side] = TMath::Cos( fStereo[side] * TMath::DegToRad() );
    fSinStereo[side] = TMath::Sin( fStereo[side] * TMath::DegToRad() );
    Double_t tanPhi = fSinStereo[side] / fCosStereo[side];
    fStripShift[side] = TMath::Nint(fDy * tanPhi / fPitch[side]);
  }

  // --- Flag parameters to be set if test is ok
  fIsSet = SelfTest();

}
// -------------------------------------------------------------------------



ClassImp(CbmStsSensorTypeDssd)
