/**@file CbmDaqMap.h
 **@date 03.012.2009
 **author V. Friese <v.friese@gsi.de>
 **
 ** Provides the mapping of DAQ channel to detector channel
 ** 
 ** At the moment: implemented only for STS stations
 **/



#ifndef CBMDAQMAP_H
#define CBMDAQMAP_H 1


#include "TObject.h"
#include "TString.h"


class CbmDaqMap : public TObject
{


 public:

  /**   Default constructor   **/
  CbmDaqMap();


  /**   Standard constructor 
   **@param iRun   run number
   **/
  CbmDaqMap(Int_t iRun);


  /**   Destructor   **/
  virtual ~CbmDaqMap();


  /**   Set trun number  */
  void SetRun(Int_t iRun) { fRun = iRun; }


  /**  System Id from ROC Id  **/
  Int_t GetSystemId(Int_t rocId);


  /**  STS station from ROC Id  **/
  Int_t GetStsStation(Int_t rocId);

  /**  MUCH station from ROC Id  **/
  Int_t GetMuchStation(Int_t rocId);

  /**  Fiber Hodoscope station from ROC Id  **/
  Int_t GetFiberHodoStation(Int_t rocId);

  /**  STS sensor side from ROC and NXYTER Id **/
  Int_t GetStsSensorSide(Int_t rocId);

  /** STS channel from ROC, NXYTER and NXYTER channel **/
  Int_t GetStsChannel(Int_t rocId, Int_t nxId, Int_t nxChannel);

  /** Fiber Hodoscope channel from ROC, NXYTER and NXYTER channel **/
  Int_t GetFiberHodoChannel(Int_t rocId, Int_t nxId, Int_t nxChannel);


  /**   Mapping
   **@param iRoc   ROC number (from hit message)
   **@param iNx    NXYter number (from hit message) 
   **@param iId    Channel ID (from hit message)
   **@param iStation  Station number (return)
   **@param iSector   Sector number ( return)
   **@param iSide     Front side (0) / Back side (1) ( return)
   **@param iChannel  Channel number (return)  
  **/
  Bool_t Map(Int_t iRoc, Int_t iNx, Int_t iId,
	     Int_t& iStation, Int_t& iSector, Int_t& iSide, Int_t& iChannel);
    


 private:

  void InitializeFiberHodoMapping();
  
  Int_t fRun;        /** Run number  **/
  Int_t fFiberHodoFiber[128];  /** Mapping from fiber hodoscope feb channel to fiber number **/
  Int_t fFiberHodoPlane[128];  /** Mapping from fiber hodoscope feb channel to plane number 1=X, 2=Y **/
  Int_t fFiberHodoPixel[128];  /** Mapping from fiber hodoscope feb channel to pixel number **/



  ClassDef(CbmDaqMap,1);

};

#endif



  
