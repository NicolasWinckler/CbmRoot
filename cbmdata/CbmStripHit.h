/*
 * CbmStripHit.h
 *
 *  Created on: Mar 18, 2009
 *      Author: andrey
 */

#ifndef CBMSTRIPHIT_H_
#define CBMSTRIPHIT_H_

#include "CbmBaseHit.h"

class TVector3;

class CbmStripHit :public CbmBaseHit {
public:
	CbmStripHit();
	CbmStripHit(
			Int_t detectorId,
			Double_t u,
			Double_t phi,
			Double_t z,
			Double_t du,
			Double_t dphi,
			Double_t dz,
			Int_t refId);
	CbmStripHit(
			Int_t detectorId,
			const TVector3& pos,
			const TVector3& err,
			Int_t refId);
	virtual ~CbmStripHit();

	virtual void Print() const;

	Double_t GetU() const {return fU;}
	Double_t GetPhi() const {return fPhi;}
	Double_t GetDu() const {return fDu;}
	Double_t GetDphi() const {return fDphi;}

	void SetU(Double_t u) {fU = u;}
	void SetPhi(Double_t phi) {fPhi = phi;}
	void SetDu(Double_t du) {fDu = du;}
	void SetDphi(Double_t dphi) {fDphi = dphi;}

private:
	Double_t fU; // U coordinate in the rotated c.s [cm]
	Double_t fDu; // U error [cm]
	Double_t fPhi; // strip rotation angle [rad]
	Double_t fDphi; // strip rotation error [rad]

	ClassDef(CbmStripHit, 1);
};

#endif /* CBMSTRIPHIT_H_ */
