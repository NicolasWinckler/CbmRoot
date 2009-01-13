#ifndef CBMECALTIMEDIGITIZER_H
#define CBMECALTIMEDIGITIZER_H

#include "CbmTask.h"

#include "TString.h"

#include <list>

class CbmEcalStructure;
class CbmEcalCell;
class CbmEcalCalibration;
class TFormula;

class CbmEcalTimeDigitizer : public CbmTask
{
public:
  CbmEcalTimeDigitizer(const char* name, const Int_t iVerbose=0, const char* config="");
public:
  /** Default constructor **/
  CbmEcalTimeDigitizer() {};

  /** Initing routine **/
  virtual InitStatus Init();

  /** Loop procedure **/
  virtual void Exec(Option_t* option);

  /** Finishing routine **/
  virtual void Finish();

  /** Destructor **/
  virtual ~CbmEcalTimeDigitizer() {};
private:
  /** Calculate time in given cell **/
  void CalculateTime(CbmEcalCell* cell);
  /** Set size of time/energy deposition array **/
  void SetArraySize(Int_t size);
  /** Calorimeter structure **/
  CbmEcalStructure* fStr;		//!
  /** All cells in the calorimeter **/
  std::list<CbmEcalCell*> fCells;		//!
  /** Formuta of time response **/
  TFormula* fResponse;			//!
  /** Name of configuration file **/
  TString fConfigName;			//!
  /** Enregy threshold for the cell **/
  Double_t fThreshold;
  /** Event number **/
  Int_t fEvent;
  /** A calibration of the calorimeter **/
  CbmEcalCalibration* fCal;		//!

  /** An array of emergy deposition with time **/
  Int_t fSize;
  Double_t* fE;				//!
  Double_t* fT;				//!
  Int_t* fI;				//!

  ClassDef(CbmEcalTimeDigitizer,1)
};

#endif 

