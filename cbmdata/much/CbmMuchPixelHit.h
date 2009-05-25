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
			Int_t refId);

	/** Standard constructor
	  *@param detId  Unique detector ID (including module number)
	  *@param pos Position in global c.s. [cm]
      *@param err Errors of position in global c.s. [cm]
	  *@param dxy  Covariance of x and y
	  *@param refId Index of digi or cluster
	  **/
	CbmMuchPixelHit(
			Int_t detectorId,
			const TVector3& pos,
			const TVector3& err,
			Double_t dxy,
			Int_t refId);

	/** Standard constructor
	  *@param detId     Unique detector ID (including module number)
	  *@param pos       Position in global c.s. [cm]
	  *@param err       Errors of position in global c.s. [cm]
	  *@param dxy       Covariance of x and y
	  *@param refId     index of digi or cluster
	  *@param times     Time since event start [ns]
	  *@param dTime     Time resolution [ns]
	**/
//	CbmMuchPixelHit(
//			Int_t detectorId,
//			const TVector3& pos,
//			const TVector3& err,
//			Double_t dxy,
//			Int_t refId,
//			const Double_t* times,
//			Double_t dtime);

	virtual ~CbmMuchPixelHit();

//	Double_t GetTime(Int_t i) const {return fTime[i];}
//	const Double_t* GetTimes() const {return fTime;}
//	Double_t GetDTime() const {return fDTime;}
//
//	void SetTime(Double_t time, Int_t i) {fTime[i] = time;}
//	void SetTimes(const Double_t* time) {fTime[0] = time[0]; fTime[1] = time[1]; fTime[2] = time[2];}
//	void SetDTime(Double_t dtime) {fDTime = dtime;}

//private:
//	Double_t fTime[3];    // Time since event start [ns]
//	Double_t fDTime;      // Time resolution [ns]

	ClassDef(CbmMuchPixelHit, 1);
};

#endif /* CBMMUCHPIXELHIT_H_ */
