/** CbmLitFieldQa.h
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2009
 * @version 1.0
 **
 ** Task for checking magnetic field map and
 ** its approximation.
 **/

#ifndef CBMLITFIELDQA_H_
#define CBMLITFIELDQA_H_

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
class CbmLitFieldGridCreator;
class CBmLitPolynom;

class CbmLitFieldQa : public FairTask
{
public:
   /* Constructor */
   CbmLitFieldQa();

   /* Destructor */
   virtual ~CbmLitFieldQa();

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
   Bool_t IsCheckGridCreator() const {return fIsCheckGridCreator;}
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
   void IsCheckGridCreator(Bool_t isCheckGridCreator) {fIsCheckGridCreator = isCheckGridCreator;}
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
   /* Creates histograms */
   void CreateHistos();

   /* */
   void CreateFitterErrHistos();

   /* */
   void CreateGridErrHistos();

   /* Fills graphs for field map and field approximation
    * for each field component (Bx, By, Bz, |B|) */
   void FillBHistos();

   /* Checks field approximation tool */
   void CheckFieldFitter();

   /* Checks field creator tool */
   void CheckGridCreator();

   /* Fills histograms for magnetic field map along Z
    * for different polar angles and each field component
    * (Bx, By, Bz, |B|) */
   void CheckFieldMap();

   /* Draws canvas with histograms for each approximated field slice
    * @param v Defines field component (BX, BY, BZ, MOD)
    * @param opt Drawing option: "grid" to plot results for grid creator
    * "apr" to plot results for field fitter */
   void DrawSlices(Int_t v, const std::string& opt);

   /* Draws comparison for different polynom orders for each slice
    * @param opt Should be "rel" for relative errors or "abs" for absolute errors */
   void DrawPoly(const std::string& opt);

   /* Draws field components and its errors for each slice */
   void DrawFieldSlices();

   /* Draws field components along Z coordinate */
   void DrawFieldAlongZ();

   // Pointer to the magnetic field map
   FairField* fField;
   // List of histograms
   TList* fHistoList;
   // Number of slices along Z for field approximation
   Int_t fNofSlices;

   // Slice is defined as rectangle at a certain Z position
   // with upper left corner coordinate equals to [-X, -Y]
   // and lower right corner coordinate equals to [X, Y].
   // Z, X, Y coordinates for each slice.
   // [slice number]
   std::vector<Double_t> fZpos; // Z position of the slice
   std::vector<Double_t> fXpos; // X coordinate
   std::vector<Double_t> fYpos; // Y coordinate

   // Output directory for images
   std::string fOutputDir;

   //
   // Field approximation variables
   //
   // Acceptance angle for X
   Double_t fXangle;
   // Acceptance angle for Y
   Double_t fYangle;
   // Number of bins for X
   Int_t fNofBinsX;
   // Number of bins for Y
   Int_t fNofBinsY;
   // If true than only values inside an ellipse will be used for field approximation
   Bool_t fUseEllipseAcc;
   // Degree of the polynom
   UInt_t fPolynomDegreeIndex;
   // Number of polynoms for tests
   UInt_t fNofPolynoms;
   // Array with polynom degrees to be analyzed
   std::vector<UInt_t> fDegrees;
   // Field fitter tool for each polynom degree
   std::vector<CbmLitFieldFitter*> fFitter;

   //
   // Field grid
   //
   // Field grid creator tool
   CbmLitFieldGridCreator* fGridCreator;

   //
   // Field map along Z axis
   //
   std::vector<Double_t> fAlongZAngles; // Polar angles [grad]
   Double_t fZMin; // Minimum Z position [cm]
   Double_t fZMax; // Maximum Z position [cm]
   Double_t fZStep; // Step size [cm]

   //
   // Histograms and graphs
   //
   // Field map graph for each component and each slice
   // [BX, BY, BZ, MOD][slice number]
   std::vector<std::vector<TGraph2D*> >fhBGraph;
   // Approximated field graph for each component, each slice and each polynom order
   // [BX, BY, BZ, MOD][slice number][polynom order]
   std::vector<std::vector<std::vector<TGraph2D*> > >fhBAprGraph;

   // Error histograms
   // [BX, BY, BZ, MOD][slice number][polynom number]
   std::vector<std::vector<std::vector<TH2D*> > >fhBErrH2D; // 2D absolute error error distribution in (X, Y)
   std::vector<std::vector<std::vector<TH1D*> > >fhBErrH1D; // Absolute error
   std::vector<std::vector<std::vector<TH1D*> > >fhBRelErrH1D; // Relative error
   std::vector<std::vector<std::vector<TH2D*> > >fhBRelErrH2D; // 2D relative error distribution in (X, Y)

   // Error histograms for grid creator tool
   // [BX, BY, BZ, MOD][slice number]
   std::vector<std::vector<TH2D*> > fhGridBErrH2D; // 2D absolute error error distribution in (X, Y)
   std::vector<std::vector<TH1D*> > fhGridBErrH1D; // Absolute error
   std::vector<std::vector<TH1D*> > fhGridBRelErrH1D; // Relative error
   std::vector<std::vector<TH2D*> > fhGridBRelErrH2D; // 2D relative error distribution in (X, Y)
   // Grid field graph for each component and each slice
   // [BX, BY, BZ][slice number]
   std::vector<std::vector<TGraph2D*> > fhGridBGraph;

   // Field map values histograms along Z
   // [BX, BY, BZ][polynom number]
   std::vector<std::vector<TGraph*> >fhBAlongZGraph;

   Bool_t fIsCheckFieldApproximation; // If true than field approximation is checked
   Bool_t fIsCheckFieldMap; // If true than field map is checked
   Bool_t fIsCheckGridCreator; // If true than field creator is checked

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

   ClassDef(CbmLitFieldQa, 1);
};

#endif /* CBMLITFIELDQA_H_ */
