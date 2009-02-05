// -------------------------------------------------------------------------
// -----                  CbmMuchHitProducerQa header file             -----
// -----                  Created 16/11/07  by E. Kryshen              -----
// -------------------------------------------------------------------------

#ifndef CBMMUCHHITPRODUCERQA_H
#define CBMMUCHHITPRODUCERQA_H

#include "CbmTask.h"
#include "TH2.h"

class TClonesArray;

class CbmGeoMuchPar;
class CbmMuchDigiPar;
class CbmMuchDigiScheme;

class TH1D;

class CbmMuchHitProducerQa : public CbmTask {

public:
  CbmMuchHitProducerQa(const char *name="MuchHitProducer", Int_t verbose = 1);
  virtual ~CbmMuchHitProducerQa();
  virtual InitStatus Init();
  virtual void Exec(Option_t * option);
  virtual void Finish();
  virtual void SetParContainers();
private:
  Int_t fVerbose;
  Int_t fEvent;
  TClonesArray *fPoints;
  TClonesArray *fDigis;
  TClonesArray *fDigiMatches;
  TClonesArray *fClusters;
  TClonesArray *fHits;

  CbmGeoMuchPar*     fGeoPar;        // Geometry parameter container
  CbmMuchDigiPar*    fDigiPar;       // Digitization parameter container
  CbmMuchDigiScheme* fDigiScheme;    // Digitization scheme

  Int_t  fNstations;
  TH1D** fhOccupancyR;
  TH1D** fhPadsTotalR;
  TH1D** fhPadsFiredR;

  TH1D* fhPoolX;
  TH1D* fhPoolY;

  ClassDef(CbmMuchHitProducerQa,1) 
};

#endif 

