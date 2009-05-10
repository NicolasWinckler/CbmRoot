#ifndef CBMLITFINDGLOBALTRACKSIDEAL_H_
#define CBMLITFINDGLOBALTRACKSIDEAL_H_

#include "FairTask.h"

#include <map>

class TClonesArray;

class CbmLitFindGlobalTracksIdeal : public FairTask
{
public:
	CbmLitFindGlobalTracksIdeal();
	virtual ~CbmLitFindGlobalTracksIdeal();

	virtual void SetParContainers();
	virtual InitStatus Init();
	virtual InitStatus ReInit();
	virtual void Exec(Option_t* opt);

private:
	virtual void Finish();

	void FillMapSts();
	void FillMapTrd();
	void FillMapMuch();
	void FillMapTof();
	void CreateGlobalTracks();

	Bool_t fIsElectronSetup;

	TClonesArray* fMCTracks;

	TClonesArray* fStsMatches;
	TClonesArray* fMuchMatches;
	TClonesArray* fTrdMatches;
	TClonesArray* fTofMCPoints;
	TClonesArray* fTofHits;
	TClonesArray* fGlobalTracks;   //output

	std::map<Int_t, Int_t> fMcStsMap;
	std::map<Int_t, Int_t> fMcTrdMap;
	std::map<Int_t, Int_t> fMcMuchMap;
	std::map<Int_t, Int_t> fMcTofMap;

	Int_t fEventNo;


	ClassDef(CbmLitFindGlobalTracksIdeal, 1);
};

#endif /* CBMLITFINDGLOBALTRACKSIDEAL_H_ */
