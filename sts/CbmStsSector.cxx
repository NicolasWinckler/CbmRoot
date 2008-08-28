// -------------------------------------------------------------------------
// -----                   CbmStsStripSector source file               -----
// -----                  Created 02/03/05  by V. Friese               -----
// -------------------------------------------------------------------------

#include "CbmStsSector.h"

#include "CbmStsSectorDigiPar.h"

#include "TMath.h"

#include <iostream>
#include <list>
#include <vector>

using std::cout;
using std::endl;
using std::pair;
using std::vector;


// -----   Default constructor   -------------------------------------------
CbmStsSector::CbmStsSector() {
  fDetectorId = 0;
  fX0 = fY0 = fRotation = fLx = fLy = fDx = fDy = fStereo = 0.;
  fNChannelsFront = fNChannelsBack = 0;
  fSigmaX = fSigmaY = fSigmaXY = 0.;
  cout << "-W- CbmStsSector: Do not use this constructor! " << endl;
}
// -------------------------------------------------------------------------



// -----   Enhanced constructor (by z0 and d)  ------------------------------------------
CbmStsSector::CbmStsSector(TString tempName, Int_t detId, Int_t iType, Double_t x0, Double_t y0, Double_t z0,
			   Double_t rotation, Double_t lx, Double_t ly, 
			   Double_t d, Double_t dx, Double_t dy, Double_t stereo)
{

  fName = tempName.Data();
  fDetectorId = detId;
  fType       = iType;
  fX0         = x0;
  fY0         = y0;
  fZ0         = z0;  // z position of the station
  fRotation   = rotation;
  fLx         = lx;
  fLy         = ly;
  fD          = d;   // thickness of the station
  fDx         = dx;
  fDy         = dy;
  fStereo     = stereo;

  Double_t dbNoX = fLx / fDx;
  // Calculate number of channels
  if ( fType == 1 ) {             // Pixel sensor
    Double_t dbNoY = fLy / fDy;
    fNChannelsFront = Int_t( TMath::Ceil ( dbNoX ) 
			    *TMath::Ceil ( dbNoY ));
    fNChannelsBack  = 0;
  }
  else if ( fType == 2 ) {        // strip sensor
    fNChannelsFront = Int_t(TMath::Ceil ( dbNoX ));
    Double_t aPlus  = TMath::Abs( fLy * TMath::Tan(fStereo) );
    Double_t dbNoB = ( fLx + aPlus ) / fDx ;
    fNChannelsBack  = Int_t(TMath::Ceil ( dbNoB ));
  }
  else if ( fType == 3 ) {        // strip sensor with double metal layer
    fNChannelsFront = Int_t(TMath::Ceil ( dbNoX ));
    fNChannelsBack  = fNChannelsFront;
  }
  else {
    cout << "-E- CbmStsSector: Illegal sector type " << fType << endl;
    Fatal("", "Illegal sector type");
  }

  // Calculate errors and covariance
  if ( fType == 1 ) {
    fSigmaX  = fDx / TMath::Sqrt(12);
    fSigmaY  = fDy / TMath::Sqrt(12);
    fSigmaXY = 0.;
  }
  else if ( fType == 2 || fType == 3 ) {
    Double_t tanSt = TMath::Tan(fStereo);
    fSigmaX  = fDx / TMath::Sqrt(12);
    fSigmaY  = fDx / TMath::Sqrt(6) / TMath::Abs(tanSt);
    fSigmaXY = -1. * fDx * fDx / 12. / tanSt;
  }
  else {
    cout << "-E- CbmStsSector: Illegal sector type " << fType << endl;
    Fatal("", "Illegal sector type");
  }

  // Transform errors to global c.s. 
  //   if ( tempName.Contains("stsstation02") || tempName.Contains("stsstation07") ) {
  //     rotation+= 90.*TMath::DegToRad();
  //   }

  Double_t cosRot = TMath::Cos(rotation);
  Double_t sinRot = TMath::Sin(rotation);
  Double_t vX  = cosRot * cosRot * fSigmaX * fSigmaX
               - 2. * cosRot * sinRot * fSigmaXY
               + sinRot * sinRot * fSigmaY * fSigmaY;
  Double_t vY  = sinRot * sinRot * fSigmaX * fSigmaX
               + 2. * cosRot * sinRot * fSigmaXY
               + cosRot * cosRot * fSigmaY * fSigmaY;
  Double_t vXY = cosRot * sinRot * fSigmaX * fSigmaX
               + ( cosRot*cosRot - sinRot*sinRot ) * fSigmaXY
               - cosRot * sinRot * fSigmaY * fSigmaY;
  fSigmaX  = TMath::Sqrt(vX);
  fSigmaY  = TMath::Sqrt(vY);
  fSigmaXY = vXY;

  //  TString errorString = Form("%.5f  -  %.5f  - %.5f",fSigmaX,fSigmaY,fSigmaXY);
  //   cout << "Hello, I am sector \"" << tempName.Data() << "\", errors: " 
  //        << errorString.Data() << endl;
}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
CbmStsSector::CbmStsSector(Int_t detId, Int_t iType, Double_t x0, 
			   Double_t y0, Double_t rotation, Double_t lx, 
			   Double_t ly, Double_t dx, Double_t dy, 
			   Double_t stereo) {

  fDetectorId = detId;
  fType       = iType;
  fX0         = x0;
  fY0         = y0;
  fRotation   = rotation;
  fLx         = lx;
  fLy         = ly;
  fDx         = dx;
  fDy         = dy;
  fStereo     = stereo;

  Double_t dbNoX = fLx / fDx;
  // Calculate number of channels
  if ( fType == 1 ) {             // Pixel sensor
    Double_t dbNoY = fLy / fDy;
    fNChannelsFront = Int_t( TMath::Ceil ( dbNoX ) 
			    *TMath::Ceil ( dbNoY ));
    fNChannelsBack  = 0;
  }
  else if ( fType == 2 ) {        // strip sensor
    fNChannelsFront = Int_t(TMath::Ceil ( dbNoX ));
    Double_t aPlus  = TMath::Abs( fLy * TMath::Tan(fStereo) );
    Double_t dbNoB = ( fLx + aPlus ) / fDx ;
    fNChannelsBack  = Int_t(TMath::Ceil ( dbNoB ));
  }
  else if ( fType == 3 ) {        // strip sensor with double metal layer
    fNChannelsFront = Int_t(TMath::Ceil ( dbNoX ));
    fNChannelsBack  = fNChannelsFront;
  }
  else {
    cout << "-E- CbmStsSector: Illegal sector type " << fType << endl;
    Fatal("", "Illegal sector type");
  }

  // Calculate errors and covariance
  if ( fType == 1 ) {
    fSigmaX  = fDx / TMath::Sqrt(12);
    fSigmaY  = fDy / TMath::Sqrt(12);
    fSigmaXY = 0.;
  }
  else if ( fType == 2 || fType == 3 ) {
    Double_t tanSt = TMath::Tan(fStereo);
    fSigmaX  = fDx / TMath::Sqrt(12);
    fSigmaY  = fDx / TMath::Sqrt(6) / TMath::Abs(tanSt);
    fSigmaXY = -1. * fDx * fDx / 12. / tanSt;
  }
  else {
    cout << "-E- CbmStsSector: Illegal sector type " << fType << endl;
    Fatal("", "Illegal sector type");
  }

  // Transform errors to global c.s. 
  Double_t cosRot = TMath::Cos(fRotation);
  Double_t sinRot = TMath::Sin(fRotation);
  Double_t vX  = cosRot * cosRot * fSigmaX * fSigmaX
               - 2. * cosRot * sinRot * fSigmaXY
               + sinRot * sinRot * fSigmaY * fSigmaY;
  Double_t vY  = sinRot * sinRot * fSigmaX * fSigmaX
               + 2. * cosRot * sinRot * fSigmaXY
               + cosRot * cosRot * fSigmaY * fSigmaY;
  Double_t vXY = cosRot * sinRot * fSigmaX * fSigmaX
               + ( cosRot*cosRot - sinRot*sinRot ) * fSigmaXY
               - cosRot * sinRot * fSigmaY * fSigmaY;
  fSigmaX  = TMath::Sqrt(vX);
  fSigmaY  = TMath::Sqrt(vY);
  fSigmaXY = vXY;

}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
CbmStsSector::~CbmStsSector() { };
// -------------------------------------------------------------------------



// -----   Public method GetChannel   --------------------------------------
Int_t CbmStsSector::GetChannel(Double_t x, Double_t y, Int_t iSide) {

  // Check iSide
  if (iSide !=0 && iSide != 1) {
    cout << "-W- CbmStsSector::GetChannel: Illegal side number " 
	 << iSide << endl;
    return -1;
  }

  // Calculate internal coordinates. Return -1 if outside sector.
  Double_t xint  = 0;
  Double_t yint  = 0;
  if ( ! IntCoord(x, y, xint, yint) ) return -1;
  Int_t    iChan = 0;

  Double_t dbNoX = fLx / fDx;
  // Case pixel sensor: iChannel = iRow * nColumns + iColumn
  if ( fType == 1 ) {
    Int_t nCol = Int_t(TMath::Ceil( dbNoX));
    Int_t iCol = Int_t(xint / fDx);
    Int_t iRow = Int_t(yint / fDy);
    iChan = iRow * nCol + iCol;
    if ( iChan < 0 || iChan > fNChannelsFront ) {
      cout << "-E- CbmStsSector::GetChannel: "
	   << "Channel number " << iChan << " exceeds limit " 
	   << fNChannelsFront << endl;
      cout << GetStationNr() << " " << GetSectorNr() << endl;
      cout << x << " " << y << " " << iSide << endl;
      Fatal("GetChannel", "illegal channel number");
    }
  }

  // Case strip sensor 1
  else if ( fType == 2 ) {
    if ( iSide == 0 ) {        // Front side
      iChan = Int_t ( xint / fDx );
      if ( iChan < 0 || iChan > fNChannelsFront ) {
	cout << "-E- CbmStsSector::GetChannel: "
	     << "Front channel number " << iChan << " exceeds limit " 
	     << fNChannelsFront << endl;
	Fatal("GetChannel", "illegal channel number");
      }
    }
    else {                     // Back side
      // Project point along backside strip to y=0
      Double_t xp = xint + yint * TMath::Tan(fStereo);
      // Digitise
      iChan = Int_t ( xp / fDx );
      if ( iChan < 0 || iChan > fNChannelsBack ) {
	cout << "-E- CbmStsSector::GetChannel: "
	     << "Back channel number " << iChan << " exceeds limit " 
	     << fNChannelsBack << endl;
	cout << "    Sector " << GetSectorNr() << " of station " 
	     << GetStationNr() << endl;
	cout << "    Point coordinates: (" << x << ", " << y 
	     << ") cm" << endl;
	Fatal("GetChannel", "illegal channel number");
      }
    }
  }

  // Case strip sensor 2 (with double metal layer)
  else if ( fType == 3 ) {
    if ( iSide == 0 ) {      // Front side
      iChan = Int_t ( xint / fDx );
      if ( iChan < 0 || iChan > fNChannelsFront ) {
	cout << "-E- CbmStsSector::GetChannel: "
	     << "Front channel number " << iChan << " exceeds limit " 
	     << fNChannelsFront << endl;
	Fatal("GetChannel", "illegal channel number");
      }
    }
    else {                   // Back side
      // Project point along backside strip to y = 0
      Double_t xp = xint + yint * TMath::Tan(fStereo);
      // Calculate modulo w.r.t. sector x width
      xp = xp - TMath::Floor(xp/fLx) * fLx;
      // Digitise
      iChan = Int_t( xp / fDx );
      if ( iChan < 0 || iChan > fNChannelsBack ) {
	cout << "-E- CbmStsSector::GetChannel: "
	     << "Back channel number " << iChan << " exceeds limit " 
	     << fNChannelsBack << endl;
	cout << "    Sector " << GetSectorNr() << " of station " 
	     << GetStationNr() << endl;
	cout << "    Point coordinates: (" << x << ", " << y 
	     << ") cm" << endl;	
	Fatal("GetChannel", "illegal channel number");
      }
    }
  }

  return iChan;
}
// -------------------------------------------------------------------------

    
// -----   Public method GetChannel   --------------------------------------
Float_t CbmStsSector::GetChannelPlus(Double_t x, Double_t y, Int_t iSide) {

  // Check iSide
  if (iSide !=0 && iSide != 1) {
    cout << "-W- CbmStsSector::GetChannel: Illegal side number " 
	 << iSide << endl;
    return -1;
  }

  // Calculate internal coordinates. Return -1 if outside sector.
  Double_t xint  = 0;
  Double_t yint  = 0;
  if ( ! IntCoord(x, y, xint, yint) ) return -1;
  Float_t    iChan = 0;

  Double_t dbNoX = fLx / fDx;
  // Case pixel sensor: iChannel = iRow * nColumns + iColumn
  if ( fType == 1 ) {
    Int_t nCol = Int_t(TMath::Ceil( dbNoX));
    Int_t iCol = Int_t(xint / fDx);
    Int_t iRow = Int_t(yint / fDy);
    iChan = iRow * nCol + iCol;
    if ( iChan < 0 || iChan > fNChannelsFront ) {
      cout << "-E- CbmStsSector::GetChannel: "
	   << "Channel number " << iChan << " exceeds limit " 
	   << fNChannelsFront << endl;
      cout << GetStationNr() << " " << GetSectorNr() << endl;
      cout << x << " " << y << " " << iSide << endl;
      Fatal("GetChannel", "illegal channel number");
    }
  }

  // Case strip sensor 1
  else if ( fType == 2 ) {
    if ( iSide == 0 ) {        // Front side
      iChan = Int_t ( xint / fDx );
      if ( iChan < 0 || iChan > fNChannelsFront ) {
	cout << "-E- CbmStsSector::GetChannel: "
	     << "Front channel number " << iChan << " exceeds limit " 
	     << fNChannelsFront << endl;
	Fatal("GetChannel", "illegal channel number");
      }
    }
    else {                     // Back side
      // Project point along backside strip to y=0
      Double_t xp = xint + yint * TMath::Tan(fStereo);
      // Digitise
      iChan = Int_t ( xp / fDx );
      if ( iChan < 0 || iChan > fNChannelsBack ) {
	cout << "-E- CbmStsSector::GetChannel: "
	     << "Back channel number " << iChan << " exceeds limit " 
	     << fNChannelsBack << endl;
	cout << "    Sector " << GetSectorNr() << " of station " 
	     << GetStationNr() << endl;
	cout << "    Point coordinates: (" << x << ", " << y 
	     << ") cm" << endl;
	Fatal("GetChannel", "illegal channel number");
      }
    }
  }

  // Case strip sensor 2 (with double metal layer)
  else if ( fType == 3 ) {
    if ( iSide == 0 ) {      // Front side
      iChan = xint / fDx;
      if ( iChan < 0 || iChan > fNChannelsFront ) {
	cout << "-E- CbmStsSector::GetChannel: "
	     << "Front channel number " << iChan << " exceeds limit " 
	     << fNChannelsFront << endl;
	Fatal("GetChannel", "illegal channel number");
      }
    }
    else {                   // Back side
      // Project point along backside strip to y = 0
      Double_t xp = xint + yint * TMath::Tan(fStereo);
      // Calculate modulo w.r.t. sector x width
      xp = xp - TMath::Floor(xp/fLx) * fLx;
      // Digitise
      iChan = xp / fDx;
      if ( iChan < 0 || iChan > fNChannelsBack ) {
	cout << "-E- CbmStsSector::GetChannel: "
	     << "Back channel number " << iChan << " exceeds limit " 
	     << fNChannelsBack << endl;
	cout << "    Sector " << GetSectorNr() << " of station " 
	     << GetStationNr() << endl;
	cout << "    Point coordinates: (" << x << ", " << y 
	     << ") cm" << endl;	
	Fatal("GetChannel", "illegal channel number");
      }
    }
  }

  return iChan;
}
// -------------------------------------------------------------------------

    



// -----   Public method Inside   ------------------------------------------
Bool_t CbmStsSector::Inside(Double_t x, Double_t y) {
  Double_t xint, yint;
  return IntCoord(x, y, xint, yint);
}
// -------------------------------------------------------------------------



// -----   Public method ActivateChannels   --------------------------------
Bool_t CbmStsSector::ActivateChannels(Int_t ipt, 
					   Double_t x, Double_t y) {

  Int_t iFront = FrontStripNumber(x, y);
  if (iFront < 0) return kFALSE;
  Int_t iBack  = BackStripNumber(x, y);
  if (iBack < 0) {
    cout << "-W- CbmStsSector::ActivateChannels: "
	 << "No back strip number!" << endl;
    cout << "    Coordinates: (" << x << ", " << y << ")" << endl;
    cout << "    Sector: " << fDetectorId << ", Front Strip: " 
	 << iFront << endl;
    return kFALSE;
  }

  fFrontActive.insert(iFront);
  fBackActive.insert(iBack);
  pair<Int_t,Int_t> a(iFront,iBack);
  fTrueHits[a] = ipt;

  return kTRUE;
}
// -------------------------------------------------------------------------

  


// -----   Public method Intersect   ---------------------------------------
Int_t CbmStsSector::Intersect(Int_t iFStrip, Int_t iBStrip,
				    vector<Double_t>& xCross, 
				    vector<Double_t>& yCross) {

  // Reset STL vector of intersection y coordinates
  xCross.clear();
  yCross.clear();

  // Check for strip numbers
  if ( iFStrip < 0 || iFStrip > fNChannelsFront) {
    cout << "-W- CbmStsSector::Intersect: "
	 << "Invalid front channel number ! "
	 << iFStrip << " " << fNChannelsFront << endl;
    return 0;
  }
  if ( iBStrip < 0 || iBStrip > fNChannelsBack) {
    cout << "-W- CbmStsSector::Intersect: "
	 << "Invalid back channel number ! "
	 << iBStrip << " " << fNChannelsBack << endl;
    return 0;
  }

  // x coordinate of centre of front side strip readout pad
  Double_t xF = Double_t( iFStrip + 0.5 ) * fDx;

  // x coordinate of centre of back side strip readout pad
  Double_t xB = Double_t( iBStrip + 0.5 ) * fDx;

  // Maximal number of back strip segments
  Int_t nSeg = Int_t ( fLy * TMath::Tan(fStereo) / fLx ) + 2;

  // Loop over back strip segments
  for (Int_t iSeg=0; iSeg<nSeg; iSeg++) {

    // y coordinate of intersection of back strip segment with front strip
    Double_t y = (xB - xF - Double_t(iSeg) * fLx) / TMath::Tan(fStereo);
    if ( y < 0 || y > fLy ) continue;

    // Transform x and y coordinates to the global c.s.
    Double_t xHit = xF * TMath::Cos(fRotation) -  y * TMath::Sin(fRotation);
    Double_t yHit = y  * TMath::Cos(fRotation) + xF * TMath::Sin(fRotation);

    // Fill coordinates in return arrays
    xCross.push_back(xHit);
    yCross.push_back(yHit);

  }

  return xCross.size();

}
// -------------------------------------------------------------------------



// -----   Public method PointIndex   --------------------------------------
Int_t CbmStsSector::PointIndex(Int_t iFStrip, Int_t iBStrip) {
  pair<Int_t,Int_t> a(iFStrip, iBStrip);
  if (fTrueHits.find(a) == fTrueHits.end()) return -1;
  return fTrueHits[a];
}
// -------------------------------------------------------------------------



// -----   Public method Reset   -------------------------------------------
void CbmStsSector::Reset() {
  fFrontActive.clear();
  fBackActive.clear();
  fTrueHits.clear();
}
// -------------------------------------------------------------------------



// -----   Public method Print   -------------------------------------------
void CbmStsSector::Print() {
  cout << "   Sector Nr. ";
  cout.width(3);
  cout << GetSectorNr() << ", Type ";
  cout.width(1);
  cout << fType << ", centre (";
  cout.width(6);
  cout << fX0 << ", ";
  cout.width(6);
  cout << fY0 << ") cm, rotation " ;
  cout.width(6);
  cout << fRotation*180./TMath::Pi() << " deg., lx = ";
  cout.width(3);
  cout << fLx << " cm, ly = ";
  cout.width(3);
  cout << fLy << " cm, channels: " << GetNChannels() << endl;
}
// -------------------------------------------------------------------------



// -----   Private method FrontStripNumber   -------------------------------
Int_t CbmStsSector::FrontStripNumber(Double_t x, Double_t y) const {

  Double_t xint = 0., yint = 0.;

  // Calculate internal coordinates. 
  // If point is inside sector, return strip number
  if ( IntCoord(x, y, xint, yint) ) {
  Int_t iStrip = Int_t( xint / fDx );
  if (iStrip < 0 || iStrip > fNChannelsFront) {
    cout << "-E- CbmStsSector::FrontStripNumber: Invalid strip number"
	 << "  " << iStrip << endl;
    cout << "    Sector " << fDetectorId << ", x = " << x << ", y = " 
	 << y << endl;
    Fatal("FrontStripNumber", "Invalid strip number" );
  }
  return iStrip;
  }

  // Return -1 if point is outside sector
  return -1;

}
// -------------------------------------------------------------------------



// -----   Private method BackStripNumber   --------------------------------
Int_t CbmStsSector::BackStripNumber(Double_t x, Double_t y) const {

  Double_t xint = 0., yint = 0.;

  // Calculate internal coordinates. 
  // If point is inside sector, calculate projection onto readout line
  // and determine channel number
  if ( IntCoord(x, y, xint, yint) ) {
    
    Double_t x0 = x + y * TMath::Tan(fStereo);
    Int_t nShift = Int_t( x0 / fLx );
    x0 = x0 - nShift * fLx;
    Int_t iStrip = Int_t ( x0 / fDx );
    if (iStrip < 0 || iStrip > fNChannelsBack) {
      cout << "-E- CbmStsSector::BackStripNumber: Invalid strip number"
	   << "  " << iStrip << endl;
      cout << "    Sector " << fDetectorId << ", x = " << x << ", y = " 
	   << y << endl;
      Fatal("BackStripNumber", "Invalid strip number" );
  }
    return iStrip;
  }

  // Return -1 if point is outside sector
  return -1;

}
// -------------------------------------------------------------------------



// -----   Private method IntCoord   ---------------------------------------
Bool_t CbmStsSector::IntCoord(Double_t x, Double_t y,
				   Double_t& xint, Double_t& yint) const {

  // Translation into sector centre system
  x = x - fX0;
  y = y - fY0;;

  // Rotation around the sector centre
  xint = x * TMath::Cos(fRotation) + y * TMath::Sin(fRotation);
  yint = y * TMath::Cos(fRotation) - x * TMath::Sin(fRotation);

  // Translation into sector corner system
  xint = xint + fLx/2.;
  yint = yint + fLy/2.;

  // Check whether point is inside the sector
  if ( ! IsInside(xint, yint) ) {
    xint = yint = 0.;
    return kFALSE;
  }

  return kTRUE;

}
// -------------------------------------------------------------------------



// -----   Private method IsInside   ---------------------------------------
Bool_t CbmStsSector::IsInside(Double_t xint, Double_t yint) const {
  if ( xint < 0.  ) return kFALSE;
  if ( xint > fLx ) return kFALSE;
  if ( yint < 0.  ) return kFALSE;
  if ( yint > fLy ) return kFALSE;
  return kTRUE;
}
// -------------------------------------------------------------------------




ClassImp(CbmStsSector)
