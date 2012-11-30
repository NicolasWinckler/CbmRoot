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
		 kECAL, kPSD, kSTT, kTutDet};


enum DataType {  
  kUnknown, kMCTrack,
  kStsPoint, kStsDigi, kStsCluster, kStsHit
};



#include "TObject.h"

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
