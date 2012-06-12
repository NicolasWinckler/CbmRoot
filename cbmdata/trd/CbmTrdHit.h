/** CbmTrdHit.h
 ** updated by Andrey Lebedev 26/05/2009
 **
 ** Class for hits in TRD detector.
 ** Derives from CbmPixelHit.
 **/

#ifndef CBMTRDHIT_H_
#define CBMTRDHIT_H_

#include "CbmPixelHit.h"

class TVector3;

class CbmTrdHit : public CbmPixelHit
{
public:
	/** Default constructor */
	CbmTrdHit();

	/** Standard constructor
	  *@param detectorId Unique detector ID
	  *@param pos Position in global c.s. [cm]
          *@param dpos Errors of position in global c.s. [cm]
	  *@param dxy XY correlation of the hit
	  *@param refId Index of digi or cluster
	  *@param planeId Detector plane identifier
	  *@param eLossTR Energy losses of electron via TR
	  *@param eLossdEdx Energy deposition in the detector without TR
	  *@param eLoss TR + dEdx
	  **/
	CbmTrdHit(
			Int_t detectorId,
			TVector3& pos,
			TVector3& dpos,
			Double_t dxy,
			Int_t refId,
			Int_t planeId,
			Double_t eLossTR,
			Double_t eLossdEdx,
			Double_t eLoss);

	virtual ~CbmTrdHit();
	Int_t GetDetId() const { return fDetId; }
	Int_t GetPlaneId() const { return fPlaneId; }
	Double_t GetELoss() const { return fELoss; }
	Double_t GetELossdEdX() const { return fELossdEdx; }
	Double_t GetELossTR() const { return fELossTR; }

	void SetELoss(Double_t loss) { fELoss = loss; }
	void SetELossTR(Double_t loss) { fELossTR = loss; }
	void SetELossdEdx(Double_t loss) { fELossdEdx = loss; }

private:
    Int_t           fDetId;
    Int_t           fPlaneId;   // unique ID of TRD plane in the setup 
    Double32_t      fELossTR;   // energy losses of electron via TR
    Double32_t      fELossdEdx; // energy deposition in the detector without TR
    Double32_t      fELoss;     // TR + dEdx

    ClassDef(CbmTrdHit, 2);
};

#endif
