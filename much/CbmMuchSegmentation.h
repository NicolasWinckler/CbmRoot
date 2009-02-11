/** CbmMuchSegmentation.h
 *@author Mikhail Ryzhinskiy <m.ryzhinskiy@gsi.de>
 *@since 20.06.07
 *@version 1.0
 **
 ** class for making parameter file for MUCH digitizer
 ** 
 **/

#ifndef CBMMUCHSEGMENTATION_H
#define CBMMUCHSEGMENTATION_H 1


#include "FairTask.h"

#include <vector>

class TClonesArray;
class CbmGeoMuchPar;
class CbmMuchLayerSide;
class CbmMuchModule;
class CbmMuchSector;
class TH1D;

using std::vector;

class CbmMuchSegmentation : public FairTask{
 public:

  /** Default constructor **/
  CbmMuchSegmentation();

  /** Default constructor **/
  CbmMuchSegmentation(char* digiFileName);

  virtual InitStatus Init();
  virtual void Exec(Option_t * option);
  virtual void Finish();

  /** Destructor **/
  virtual ~CbmMuchSegmentation();

 private:
  Int_t         fEvents;
  TClonesArray* fPoints;
  TH1D**        fHistHitDensity;
  Int_t         fNStations;
  TObjArray*    fStations;
  
  Char_t*           fDigiFileName;          // Digitization file
  CbmGeoMuchPar*    fGeoPar;                // Geometry parameters container
  vector<Double_t>  fExp0;                  // Fit parameters for EXP distribution for each station
  vector<Double_t>  fExp1;                  // Fit parameters for EXP distribution for each station
  
  /** Get parameter containers **/
  virtual void SetParContainers();

  /** Intialisation **/
  void InitLayerSide(CbmMuchLayerSide* layerSide);
  void SegmentModule(CbmMuchModule* module);
  void SegmentSector(CbmMuchModule* module, CbmMuchSector* sector);
  Bool_t ShouldSegmentByX(CbmMuchSector* sector);
  Bool_t ShouldSegmentByY(CbmMuchSector* sector);
  Int_t IntersectsRad(CbmMuchSector* sector, Double_t radius);
  void Print();

  ClassDef(CbmMuchSegmentation,1)
};

#endif
