#ifndef CBMLITCHECKFIELD_H_
#define CBMLITCHECKFIELD_H_

#include "FairTask.h"
#include "CbmLitMyField.h"
#include "CbmLitFieldSlice.h"

#include <vector>

class FairField;
class TH1D;
class TH2D;
class TList;
class TF1;
class TF2;

class CbmLitCheckField : public FairTask
{
public:
	CbmLitCheckField();
	virtual ~CbmLitCheckField();

	virtual InitStatus Init();
	virtual void Exec(
			Option_t* opt);
	virtual void Finish();
	virtual void SetParContainers();

	Bool_t IsDrawBx() const {return fDrawBx;}
	Bool_t IsDrawBy() const {return fDrawBy;}
	Bool_t IsDrawBz() const {return fDrawBz;}

	void IsDrawBx(Bool_t drawBx) {fDrawBx = drawBx;}
	void IsDrawBy(Bool_t drawBy) {fDrawBy = drawBy;}
	void IsDrawBz(Bool_t drawBz) {fDrawBz = drawBz;}

private:
	void FillHistos();
	void CreateHistos();
	void WriteToFile();
//	void FitHistos(Int_t v);
	void FindErrors();
	void DrawHistos(Int_t v);
	void DrawFunc(Int_t v);

	FairField* fField;

	TList* fHistoList;

	std::vector<CbmLitFieldSlice> fSlices;

//	Int_t fNZ;
    std::vector<Double_t> fZpos;
    std::vector<Double_t> fXpos;
    std::vector<Double_t> fYpos;
    std::vector<Int_t> fNofBinsX;
    std::vector<Int_t> fNofBinsY;
	std::vector<std::vector<TH2D*> >fhB;
//	std::vector<CbmLitFieldSlice> fhSlices;
	std::vector<std::vector<TH2D*> >fhBErrH2D;
	std::vector<std::vector<TH1D*> >fhBErrH1D;
	std::vector<std::vector<TH1D*> >fhBRelErrH1D;

	Bool_t fDrawBx;
	Bool_t fDrawBy;
	Bool_t fDrawBz;

	const Int_t BX;
	const Int_t BY;
	const Int_t BZ;

	ClassDef(CbmLitCheckField, 1);
};

#endif /* CBMLITCHECKFIELD_H_ */
