
/** CbmTrdElectronsQa
 * Author: Semen Lebedev (S.Lebedev@gsi.de)
 **/


#ifndef CBM_TRD_ELECTRONS_QA
#define CBM_TRD_ELECTRONS_QA 1


#include "FairTask.h"

class TClonesArray;


class CbmTrdElectronsQa : public FairTask
{
public:

	CbmTrdElectronsQa();

	~CbmTrdElectronsQa();


	virtual InitStatus Init();
	virtual void Exec(Option_t* opt);
	virtual void Finish();

private:

	TClonesArray* fMCTracks;
	TClonesArray* fMCPoints;
	TClonesArray* fTrdTracks;
	TClonesArray* fTrdTrackMatches;
	TClonesArray* fTrdHits;

	Int_t fEventNum;

	ClassDef(CbmTrdElectronsQa,1);

};

#endif
