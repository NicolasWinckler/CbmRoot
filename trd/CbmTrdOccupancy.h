#ifndef CBMTRDOCCUPANCY_H
#define CBMTRDOCCUPANCY_H

#include "CbmTrdDigiMatch.h"
#include "FairTask.h"

#include <map>
#include <list>
#include <vector>
#include "CbmTrdDigi.h"

class CbmTrdDigiPar;
class CbmTrdModule;
class CbmTrdGeoHandler;

class TClonesArray;
class TH1F;
class TH1I;
class TH2I;
class TH2F;
class TProfile;
class TProfile2D;
class TCanvas;
class CbmTrdDigiMatch;

typedef struct Pad
{
  Double_t SizeX;
  Double_t SizeY;
  Bool_t Fired;
Pad() : SizeX(0), SizeY(0), Fired(false) {}
} Pad;

typedef struct OccupancyModule
{
  Int_t Station;
  Int_t Layer;
  Int_t moduleId;
  Int_t nPad;
  Int_t nxPad;
  Int_t nyPad;
  Int_t NoSectors;
  std::vector<std::vector<Pad*> > PadPlane;
  
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

  OccupancyModule () : Station(-1), Layer(-1), moduleId(-1), PadPlane(), nPad(0),
    nxPad(0), nyPad(0), NoSectors(0), SectorSizeX(), SectorSizeY(), PadSizeX(), 
    PadSizeY(), SecxPad(), SecyPad(), ModuleSizeX(0.), ModuleSizeY(0.), 
    ModulePositionX(0.), ModulePositionY(0.), ModulePositionZ() {}
} OccupancyModule;

class CbmTrdOccupancy : public FairTask {

 public:
  CbmTrdOccupancy();
  CbmTrdOccupancy(const char *name, const char *title="CBM Task");
  virtual ~CbmTrdOccupancy();
  virtual InitStatus ReInit();
  virtual InitStatus Init();
  virtual void SetParContainers();
  virtual void Exec(Option_t * option);
  virtual void FinishEvent();
  virtual void FinishTask(){;}
  void Register();
  void SaveHistos2File();

 private:
  CbmTrdOccupancy& operator=(const CbmTrdOccupancy&);
  CbmTrdOccupancy(const CbmTrdOccupancy&);
  TClonesArray *fDigis; //! TRD digis
  CbmTrdDigiPar  *fDigiPar;    //!
  CbmTrdModule   *fModuleInfo; //!
  CbmTrdGeoHandler* fGeoHandler; //!
  TH1I *fDigiChargeSpectrum;
  TH2I *fLayerDummy;
  
  std::map<Int_t, OccupancyModule*> fModuleMap; //!
  std::map<Int_t, OccupancyModule*>::iterator fModuleMapIt; //!
  
  std::map<Int_t, TH2F*> fModuleOccupancyMap; //!
  std::map<Int_t, TH2F*>::iterator fModuleOccupancyMapIt; //!
  std::map<Int_t, TCanvas*> fLayerOccupancyMap; //!
  std::map<Int_t, TCanvas*>::iterator fLayerOccupancyMapIt; //!
  ClassDef(CbmTrdOccupancy,2)

    };
#endif // CBMTRDCLUSTERIZERFAST_H
