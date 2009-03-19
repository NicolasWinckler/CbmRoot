/*
 * CbmBaseHit.h
 *
 *  Created on: Mar 17, 2009
 *      Author: andrey
 */

#ifndef CBMBASEHIT_H_
#define CBMBASEHIT_H_

enum HitType {
	kHIT,
	kPIXELHIT,
	kSTRIPHIT,
	kMUCHPIXELHIT,
	kMUCHSTRAWHIT
};

#include "TObject.h"
#include "TString.h"

class CbmBaseHit : public TObject
{
public:
	CbmBaseHit();
	CbmBaseHit(
			Int_t detectorId,
			Double_t z,
			Double_t dz,
			Int_t refId);
	virtual ~CbmBaseHit();

	virtual void Print() const = 0;

	HitType GetType() const {return fType;}
	Double_t GetZ() const {return fZ;}
	Double_t GetDz() const {return fDz;}
	Int_t GetRefId() const {return fRefId;}
	Int_t GetDetectorId() const {return fDetectorId;}

	void SetType(HitType type) {fType = type;};
	void SetZ(Double_t z) {fZ = z;}
	void SetDz(Double_t dz) {fDz = dz;}
	void SetRefId(Int_t refId) {fRefId = refId;}
	void SetDetectorId(Int_t detectorId) {fDetectorId = detectorId;}

private:
	HitType fType; // hit type
	Double_t fZ; // Z position of hit [cm]
	Double_t fDz; // Z position error [cm]
	Int_t fRefId; // some reference id
	Int_t fDetectorId; // detector unique identifier

	ClassDef(CbmBaseHit, 1);
};

#endif /* CBMBASEHIT_H_ */
