/*
 * CbmRichRingLight.h
 *
 *  Created on: 09.03.2010
 *      Author: Semen Lebedev
 */

#ifndef CBMRICHRINGLIGHT_H_
#define CBMRICHRINGLIGHT_H_

#include <vector>
#include <math.h>

class CbmRichRingLight
{
public:
    CbmRichRingLight(){
    	fHitCollection.reserve(40);
    }

	~CbmRichRingLight(){
		fHitCollection.clear();
	}

	void AddHit(UShort_t pHit)  {fHitCollection.push_back(pHit);}
	void RemoveHit(UShort_t hitId){
		std::vector<UShort_t>::iterator it;
		for (it = fHitCollection.begin(); it!=fHitCollection.end(); it++){
			if (hitId == *it){
				fHitCollection.erase(it);
				return;
			}
		}
	}

	Int_t GetNofHits() const {return fHitCollection.size(); }
	UShort_t GetHit(Int_t i) {return fHitCollection[i];}

	void SetCenterX(Float_t x) {fCenterX = x;}
    void SetCenterY(Float_t y) {fCenterY = y;}
	void SetRadius(Float_t r) {fRadius = r;}
	void SetChi2(Float_t chi2) {fChi2 = chi2;}
	void SetAngle( Float_t angle) {fAngle = angle;}
	void SetNofHitsOnRing(UShort_t onring) {fNofHitsOnRing = onring;}
	/** number between -1 and 1: -1 = fake ring, 1 = good ring (selection by neural net)*/
	void SetSelectionNN (Float_t selectionNN ) {fSelectionNN = selectionNN;}
	void SetRecFlag(Short_t recFlag){fRecFlag = recFlag;}

	Float_t GetCenterX() const {return fCenterX;}
	Float_t GetCenterY() const {return fCenterY;}
	Float_t GetRadius() const {return fRadius;}
	Float_t GetSelectionNN() const {return fSelectionNN;}
	Float_t GetChi2() const {return fChi2;}
	Float_t GetNDF() const {return (Float_t)(GetNofHits()-5);}
	Float_t GetRadialPosition() const{
		if (fCenterY > 0.f){
		       return sqrt(fCenterX*fCenterX +(fCenterY - 110.f)*(fCenterY - 110.f));
		   } else {
		       return sqrt(fCenterX*fCenterX+(fCenterY + 110.f)*(fCenterY + 110.f));
		};
	}

	Float_t GetAngle() const {return fAngle;}
	UShort_t GetNofHitsOnRing() const {return fNofHitsOnRing;}
	Short_t GetRecFlag(){return fRecFlag;}

private:
   std::vector<UShort_t> fHitCollection; /** STL container to hold the hit indexes */

protected:
    Float_t fCenterX;
	Float_t fCenterY;
	Float_t fRadius;
	Float_t fChi2;
	Float_t fAngle;
	UShort_t fNofHitsOnRing;
	Short_t fRecFlag;
	Float_t fSelectionNN;
};

#endif /* CBMRICHRINGLIGHT_H_ */
