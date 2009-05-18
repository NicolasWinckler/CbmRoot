/** CbmMuchModule.h
 *
 *@author  M.Ryzhinskiy <m.ryzhinskiy@gsi.de>
 *@version 1.0
 *@since   11.02.08
 **
 ** This class holds geometry parameters of much modules
 **
 **/

#ifndef CBMMUCHMODULE_H
#define CBMMUCHMODULE_H 1

#include "CbmMuchGeoScheme.h"

#include "TObject.h"
#include "TVector3.h"
#include "TPave.h"


class CbmMuchModule : public TPave
{

public:

  /** Default constructor **/
  CbmMuchModule();

  /** Standard constructor
   *@param detId     Detector ID
   *@param position  Location of the module center in global c.s. (all dimensions in [cm])
   *@param size      Size of the module (all dimensions in [cm])
   *@param cutRadius Radius of the cut (if any, otherwise = -1.) [cm]
   **/
  CbmMuchModule(Int_t detId, TVector3 position, TVector3 size, Double_t cutRadius);

  /** Standard constructor
   *@param iStation  Station index
   *@param iLayer    Layer index
   *@param iSide     Defines side of the layer (0 - Front, 1 - Back)
   *@param iModule   Module index
   *@param position  Location of the module center in global c.s. (all dimensions in [cm])
   *@param size      Size of the module (all dimensions in [cm])
   *@param cutRadius Radius of the cut (if any, otherwise = -1.) [cm]
   **/
  CbmMuchModule(Int_t iStation, Int_t iLayer, Bool_t iSide, Int_t iModule, TVector3 position, TVector3 size, Double_t cutRadius);

  /** Destructor **/
  virtual ~CbmMuchModule();

  /** Accessors */
  Int_t    GetDetectorId()    const { return fDetectorId; }
  Double_t GetCutRadius()     const { return fCutRadius; }
  TVector3 GetSize()           const { return fSize; }
  TVector3 GetPosition()      const { return fPosition; }
  Int_t    GetDetectorType()  const { return fDetectorType; }

  virtual Bool_t InitModule() {}
  virtual void DrawModule(Color_t color) {}


protected:
  Int_t                  fDetectorId;            // Unique detector ID
  Int_t                  fDetectorType;          // Detector type
  Double_t               fCutRadius;             // Radius of the cut (if any, otherwise = -1.) [cm]
  TVector3               fSize;                  // Size vector of the module (all dimensions in [cm])
  TVector3               fPosition;              // Location vector of the module center in global c.s. (all dimensions in [cm])

  ClassDef(CbmMuchModule,1);
};
#endif


