// -------------------------------------------------------------------------
// -----                      CbmMvdCluster header file            -----
// -----
// -------------------------------------------------------------------------


/** CbmMvdCluster.h



**/


#ifndef CBMMVDCLUSTER_H
#define CBMMVDCLUSTER_H 1


//#include "CbmHit.h"
//#include "TVector3.h"
#include "CbmMvdHit.h"

#include "TVector3.h"

class CbmMvdCluster : public CbmMvdHit
{

 public:    

  /** Default constructor **/
  CbmMvdCluster();


  /** Standard constructor 
  *@param statNr Station number
  *@param pos    Position coordinates [cm]
  *@param dpos   Errors in position coordinates [cm]
  *@param flag   Hit flag
  *@param chargeArray Array of charge of 49 pixels
  **/
  CbmMvdCluster(Int_t statNr, TVector3& pos, TVector3& dpos, Int_t flag, 
                 Short_t chargeArray[49] , Double_t pixelSizeX, Double_t pixelSizeY);


  /** Destructor **/
  virtual ~CbmMvdCluster();


  /** Accessors **/
  Short_t GetPointInfo(Int_t index){return fPointInfo[index];};
  Float_t GetPointX(Int_t index){return fPointX[index];}; //MC-Information for debugging
  Float_t GetPointY(Int_t index){return fPointY[index];}; //MC-Information for debugging
  Short_t GetContributors(){return fContributors;};
  Short_t* GetChargeArray(){return fChargeArray;}
  Short_t* GetPointInfo(){return fPointInfo;} // MC-Information for debugging

  /** Modifiers **/
  void SetPointInfo(Short_t info, Int_t cell){fPointInfo[cell]=info;}
  void SetPointXY (Float_t x, Float_t y, Int_t cell) {fPointX[cell]=x; fPointY[cell]=y;}
  void SetDebuggingInfo(Short_t pointInfo[49], Float_t pointX[5],Float_t pointY[5]);
  void SetContributors(Short_t contributors){fContributors=contributors;}


 //protected:

     Short_t fChargeArray[49];
     Float_t fPixelSizeX;
     Float_t fPixelSizeY;
     
     //debugging parameters
     Short_t fPointInfo[49];
     Float_t fPointX[5];
     Float_t fPointY[5];
     Short_t fContributors;

  ClassDef(CbmMvdCluster,1);

};


#endif
