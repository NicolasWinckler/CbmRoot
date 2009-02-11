/******************************************************************************
*  $Id: CbmRichTestHits.h,v 1.1 2006/02/09 16:35:24 hoehne Exp $
*
*  Class  : CbmRichTestHits
*  Description: Header file of CbmRichTestHits:
*               This class fills some histograms for a fast check of simulated data
*               Use macro macro/rich/CbmRichTestHits.C
*
*  Author : Claudia Hoehne
*  E-mail : c.hoehne@gsi.de
*
*******************************************************************************
*  $Log: CbmRichTestHits.h,v $
*  Revision 1.1  2006/02/09 16:35:24  hoehne
*  initial version: class for fast check of HitProducer
*
*
*
*
*******************************************************************************/



#ifndef CBMRICHTestHits_H
#define CBMRICHTestHits_H

#include "CbmGeoRichPar.h"

#include "FairTask.h"
#include "FairRootManager.h"
#include "FairBaseParSet.h"

#include "TClonesArray.h"
#include "TH1D.h"
#include "TH2D.h"

class CbmRichTestHits : public FairTask {

public:

  /** Default constructor **/
  CbmRichTestHits();

  /** Standard Constructor with name, title, and verbosity level
    *@param verbose      verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
  */
  CbmRichTestHits(const char *name, const char *title, Int_t verbose);

  /** Destructor **/
  virtual ~CbmRichTestHits();

  /** Initialization of the task **/
  virtual void SetParContainers();
  virtual InitStatus Init();

  /** Executed task **/
  virtual void Exec(Option_t* option);

  /** Finish task **/
  virtual void Finish();

protected:


private:

  TClonesArray* fRichHitArray;     // RICH hits
  TClonesArray* fMCRichPointArray;     // RICH MC points
  TClonesArray* fMCTrackArray;         // MC Tracks

  Int_t nevents;

  /** Verbosity level **/
  Int_t fVerbose;

  // histograms
  TH2D *fh_Det1ev;      //! photodetector plane for 1st event
  TH2D *fh_Det1ev_zoom; //! photodetector plane for 1st event, zoom in
  TH2D *fh_Detall;      //! photodetector plane for all events
  TH2D *fh_Detall_zoom; //! photodetector plane for all events, zoom in for fine-structure
  TH2D *fh_n_vs_p;      //! Number of hits versus momentum

  TH1D *fh_Nhits;        //! Number of hits/ring

  TH1D *fh_Nall;        //! Number of all rings
  TH1D *fh_Nel;         //! Number of electron rings
  TH1D *fh_Nelprim;     //! Number of electron rings with STS>=6
  TH1D *fh_Npi;         //! Number of pion rings
  TH1D *fh_Nk;          //! Number of kaon rings
  TH1D *fh_Nhad;        //! Number of hadrons crossing the PMT plane
  TH1D *fh_Nnoise;      //! Number of noise hits

  // geometry parameters
  TObjArray *fSensNodes;
  CbmGeoRichPar *fPar;           //!
  Double_t fDetZ;       /** Z-coordinate of photodetector */

  ClassDef(CbmRichTestHits,1)

};

#endif
