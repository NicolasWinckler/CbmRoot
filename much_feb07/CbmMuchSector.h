/** CbmMuchSector.h
 *@author  M.Ryzhinskiy <m.ryzhinskiy@gsi.de>
 *@since   15.03.07
 *@version 1.0
 **
 ** This class describes the digitization scheme for a sector of the MuCh.
 ** The sector is a rectangle of size fLx and fLy. The following
 ** types are implemented:
 ** 1 = GEMs
 ** 2 = CPCs
 **/


#ifndef CBMMUCHSECTOR_H
#define CBMMUCHSECTOR_H 1


#include "TNamed.h"
#include "TPolyLine.h"
#include "TObjArray.h"


#include <map>
#include <set>
#include <vector>

class CbmMuchSectorDigiPar;
class CbmMuchDigitize;
class CbmMuchPad;

class CbmMuchSector : public TNamed
{

 public:

  /** Default constructor **/
  CbmMuchSector();


  /** Standard constructor
   *@param detId     Unique detector identifier
   *@param iType     Sensor type (1,2)
   *@param x0        sector centre x coordinate in global c.s [cm]
   *@param y0        sector centre y coordinate in global c.s [cm]
   *@param rotation  rotation in global c.s. [rad]
   *@param lx        sector length in x [cm]
   *@param ly        sector length in y [cm]
   *@param nCols     number of pads in horizontal direction
   *@param nRows     number of pads in vertical direction
   *@param iCannels  Number of channels the sector has.
   **/
  CbmMuchSector(Int_t detId, Int_t iType, Double_t x0, Double_t y0,
	       Double_t rotation, Double_t lx, Double_t ly, 
	       Int_t nCols, Int_t nRows, Int_t iChannels = 128);
  CbmMuchSector(TString tempName, Int_t detId, Int_t iType, Double_t x0, Double_t y0, Double_t z0,
	       Double_t rotation, Double_t lx, Double_t ly, 
	       Double_t d, Int_t nCols, Int_t nRows, Int_t iChannels = 128);


  /** Destructor **/
  virtual ~CbmMuchSector();


  /** Accessors **/
  inline TString GetDetectorName() const { return fName.Data(); }
  inline Int_t    GetDetectorId() const { 
    return fDetectorId; }
  inline Int_t    GetSystemId()   const {  
    return ( fDetectorId & (15<<24) ) >> 24; }
  inline Int_t    GetStationNr()  const { 
    return ( fDetectorId & (255<<16) ) >> 16; }
  inline Int_t    GetSectorNr()   const {  // sector number within the station
    return ( fDetectorId & (32767<<1) ) >> 1; }
  inline Int_t    GetType()       const { return fType; }
  inline Double_t GetX0()         const { return fX0; }
  inline Double_t GetY0()         const { return fY0; }
  inline Double_t GetZ0()         const { return fZ0; }
  inline Double_t GetRotation()   const { return fRotation; }
  inline Double_t GetSinRot()     const { return fSinRot; }
  inline Double_t GetCosRot()     const { return fCosRot; }
  inline Double_t GetLx()         const { return fLx; }
  inline Double_t GetLy()         const { return fLy; }
  inline Double_t GetDx()         const { return fDx; }
  inline Double_t GetDy()         const { return fDy; }
  inline Double_t GetD()          const { return fD;  }
  inline Int_t    GetNCols()      const { return fNCols; }
  inline Int_t    GetNRows()      const { return fNRows; }
  inline Double_t GetSigmaX()     const { return fSigmaX; }
  inline Double_t GetSigmaY()     const { return fSigmaY; }
  inline Double_t GetSigmaXY()    const { return fSigmaXY; }
  inline Int_t    GetNChannels()  const { return fNChannels; }

  //-------------------- Pad info begin -----------------------------------//
  /** Fills an array of CbmMuchPad objects **/
  void AddPads();
  /** Gets vertices coordinates of a pad corresponding to a given channel number **/
  void GetPadVertices(Int_t iChannel, Double_t* xPad, Double_t* yPad);
  inline CbmMuchPad* GetPad(Int_t iChannel) const { return fPads[iChannel]; }
  inline Double_t GetPadRadius()  const { return fPadRadius; }
  TPolyLine GetPadPolygon(Int_t iChannel);
  Double_t GetPadX0(Int_t iChannel);
  Double_t GetPadY0(Int_t iChannel);
  //-------------------- Pad info end -------------------------------------//


  /** Adds a sector to the list of neighbour sectors of this one. **/
  void AddNeighbour(CbmMuchSector* sector);
  std::map<Int_t, CbmMuchSector*> GetNeighbours() { return fNeighbours; } 

  /** Calculate channel number for a coordinate pair.
   *@param x      x coordinate in global c.s. [cm]
   *@param y      y coordinate in global c.s. [cm]
   *@value iChan  channel number. -1 if point is outside sector.
   **/
  Int_t GetChannel(Double_t x, Double_t y);


  /** Test whether a coordinate pair (x,y) in global coordinates is
   ** inside the sector **/
  Bool_t Inside(Double_t x, Double_t y);


  /** Activate the channels corresponding to a MCPoint.
   *@param ipt   Index of MCPoint
   *@param x     x coordinate of point (global c.s.)
   *@param y     y coordinate of point (global c.s.)
   *@value  kTRUE if the point is inside the sector, else kFALSE
   **/
  Bool_t ActivateChannels(Int_t ipt, Double_t x, Double_t y);


  /** The index of the MCPoint that has caused a pad to be fired.
   *@param iPad   Pad index
   *@value Index of MCPoint
   **/
  Int_t GetPointIndex(Int_t iPad);


  /** Clear the maps of fired strips **/
  void Reset();


  /** Screen output  **/
  void Print();
  


 private:

  /** -------------   Data members   --------------------------**/

  TString    fName;         // Station name
  Int_t      fDetectorId;   // Unique detector ID
  Int_t      fType;         // Sensor type 
  Double_t   fX0, fY0;      // Coordinates of the sector centre [cm]
  Double_t   fRotation;     // Rotation angle in global c.s. [rad]
  Double_t   fSinRot;       // Sin(fRotation)
  Double_t   fCosRot;       // Cos(fRotation)
  Double_t   fLx, fLy;      // Dimensions of the sector [cm]
  Int_t      fNCols;        // Number of pads in horizontal direction
  Int_t      fNRows;        // Number of pads in vertical direction
  Double_t   fZ0;           // z position of the sector [cm]
  Double_t   fD;            // Thickness of the sector [cm]
  Int_t      fNChannels;    // Number of channels
  Double_t   fDx;           // Width of pads [cm]
  Double_t   fDy;           // Length of pads [cm]
  Double_t   fPadRadius;    // Pad radius [cm]
//  TObjArray* fPads;         // Array of pads
  std::vector<CbmMuchPad*> fPads;


  /** STL map from number of sector to the CbmMuchSector
   * for each sector, which is neighbour to this one. 
   **/
  std::map<Int_t, CbmMuchSector*> fNeighbours; 

  /** Errors of hit coordinates: size/sqrt(12) **/
  // Logically, this belongs to the HitFinder, but it is here
  // for performance reasons: to be executed once per sector, not for
  // each MCPoint
  Double_t fSigmaX;   // RMS in x, global c.s. [cm]
  Double_t fSigmaY;   // RMS in y, global c.s. [cm]
  Double_t fSigmaXY;  // Covariance in global c.s. [cm**2]

  /** STL sets containing the active channels **/
  std::set<Int_t> fActive;       //!

  /** STL map from the index of the fired pad to the 
   ** index of the MCPoint **/
  std::map< Int_t, Int_t > fTrueHits;      //!

  /** -------------   Private methods   ------------------------**/

  /** Calculate coordinates in internal coordinate system
   *@param x     x coordinate in global system
   *@param y     y coordinate in global system
   *@param xint  internal x coordinate (return)
   *@param yint  internal y coordinate (return)
   *@value kTRUE if point is inside the sector
   **/
  Bool_t IntCoord(Double_t x, Double_t y, 
		  Double_t& xint, Double_t& yint) const;

  
  /** Check whether a point is inside the sector
   *@param xint  x coordinate of point (internal system)
   *@param yint  y coordinate of point (internal system)
   *@value kTRUE if inside sector, else kFALSE
   **/
  Bool_t IsInside(Double_t xint, Double_t yint) const;


  friend class CbmMuchDigitize;

  ClassDef(CbmMuchSector,1);

};




#endif
