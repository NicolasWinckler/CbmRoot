/** CbmMuchSegmentAuto.h
 *@author Mikhail Ryzhinskiy <m.ryzhinskiy@gsi.de>
 *@since 20.06.07
 *@version 1.0
 **
 ** class for making parameter file for MUCH digitizer
 **
 **/

#ifndef CBMMUCHSEGMENTAUTO_H
#define CBMMUCHSEGMENTAUTO_H 1


#include "FairTask.h"

#include <vector>

class TClonesArray;
class CbmGeoMuchPar;
class CbmMuchLayerSide;
class CbmMuchModule;
class CbmMuchSector;
class TH1D;

using std::vector;

class CbmMuchSegmentAuto : public FairTask{
 public:

  /** Default constructor **/
  CbmMuchSegmentAuto();

  /** Default constructor **/
  CbmMuchSegmentAuto(char* digiFileName);

  void SetNStations(Int_t nStations);
  void SetSigmaMin(Double_t* sigmaXmin, Double_t* sigmaYmin);
  void SetSigmaMax(Double_t* sigmaXmax, Double_t* sigmaYmax);
  void SetOccupancyMax(Double_t* occupancyMax);


  virtual InitStatus Init();
  virtual void Exec(Option_t * option);
  virtual void FinishTask();

  /** Destructor **/
  virtual ~CbmMuchSegmentAuto();

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

  vector<Double_t>   fSigmaXmin;        // Minimum sigma in X [mm]
  vector<Double_t>   fSigmaYmin;        // Minimum sigma in Y [mm]
  vector<Double_t>   fSigmaXmax;        // Maximum sigma in X [mm]
  vector<Double_t>   fSigmaYmax;        // Maximum sigma in Y [mm]
  vector<Double_t>   fOccupancyMax;     // Maximum occupancy

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

  ClassDef(CbmMuchSegmentAuto,1)
};

#endif
