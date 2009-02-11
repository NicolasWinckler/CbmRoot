/** CbmMuchMaximalChargeQa.h
 *@author Mikhail Ryzhinskiy <m.ryzhinskiy@gsi.de>
 *@since 01.02.08
 *@version 1.0
 **
 ** CBM task class to find upper charge limit for pads.
 ** Task level RECO.
 **/

#ifndef CBMMUCHMAXIMALCHARGEQA_H
#define CBMMUCHMAXIMALCHARGEQA_H

#include "CbmGeoMuchPar.h"
#include "CbmMuchDigiPar.h"
#include "CbmMuchDigiScheme.h"

#include "FairTask.h"
#include "TClonesArray.h"
#include "TH1.h"

#include <map>

using std::map;

class CbmMuchMaximalChargeQa : public FairTask {

 public:

  /** Standard constructor. **/
  CbmMuchMaximalChargeQa(const char *name="MuchMaximalChargeQa", Int_t verbose = 1);

  /** Destructor. **/
  virtual ~CbmMuchMaximalChargeQa();

  /** Exec **/
  virtual void Exec(Option_t * option);

 private:
  Int_t                 fEvent;         // Number of currently processing event
  TClonesArray*         fPoints;        // Input array of CbmMuchPoint objects
  TClonesArray*         fDigiMatches;   // Input array of CbmMuchDigiMatch objects
  TClonesArray*         fDigis;         // Input array of CbmMuchDigi objects
  TClonesArray*         fMCTracks;      // Input array of MCTrack
  map<Double_t, TH1D*>  fQHistsMap;     // Histograms for charge collected by pads
  CbmGeoMuchPar*        fGeoPar;        // Geometry parameter container
  CbmMuchDigiPar*       fDigiPar;       // Digitization parameter container
  CbmMuchDigiScheme*    fDigiScheme;    // Digitization scheme

  virtual void SetParContainers();

  /** Intialization **/
  virtual InitStatus Init();

  /** Reinitialization **/
  virtual InitStatus ReInit();

  /** Finish **/
  virtual void Finish();

  ClassDef(CbmMuchMaximalChargeQa,1) 
};

#endif 

