#ifndef CBMECALTRACKINGMUCH_H
#define CBMECALTRACKINGMUCH_H

#include "FairTask.h"

#include "TString.h"

#include <list>

class TClonesArray;
class CbmEcalStructure;
class CbmEcalInf;
class CbmStsKFTrackFitter;

class CbmEcalTrackingMuch : public FairTask
{
public:
  CbmEcalTrackingMuch(const char* name, const Int_t iVerbose=0);
public:
  /** Default constructor **/
  CbmEcalTrackingMuch() {};

  /** Initing routine **/
  virtual InitStatus Init();

  /** Loop procedure **/
  virtual void Exec(Option_t* option);

  /** Finishing routine **/
  virtual void Finish();

  /** Destructor **/
  virtual ~CbmEcalTrackingMuch() {};
private:
  /** An array of STS tracks **/
  TClonesArray* fStsTracks;			//!
  /** A calorimeter structure **/
  CbmEcalStructure* fStr;			//!
  /** Calorimeter information **/
  CbmEcalInf* fInf;				//!
  /** Calorimeter Z **/
  Float_t fCaloZ;
  /** Track fitter **/
  CbmStsKFTrackFitter* fFitter;			//!
  /** Extrapolated tracks **/
  TClonesArray* fTrackPar;			//!

  ClassDef(CbmEcalTrackingMuch,1)
};

#endif 

