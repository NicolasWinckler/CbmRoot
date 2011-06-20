// -------------------------------------------------------------------------
// -----                  CbmRichHitProducer header file               -----
// -----               Created 21/05/04  by B. Polichtchouk            -----
// -----                  modified 13/07/04  by C. Hoehne              -----
// -------------------------------------------------------------------------


/**  CbmRichHitProducer.h
 *@author B. Polichtchouk
 **
 ** Class for producing RICH hits directly from MCPoints
 ** See also macro/rich/CbmRichHitsProd.C
 **/


#ifndef CBMRICHHITPRODUCER_H
#define CBMRICHHITPRODUCER_H

#include "CbmRichHit.h"
#include "CbmGeoRichPar.h"

#include "FairTask.h"
#include "CbmStack.h"
#include "FairBaseParSet.h"

#include "TClonesArray.h"
#include "TString.h"

class CbmRichHitProducer : public FairTask {

public:

  /* Default constructor */
  CbmRichHitProducer();

  /* Standard constructor
   *@param pmt_rad Radius of photomultiplier [cm]
   *@param pmt_dis Distance between PMT tubel [cm]
   *@param det_type Detector type (choose: 1=Protvino, 2=CsI, 3=Hamamatsu
   *@param noise Number of noise hits
   *@param verbose Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
   *@param colleff Collection efficiency for photoelectrons in PMT optics
   *@param s_mirror Additional scattering in mirror: results in smeared point in PMT plane*/
  CbmRichHitProducer(
        Double_t pmt_rad,
        Double_t pmt_dist,
        Int_t det_type,
        Int_t noise,
        Int_t verbose,
        Double_t colleff,
        Double_t s_mirror);

  /* Constructor with name and title. Puts default parameter values.*/
  CbmRichHitProducer(const char *name, const char *title);

  /* Destructor */
  virtual ~CbmRichHitProducer();

  /* Initialization of the task */
  virtual void SetParContainers();

  virtual InitStatus Init();

  /* Executed task
   * @param option*/
  virtual void Exec(
        Option_t* option);

  /* Add cross talk hits
   * @param x X position of the central hit
   * @param y Y position of the central hit
   * @param pointInd Index of the Rich Point of the central hit
   * @param RichDetID ID of the RICH detector*/
  void AddCrossTalkHits(
        Double_t x,
        Double_t y,
        Int_t pointInd,
        Int_t RichDetID);

  /* Finish task */
  virtual void Finish();

  /* Adds a RichHit to the HitCollection
   * @param posHit
   * @param posHitErr
   * @param detID
   * @param pmtID
   * @param ampl
   * @param index*/
  void AddHit(
        TVector3 &posHit,
        TVector3 &posHitErr,
	     Int_t detID,
	     Int_t pmtID,
	     Double_t ampl,
	     Int_t index);

  /* Finds hit position in PMT plane
   * @param xPoint
   * @param yPoint
   * @param xHit
   * @param yHit
   * @param pmtID*/
  void FindRichHitPositionSinglePMT(
        Double_t xPoint,
        Double_t yPoint,
        Double_t& xHit,
        Double_t& yHit,
        Int_t& pmtID);

  /* Finds hit position in MAMPT plane]
   * @param sigma
   * @param xPoint
   * @param yPoint
   * @param xHit
   * @param yHit
   * @param pmtID*/
  void FindRichHitPositionMAPMT(
        Double_t sigma,
        Double_t xPoint,
        Double_t yPoint,
        Double_t& xHit,
        Double_t& yHit,
        Int_t & pmtID);

  /* Finds hit position in MAMPT plane]
   * @param xPoint
   * @param yPoint
   * @param xHit
   * @param yHit
   * @param pmtID*/
  void FindRichHitPositionCsI(
        Double_t xPoint,
        Double_t yPoint,
        Double_t& xHit,
        Double_t& yHit,
        Int_t& pmtID);

  Double_t OnePhotonAmplitude(
        Double_t x);

  Double_t GetAmplitude();

  void SetPhotoDetPar(
        Int_t det_type,
        Double_t& lambda_min,
        Double_t& lambda_max,
        Double_t& lambda_step,
        Double_t efficiency[]);

  /* Set the parameters to the default values. */
  void SetDefaultParameters();

private:

  TClonesArray* fListRICHpts; // RICH MC points
  TClonesArray* fHitCollection; // RICH hits
  TClonesArray* fListStack; // Tracks
  Int_t fNHits; // Number of hits
  Int_t fNDoubleHits; // Number of double hits

  Double_t c; // speed of light
  Double_t h; // Planck constant
  Double_t e; // elementary charge
  Double_t nrefrac; // refraction index
  Int_t detection; // flag for detection
  Int_t nevents;

  Double_t fDetX; // X-coordinate of photodetector
  Double_t fDetY; // Y-coordinate of photodetector
  Double_t fDetZ; // Z-coordinate of photodetector
  Double_t fDetZ_org; // X-coordinate of photodetector (original from parameter file)
  Double_t fDetWidthX; // width of photodetector in x
  Double_t fDetWidthY; // width of photodetector in y

  TObjArray *fSensNodes;
  TObjArray *fPassNodes;
  CbmGeoRichPar *fPar;

  Int_t fVerbose; // Verbosity level

  // Parameters of photodetector
  Double_t fPhotomulRadius; // radius of photomultiplier
  Double_t fPhotomulDist; // radius of photomultiplier
  Int_t fDetType; // detector type
  Int_t fNoise; // number of noise hits
  Double_t fColl; // collection efficiency for photoelectrons in PMT optics
  Double_t fSMirror; // additinal smearing of MC Points due to light scattering in mirror

  Double_t theta; // theta = angle by which photodetector was tilted (around x-axis)
  Double_t phi; // phi = angle by which photodetector was tilted (around y-axis)

  Double_t fCrossTalkHitProb;// probability of cross talk hit
  Int_t fNofCrossTalkHits; // Number of cross talk hits

  ClassDef(CbmRichHitProducer,1)
};

#endif
