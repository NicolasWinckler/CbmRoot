/** CbmSimSetup.h
 *@Author V.Friese  <V.Friese@Gsi.De>
 *@Since 12.06.2007
 **
 ** Defines unique identifier for all CBM detector systems.
 **
 ** 29.04.2010 V. Friese: Made a class with implementation file.
 **/



#ifndef CBMDETECTORLIST_H
#define CBMDETECTORLIST_H 1
 
enum DetectorId {kREF,  kMVD, kSTS, kRICH, kMUCH, kTRD, kTOF, 
		 kECAL, kZDC, kSTT, kTutDet};


enum DataType {  
  kUnknown=0, kMCTrack=1,
  kMvdPoint=10, kMvdDigi=11, kMvdCluster=12, kMvdHit=13,
  kStsPoint=20, kStsDigi=21, kStsCluster=22, kStsHit=23,
  kRichPoint=30, kRichRing=31,
  kMuchPoint=40, kMuchDigi=41, kMuchCluster=42, kMuchPixelHit=43, 
    kMuchStrawHit=44,
  kTrdPoint=50, kTrdDigi=51, kTrdCluster=52, kTrdHit=53,
  kTofPoint=60, kTofHit=61,
  kEcalPoint=70, kEcalPointLite=71, kEcalCluster=72, kEcalHit=73,  
    kEcalHitFastMC=74, 
  kZdcPoint=80,
  kSttPoint=90, kSttHit=91,
  kTutDetPoint=100,
  kStsTrack=110, kTrdTrack=111, kMuchTrack=112, kSttTrack=113,
    kGlobalTrack=114
};



#include "TObject.h"
#include "TString.h"


class FairDetector;



class CbmDetectorList : public TObject
{

 public:

  /**   Constructor   **/
  CbmDetectorList();


  /**   Destructor  **/
  virtual ~CbmDetectorList() { }


  /**   Get system name
   *@param det   System identifier (type DetectorId)
   *@param name  (return) System name (lower case) 
   **/
  static void GetSystemName(DetectorId det, TString& name);



  ClassDef(CbmDetectorList, 1);

};


#endif
