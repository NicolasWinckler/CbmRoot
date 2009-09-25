#ifndef CBMTRDMODULE_H
#define CBMTRDMODULE_H 1

#include "TNamed.h"

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
//  CbmTrdModule(Int_t detId, Double_t x, Double_t y, Double_t z, 
//               Double_t sizex, Double_t sizey);

  CbmTrdModule(Int_t detId, Double_t x, Double_t y, Double_t z, 
               Double_t sizex, Double_t sizey, 
               Double_t padsizex, Double_t padsizey, Bool_t rotated);

  /** Destructor **/
  virtual ~CbmTrdModule();


  /** Accessors **/
  Int_t    GetDetectorId() const { return fDetectorId; }
  Double_t GetX()          const { return fX; }
  Double_t GetY()          const { return fY; }
  Double_t GetZ()          const { return fZ; }
  Double_t GetSizex()      const { return fSizex; }
  Double_t GetSizey()      const { return fSizey; }
  Double_t GetPadSizex()   const { return fPadSizex; }
  Double_t GetPadSizey()   const { return fPadSizey; }
  Bool_t IsRotated()     const { return fIsRotated; }

 private:

  /** -------------   Data members   --------------------------**/

  Int_t    fDetectorId;   // Unique detector ID
  Double_t fX;            // center of module in global c.s. [cm]
  Double_t fY;            // center of module in global c.s. [cm]
  Double_t fZ;            // center of module in global c.s. [cm]
  Double_t fSizex;        // module size in x [cm]
  Double_t fSizey;        // module size in y [cm]	      
  Double_t fPadSizex;     // size of the readout pad in x [cm]
  Double_t fPadSizey;     // size of the readout pad in x [cm]
  Bool_t   fIsRotated;    // Flag shows chamber rotation
 
  ClassDef(CbmTrdModule,1);

};




#endif
