/** CbmMuchStrawHit.h
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2009
 * @version 1.0
 **
 ** Class for straw hits in MUCH detector.
 ** Derives from CbmStripHit.
 **/
#ifndef CBMMUCHSTRAWHIT_H_
#define CBMMUCHSTRAWHIT_H_

#include "CbmStripHit.h"

class TVector3;

class CbmMuchStrawHit : public CbmStripHit
{
public:
	/** Default constructor */
	CbmMuchStrawHit();

	/**
	 * Standard constructor.
	 * @param detectorId detector unique identifier
	 * @param u coordinate in the rotated c.s. [cm]
	 * @param phi strip rotation angle [rad]
	 * @param z Z position of the hit [cm]
	 * @param du U measurement error [cm]
	 * @param dphi PHI measurement error [rad]
	 * @param z Z position of the hit [cm]
	 * @param refId some reference ID
	 **/
	CbmMuchStrawHit(
			Int_t detectorId,
			Double_t u,
			Double_t phi,
			Double_t z,
			Double_t du,
			Double_t dphi,
			Double_t dz,
			Int_t refId,
			Int_t planeId);

	/** Standard constructor
		*@param detId Unique detector ID (including module number)
		*@param pos U coordinate, rotation angle Phi, Z position in c.s.
		*@param err Errors c.s. [cm]
		*@param refId Index of digi or cluster
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
//	CbmMuchStrawHit(
//			Int_t detectorId,
//			const TVector3& pos,
//			const TVector3& err,
//			Int_t refId,
//			const Double_t* times,
//			Double_t dtime);

	virtual ~CbmMuchStrawHit();

//	Double_t GetTime(Int_t i) const {return fTime[i];}
//	const Double_t* GetTimes() const {return fTime;}
//	Double_t GetDTime() const {return fDTime;}
//
//	void SetTime(Double_t time, Int_t i) {fTime[i] = time;}
//	void SetTimes(const Double_t* time) {fTime[0] = time[0]; fTime[1] = time[1]; fTime[2] = time[2];}
//	void SetDTime(Double_t dtime) {fDTime = dtime;}
        Double_t GetX() const {return fX;}
        Double_t GetY() const {return fY;}
        Double_t* GetDouble() { return fDouble; }
        Int_t* GetInt() { return fInt; }
        Int_t GetFlag() const { return fFlag; }
	//Int_t GetPlaneId() const { Fatal(" !!! CbmMuchStrawHit::GetPlaneId()"," !!! CbmMuchStrawHit::GetPlaneId()"); }
        Int_t GetPlaneId() const { return fPlaneId; }
        Int_t GetTube() const { return fTube; }
        Int_t GetSegment() const { return fSegment; }

        void SetX(Double_t x) {fX = x;}
        void SetY(Double_t y) {fY = y;}
        void SetDouble(Int_t n, Double_t *arr) { for (Int_t i=0; i<n; ++i) fDouble[i]=arr[i]; } // set double array
        void SetInt(Int_t n, Int_t *arr) { for (Int_t i=0; i<n; ++i) fInt[i]=arr[i]; } ; // set integer array
        void SetFlag(Int_t flag) { fFlag = flag; }
        void SetPlaneId(Int_t planeId) { fPlaneId = planeId; }
        void SetTube(Int_t tube) { fTube = tube; }
        void SetSegment(Int_t segm) { fSegment = segm; }

private:
//	Double_t fTime[3];    // Time since event start [ns]
//	Double_t fDTime;      // Time resolution [ns]
        Double32_t fX;        // Global X-coord.
        Double32_t fY;        // Global Y-coord.
        Double_t fDouble[5];  //! Auxiliary double array
        Int_t fInt[5];        //! Auxiliary integer array
        Int_t fFlag;          // Flag
        Int_t fPlaneId;       // Plane ID
        Int_t fTube;          // Tube number
        Int_t fSegment;       // Tube segment

	ClassDef(CbmMuchStrawHit, 1);
};

#endif /* CBMMUCHSTRAWHIT_H_ */
