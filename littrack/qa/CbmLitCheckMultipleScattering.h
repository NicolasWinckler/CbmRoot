#ifndef CBMLITCHECKMULTIPLESCATTERING_H_
#define CBMLITCHECKMULTIPLESCATTERING_H_

#include "CbmLitMaterialInfo.h"

#include "FairTask.h"

class CbmLitMaterialEffectsImp;
class CbmLitTrackParam;
class CbmLitMaterialInfo;
class CbmMuchPoint;
class TH1F;
class TH2F;
class TClonesArray;

class CbmLitCheckMultipleScattering : public FairTask
{
public:
	CbmLitCheckMultipleScattering();
	virtual ~CbmLitCheckMultipleScattering();

    virtual InitStatus Init();

    virtual void Exec(Option_t* opt);

    virtual void Finish();

    virtual void SetParContainers();

private:
	Double_t CalcTheta(
	          const CbmLitTrackParam* par,
	          const CbmLitMaterialInfo* mat) const;

    TClonesArray* fMCTrackArray;
    TClonesArray* fMCPointArray;

    TH1F* fh_theta_mc;
    TH1F* fh_eloss_mc;
    TH1F* fh_eloss_rec;

    Int_t fNEvents;

    ClassDef(CbmLitCheckMultipleScattering, 1)
};

#endif /*CBMLITCHECKMULTIPLESCATTERING_H_*/
