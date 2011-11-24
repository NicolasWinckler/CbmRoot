// -------------------------------------------------------------------------
// -----             CbmRichProjectionProducer header file             -----
// -----   Created 23/06/05  by P.Stolpovsky(P.Stolpovsky at gsi.de)   -----
// ----- Project track by straight line from imaginary plane           -----
// ----- to the mirror and reflect it to the photodetector plane       -----
// -------------------------------------------------------------------------



#ifndef CBMRICHPROJECTIONPRODUCER_H
#define CBMRICHPROJECTIONPRODUCER_H

#include "FairTask.h"
#include "TClonesArray.h"
#include "CbmRichRing.h"
#include "FairTrackParam.h"
#include "CbmStack.h"
#include "TVector3.h"
#include "TString.h"

class CbmGeoRichPar;

class CbmRichProjectionProducer : public FairTask {

public:

   /** Default constructor */
   CbmRichProjectionProducer();

   /** Standard constructor
    *@param verbose Verbosity level
    *@param zflag Flag whether to use point in imaginary plane (zflag=1) or mirror point (zflag=2) for extrapolation
    */
   CbmRichProjectionProducer(
         Int_t verbose,
         Int_t zflag);

   /** Constructor with name and title. Puts default parameter values. */
   CbmRichProjectionProducer(
         const char *name,
         const char *title);

   /** Destructor */
   virtual ~CbmRichProjectionProducer();

   /** Initialization of the task */
   virtual InitStatus Init();

   /**  Initialization  of Parameter Containers */
   virtual void SetParContainers();

   /** Executed task */
   virtual void Exec(
         Option_t* option);

   /** Finish task */
   virtual void Finish();

protected:

   Int_t    fVerbose;// Verbosity level
   Int_t    fZflag; // Flag for extrapolation

private:

   TClonesArray* fListRICHImPlanePoint; // Starting points&directions
   TClonesArray* fListStack; // for checking workability
   TClonesArray* fProjectionTrackParam; // RICH projections as FairTrackParam

   Int_t fNHits; // Number of hits
   Int_t fEvent; // number of events

  Double_t fDetX; // X-coordinate of photodetector
  Double_t fDetY; // Y-coordinate of photodetector
  Double_t fDetZ; // Z-coordinate of photodetector
  Double_t fDetWidthX; // width of photodetector in x
  Double_t fDetWidthY; // width of photodetector in y
  Double_t fThetaDet; // tilting angle of photodetector (around x-axis)
  Double_t fPhiDet; // tilting angle of photodetector (around y-axis)
  
  Double_t fDetX_transf; // X-coordinate of photodetector (transformed system)
  Double_t fDetY_transf; // Y-coordinate of photodetector (transformed system)
  Double_t fDetZ_transf; // Z-coordinate of photodetector (transformed system)

  Double_t fZm; // Z-coordinate of mirror center
  Double_t fYm; // Y-coordinate of mirror center
  Double_t fXm; // X-coordinate of mirror center
  Double_t fR; // mirror radius
  Double_t fThetaM; // mirror tilting angle
  Double_t fSPHE_theta; // theta angle for SPHE
  Double_t fSPHE_phi; // phi angle for SPHE
  
  Double_t fmax_x; // reasonable max x value for track extrapolation
  Double_t fmax_y; // reasonable max y value for track extrapolation

  TObjArray *fSensNodes;
  TObjArray *fPassNodes;
  CbmGeoRichPar *par;

  /** Set the parameters to the default values. **/
  void SetDefaultParameters();

  ClassDef(CbmRichProjectionProducer,1)

};

#endif
