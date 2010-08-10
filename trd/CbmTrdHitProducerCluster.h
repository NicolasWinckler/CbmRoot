#ifndef CBMTRDHITPRODUCERCLUSTER_H
#define CBMTRDHITPRODUCERCLUSTER_H 

#include "FairTask.h"
#include "CbmTrdDetectorId.h"

#include "TVector3.h"
#include <vector>
#include <list>
#include <map>

class CbmTrdDigiPar;
class CbmTrdModule;
class TClonesArray;

typedef struct MyHit
{
  Int_t digiId;
  Int_t rowId;
  Int_t colId;
  //
  Int_t nCol;
  Int_t nRow;
  //
  Int_t moduleId;
  Float_t xPos;
  Float_t yPos;
  Float_t zPos;
  Float_t dxPos;
  Float_t dyPos;
  Float_t dzPos;
  Float_t charge;
} MyHit;

typedef struct ModulePara
{
  Int_t Station;
  Int_t Layer;
  Int_t moduleId;
  Int_t xPos;
  Int_t yPos;
  Int_t zPos;
  Int_t nCol;
  Int_t nRow;
  Int_t NoSectors;
  
  std::vector<Float_t> SectorSizeX;
  std::vector<Float_t> SectorSizeY;
  std::vector<Float_t> PadSizeX;
  std::vector<Float_t> PadSizeY;
  std::vector<Int_t> SecCol;
  std::vector<Int_t> SecRow;
 
  Float_t ModuleSizeX;
  Float_t ModuleSizeY;
  Float_t ModulePositionX;
  Float_t ModulePositionY;
  Float_t ModulePositionZ;
} ModulePara;

typedef std::list<MyHit*> MyHitList;


class CbmTrdHitProducerCluster : public FairTask
{
 public:

// ---- Default constructor -------------------------------------------
  CbmTrdHitProducerCluster();
// ---- Destructor ----------------------------------------------------
  virtual ~CbmTrdHitProducerCluster();
 /** Initialisation **/
  virtual InitStatus ReInit();
  virtual InitStatus Init();
  virtual void SetParContainers();
  
  /** Executed task **/
  virtual void Exec(Option_t * option);

  /** Finish task **/
  virtual void Finish();
  void Register();

 private:
  void GetModuleInfo(Int_t qMaxIndex/*, MHitMap* ModuleHitMap*/);
  void SortClusterDigi();
  void PrfReco(Int_t qMaxIndex, Float_t qMax);
  void SimpleReco(Int_t qMaxIndex, Float_t qMax, ModulePara* mPara, MyHit* hit/*, MHitMap* ModuleHitMap*/);
  void DrawHits();
  void AddHit(Int_t iHit, Int_t detectorId, TVector3& pos, TVector3& dpos, Double_t dxy, Int_t planeId, Double_t eLossTR, Double_t eLossdEdx, Double_t eLoss);


  TClonesArray*     fDigis;       /** Input array of CbmTrdDigi **/
  TClonesArray*     fClusters;    /** Input array of CbmTrdCluster **/
  TClonesArray*     fClusterHits; /** Output array of CbmTrdHit **/

  CbmTrdDigiPar *fDigiPar;   //!
  CbmTrdModule  *fModuleInfo; //!
  
  CbmTrdDetectorId fTrdId; //!

std::map<Int_t, MyHitList*> ModuleHitMap;

  ClassDef(CbmTrdHitProducerCluster,1);
};
#endif
