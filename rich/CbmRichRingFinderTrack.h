// --------------------------------------------------------------------------------------
// -----                 CbmRichRingFinderTrack source file                         -----
// ----- implementation: Simeon Lebedev (salebedev@jinr.ru)  -----
// -------------------------------------------------------------------------

#ifndef CBM_RICH_RING_FINDER_TRACK_H
#define CBM_RICH_RING_FINDER_TRACK_H

#include <vector>
#include <set>
#include <map>
#include "TClonesArray.h"
#include "CbmRichRing.h"
/*#include "CbmTask.h"
#include "CbmRootManager.h"*/
#include "CbmRichRingFinder.h"

class CbmRichTrackMyPoint {
public:
	Double_t fX;
	Double_t fY;
	Int_t fId;
	Int_t fRefIndex;
	static Bool_t CmpUp(const CbmRichTrackMyPoint &m1,
			  const CbmRichTrackMyPoint &m2){
		return m1.fX < m2.fX;
	};
	CbmRichTrackMyPoint(){fX=fY=0.;fId=0;}
	~CbmRichTrackMyPoint(){}
};

class CbmRichRingFinderTrack : public CbmRichRingFinder {
	static const Int_t kMAX_POINTS = 3500;//Maximum number of points
	//geometry of rich detector
	static const Double_t kXMAX = 170.0;
	static const Double_t kXMIN = -170.0;
	static const Double_t kYMAX = 250.0;
	static const Double_t kYMIN = -250.0;
	static const Int_t kMAXTRACKS = 600;
	static const Int_t kMAXHISTS = 200;

	Int_t fNBins;//number of bins in histogramm

	Double_t fMinRadius;// min radius
	Double_t fMaxRadius;//max radius
	Int_t fCut;//number of points cut

	std::vector< std::vector <Int_t> >  fHist;



	std::vector<CbmRichTrackMyPoint> fGuidance;		//track extrapolations from STS to RICH
	std::vector<CbmRichTrackMyPoint> fData;  		//Rich hits

	std::vector<CbmRichRing> fFoundRings;//collect found rings
	
	Int_t fNProjCount;

	  /** Verbosity level **/
  	Int_t fVerbose;

public:
  	CbmRichRingFinderTrack();// Default constructor
  	CbmRichRingFinderTrack ( Int_t verbose);// Standard constructor
	~CbmRichRingFinderTrack();

	void SetParameters(Int_t NBins, Double_t MinRadius,Double_t MaxRadius, Int_t Cut);
	void InitArray();

	void InitHist1D(Double_t nbins, Int_t cpinc);
	void FindCenters(Double_t nbins);


	virtual void Init();
	 virtual Int_t DoFind(TClonesArray* rHitArray,
	 		      TClonesArray* rProjArray,
		       	      TClonesArray* rRingArray);
	ClassDef(CbmRichRingFinderTrack,1);
};
#endif // CBM_RICH_RING_FINDER_TRACK_H
