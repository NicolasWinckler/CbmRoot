/******************************************************************************
*  $Id: CbmRichTestSim.h,v 1.2 2006/02/09 16:32:02 hoehne Exp $
*
*  Class  : CbmRichTestSim
*  Description: Header file of CbmRichTestSim:
*               This class fills some histograms for a fast check of simulated data
*               Use macro macro/rich/CbmRichTestSim.C
*
*  Author : Claudia Hoehne
*  E-mail : c.hoehne@gsi.de
*
*******************************************************************************
*  $Log: CbmRichTestSim.h,v $
*  Revision 1.2  2006/02/09 16:32:02  hoehne
*  improve
*
*  Revision 1.1  2006/02/03 08:59:25  hoehne
*  initial version: class and header for performing a fast test of the simulation output for RICH
*
*
*
*******************************************************************************/



#ifndef CBMRICHTestSim_H
#define CBMRICHTestSim_H

#include "CbmGeoRichPar.h"

#include "FairTask.h"
#include "FairRootManager.h"
#include "FairBaseParSet.h"

#include "TClonesArray.h"
#include "TH1D.h"
#include "TH2D.h"

class CbmRichTestSim : public FairTask {

public:

  /** Default constructor **/
  CbmRichTestSim();

  /** Standard Constructor with name, title, and verbosity level
    *@param verbose      verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
  */
  CbmRichTestSim(const char *name, const char *title, Int_t verbose);

  /** Destructor **/
  virtual ~CbmRichTestSim();

  /** Initialization of the task **/
  virtual void SetParContainers();
  virtual InitStatus Init();

  /** Executed task **/
  virtual void Exec(Option_t* option);

  /** Finish task **/
  virtual void Finish();

protected:


private:

  TClonesArray* fMCRichPointArray;     // RICH MC points
  TClonesArray* fMCTrackArray;         // MC Tracks

  Int_t nevents;

  /** Verbosity level **/
  Int_t fVerbose;

  // histograms
  TH2D *fh_Det1ev;      //! photodetector plane for 1st event
  TH2D *fh_Det1ev_zoom; //! photodetector plane for 1st event, zoom in
  TH2D *fh_Detall;      //! photodetector plane for all events
  TH2D *fh_n_vs_p;      //! Number of points versus momentum
  TH2D *fh_v_el;        //! (y,z) position of production vertex for electrons

  TH1D *fh_Nall;        //! Number of all rings
  TH1D *fh_Nel;         //! Number of electron rings
  TH1D *fh_Nelprim;     //! Number of electron rings with STS>=6
  TH1D *fh_Npi;         //! Number of pion rings
  TH1D *fh_Nk;          //! Number of kaon rings
  TH1D *fh_Nhad;        //! Number of all hadrons crossing the PMT plane

  // geometry parameters
  TObjArray *fSensNodes;
  CbmGeoRichPar *fPar;           //!
  Double_t fDetZ;       /** Z-coordinate of photodetector */

  ClassDef(CbmRichTestSim,1)

};

#endif
