/** CbmMuchDigitizeStraws.h
  * Add comment
 **/

#ifndef CBMMUCHDIGITIZESTRAWS_H
#define CBMMUCHDIGITIZESTRAWS_H 1

#include "FairTask.h"

#include "TStopwatch.h"
#include "TClonesArray.h"
#include "TString.h"

#include "CbmMuchPoint.h"
#include "CbmMuchDigi.h"
#include "CbmMuchDigiMatch.h"
#include "CbmMuchGeoScheme.h"

class CbmMuchDigitizeStraws : public FairTask
{

 public:

  /** Default constructor */
  CbmMuchDigitizeStraws();

  /** Standard constructor */
  CbmMuchDigitizeStraws(Int_t iVerbose);

  /** Constructor with name */
  CbmMuchDigitizeStraws(const char* name, const char* digiFileName, Int_t iVerbose);

  /** Destructor */
  virtual ~CbmMuchDigitizeStraws();

  /** Execution */
  virtual void Exec(Option_t* opt);

  /** Sets the time resolution. **/
  void SetDTime(Double_t dTime) { fDTime = dTime; }

  /** Gets time resolution **/
  Double_t GetDTime() { return fDTime; }

 private:

  CbmMuchGeoScheme*  fGeoScheme;     // TODO add comments
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

  /** Finish **/
  virtual void FinishTask();

  /** Get parameter containers **/
  virtual void SetParContainers();

  /** Initialization **/
  virtual InitStatus Init();

  /** Reinitialization **/
  virtual InitStatus ReInit();

  /** Reset eventwise counters **/
  void Reset();

  Bool_t ExecStraws(CbmMuchPoint* point, Int_t iPoint);

  ClassDef(CbmMuchDigitizeStraws,1)
};
#endif
