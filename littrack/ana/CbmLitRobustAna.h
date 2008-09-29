#ifndef CBMLITROBUSTANA_H_
#define CBMLITROBUSTANA_H_

#include "CbmLitDetectorLayout.h"

#include "TObject.h"

class CbmLitTrack;
class TH1D;
class TH2D;
class TList;

class CbmLitRobustAna
{
public:	
	virtual ~CbmLitRobustAna();
	
	static CbmLitRobustAna* Instance();
	
	void Fill(
			CbmLitTrack* track);
	
	void Fill(
			CbmLitTrack* track,
			Int_t iter);
	
	void FillWeight(
			CbmLitTrack* track,
			Int_t iter);
	
	void FillErrFit(
			CbmLitTrack* track,
			Int_t iter);
	
	void WriteToFile();	

protected:
	CbmLitRobustAna();
	
private:
	void CreateHistos();
	
	static CbmLitRobustAna* fInstance;
	
	TList* fHistoList;
	
	TH2D* fhStationNofHits;
	TH2D* fhIterChi2;	
	TH2D* fhIterWeight;
	
	TH1D* fhEFIter;
	TH1D* fhEFNofHits;
	TH2D* fhEFIterNofHits;
	
	CbmLitDetectorLayout fLayout;
	
	ClassDef(CbmLitRobustAna, 1);
};

#endif /*CBMLITROBUSTANA_H_*/
