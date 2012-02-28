#ifndef CBMTRDCLUSTERIZERFAST_H
#define CBMTRDCLUSTERIZERFAST_H

#include "CbmTrdDetectorId.h"
#include "CbmTrdDigiMatch.h"
#include "FairTask.h"

#include <map>
#include <list>
#include <vector>
#include "CbmTrdDigi.h"

class CbmTrdDigiPar;
//class CbmTrdDigi;
class CbmTrdModule;
class CbmTrdRadiator;

class TClonesArray;
class TH1F;
class TH2F;
class TProfile;
class CbmTrdDigiMatch;

typedef struct ClusterPad
{
  Double_t SizeX;
  Double_t SizeY;
  Double_t Charge;
  std::vector<Int_t> MCIndex;
ClusterPad() : SizeX(0), SizeY(0), Charge(0), MCIndex() {}
} ClusterPad;


typedef struct ClusterModule
{
  Int_t Station;
  Int_t Layer;
  Int_t moduleId;
  Int_t nxPad;
  Int_t nyPad;
  Int_t NoSectors;
  std::vector<std::vector<ClusterPad*> > PadPlane;
  
  std::vector<Float_t> SectorSizeX;
  std::vector<Float_t> SectorSizeY;
  std::vector<Float_t> PadSizeX;
  std::vector<Float_t> PadSizeY;
  std::vector<Int_t> SecxPad;
  std::vector<Int_t> SecyPad;
 
  Float_t ModuleSizeX;
  Float_t ModuleSizeY;
  Float_t ModulePositionX;
  Float_t ModulePositionY;
  Float_t ModulePositionZ;

  ClusterModule () : Station(-1), Layer(-1), moduleId(-1), PadPlane(),
    nxPad(0), nyPad(0), NoSectors(0), SectorSizeX(), SectorSizeY(), PadSizeX(), 
    PadSizeY(), SecxPad(), SecyPad(), ModuleSizeX(0.), ModuleSizeY(0.), 
    ModulePositionX(0.), ModulePositionY(0.), ModulePositionZ() {}
} ClusterModule;

class CbmTrdClusterizerFast : public FairTask {

 public:

  /** Default constructor **/
  CbmTrdClusterizerFast();

  /** Standard constructor **/
  CbmTrdClusterizerFast(const char *name, const char *title="CBM Task",
			CbmTrdRadiator *radiator=NULL, Bool_t oneClusterPerPoint=false, Bool_t wireQuantisation=false);
  /*
    oneClusterPerPoint: produces just one cluster per MC-track if true -> less ghost hits
    produces n cluster per track depending on the path length within the detector volume -> leads to at least one hit per crossed pad row -> high number of ghost hits
*/

  /** Destructor **/
  virtual ~CbmTrdClusterizerFast();

  /** Initialisation **/
  virtual InitStatus ReInit();
  virtual InitStatus Init();
  virtual void SetParContainers();

  /** Executed task **/
  virtual void Exec(Option_t * option);

  /** Finish (called after each event) **/
  virtual void FinishEvent();
 
  /** Finish task (called after all event) **/
  virtual void FinishTask(){;}

  /*
    void AddHit(TVector3 &posHit, TVector3 &posHitErr,
    Int_t TrackID, Int_t PlaneID, Int_t ref, Double_t ELoss,
    Double_t ELossTR, Double_t ELossdEdX);
  */

  void Register();

 private:

  CbmTrdClusterizerFast& operator=(const CbmTrdClusterizerFast&);
  CbmTrdClusterizerFast(const CbmTrdClusterizerFast&);

  void GetModuleInformationFromDigiPar(Int_t VolumeID);
  void InitPadPlane(ClusterModule* mCluster);
  void GetModuleInformation();

  void AddDigi(const Int_t pointID, Int_t ixPad, Int_t iyPad, Int_t nxPad, Int_t nyPad, Double_t iCharge); 

  void CalcDigisOnPadPlane(Double_t* clusterPosInPadLL, Int_t* PadMax, Double_t ELoss, Int_t pointId);

  void SplitPathSlices(Double_t* local_inLL, Double_t* local_outLL, Double_t ELoss, Int_t pointId);

  void WireQuantisation();

  void ChargeConservation();

  Double_t CalcMathieson(Double_t x, Double_t W);

  void TransformLL2C(Double_t* LLCoordinate, Double_t* CCoordinate);
 
  void TransformC2LL(Double_t* CCoordinate, Double_t* LLCoordinate);

  void GetClusterDisplacement(Double_t* clusterPosInModuleLL, Double_t* clusterPosInPadLL, Int_t* PadMax);

  Int_t GetyPad(Double_t tempPosY);

  Int_t GetxPad(Double_t tempPosX);

  Bool_t fDebug;
  Bool_t fOneClusterPerPoint;
  Bool_t fWireQuantisation;

  Int_t Digicounter;
  Double_t fLayerZ[12];
  Double_t fTime;
  Int_t   fModuleType;
  Int_t   fModuleCopy;
  Int_t   fModuleID;//Unique number for detector module
  Int_t   fMCindex;// index to MCPoint

  static const Int_t fPadNrX = 11;//15;//7; // has to be odd
  static const Int_t fPadNrY = 11;//5;//5; // has to be odd
 
  //static const Int_t fNoSectors = 3;
  static const Int_t endOfMathiesonArray = 35; //+- mm

  Float_t fEfficiency; // Digi production efficiency (0-100%)

  


  TClonesArray *fTrdPoints; //! Trd MC points
  TClonesArray *fDigiCollection; //! TRD digis
  TClonesArray *fDigiMatchCollection; //! Corresponding MCPoints to TRD digis
  TClonesArray *fMCStacks;  //! MC Track information

  CbmTrdDigiPar  *fDigiPar;    //!
  CbmTrdModule   *fModuleInfo; //!
  CbmTrdRadiator *fRadiators;  //!

  CbmTrdDetectorId fTrdId; //!
 
  std::map<Int_t, ClusterModule*> fModuleClusterMap;
  std::map<Int_t, ClusterModule*>::iterator fModuleClusterMapIt;

  ClassDef(CbmTrdClusterizerFast,1)

    };
#endif // CBMTRDCLUSTERIZERFAST_H