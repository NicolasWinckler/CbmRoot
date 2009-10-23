
/* $Id: CbmRichHitProducer.h,v 1.6 2006/07/07 13:48:53 hoehne Exp $ */

/* History of cvs commits:
 *
 * $Log: CbmRichHitProducer.h,v $
 * Revision 1.6  2006/07/07 13:48:53  hoehne
 * -> 3 different types of photodetectors may be chosen differing in geometry (pixelsize) and quantum efficiency
 *    specify 1 = single PMTS a la Protvino
 *            2 = MAPMT, Hamamatsu H8500
 *            3 = CsI, panels similar to ALICE
 *    for DetType when running the HitProducer
 *
 * Revision 1.5  2006/02/09 16:27:16  hoehne
 * noise added, trackID removed (go via RefIndex)
 *
 *
 *
 */
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

  /** Default constructor **/
  CbmRichHitProducer();

  /** Standard constructor
   *@param pmt_rad      radius of photomultiplier [cm]
   *@param pmt_dis      distance between PMT tubel [cm]
   *@param det_type     detector type (choose: 1=Protvino, 2=CsI, 3=Hamamatsu
   *@param noise        number of noise hits
   */
  CbmRichHitProducer(Double_t pmt_rad, Double_t pmt_dist, Int_t det_type, Int_t noise);

  /** Standard constructor
   *@param pmt_rad      radius of photomultiplier [cm]
   *@param pmt_dis      distance between PMT tubel [cm]
   *@param det_type     detector type (choose: 1=Protvino, 2=CsI, 3=Hamamatsu
   *@param noise        number of noise hits
   *@param verbose      verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
   */
  CbmRichHitProducer(Double_t pmt_rad, Double_t pmt_dist, Int_t det_type, Int_t noise, Int_t verbose);

/** Standard constructor
   *@param pmt_rad      radius of photomultiplier [cm]
   *@param pmt_dis      distance between PMT tubel [cm]
   *@param det_type     detector type (choose: 1=Protvino, 2=CsI, 3=Hamamatsu
   *@param noise        number of noise hits
   *@param verbose      verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
   *@param colleff      collection efficiency for photoelectrons in PMT optics
   */
  CbmRichHitProducer(Double_t pmt_rad, Double_t pmt_dist, Int_t det_type, Int_t noise, Int_t verbose, Double_t colleff);

  /** Constructor with name and title. Puts default parameter values. */
  CbmRichHitProducer(const char *name, const char *title);

  /** Destructor **/
  virtual ~CbmRichHitProducer();

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

  /** Verbosity level **/
  Int_t fVerbose;
  
  /** Parameters of photodetector **/
  Double_t fPhotomulRadius;   //** radius of photomultiplier */
  Double_t fPhotomulDist;     //** radius of photomultiplier */
  Int_t  fDetType;            //** detector type */
  Int_t  fNoise;              //** number of noise hits */
  Double_t  fColl;            //** collection efficiency for photoelectrons in PMT optics */
  
  Double_t theta;   //** theta = angle by which photodetector was tilted (around x-axis)*/ 
  Double_t phi;   //** phi = angle by which photodetector was tilted (around y-axis)*/ 

  ClassDef(CbmRichHitProducer,1)

};

#endif
