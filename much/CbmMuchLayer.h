/** CbmMuchLayer.h
 *@author  M.Ryzhinskiy <m.ryzhinskiy@gsi.de>
 *@version 1.0
 *@since   11.02.08
 **
 ** This class holds the transport geometry parameters
 ** of one MuCh tracking layer.
 **
 **/


#ifndef CBMMUCHLAYER_H
#define CBMMUCHLAYER_H 1

#include "TObject.h"

#include "CbmMuchLayerSide.h"


class CbmMuchLayer : public TObject
{

 public:

  /** Default constructor **/
  CbmMuchLayer();

  /** Standard constructor
   *@param detId     Detector ID
   *@param z         z position of layer center [cm]
   *@param zRel      z position of layer center relative to station center [cm]
   *@param dz        half-thickness of the layer [cm]
   **/
  CbmMuchLayer(Int_t detId, Double_t z, Double_t zRel, Double_t dz);

  /** Standard constructor
   *@param iStation  Station index
   *@param iLayer    Layer index
   *@param z         z position of layer center [cm]
   *@param zRel      z position of layer center relative to station center [cm]
   *@param dz        half-thickness of the layer [cm]
   **/
  CbmMuchLayer(Int_t iStation, Int_t iLayer, Double_t z, Double_t zRel, Double_t dz);

  /** Destructor **/
  virtual ~CbmMuchLayer();

  /** Accessors **/
  Int_t          GetDetectorId()  const { return fDetectorId; }
  CbmMuchLayerSide* GetSideF()           { return &fSideF; }
  CbmMuchLayerSide* GetSideB()           { return &fSideB; }
  CbmMuchLayerSide* GetSide(Bool_t side) { return (side ? &fSideB : &fSideF); }
  Double_t GetSupportDx()    const { return fSupportDx; }
  Double_t GetSupportDy()    const { return fSupportDy; }
  Double_t GetSupportDz()    const { return fSupportDz; }
  void SetSupportDx(Double_t supDx) { fSupportDx = supDx; }
  void SetSupportDy(Double_t supDy) { fSupportDy = supDy; }
  void SetSupportDz(Double_t supDz) { fSupportDz = supDz; }
  Double_t GetZ() { return fZ; }
  Double_t GetZtoStationCenter() { return fZtoStationCenter; }
  Double_t GetDz() { return fDz; }

 protected:
  Int_t   fDetectorId;  // Unique detector ID
  Double32_t fZ;           // z position of station centre (midplane) [cm]
  CbmMuchLayerSide fSideF; // Front side of the layer
  CbmMuchLayerSide fSideB; // Back side of the layer
  Double_t   fSupportDx;   // Support half-width
  Double_t   fSupportDy;   // Support half-height
  Double_t   fSupportDz;   // Support half-thickness
  Double_t   fZtoStationCenter; // Relative position of the layer center with respect
                                // to the station center
  Double_t   fDz;               // Half-thinkness of the layer

  ClassDef(CbmMuchLayer,1);
};
#endif
