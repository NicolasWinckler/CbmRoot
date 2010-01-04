/** CbmAnaDimuonAnalysisDraw.h
 *@author E.Kryshen <e.kryshen@gsi.de>
 *@since 2010-01-02
 **
 **/

#ifndef CBMANADIMUONHISTO_H_
#define CBMANADIMUONHISTO_H_ 1

#include "FairTask.h"
#include "TString.h"

class CbmAnaDimuonHisto;
class CbmAnaMuonCandidate;
class TClonesArray;
class TH1D;
class TH2D;
class TH3D;
class TLorentzVector;
class TTree;

class CbmAnaDimuonHisto : public FairTask{
public:
  /** Default constructor **/
  CbmAnaDimuonHisto();

  /** Standard constructor
  *@param name   Name of class
  *@param title  Task title
  **/
  CbmAnaDimuonHisto(const char* name, TString histoFileName);

  /** Destructor **/
  virtual ~CbmAnaDimuonHisto();

  /** Initialisation **/
  virtual InitStatus Init();

  /** Task execution **/
  virtual void Exec(Option_t* opt);

  /** Finish at the end **/
  virtual void Finish();

  /** SetParContainers **/
  virtual void SetParContainers();

  inline void SetMbinning(Double_t min, Double_t max, Int_t nBins) {
    fMmin=min;
    fMmax=max;
    fMbins=nBins;
  }
private:
  Int_t         fEvent;             //!
  TClonesArray* fMuCandidates;      //!
  TClonesArray* fDimuonCandidates;  //!
  TTree* fTree;                     //!
  TString  fHistoFileName;

  TH1D* fDimuonMmc;
  TH1D* fDimuonMrc;
  TH1D* fBgdM;
  
  Double_t fMmin;
  Double_t fMmax;
  Int_t fMbins;

  ClassDef(CbmAnaDimuonHisto,1);
};

#endif
