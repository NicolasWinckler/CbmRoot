/*
 * CbmMuchStrawHit.h
 *
 *  Created on: Mar 19, 2009
 *      Author: andrey
 */

#ifndef CBMMUCHSTRAWHIT_H_
#define CBMMUCHSTRAWHIT_H_

#include "CbmStripHit.h"

class TVector3;

class CbmMuchStrawHit : public CbmStripHit
{
public:
	CbmMuchStrawHit();
	/** Standard constructor
		*@param detId     Unique detector ID (including module number)
		*@param pos       U coordinate, rotation angle Phi, Z position in c.s.
		*@param err       Errors c.s. [cm]
		*@param refId     Index of digi or cluster
		**/
	CbmMuchStrawHit(
			Int_t detectorId,
			const TVector3& pos,
			const TVector3& err,
			Int_t refId);

	/** Standard constructor
	*@param detId     Unique detector ID (including module number)
	*@param pos       U coordinate, rotation angle Phi, Z position in c.s.
	*@param err       Errors in global c.s. [cm]
	*@param refId     index of digi or cluster
	*@param times     Time since event start [ns]
	*@param dtime     Time resolution [ns]
	**/
	CbmMuchStrawHit(
			Int_t detectorId,
			const TVector3& pos,
			const TVector3& err,
			Int_t refId,
			const Double_t* times,
			Double_t dtime);
	virtual ~CbmMuchStrawHit();

	Double_t GetTime(Int_t i) const {return fTime[i];}
	const Double_t* GetTimes() const {return fTime;}
	Double_t GetDTime() const {return fDTime;}

	void SetTime(Double_t time, Int_t i) {fTime[i] = time;}
	void SetTimes(const Double_t* time) {fTime[0] = time[0]; fTime[1] = time[1]; fTime[2] = time[2];}
	void SetDTime(Double_t dtime) {fDTime = dtime;}

private:
	Double_t fTime[3];    // Time since event start [ns]
	Double_t fDTime;      // Time resolution [ns]

	ClassDef(CbmMuchStrawHit, 1);
};

#endif /* CBMMUCHSTRAWHIT_H_ */
