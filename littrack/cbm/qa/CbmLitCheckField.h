#ifndef CBMLITCHECKFIELD_H_
#define CBMLITCHECKFIELD_H_

#include "FairTask.h"

#include <vector>

class FairField;
class TH1D;
class TH2D;
class TGraph;
class TGraph2D;
class TList;
class TF1;
class TF2;
class CbmLitFieldFitter;
class CBmLitPolynom;

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
   Bool_t IsDrawMod() const {return fDrawMod;}
   Bool_t IsDrawFieldMap() const {return fDrawFieldMap;}
   Bool_t IsDrawPhd() const {return fDrawPhd;}
   Bool_t IsDrawPoly() const {return fDrawPoly;}
   Bool_t IsDrawSlices() const {return fDrawSlices;}
   Bool_t IsFixedBounds() const {return fFixedBounds;}

   void IsDrawBx(Bool_t drawBx) {fDrawBx = drawBx;}
   void IsDrawBy(Bool_t drawBy) {fDrawBy = drawBy;}
   void IsDrawBz(Bool_t drawBz) {fDrawBz = drawBz;}
   void IsDrawMod(Bool_t drawMod) {fDrawMod = drawMod;}
   void IsDrawFieldMap(Bool_t drawFieldMap) {fDrawFieldMap = drawFieldMap;}
   void IsDrawPhd(Bool_t drawPhd) {fDrawPhd = drawPhd;}
   void IsDrawPoly(Bool_t drawPoly) {fDrawPoly = drawPoly;}
   void IsDrawSlices(Bool_t drawSlices) {fDrawSlices = drawSlices;}
   void IsFixedBounds(Bool_t fixedBounds) {fFixedBounds = fixedBounds;}

   void SetXangle(double xangle) {fXangle = xangle;}
   void SetYangle(double yangle) {fYangle = yangle;}
   void SetNofBinsX(int nofBinsX) {fNofBinsX = nofBinsX;}
   void SetNofBinsY(int nofBinsY) {fNofBinsY = nofBinsY;}
   void SetUseEllipseAcc(bool useEllipseAcc) {fUseEllipseAcc = useEllipseAcc;}
   void SetOutputDir(const std::string& dir) {fOutputDir = dir;}
   void SetPolynomDegreeIndex(unsigned int degreeIndex) {fPolynomDegreeIndex = degreeIndex;}

private:
   void CreateFieldFitter();
   void CreateHistos();
   void FillBHistos();
   void FillErrHistos();
   void FillErrHistosAlongZ();
   void FillHistosAlongZ();

   void DrawSlices(Int_t v);
   void DrawPhd(Int_t v);
   void DrawPoly(const std::string& opt);
   void DrawField();
   void DrawFieldAlongZ();

   FairField* fField;

   TList* fHistoList;

   Int_t fNofSlices;

   std::vector<Double_t> fZpos;
   std::vector<Double_t> fXpos;
   std::vector<Double_t> fYpos;
   std::vector<std::vector<std::vector<Double_t> > > fCx;
   std::vector<std::vector<std::vector<Double_t> > > fCy;
   std::vector<std::vector<std::vector<Double_t> > >fCz;

//    std::vector<std::vector<TH2D*> >fhB;
   std::vector<std::vector<TGraph2D*> >fhBGraph;
   std::vector<std::vector<std::vector<TGraph2D*> > >fhBAprGraph;

   // [BX, BY, BZ][slice number][polynom number]
   std::vector<std::vector<std::vector<TH2D*> > >fhBErrH2D;
   std::vector<std::vector<std::vector<TH1D*> > >fhBErrH1D;
   std::vector<std::vector<std::vector<TH1D*> > >fhBRelErrH1D;
   std::vector<std::vector<std::vector<TH2D*> > >fhBRelErrH2D;

// std::vector<std::vector<std::vector<TH1D*> > > fhBErrPolyH1D;

   // Histograms along Z
   // [BX, BY, BZ][polynom number]
   std::vector<std::vector<TH1D*> > fhBErrAlongZH1D;
   std::vector<std::vector<TH1D*> > fhBRelErrAlongZH1D;
   std::vector<std::vector<TGraph*> >fhBAlongZGraph;
   std::vector<double> fAlongZAngles;
   double fZMin;
   double fZMax;
   double fZStep;

   Bool_t fDrawBx;
   Bool_t fDrawBy;
   Bool_t fDrawBz;
   Bool_t fDrawMod;
   Bool_t fDrawFieldMap;
   Bool_t fDrawPhd;
   Bool_t fDrawPoly;
   Bool_t fDrawSlices;

   Bool_t fFixedBounds; // fixed bounds for error histograms

   static const Int_t BX = 0;
   static const Int_t BY = 1;
   static const Int_t BZ = 2;
   static const Int_t MOD = 3;

   double fXangle; // acceptance angle for X
   double fYangle; // acceptance angle for Y

   int fNofBinsX; // number of bins for X
   int fNofBinsY; // number of bins for Y

   bool fUseEllipseAcc; // if true than only values inside a certain ellipse will be fitted

   std::string fOutputDir; // input directory for images

   std::vector<CbmLitFieldFitter*> fFitter; // field fitter tool

   unsigned int fPolynomDegreeIndex; // degree of the polynom

   unsigned int fNofPolynoms; // number of polynoms for tests

   std::vector<unsigned int> fDegrees; // array with polynom degrees to be analyzed

   ClassDef(CbmLitCheckField, 1);
};

#endif /* CBMLITCHECKFIELD_H_ */
