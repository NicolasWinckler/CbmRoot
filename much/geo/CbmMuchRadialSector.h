/** CbmMuchRadialSector.h
 *@author  E.Kryshen <e.kryshen@gsi.de>
 *@since   6.02.12
 *@version 1.0
 **
 ** This class describes the digitization scheme for a ring of pads
 **
 **/
#ifndef CbmMuchRadialSector_H
#define CbmMuchRadialSector_H 1

#include "TString.h"
#include "TCrown.h"
#include "TObjArray.h"
#include "TVector3.h"
#include "TArrayI.h"

#include <vector>


class CbmMuchPad;

using std::vector;

class CbmMuchRadialSector : public TCrown
{

 public:

  /** Default constructor **/
  CbmMuchRadialSector();

  /** Standard constructor
   */
  CbmMuchRadialSector(Int_t detId, Int_t iSector, Double_t rMin, Double_t rMax, Double_t phi0, Double_t pad_angle, Int_t nPads);
  
  /** Destructor */
  virtual ~CbmMuchRadialSector();

  /** Gets detector id including module number.   */
  Int_t GetDetectorId()  const { return fDetectorId; }

  /** Gets sector index within the module.   */
  Int_t GetSectorIndex() const { return fSectorIndex;}

  /** Gets number of channels. */
  Int_t    GetNChannels()  const { return fNChannels; }

  /** Calculate channel number for a coordinate pair.
   *@param x   Global x coordinate [cm]
   *@param y   Global y coordinate [cm]
   *@return    Number of a channel. -1 if the point is outside the sector.
   */
  Int_t GetChannel(Double_t x, Double_t y);
  Int_t GetChannel(Double_t phi);
  // TODO
  Double_t GetPadAngle() const { return fPadAngle; }
  // TODO
  void AddPads();
  void DrawPads();
private:
  Int_t      fDetectorId;   // Detector ID (including module number)
  Int_t      fSectorIndex;  // Sector index within the module
  Int_t      fNChannels;    // Number of channels
  TObjArray  fPads;         // Array of pads corresponding to different channels
  TArrayI    fNeighbours;   // Array of indices of neighbour sectors
  Double_t   fPadAngle;     // Pad azimuthal angle in degrees
  Double_t   fPhi0;         // 
  ClassDef(CbmMuchRadialSector,1);
};




#endif
