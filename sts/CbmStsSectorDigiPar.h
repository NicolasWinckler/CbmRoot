// -------------------------------------------------------------------------
// -----                 CbmStsSectorDigiPar header file               -----
// -----                 Created 27/06/05  by V. Friese                -----
// -------------------------------------------------------------------------


/** CbmStsSectorDigiPar.h
 *@author V.Friese <v.friese@gsi.de>
 **
 ** Parameter container for the digitisation of a sector of a STS
 ** strip station.
 ** Supports the following digitisation types:
 ** 1 = hybrid pixel
 ** 2 = strip, readout of all strips
 ** 3 = strip, readout at bottom corner, double metal layer
 ** Coordinates and rot. angle are in the global coordinate system. 
 ** In the internal coordinate system, x is across the front side strips
 ** and y along the front side strips. Irrelevant for pixel detectors.
 ** Angles are stored and accessed in radians, but must be given in
 ** degrees in the constructor.
 **/



#ifndef CBMSTSSECTORDIGIPAR_H
#define CBMSTSSECTORDIGIPAR_H 1



#include "TObject.h"



class CbmStsSectorDigiPar : public TObject
{

 public:

  /** Default constructor **/
  CbmStsSectorDigiPar();


  /** Standard constructor. For the parameters, see description of
   ** private data members. Angles must be given in radians. **/
  CbmStsSectorDigiPar(Int_t iSector, Int_t iType, Double_t x0,
		      Double_t y0, Double_t rotation, Double_t lx, 
		      Double_t ly, Double_t dx, Double_t dy,
		      Double_t stereo);
  CbmStsSectorDigiPar(Int_t iSector, Int_t iType, Double_t x0,
		      Double_t y0, Double_t z0, Double_t rotation, Double_t lx, 
		      Double_t ly, Double_t d, Double_t dx, Double_t dy,
		      Double_t stereo);


  /** Destructor **/
  virtual ~CbmStsSectorDigiPar();


  /** Accessors. See private data members for description **/
  Int_t    GetSectorNr()   const { return fSectorNr; }
  Int_t    GetType()       const { return fType; }       // 1, 2 or 3
  Double_t GetX0()         const { return fX0; }         // [cm]
  Double_t GetY0()         const { return fY0; }         // [cm]
  Double_t GetZ0()         const { return fZ0; }         // [cm]
  Double_t GetRotation()   const { return fRotation; }   // [rad]
  Double_t GetLx()         const { return fLx; }         // [cm]
  Double_t GetLy()         const { return fLy; }         // [cm]
  Double_t GetD()          const { return fD;  }
  Double_t GetDx()         const { return fDx; }         // [cm]
  Double_t GetDy()         const { return fDy; }         // [cm]
  Double_t GetStereo()     const { return fStereo; }     // [rad]

  void SetZ0(Double_t z0)        { fZ0 = z0; }         // [cm]
  void SetD (Double_t d)         { fD  = d;  }         // [cm]


 private:

  /** Sector number in station **/	
  Int_t    fSectorNr;

  /** Type of sector.
   ** 1 = hybrid pixel sensor, no charge sharing;
   ** 2 = strip sensor, readout of all strips on the back side
   ** 3 = strip sensor, readout at bottom corner, double metal layer **/
  Int_t    fType;

  /** x coordinate of sector centre in the station c.s. [cm] **/
  Double_t fX0;

  /** y coordinate of sector centre in the station c.s. [cm] **/
  Double_t fY0;

  /** Rotation angle around z axis in the station c.s [rad] **/
  Double_t fRotation;

  /** Length of sector along x axis [cm] **/
  Double_t fLx;

  /** Length of sector along y axis [cm] **/
  Double_t fLy;

  /** Strip readout pitch or pixel size along x [cm] **/ 
  Double_t fDx;

  /** Pixel size along y [cm]. Irrelevant for strip sensor. **/
  Double_t fDy;

  /** Stereo angle of back side strips. Irrelevant for pixel sensors. **/
  Double_t fStereo;

  Double_t fZ0;           // z position of the sector [cm]
  Double_t fD;            // thickness of the sector [cm]

  ClassDef(CbmStsSectorDigiPar,1);

};


#endif
