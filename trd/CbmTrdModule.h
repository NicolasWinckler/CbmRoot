#ifndef CBMTRDMODULE_H
#define CBMTRDMODULE_H 1

#include "TNamed.h"
#include "TArrayD.h"

class CbmTrdPoint;
class TVector3;

class CbmTrdModule : public TNamed
{

 public:

  /** Default constructor **/
  CbmTrdModule();


  /** Standard constructor
   *@param detId     Unique detector identifier
   *@param x0        sector centre x coordinate in global c.s [cm]
   *@param y0        sector centre y coordinate in global c.s [cm]
   *@param z0        sector centre z coordinate in global c.s [cm]
   *@param sizex     module size in x [cm]
   *@param sizey     module size in y [cm]
   *@param padsizex  size of the readout pad in x [cm]
   *@param padsizey  size of the readout pad in y [cm]
   **/

  CbmTrdModule(Int_t detId, Double_t x, Double_t y, Double_t z, 
               Double_t sizex, Double_t sizey, 
               Bool_t rotated);

  CbmTrdModule(Int_t detId, Double_t x, Double_t y, Double_t z, 
               Double_t sizex, Double_t sizey, Int_t nSectors, 
               TArrayD sectorX, TArrayD sectorY, TArrayD sectorZ,
               TArrayD sectorSizeX, TArrayD sectorSizeY,
               TArrayD padSizeX, TArrayD padSizeY,      
               Bool_t rotated); 

  CbmTrdModule(Int_t detId, Double_t x, Double_t y, Double_t z, 
               Double_t sizex, Double_t sizey, Int_t nSectors, 
               TArrayD sectorSizeX, TArrayD sectorSizeY,
               TArrayD padSizeX, TArrayD padSizeY,      
               Bool_t rotated); 


  /** Destructor **/
  virtual ~CbmTrdModule();

  /** Accessors **/
  Int_t    GetDetectorId() const { return fDetectorId; }
  Double_t GetX()          const { return fX; }
  Double_t GetY()          const { return fY; }
  Double_t GetZ()          const { return fZ; }

  Double_t GetSizex()      const { return fSizex; }
  Double_t GetSizey()      const { return fSizey; }

  Double_t GetPadSizex(Int_t i)   const { return fPadSizex.At(i); }
  Double_t GetPadSizey(Int_t i)   const { return fPadSizey.At(i); }
  Double_t GetSectorSizex(Int_t i)   const { return fSectorSizex.At(i); }
  Double_t GetSectorSizey(Int_t i)   const { return fSectorSizey.At(i); }

  Int_t    GetSector(Double_t *local_point);

  Bool_t IsRotated()     const { return fIsRotated; }


  void GetPadInfo(CbmTrdPoint *trdPoint, Int_t &Col, 
                  Int_t &Row, Int_t &PadId);

  void GetPosition(const Int_t Col, const Int_t Row, 
                   const Int_t VolumeId, const Int_t Sector, 
                   TVector3 &posHit, TVector3 &posHitErr);

 private:

  /** -------------   Data members   --------------------------**/

  Int_t    fDetectorId;   // Unique detector ID
  Double_t fX;            // center of module in global c.s. [cm]
  Double_t fY;            // center of module in global c.s. [cm]
  Double_t fZ;            // center of module in global c.s. [cm]
  Double_t fSizex;        // module size in x [cm]
  Double_t fSizey;        // module size in y [cm]
	      
  Int_t    fNoSectors;    // Number sectors for this Module
  TArrayD  fSectorX;      // center of sectors local c.s. [cm]
  TArrayD  fSectorY;      // center of sectors local c.s. [cm]
  TArrayD  fSectorZ;      // center of sectors local c.s. [cm]
  TArrayD  fSectorBeginX;  // begin of sector [cm]
  TArrayD  fSectorBeginY;  // begin of sector [cm]
  TArrayD  fSectorEndX;  // end of sector [cm]
  TArrayD  fSectorEndY;  // end of sector [cm]
  TArrayD  fSectorSizex;  // sector size in x [cm]
  TArrayD  fSectorSizey;  // sector size in y [cm]

  TArrayD  fPadSizex;     // size of the readout pad in x [cm]
  TArrayD  fPadSizey;     // size of the readout pad in y [cm]


  Bool_t   fIsRotated;    // Flag shows chamber rotation
 
  /** --------------- private functions ----------------------**/

  void GetModuleInformation(Int_t VolumeID, Double_t *local_point, 
                            Int_t &Col, Int_t &Row);


  ClassDef(CbmTrdModule,2);

};




#endif
