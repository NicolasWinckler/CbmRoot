#ifndef CBM_RICH_FUZZY_KE
#define CBM_RICH_FUZZY_KE 1

#include "CbmRichHit.h"
#include "CbmRichRing.h"

#include "TObject.h"


#include <vector>
#include <cmath>

using std::vector;


class Hit{
public:
	Double_t fX;
	Double_t fY;
	Int_t fHitId;
};

class Ellipse
{
public:
	Double_t fR; // 2*a
	Double_t fV11; // xf1
	Double_t fV12; //yf1
	Double_t fV21; // xf2
	Double_t fV22; //yf2	
};

class CbmRichFuzzyKE
{
public:
	CbmRichFuzzyKE();
	void Minimize();
	void GenerateEllipses();
	void Draw(Int_t color);
	
	void SetHits(const std::vector<Hit>& hit){fHits = hit;}
	void SetEllipses(const std::vector<Ellipse>& ellipse){fEllipses = ellipse;}	

	const std::vector<Hit>& GetHits(){return fHits;}
	const std::vector<Ellipse>& GetEllipses(){return fEllipses;}	
	const std::vector<vector<Double_t> >& GetU(){return fU;}
	
	static Ellipse ParTransform(CbmRichRing* ellipse);
	
private:
	Double_t u(Int_t i, Int_t j);
	Double_t r(Int_t i);
	Double_t v11(Int_t i);
	Double_t v12(Int_t i);	
	Double_t v21(Int_t i);
	Double_t v22(Int_t i);
	Double_t D(Int_t i, Int_t j);
	Double_t d1(Int_t i, Int_t j);
	Double_t d2(Int_t i, Int_t j);	
	Double_t teta(Int_t i, Int_t j, Int_t focus);
	std::vector<Double_t> GetXYABP(Ellipse el);
	
	std::vector<Hit> fHits;
	std::vector<Ellipse> fEllipses;
	std::vector<vector<Double_t> > fU;
	std::vector<Double_t> fRNew;
	std::vector<vector<Double_t> > fUNew;	
	
	Double_t fQ;
	
	ClassDef(CbmRichFuzzyKE,1);

};

#endif