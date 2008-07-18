/** CbmMuchLayerSide.h
 *@author  M.Ryzhinskiy <m.ryzhinskiy@gsi.de>
 *@version 1.0
 *@since   11.02.08
 **
 ** This class holds the transport geometry parameters 
 ** of one MuCh tracking layer side.
 **
 **/


#ifndef CBMMUCHLAYERSIDE_H
#define CBMMUCHLAYERSIDE_H 1

#include "TObjArray.h"
#include "Gtypes.h"

class CbmMuchModule;

class CbmMuchLayerSide : public TObject
{

 public:

  /** Default constructor **/
  CbmMuchLayerSide();

  /** Standard constructor 
  *@param detId     Detector identofier 
  *@param z         z position of station centre [cm]
  **/
  CbmMuchLayerSide(Long64_t detId, Double_t z);

  /** Standard constructor 
   *@param iStation  Station ID
   *@param iLayer    Layer ID 
   *@param iSide     Defines side (0 - Front, 1 - Back)
   *@param z         z position of station centre [cm]
   **/
  CbmMuchLayerSide(Int_t iStation, Int_t iLayer, Bool_t iSide, Double_t z);

  /** Destructor **/
  virtual ~CbmMuchLayerSide();

  /** Accessors **/
  Long64_t   GetDetectorId() const { return fDetectorId; }
  Int_t      GetNModules()   const { return fModules.GetEntriesFast(); }
  TObjArray* GetModules()          { return &fModules; }
  Double_t   GetZ()                { return fZ;}
  void SetZ(Double_t z) { fZ = z; }
  
  CbmMuchModule* GetModule(Int_t iModule) const { return (CbmMuchModule*)fModules.At(iModule); }

  /** Adds given CbmMuchModule to the internal list. 
   *@param module  CbmMuchModule which should be added to the array. **/
  void AddModule(CbmMuchModule* module);

  void DrawModules(Color_t color=kYellow, 
                   Bool_t modulesVisible=true, Bool_t sectorsVisible=true);
  
 protected:
  Long64_t   fDetectorId;                   // Unique detector ID
  Double32_t fZ;                            // z position of station centre (midplane) [cm]
  TObjArray  fModules;                      // Array of CbmMuchModule objects

  ClassDef(CbmMuchLayerSide,1);
};
#endif
