/** CbmPixelHit.h
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2009
 * @version 1.0
 **
 ** Base class for pixel hits used for tracking in CBM.
 ** Derives from CbmBaseHit.
 ** Additional members are x, y coordinates and x, y, dxy covariances.
 **/
#ifndef CBMPIXELHIT_H_
#define CBMPIXELHIT_H_

#include "CbmBaseHit.h"

#include "TVector3.h"

class CbmPixelHit : public CbmBaseHit
{
public:
	/** Default constructor */
	CbmPixelHit();

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
	CbmPixelHit(
			Int_t detectorId,
			Double_t x,
			Double_t y,
			Double_t z,
			Double_t dx,
			Double_t dy,
			Double_t dz,
			Double_t dxy,
			Int_t refId);

	/**
	 * Standard constructor.
	 * @param detectorId detector unique identifier
	 * @param pos position of the hit as TVector3 [cm]
	 * @param err position errors of the hit as TVector3 [cm]
	 * @param dxy XY correlation of the hit
	 * @param refId some reference ID
	 **/
	CbmPixelHit(
			Int_t detectorId,
			const TVector3& pos,
			const TVector3& err,
			Double_t dxy,
			Int_t refId);

	/** Destructor */
	virtual ~CbmPixelHit();

	/**
	 * Derived from CbmBaseHit. Virtual function. Should be reimplemented in derived class.
	 * Prints the CbmPixelHit object current state to the standard std::cout output.
	 **/
	virtual void Print() const;

	/** Returns X position of the hit */
	Double_t GetX() const {return fX;}

	/** Returns Y position of the hit */
	Double_t GetY() const {return fY;}

	/** Returns X position error of the hit */
	Double_t GetDx() const {return fDx;}

	/** Returns Y position error of the hit */
	Double_t GetDy() const {return fDy;}

	/** Returns XY correlation */
	Double_t GetDxy() const {return fDxy;}

	/**
	 * Copies hit position to pos
	 * @param pos Output hit position
	 */
	void Position(TVector3& pos) const;

	/**
	 * Copies hit position error to pos
	 * @param pos Output hit position error
	 */
	void PositionError(TVector3& dpos) const;

	/**
	 * Sets X position of the hit.
	 * @param x new X position of the hit [cm]
	 **/
	void SetX(Double_t x) {fX = x;}

	/**
	 * Sets Y position of the hit.
	 * @param y new Y position of the hit [cm]
	 **/
	void SetY(Double_t y) {fY = y;}

	/**
	 * Sets X position error of the hit.
	 * @param dx new Y position error of the hit [cm]
	 **/
	void SetDx(Double_t dx) {fDx = dx;}

	/**
	 * Sets Y position error of the hit.
	 * @param dy new Y position error of the hit [cm]
	 **/
	void SetDy(Double_t dy) {fDy = dy;}

	/**
	 * Sets XY correlation.
	 * @param dxy new XY correlation
	 **/
	void SetDxy(Double_t dxy) {fDxy = dxy;}

	/**
	 * Sets position of the hit.
	 * @param pos new hit position
	 **/
	void SetPosition(const TVector3& pos);

	/**
	 * Sets position error of the hit.
	 * @param dpos new hit position error
	 **/
	void SetPositionError(const TVector3& dpos);

private:
	Double_t fX, fY; // X, Y positions of hit [cm]
	Double_t fDx, fDy; // X, Y errors [cm]
	Double_t fDxy; // XY correlation

	ClassDef(CbmPixelHit, 1);
};


inline void CbmPixelHit::Position(TVector3& pos) const
{
	pos.SetXYZ(GetX(), GetY(), GetZ());
}


inline void CbmPixelHit::PositionError(TVector3& dpos) const
{
	dpos.SetXYZ(GetDx(), GetDy(), GetDz());
}

inline void CbmPixelHit::SetPosition(const TVector3& pos)
{
	SetX(pos.X());
	SetY(pos.Y());
	SetZ(pos.Z());
}

inline void CbmPixelHit::SetPositionError(const TVector3& dpos)
{
	SetDx(dpos.X());
	SetDy(dpos.Y());
	SetDz(dpos.Z());
}

#endif /* CBMPIXELHIT_H_ */
