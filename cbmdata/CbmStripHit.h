/** CbmStripHit.h
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2009
 * @version 1.0
 **
 ** Base class for strip-like hits used for tracking in CBM.
 ** Derives from CbmBaseHit.
 ** Additional members are u coordinate, phi angle and du, dphi measurement errors.
 **/
#ifndef CBMSTRIPHIT_H_
#define CBMSTRIPHIT_H_

#include "CbmBaseHit.h"

class TVector3;

class CbmStripHit :public CbmBaseHit
{
public:
	/** Default constructor */
	CbmStripHit();

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
	CbmStripHit(
			Int_t detectorId,
			Double_t u,
			Double_t phi,
			Double_t z,
			Double_t du,
			Double_t dphi,
			Double_t dz,
			Int_t refId);

	/**
	 * Standard constructor.
	 * @param detectorId detector unique identifier
	 * @param pos position of the hit as TVector3 (u, phi, z) [cm]
	 * @param err position errors of the hit as TVector3 (du, dphi, dz) [cm]
	 * @param refId some reference ID
	 **/
	CbmStripHit(
			Int_t detectorId,
			const TVector3& pos,
			const TVector3& err,
			Int_t refId);

	/** Destructor */
	virtual ~CbmStripHit();

	/**
	 * Derived from CbmBaseHit. Virtual function. Should be reimplemented in derived class.
	 * Prints the CbmStripHit object current state to the standard std::cout output.
	 **/
	virtual void Print() const;

	/** Returns U position in the rotated c.s. */
	Double_t GetU() const {return fU;}

	/** Returns strip rotation angle */
	Double_t GetPhi() const {return fPhi;}

	/** Returns U measurement error */
	Double_t GetDu() const {return fDu;}

	/** Returns Phi measurement error*/
	Double_t GetDphi() const {return fDphi;}

	/**
	 * Sets U position in the rotated c.s.
	 * @param u new U position in the rotated c.s. [cm]
	 **/
	void SetU(Double_t u) {fU = u;}

	/**
	 * Sets strip rotation angle.
	 * @param phi new strip rotation angle [rad]
	 **/
	void SetPhi(Double_t phi) {fPhi = phi;}

	/**
	 * Sets coordinate error.
	 * @param du new coordinate error [cm]
	 **/
	void SetDu(Double_t du) {fDu = du;}

	/**
	 * Sets strip rotation angle error.
	 * @param dphi new strip rotation angle error [rad]
	 **/
	void SetDphi(Double_t dphi) {fDphi = dphi;}

private:
	Double_t fU; // U coordinate in the rotated c.s [cm]
	Double_t fDu; // U error [cm]
	Double_t fPhi; // strip rotation angle [rad]
	Double_t fDphi; // strip rotation error [rad]

	ClassDef(CbmStripHit, 1);
};

#endif /* CBMSTRIPHIT_H_ */
