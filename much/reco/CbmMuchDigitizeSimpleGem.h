/** CbmMuchDigitizeSimpleGem.h
 *@author Mikhail Ryzhinskiy <m.ryzhinskiy@gsi.de>
 *@since 19.03.07
 *@version 1.0
 **
 ** CBM task class for digitizing MUCH
 ** Task level RECO
 ** Produces objects of type CbmMuchDigi out of CbmMuchPoint.
 **/

#ifndef CBMMUCHDIGITIZESIMPLEGEM_H
#define CBMMUCHDIGITIZESIMPLEGEM_H 1

#include "TH1.h"
#include "TH2.h"

#include "FairTask.h"

#include "TStopwatch.h"
#include "TPolyLine.h"
#include "TMath.h"
#include "TLine.h"
#include "TRandom3.h"
#include "TRandom.h"
#include "TClonesArray.h"
#include "TFile.h"

#include "CbmMuchDigi.h"
#include "CbmMuchDigiMatch.h"
#include "CbmMuchGeoScheme.h"

#include <set>
#include <map>


class CbmMuchSector;
class CbmMuchPoint;
class TFile;

class CbmMuchDigitizeSimpleGem : public FairTask
{

public:

  /** Default constructor */
  CbmMuchDigitizeSimpleGem();

  /** Standard constructor */
  CbmMuchDigitizeSimpleGem(Int_t iVerbose);

  /** Constructor with name */
  CbmMuchDigitizeSimpleGem(const char* name, const char* digiFileName, Int_t iVerbose);

  /** Destructor */
  virtual ~CbmMuchDigitizeSimpleGem();

  /** Execution */
  virtual void Exec(Option_t* opt);

  /** Sets the time resolution. **/
  void SetDTime(Double_t dTime) { fDTime = dTime; }

  /** Gets time resolution **/
  Double_t GetDTime() { return fDTime; }

private:

  CbmMuchGeoScheme*  fGeoScheme;     // Object responsible for geometry description
  TString            fDigiFile;      // Digitization file

  TClonesArray*      fPoints;        // Input array of CbmMuchPoint
  TClonesArray*      fDigis;         // Output array of CbmMuchDigi
  TClonesArray*      fDigiMatches;   // Output array of CbmMuchDigiMatches
  Int_t              fNFailed;       // Total number of points which digitization has failed
  Int_t              fNOutside;      // Total number of points which was found outside a detector
  Int_t              fNMulti;        // Total number of channels that was hitby several points
  Double_t           fDTime;         // Time resolution [ns]
  Int_t              fEvent;         // Event counter
  TStopwatch         fTimer;         // Timer

  /** Map of active channels to index of MuchDigi. **/
  std::map<std::pair<Int_t, Long64_t>, Int_t> fChannelMap;

  /** Finish **/
  virtual void FinishTask();

  /** Get parameter containers **/
  virtual void SetParContainers();

  /** Initialization **/
  virtual InitStatus Init();

  /** Re-initialization **/
  virtual InitStatus ReInit();

  /** Reset eventwise counters **/
  void Reset();

  /** Simple digis production without avalanches **/
  Bool_t ExecSimple(CbmMuchPoint* point, Int_t iPoint);

  ClassDef(CbmMuchDigitizeSimpleGem,1)
};
#endif
