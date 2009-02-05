/** CbmMuchSectorDigiPar.h
 *@author   M.Ryzhinskiy <m.ryzhinskiy@gsi.de>
 *@version  1.0
 *@since    14.03.07
 **
 ** Parameter container for the digitization of a sector of a MuCh
 ** station.
 ** Supports the following digitisation types:
 ** 1 = GEMs
 ** 2 = CPC
 **
 ** Coordinates are given in the station coordinate system. 
 **/



#ifndef CBMMUCHSECTORDIGIPAR_H
#define CBMMUCHSECTORDIGIPAR_H 1



#include "TObject.h"



class CbmMuchSectorDigiPar : public TObject
{

 public:

  /** Default constructor **/
  CbmMuchSectorDigiPar();


  /** Standard constructor. For the parameters, see description of
   ** private data members.
   **/
  CbmMuchSectorDigiPar(Int_t iSector, Int_t iType, Double_t x0,
		       Double_t y0, Double_t lx, 
		       Double_t ly, Int_t nCols, Int_t nRows);
  CbmMuchSectorDigiPar(Int_t iSector, Int_t iType, Double_t x0,
		       Double_t y0, Double_t z0, Double_t lx, 
		       Double_t ly, Double_t d, Int_t nCols, Int_t nRows);


  /** Destructor **/
  virtual ~CbmMuchSectorDigiPar();


  /** Accessors. See private data members for description **/
  Int_t    GetSectorNr()   const { return fSectorNr; }
  Int_t    GetType()       const { return fType; }       // 1 or 2
  Double_t GetX0()         const { return fX0; }         // [cm]
  Double_t GetY0()         const { return fY0; }         // [cm]
  Double_t GetZ0()         const { return fZ0; }         // [cm]
  Double_t GetLx()         const { return fLx; }         // [cm]
  Double_t GetLy()         const { return fLy; }         // [cm]
  Double_t GetD()          const { return fD;  }
  Int_t    GetNCols()      const { return fNCols; }
  Int_t    GetNRows()      const { return fNRows; }

  void SetZ0(Double_t z0)        { fZ0 = z0; }         // [cm]
  void SetD (Double_t d)         { fD  = d;  }         // [cm]


 private:

  /** Sector number in station **/	
  Int_t    fSectorNr;

  /** Type of sector.
   ** 1 = GEM;
   ** 2 = CPC
   **/
  Int_t    fType;

  /** x coordinate of sector centre in the station c.s. [cm] **/
  Double_t fX0;

  /** y coordinate of sector centre in the station c.s. [cm] **/
  Double_t fY0;

  /** Length of sector along x axis [cm] **/
  Double_t fLx;

  /** Length of sector along y axis [cm] **/
  Double_t fLy;

  /** Number of pads in horizontal direction  **/ 
  Int_t fNCols;

  /** Number of pads in vertical direction  **/ 
  Int_t fNRows;

  Double_t fZ0;           // z position of the sector [cm]
  Double_t fD;            // thickness of the sector [cm]

  ClassDef(CbmMuchSectorDigiPar,1);

};


#endif
