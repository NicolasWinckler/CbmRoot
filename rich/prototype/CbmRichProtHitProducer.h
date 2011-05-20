// -------------------------------------------------------------------------
// -----                CbmRichProtHitProducer header file             -----
// -------------------------------------------------------------------------


#ifndef CBMRICHPROTHITPRODUCER_H
#define CBMRICHPROTHITPRODUCER_H

#include "CbmRichHit.h"
#include "CbmGeoRichPar.h"

#include "FairTask.h"
#include "CbmStack.h"
#include "FairBaseParSet.h"

#include "TClonesArray.h"
#include "TString.h"

class CbmRichProtHitProducer : public FairTask {

public:

  /** Default constructor **/
  CbmRichProtHitProducer();

/** Standard constructor
   *@param pmt_rad      radius of photomultiplier [cm]
   *@param pmt_dis      distance between PMT tubel [cm]
   *@param det_type     detector type (choose: 1=Protvino, 2=CsI, 3=Hamamatsu
   *@param noise        number of noise hits
   *@param verbose      verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
   *@param colleff      collection efficiency for photoelectrons in PMT optics
   *@param s_mirror     additional scattering in mirror: results in smeared point in PMT plane
   */
  CbmRichProtHitProducer(Double_t pmt_rad, Double_t pmt_dist, Int_t det_type, Int_t noise, Int_t verbose, Double_t colleff, Double_t s_mirror);

  /** Constructor with name and title. Puts default parameter values. */
  CbmRichProtHitProducer(const char *name, const char *title);

  /** Destructor **/
  virtual ~CbmRichProtHitProducer();

  /** Initialization of the task **/
  virtual void SetParContainers();
  virtual InitStatus Init();

  /** Executed task **/
  virtual void Exec(Option_t* option);

  /** Finish task **/
  virtual void Finish();

  /** method AddHit
   **
   ** Adds a RichHit to the HitCollection
   **/
  void AddHit(TVector3 &posHit, TVector3 &posHitErr,
	      Int_t detID, Int_t pmtID, Double_t ampl, Int_t index);

  /** method: FindRichHitPosition-> finds hit position in PMT plane **/
  void FindRichHitPositionSinglePMT(Double_t xPoint, Double_t yPoint,
			Double_t& xHit, Double_t& yHit, Int_t & pmtID);
  void FindRichHitPositionMAPMT(Double_t sigma, Double_t xPoint, Double_t yPoint,
			Double_t& xHit, Double_t& yHit, Int_t & pmtID);
  void FindRichHitPositionCsI(Double_t xPoint, Double_t yPoint,
			Double_t& xHit, Double_t& yHit, Int_t & pmtID);

  Double_t OnePhotonAmplitude(Double_t x);
  Double_t GetAmplitude();

  void SetPhotoDetPar(Int_t det_type, Double_t& lambda_min, Double_t& lambda_max, Double_t& lambda_step, Double_t efficiency[]);


  /** Set the parameters to the default values. **/
  void SetDefaultParameters();

protected:

private:

  TClonesArray* fListRICHpts; // RICH MC points
  TClonesArray* fHitCollection; // RICH hits
  TClonesArray* fListStack;     // Tracks
  Int_t fNHits;                // Number of hits
  Int_t fNDoubleHits;           // Number of double hits

  Double_t c;         // speed of light
  Double_t h;         // Planck constant
  Double_t e;         // elementary charge
  Double_t nrefrac;   // refraction index
  Int_t detection;    // flag for detection
  Int_t nevents;

  Double_t fDetX;       /** X-coordinate of photodetector */
  Double_t fDetY;       /** Y-coordinate of photodetector */
  Double_t fDetZ;       /** Z-coordinate of photodetector */
  Double_t fDetZ_org;   /** X-coordinate of photodetector (original from parameter file)*/
  Double_t fDetWidthX;  /** width of photodetector in x */
  Double_t fDetWidthY;  /** width of photodetector in y */


  TObjArray *fSensNodes;
  TObjArray *fPassNodes;
  CbmGeoRichPar *fPar;           //!
  FairGeoNode *fDet;
  FairRootManager* fManager;

  /** Verbosity level **/
  Int_t fVerbose;

  /** Parameters of photodetector **/
  Double_t fPhotomulRadius;   //** radius of photomultiplier */
  Double_t fPhotomulDist;     //** radius of photomultiplier */
  Int_t  fDetType;            //** detector type */
  Int_t  fNoise;              //** number of noise hits */
  Double_t  fColl;            //** collection efficiency for photoelectrons in PMT optics */
  Double_t  fSMirror;         //** additinal smearing of MC Points due to light scattering in mirror */

  Double_t theta;   //** theta = angle by which photodetector was tilted (around x-axis)*/
  Double_t phi;   //** phi = angle by which photodetector was tilted (around y-axis)*/

  ClassDef(CbmRichProtHitProducer,1)

};

#endif
