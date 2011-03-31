#ifndef CBMTRDHITPRODUCERCLUSTER_H
#define CBMTRDHITPRODUCERCLUSTER_H 

#include "FairTask.h"
#include "CbmTrdDetectorId.h"

#include "CbmTrdClusterizer.h"
#include "CbmTrdClusterFinderFast.h"
#include "TLegend.h"

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
  Int_t secIdX;
  Int_t secIdY;
  Int_t neighbourIds[4];
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

  Int_t NoPrfRecoDim;
} MyHit;

typedef std::list<MyHit*> MyHitList;
typedef std::map<Int_t, MyDigiList*> MyDigiListMap;

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
  Int_t GetSector(Bool_t x, Int_t DigiCol/*, ModulePara* mPara*/);
  void GetModuleInfo(Int_t qMaxIndex, MyHit* hit/*, MHitMap* ModuleHitMap*/, TH2F*& PRF);
  void SearchNeighbours(Int_t qMaxIndex, Int_t *neighbourIds/*, ModulePara* mPara*/, MyDigiList *neighbours, MyHit* hit, TH2F*& PRF);
  Float_t Prf(Float_t padWidth, Float_t sigma, Float_t qLeft, Float_t qMax, Float_t qRight);
  void PrfReco(Int_t qMaxIndex, Float_t qMax/*, ModulePara* mPara*/, Int_t *neighbourIds, MyHit* hit, TH2F*& PRF);
  void SimpleReco(Int_t qMaxIndex, Float_t qMax/*, ModulePara* mPara*/, Int_t *neighbourIds, MyHit* hit/*, MHitMap* ModuleHitMap*/, TH2F*& PRF);
  void DrawHits();
  void CalcPR(Bool_t combinatoric, Int_t qMaxDigiIndex, TH1F*& shortPR, TH1F* longPR[20], TLegend*& legend, TH2F*& PRF, MyHit *hit);
  void AddHit(Int_t iHit, Int_t detectorId, TVector3& pos, TVector3& dpos, Double_t dxy, Int_t planeId, Double_t eLossTR, Double_t eLossdEdx, Double_t eLoss);

  TClonesArray*     fTrdPoints;   /** Input array of Trd MC points **/
  TClonesArray*     fDigis;       /** Input array of CbmTrdDigi **/
  TClonesArray*     fClusters;    /** Input array of CbmTrdCluster **/
  TClonesArray*     fClusterHits; /** Output array of CbmTrdHit **/

  CbmTrdDigiPar *fDigiPar;   //!
  CbmTrdModule  *fModuleInfo; //!
  
  CbmTrdDetectorId fTrdId; //!

  Int_t fPrfSingleRecoCounter;
  Int_t fPrfDoubleRecoCounter;
  Int_t fSimpleRecoCounter;


  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  Int_t fStation;
  Int_t fLayer;
  Int_t fmoduleId;
  Int_t fxPos;
  Int_t fyPos;
  Int_t fzPos;
  Int_t fnCol;
  Int_t fnRow;
  Int_t fNoSectors;
  
  std::vector<Float_t> fSectorSizeX;
  std::vector<Float_t> fSectorSizeY;
  std::vector<Float_t> fPadSizeX;
  std::vector<Float_t> fPadSizeY;
  std::vector<Int_t> fSecCol;
  std::vector<Int_t> fSecRow;
  std::vector<Int_t> fPadSizeXArray;
  std::vector<Int_t> fPadSizeYArray;
   
  Float_t fModuleSize[3];
  Float_t fModulePosition[3];
 //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


  std::map<Int_t,Int_t> fPadSizeLongMap;

  std::vector<Int_t> fLayersBeforeStation; //! 

  std::map<Int_t, MyHitList*> ModuleHitMap;

  std::map<Int_t, MyDigiList*> moduleDigiMap; //map of <moduleId, List of struct 'MyDigi' pointer>

  ClassDef(CbmTrdHitProducerCluster,1);
};
#endif
