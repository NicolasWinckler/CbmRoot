/** CbmMuchStation.h
 *@author  M.Ryzhinskiy <m.ryzhinskiy@gsi.de>
 *@version 1.0
 *@since   15.03.07
 **
 ** This class holds the transport geometry parameters
 ** of one MuCh tracking station.
 **
 **/


#ifndef CBMMUCHSTATION_H
#define CBMMUCHSTATION_H 1

#include "TObjArray.h"
#include "CbmMuchLayer.h"

class CbmMuchStation : public TObject{
 public:

  /** Default constructor **/
  CbmMuchStation();


  /** Standard constructor
  *@param iStation  Station index
  *@param z         z position of station centre [cm]
  **/
  CbmMuchStation(Int_t iStation, Double_t z);


  /** Destructor **/
  virtual ~CbmMuchStation();

  /** Setters **/
  void SetRmin(Double_t rMin) { fRmin = rMin; }
  void SetRmax(Double_t rMax) { fRmax = rMax; }
  void SetSigmaXmin   (Double_t sigma) { fSigmaXmin    = sigma; }
  void SetSigmaYmin   (Double_t sigma) { fSigmaYmin    = sigma; }
  void SetSigmaXmax   (Double_t sigma) { fSigmaXmax    = sigma; }
  void SetSigmaYmax   (Double_t sigma) { fSigmaYmax    = sigma; }
  void SetOccupancyMax(Double_t occup) { fOccupancyMax = occup; }


  /** Accessors **/
  Int_t GetDetectorId()   const { return fDetectorId; }
  Double_t GetZ()            const { return fZ; }
  Int_t    GetNLayers()      const { return fLayers.GetEntriesFast(); }
  Double_t GetRmin()         const { return fRmin;         }
  Double_t GetRmax()         const { return fRmax;         }
  Double_t GetSigmaXmin()    const { return fSigmaXmin;    }
  Double_t GetSigmaYmin()    const { return fSigmaYmin;    }
  Double_t GetSigmaXmax()    const { return fSigmaXmax;    }
  Double_t GetSigmaYmax()    const { return fSigmaYmax;    }
  Double_t GetOccupancyMax() const { return fOccupancyMax; }

  CbmMuchLayer* GetLayer(Int_t iLayer) const { return (CbmMuchLayer*)fLayers.At(iLayer); }

  /** Add one sector to the array **/
  void AddLayer(CbmMuchLayer* layer);


 protected:
  Int_t   fDetectorId;          // Unique detector ID
  Double32_t fZ;                // z position of station centre (midplane) [cm]
  TObjArray  fLayers;           // Array of CbmMuchLayers
  Double_t   fRmin;             // Minimum radius of the station [cm]
  Double_t   fRmax;             // Maximum radius of the station [cm]
  Double_t   fSigmaXmin;        // Minimum sigma in X [mm]
  Double_t   fSigmaYmin;        // Minimum sigma in Y [mm]
  Double_t   fSigmaXmax;        // Maximum sigma in X [mm]
  Double_t   fSigmaYmax;        // Maximum sigma in Y [mm]
  Double_t   fOccupancyMax;     // Maximum occupancy

  ClassDef(CbmMuchStation,1);
};
#endif
