/** CbmMuchStationGem.h
 *@author  E.Kryshen <e.kryshen@gsi.de>
 *@version 1.0
 *@since   18.03.09
 **
 ** This class holds the transport geometry parameters
 ** of one MuCh GEM/micromegas tracking station.
 ** Derived from base CbmMuchStation class
 **/


#ifndef CBMMUCHSTATIONGEM_H
#define CBMMUCHSTATIONGEM_H 1

#include "CbmMuchStation.h"

class CbmMuchStationGem : public CbmMuchStation{
 public:

  /** Default constructor **/
  CbmMuchStationGem();


  /** Standard constructor
  *@param iStation  Station index
  *@param z         z position of station centre [cm]
  **/
  CbmMuchStationGem(Int_t iStation, Double_t z);


  /** Destructor **/
  virtual ~CbmMuchStationGem();

  /** Setters **/
  void SetSigmaXmin   (Double_t sigma) { fSigmaXmin    = sigma; }
  void SetSigmaYmin   (Double_t sigma) { fSigmaYmin    = sigma; }
  void SetSigmaXmax   (Double_t sigma) { fSigmaXmax    = sigma; }
  void SetSigmaYmax   (Double_t sigma) { fSigmaYmax    = sigma; }
  void SetOccupancyMax(Double_t occup) { fOccupancyMax = occup; }

  /** Accessors **/
  Double_t GetSigmaXmin()    const { return fSigmaXmin;    }
  Double_t GetSigmaYmin()    const { return fSigmaYmin;    }
  Double_t GetSigmaXmax()    const { return fSigmaXmax;    }
  Double_t GetSigmaYmax()    const { return fSigmaYmax;    }
  Double_t GetOccupancyMax() const { return fOccupancyMax; }

 protected:
  Double_t   fSigmaXmin;        // Minimum sigma in X [mm]
  Double_t   fSigmaYmin;        // Minimum sigma in Y [mm]
  Double_t   fSigmaXmax;        // Maximum sigma in X [mm]
  Double_t   fSigmaYmax;        // Maximum sigma in Y [mm]
  Double_t   fOccupancyMax;     // Maximum occupancy
  ClassDef(CbmMuchStationGem,1);
};
#endif
