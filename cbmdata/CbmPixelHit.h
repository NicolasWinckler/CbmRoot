/*
 * CbmPixelHit.h
 *
 *  Created on: Mar 17, 2009
 *      Author: andrey
 */

#ifndef CBMPIXELHIT_H_
#define CBMPIXELHIT_H_

#include "CbmBaseHit.h"

class TVector3;

class CbmPixelHit : public CbmBaseHit
{
public:
	CbmPixelHit();
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
	CbmPixelHit(
			Int_t detectorId,
			const TVector3& pos,
			const TVector3& err,
			Double_t dxy,
			Int_t refId);
	virtual ~CbmPixelHit();

	virtual void Print() const;

	Double_t GetX() const {return fX;}
	Double_t GetY() const {return fY;}
	Double_t GetDx() const {return fDx;}
	Double_t GetDy() const {return fDy;}
	Double_t GetDxy() const {return fDxy;}

	void SetX(Double_t x) {fX = x;}
	void SetY(Double_t y) {fY = y;}
	void SetDx(Double_t dx) {fDx = dx;}
	void SetDy(Double_t dy) {fDy = dy;}
	void SetDxy(Double_t dxy) {fDxy = dxy;}

private:
	Double_t fX, fY; // X, Y positions of hit [cm]
	Double_t fDx, fDy; // X, Y errors [cm]
	Double_t fDxy; // XY correlation

	ClassDef(CbmPixelHit, 1);
};

#endif /* CBMPIXELHIT_H_ */
