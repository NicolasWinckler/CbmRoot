/** CbmLitCheckField.h
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2009
 * @version 1.0
 **
 ** Task for checking magnetic field map and
 ** its approximation.
 **/

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
   /* Constructor */
   CbmLitCheckField();

   /* Destructor */
   virtual ~CbmLitCheckField();

   /* Inherited from FairTask */
   virtual InitStatus Init();

   /* Inherited from FairTask */
   virtual void Exec(
      Option_t* opt);

   /* Inherited from FairTask */
   virtual void Finish();

   /* Inherited from FairTask */
   virtual void SetParContainers();

   /* Getters */
   Bool_t IsCheckFieldApproximation() const {return fIsCheckFieldApproximation;}
   Bool_t IsCheckFieldMap() const {return fIsCheckFieldMap;}
   Bool_t IsDrawBx() const {return fDrawBx;}
   Bool_t IsDrawBy() const {return fDrawBy;}
   Bool_t IsDrawBz() const {return fDrawBz;}
   Bool_t IsDrawMod() const {return fDrawMod;}
   Bool_t IsDrawFieldMap() const {return fDrawFieldMap;}
   Bool_t IsDrawPoly() const {return fDrawPoly;}
   Bool_t IsDrawSlices() const {return fDrawSlices;}
   Bool_t IsFixedBounds() const {return fFixedBounds;}

   /* Setters */
   void IsCheckFieldApproximation(Bool_t isCheckFieldApproximation) {fIsCheckFieldApproximation = isCheckFieldApproximation;}
   void IsCheckFieldMap(Bool_t isCheckFieldMap) {fIsCheckFieldMap = isCheckFieldMap;}
   void IsDrawBx(Bool_t drawBx) {fDrawBx = drawBx;}
   void IsDrawBy(Bool_t drawBy) {fDrawBy = drawBy;}
   void IsDrawBz(Bool_t drawBz) {fDrawBz = drawBz;}
   void IsDrawMod(Bool_t drawMod) {fDrawMod = drawMod;}
   void IsDrawFieldMap(Bool_t drawFieldMap) {fDrawFieldMap = drawFieldMap;}
   void IsDrawPoly(Bool_t drawPoly) {fDrawPoly = drawPoly;}
   void IsDrawSlices(Bool_t drawSlices) {fDrawSlices = drawSlices;}
   void IsFixedBounds(Bool_t fixedBounds) {fFixedBounds = fixedBounds;}

   /* Setters */
   void SetXangle(double xangle) {fXangle = xangle;}
   void SetYangle(double yangle) {fYangle = yangle;}
   void SetNofBinsX(int nofBinsX) {fNofBinsX = nofBinsX;}
   void SetNofBinsY(int nofBinsY) {fNofBinsY = nofBinsY;}
   void SetUseEllipseAcc(bool useEllipseAcc) {fUseEllipseAcc = useEllipseAcc;}
   void SetOutputDir(const std::string& dir) {fOutputDir = dir;}
   void SetPolynomDegreeIndex(unsigned int degreeIndex) {fPolynomDegreeIndex = degreeIndex;}

private:
   /* Creates field fitters for each polynom degree
    * and approximates magnetic field in each
    * slice along Z */
   void CreateFieldFitter();

   /* Creates histograms */
   void CreateHistos();

   /* Fills graphs for field map and field approximation
    * for each field component (Bx, By, Bz, |B|) */
   void FillBHistos();

   /* Fills error histograms, i.e. relative and absolute
    * differences between field map value and approximated
    * value for each slice along z and each field component
    * (Bx, By, Bz, |B|) */
   void FillErrHistos();

   /* Fills histograms for magnetic field map along Z
    * for different polar angles and each field component
    * (Bx, By, Bz, |B|) */
   void FillHistosAlongZ();

   /* Draws canvas with histograms for each approximated field slice
    * @param v Defines field component (BX, BY, BZ, MOD)*/
   void DrawSlices(Int_t v);

   /* Draws comparison for different polynom orders for each slice
    * @param opt Should be "rel" for relative errors or "abs" for absolute errors */
   void DrawPoly(const std::string& opt);

   /* Draws field components and its errors for each slice */
   void DrawFieldSlices();

   /* Draws field components along Z coordinate */
   void DrawFieldAlongZ();

   FairField* fField; // Pointer to the magnetic field map

   TList* fHistoList; // List of histograms

   Int_t fNofSlices; // Number of slices along Z for field approximation

   // Slice is defined as rectangle at a certain Z position
   // with upper left corner coordinate equals to [-X, -Y]
   // and lower right corner coordinate equals to [X, Y].
   // Z, X, Y coordinates for each slice.
   // [slice number]
   std::vector<Double_t> fZpos; // Z position of the slice
   std::vector<Double_t> fXpos; // X coordinate
   std::vector<Double_t> fYpos; // Y coordinate

   // Polynom coefficients for approximated field slice
   // for each field component [Bx, By, Bz].
   // [polynom order][slice number][coefficient index]
   std::vector<std::vector<std::vector<Double_t> > > fCx; // for Bx
   std::vector<std::vector<std::vector<Double_t> > > fCy; // for By
   std::vector<std::vector<std::vector<Double_t> > >fCz; // for Bz

   // Field map graph for each component and each slice
   // [BX, BY, BZ][slice number]
   std::vector<std::vector<TGraph2D*> >fhBGraph;
   // Approximated field graph for each component, each slice and each polynom order
   // [BX, BY, BZ][slice number][polynom order]
   std::vector<std::vector<std::vector<TGraph2D*> > >fhBAprGraph;

   // Error histograms
   // [BX, BY, BZ][slice number][polynom number]
   std::vector<std::vector<std::vector<TH2D*> > >fhBErrH2D; // 2D absolute error error distribution in (X, Y)
   std::vector<std::vector<std::vector<TH1D*> > >fhBErrH1D; // Absolute error
   std::vector<std::vector<std::vector<TH1D*> > >fhBRelErrH1D; // Relative error
   std::vector<std::vector<std::vector<TH2D*> > >fhBRelErrH2D; // 2D relative error distribution in (X, Y)

   // Field map values histograms along Z
   // [BX, BY, BZ][polynom number]
   std::vector<std::vector<TGraph*> >fhBAlongZGraph;
   std::vector<Double_t> fAlongZAngles; // Polar angles [grad]
   Double_t fZMin; // Minimum Z position [cm]
   Double_t fZMax; // Maximum Z position [cm]
   Double_t fZStep; // Step size [cm]

   Bool_t fIsCheckFieldApproximation; // If true than field approximation is checked
   Bool_t fIsCheckFieldMap; // If true than field map is checked

   // Drawing options
   Bool_t fDrawBx; // Draw Bx field component histograms
   Bool_t fDrawBy; // Draw By field component histograms
   Bool_t fDrawBz; // Draw Bz field component histograms
   Bool_t fDrawMod; // Draw field module histograms
   Bool_t fDrawFieldMap; // Draw field map
   Bool_t fDrawPoly; // Draw comparison for different polynom orders
   Bool_t fDrawSlices; // Draw field slices

   Bool_t fFixedBounds; // Fixed bounds for error histograms

   // Indexes of the magnetic field components for convenience
   static const Int_t BX = 0; // Bx
   static const Int_t BY = 1; // By
   static const Int_t BZ = 2; // Bz
   static const Int_t MOD = 3; // Mod = std::sqrt(Bx*Bx + By*By + Bz*Bz)

   Double_t fXangle; // Acceptance angle for X
   Double_t fYangle; // Acceptance angle for Y

   Int_t fNofBinsX; // Number of bins for X
   Int_t fNofBinsY; // Number of bins for Y

   Bool_t fUseEllipseAcc; // If true than only values inside an ellipse will be used for field approximation

   std::string fOutputDir; // Output directory for images

   std::vector<CbmLitFieldFitter*> fFitter; // Field fitter tool for each polynom degree

   UInt_t fPolynomDegreeIndex; // Degree of the polynom

   UInt_t fNofPolynoms; // Number of polynoms for tests

   std::vector<UInt_t> fDegrees; // Array with polynom degrees to be analyzed

   ClassDef(CbmLitCheckField, 1);
};

#endif /* CBMLITCHECKFIELD_H_ */
