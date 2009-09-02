 
#ifndef CBMRICHGEOTEST_H
#define CBMRICHGEOTEST_H

#include "FairTask.h"

#include "TH1D.h"
#include "TH2D.h"
#include "TClonesArray.h"

#include <map>

using std::map;

class CbmRichGeoTest : public FairTask{

public:
	CbmRichGeoTest();
	CbmRichGeoTest(const char *name, const char *title, Int_t verbose);
	virtual ~CbmRichGeoTest();
	virtual InitStatus Init();
	virtual void Exec(Option_t* option);
	virtual void Finish();
	
	void RingParameters();
	void EventPoints();
	void MCElectrons();
private:
	
	TClonesArray* fRichHits;
	TClonesArray* fRichRings;
	TClonesArray* fRichPoints; 
	TClonesArray* fMCTracks;
	TClonesArray* fRichRingMatches; 
	
	Int_t fEventNum;
///electrons
	TH1D* fhNofHitsEl;
	TH1D* fhAaxisEl;
	TH1D* fhBaxisEl;
	TH1D* fhBoverAEl;
	TH2D* fhBoverAXYEl;
	TH2D* fhBaxisVsMomEl;
	TH2D* fhAaxisVsMomEl;
	TH1D* fhNofPointsEl;
	TH2D* fhXYEl;
	TH1D* fhMCEl;
	TH1D* fhAccEl;
	
	
///general plots
	TH1D* fhNofHitsEvent;
	TH1D* fhNofPointsEvent;
	
	TH2D* fhXYHits;
	TH2D* fhXYPoints;
	TH2D* fhXYHitsOneEvent;
	TH2D* fhXYPointsOneEvent;
	
	ClassDef(CbmRichGeoTest,1)
};

#endif

