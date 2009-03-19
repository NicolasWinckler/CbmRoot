/*
 * CbmMuchPixelHit.h
 *
 *  Created on: Mar 19, 2009
 *      Author: andrey
 */

#ifndef CBMMUCHPIXELHIT_H_
#define CBMMUCHPIXELHIT_H_

#include "CbmPixelHit.h"

class TVector3;

class CbmMuchPixelHit :public CbmPixelHit
{
public:
	CbmMuchPixelHit();
	/** Standard constructor
		*@param detId     Unique detector ID (including module number)
		*@param pos       Position in global c.s. [cm]
		*@param err       Errors of position in global c.s. [cm]
		*@param dxy       Covariance of x and y
		*@param refId     Index of digi or cluster
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
	CbmMuchPixelHit(
			Int_t detectorId,
			const TVector3& pos,
			const TVector3& err,
			Double_t dxy,
			Int_t refId,
			const Double_t* times,
			Double_t dtime);

	virtual ~CbmMuchPixelHit();

	Double_t GetTime(Int_t i) const {return fTime[i];}
	const Double_t* GetTimes() const {return fTime;}
	Double_t GetDTime() const {return fDTime;}

	void SetTime(Double_t time, Int_t i) {fTime[i] = time;}
	void SetTimes(const Double_t* time) {fTime[0] = time[0]; fTime[1] = time[1]; fTime[2] = time[2];}
	void SetDTime(Double_t dtime) {fDTime = dtime;}

private:
	Double_t fTime[3];    // Time since event start [ns]
	Double_t fDTime;      // Time resolution [ns]

	ClassDef(CbmMuchPixelHit, 1);
};

#endif /* CBMMUCHPIXELHIT_H_ */
