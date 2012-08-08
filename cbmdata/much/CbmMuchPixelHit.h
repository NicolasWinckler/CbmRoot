/** CbmMuchPixelHit.h
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2009
 * @version 1.0
 **
 ** Class for pixel hits in MUCH detector.
 ** Derives from CbmPixelHit.
 **/
#ifndef CBMMUCHPIXELHIT_H_
#define CBMMUCHPIXELHIT_H_

#include "CbmPixelHit.h"

class TVector3;

class CbmMuchPixelHit :public CbmPixelHit
{
public:
	CbmMuchPixelHit();

	/**
	 * Standard constructor.
	 * @param detectorId detector unique identifier
	 * @param x X pposition of the hit [cm]
	 * @param y Y position of the hit [cm]
	 * @param z Z position of the hit [cm]
	 * @param dx X position error of the hit [cm]
	 * @param dy Y position error of the hit [cm]
	 * @param dz Z position error of the hit [cm]
	 * @param dxy XY correlation of the hit
	 * @param refId some reference ID
	 * @param planeId detector plane identifier
	 **/
	CbmMuchPixelHit(
			Int_t detectorId,
			Double_t x,
			Double_t y,
			Double_t z,
			Double_t dx,
			Double_t dy,
			Double_t dz,
			Double_t dxy,
			Int_t refId,
			Int_t planeId,
      Double_t t,
      Double_t dt);

	/** Standard constructor
	  *@param detId  Unique detector ID (including module number)
	  *@param pos Position in global c.s. [cm]
      *@param err Errors of position in global c.s. [cm]
	  *@param dxy  XY correlation of the hit
	  *@param refId Index of digi or cluster
	  *@param planeId detectro plane identifier
	  **/
	CbmMuchPixelHit(
			Int_t detectorId,
			const TVector3& pos,
			const TVector3& err,
			Double_t dxy,
			Int_t refId,
			Int_t planeId);

	/** Standard constructor
	  *@param detId     Unique detector ID (including module number)
	  *@param pos       Position in global c.s. [cm]
	  *@param err       Errors of position in global c.s. [cm]
	  *@param dxy       Covariance of x and y
	  *@param refId     index of digi or cluster
	  *@param planeId   detectror plane identifier
	  *@param time      Time since event start [ns]
	  *@param dTime     Time resolution [ns]
	**/
	CbmMuchPixelHit(
			Int_t detectorId,
			const TVector3& pos,
			const TVector3& err,
			Double_t dxy,
			Int_t refId,
			Int_t planeId,
			Double_t time,
			Double_t dtime);

	virtual ~CbmMuchPixelHit();

	virtual Int_t GetPlaneId() const { return fPlaneId; }

	Double_t GetTime() const {return fTime;}
	Double_t GetDTime() const {return fDTime;}

	void SetTime(Double_t time) {fTime = time;}
	void SetDTime(Double_t dtime) {fDTime = dtime;}

private:
  Int_t fPlaneId;       // Plane number
  Double_t fTime;       // Time since event start [ns]
	Double_t fDTime;      // Time resolution [ns]

	ClassDef(CbmMuchPixelHit, 1);
};

#endif /* CBMMUCHPIXELHIT_H_ */
