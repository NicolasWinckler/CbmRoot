/** CbmMuchSector.h
 *@author  M.Ryzhinskiy <m.ryzhinskiy@gsi.de>
 *@since   15.03.07
 *@version 1.0
 **
 ** This class describes the digitization scheme for a sector of the MuCh.
 ** The sector is a rectangle of size fLx and fLy. 
 **
 **/
#ifndef CBMMUCHSECTOR_H
#define CBMMUCHSECTOR_H 1

#include "TString.h"
#include "TPolyLine.h"
#include "TObjArray.h"
#include "TPave.h"
#include "TVector3.h"
#include "TArrayL64.h"

#include <vector>

class CbmMuchDigitize;
class CbmMuchPad;

using std::vector;

class CbmMuchSector : public TPolyLine
{

 public:

  /** Default constructor **/
  CbmMuchSector();

  /** Standard constructor **/
  CbmMuchSector(Long64_t detId, TVector3 position, TVector3 size,
		             Int_t nCols, Int_t nRows, 
		             Int_t iChannels = 128);
  /** Destructor **/
  virtual ~CbmMuchSector();

  /** Accessors **/
  Long64_t GetDetectorId()  const { return fDetectorId; }
  TVector3 GetPosition()    const { return fPosition; }
  TVector3 GetSize()        const { return fSize; }
  Double_t GetDx()         const { return fDx; }
  Double_t GetDy()         const { return fDy; }
  Int_t    GetNCols()      const { return fNCols; }
  Int_t    GetNRows()      const { return fNRows; }
  Double_t GetSigmaX()     const { return fSigmaX; }
  Double_t GetSigmaY()     const { return fSigmaY; }
  Int_t    GetNChannels()  const { return fNChannels; }

  //-------------------- Pad info begin -----------------------------------//
  /** Fills an array of CbmMuchPad objects **/
  void AddPads();
  /** Gets vertices coordinates of a pad corresponding to a given channel number **/
  void GetPadVertices(Int_t iChannel, Double_t* xPad, Double_t* yPad);
  CbmMuchPad* GetPad(Int_t iChannel) const { return (CbmMuchPad*)fPads.At(iChannel); }
  Double_t GetPadRadius()  const { return fPadRadius; }
  Double_t GetPadX0(Int_t iChannel);
  Double_t GetPadY0(Int_t iChannel);
  //-------------------- Pad info end -------------------------------------//

  void SetNeighbours(TArrayL64 neighbourIDs) {  fNeighbours = neighbourIDs; }
  vector<CbmMuchSector*> GetNeighbours();

  /** Calculate channel number for a coordinate pair.
   *@param x      x coordinate in global c.s. [cm]
   *@param y      y coordinate in global c.s. [cm]
   *@value iChan  channel number. -1 if point is outside sector.
   **/
  Int_t GetChannel(Double_t x, Double_t y);

  /** Test whether a coordinate pair (x,y) in global coordinates is
   ** inside the sector **/
  Bool_t Inside(Double_t x, Double_t y);

  /** Screen output  **/
  void PrintInfo();
  
private:
  Long64_t   fDetectorId;   // Unique detector ID
  TVector3   fPosition;     // Sector position in global c.s.
  TVector3   fSize;         // Sector size (all dimensions in [cm])
  Int_t      fNCols;        // Number of pads in horizontal direction
  Int_t      fNRows;        // Number of pads in vertical direction
  Int_t      fNChannels;    // Number of channels
  Double_t   fDx;           // Width of pads [cm]
  Double_t   fDy;           // Length of pads [cm]
  Double_t   fPadRadius;    // Pad radius [cm]
  TObjArray  fPads;         // Array of pads corresponding to different channels
  TArrayL64  fNeighbours;   // Array of IDs of neighbour sectors

  /** Errors of hit coordinates: size/sqrt(12) **/
  // Logically, this belongs to the HitFinder, but it is here
  // for performance reasons: to be executed once per sector, not for
  // each MCPoint
  Double_t fSigmaX;   // RMS in x, global c.s. [cm]
  Double_t fSigmaY;   // RMS in y, global c.s. [cm]

  /** Calculate coordinates in internal coordinate system
   *@param x     x coordinate in global system
   *@param y     y coordinate in global system
   *@param xint  internal x coordinate (return)
   *@param yint  internal y coordinate (return)
   *@value kTRUE if point is inside the sector
   **/
  Bool_t IntCoord(Double_t x, Double_t y, 
		  Double_t& xint, Double_t& yint);
  
  /** Check whether a point is inside the sector
   *@param xint  x coordinate of point (internal system)
   *@param yint  y coordinate of point (internal system)
   *@value kTRUE if inside sector, else kFALSE
   **/
  Bool_t IsInside(Double_t xint, Double_t yint);

  ClassDef(CbmMuchSector,1);
};




#endif
