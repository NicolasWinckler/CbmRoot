/**
 * \file CbmLitFieldQa.h
 * \brief Field map approximation QA.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2009
 **/

#ifndef CBMLITFIELDQA_H_
#define CBMLITFIELDQA_H_

#include "FairTask.h"
#include <vector>

class FairField;
class TH1;
class TH2;
class TList;
class CbmLitFieldFitter;
class CbmLitFieldGridCreator;
class CBmLitPolynom;

using std::vector;
using std::string;

/**
 * \class CbmLitFieldQa
 * \brief Field map approximation QA.
 *
 * Task checks magnetic field map.
 * Compares field map with its polynomial approximation
 * and with approximation based on grid.
 *
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2009
 **/
class CbmLitFieldQa : public FairTask
{
public:
   /**
    * \brief Constructor.
    */
   CbmLitFieldQa();

   /**
    * \brief Destructor.
    */
   virtual ~CbmLitFieldQa();

   /**
    * \brief Inherited from FairTask.
    */
   virtual InitStatus Init();

   /**
    * \brief Inherited from FairTask.
    */
   virtual void Exec(
      Option_t* opt);

   /**
    * \brief Inherited from FairTask.
    */
   virtual void Finish();

   /**
    * \brief Inherited from FairTask.
    */
   virtual void SetParContainers();

   /* Setters */
   void SetSliceZPosition(const vector<Double_t>& zPos) { fZpos = zPos; }
   void SetPolynomDegrees(const vector<UInt_t>& degrees) { fPolynomDegrees = degrees; }
   void SetCheckFieldMap(Bool_t checkFieldMap) { fCheckFieldMap = checkFieldMap; }
   void SetCheckFieldApproximation(Bool_t checkFieldApproximation) { fCheckFieldApproximation = checkFieldApproximation; }
   void SetCheckGridCreator(Bool_t isCheckGridCreator) { fCheckGridCreator = isCheckGridCreator; }
   void SetDrawFieldMap(Bool_t drawFieldMap) { fDrawFieldMap = drawFieldMap; }
   void SetDrawFieldApproximation(Bool_t drawFieldApproximation) { fDrawFieldApproximation = drawFieldApproximation; }
   void SetDrawGridCreator(Bool_t drawGridCreator) { fDrawGridCreator = drawGridCreator; }
   void SetDrawBx(Bool_t drawBx) { fDrawBx = drawBx; }
   void SetDrawBy(Bool_t drawBy) { fDrawBy = drawBy; }
   void SetDrawBz(Bool_t drawBz) { fDrawBz = drawBz; }
   void SetDrawMod(Bool_t drawMod) { fDrawMod = drawMod; }
   void IsFixedBounds(Bool_t fixedBounds) { fFixedBounds = fixedBounds; }

   /* Setters */
   void SetXangle(double xangle) {fXangle = xangle;}
   void SetYangle(double yangle) {fYangle = yangle;}
   void SetNofBinsX(int nofBinsX) {fNofBinsX = nofBinsX;}
   void SetNofBinsY(int nofBinsY) {fNofBinsY = nofBinsY;}
   void SetUseEllipseAcc(bool useEllipseAcc) {fUseEllipseAcc = useEllipseAcc;}
   void SetOutputDir(const string& dir) {fOutputDir = dir;}
   void SetPolynomDegreeIndex(unsigned int degreeIndex) {fPolynomDegreeIndex = degreeIndex;}

private:
   /**
    * \brief Create histograms.
    */
   void CreateHistos();

   /**
    * \brief Create field histograms.
    */
   void CreateFieldHistos();

   /**
    * \brief Create histograms for field approximation.
    */
   void CreateFitterHistos();

   /**
    * \brief Create histograms for grid creator.
    */
   void CreateGridHistos();

   /**
    * \brief Fill graphs and histos for field map for each field component (Bx, By, Bz, |B|).
    */
   void FillBHistos();

   /**
    * \brief Fill histograms for polynomial field approximation.
    */
   void FillFieldApproximationHistos();

   /**
    * \brief fill histograms for grid creator tool.
    */
   void FillGridCreatorHistos();

   /**
    * \brief Create property tree and serialize it to JSON.
    */
   void CreatePropertyTree();

   /**
    * \brief Draw canvas with histograms for each approximated slice.
    * \param[in] v Defines field component (BX, BY, BZ, MOD).
    * \param[in] opt Drawing option:
    *                   "grid" to plot results for grid creator
    *                   "apr" to plot results for field fitter.
    */
   void DrawSlices(
         Int_t v,
         const string& opt);

   /**
    * \brief Draw comparison for different polynomial orders for each slice.
    * \param[in] opt "rel" for relative errors or "abs" for absolute errors.
    */
   void DrawPoly(
         const string& opt);

   /**
    * \brief Draw field map components for each slice.
    */
   void DrawFieldSlices();

   /**
    * \brief Draw field map components along Z coordinate.
    */
   void DrawFieldAlongZ();

   void DivideHistos(
      TH1* histo1,
      TH1* histo2,
      TH1* histo3);

   void SubtructHistos(
      TH1* histo1,
      TH1* histo2,
      TH1* histo3);

   void ConvertH2ToH1(
      const TH2* h2,
      TH1* h1);

   // Pointer to the magnetic field map
   FairField* fField;
   // List of histograms and graphs
   TList* fHistoList;
   // Number of slices along Z for field approximation
   Int_t fNofSlices;

   // Slice is defined as rectangle at a certain Z position
   // with upper left corner coordinate equals to [-X, -Y]
   // and lower right corner coordinate equals to [X, Y].
   // Z, X, Y coordinates for each slice.
   // [slice number]
   vector<Double_t> fZpos; // Z position of the slice
   vector<Double_t> fXpos; // X coordinate
   vector<Double_t> fYpos; // Y coordinate

   // Output directory for images
   string fOutputDir;

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
   vector<UInt_t> fPolynomDegrees;
   // Field fitter tool for each polynom degree
   vector<CbmLitFieldFitter*> fFitter;

   //
   // Field grid
   //
   // Field grid creator tool
   CbmLitFieldGridCreator* fGridCreator;

   //
   // Field map along Z axis
   //
   vector<Double_t> fAlongZAngles; // Polar angles [grad]
   Double_t fZMin; // Minimum Z position [cm]
   Double_t fZMax; // Maximum Z position [cm]
   Double_t fZStep; // Step size [cm]

   // Field map for each component and each slice
   // [BX, BY, BZ, MOD][slice number]
   vector<vector<TH2*> >fhB;
   // Polynomial approximation for each component, each slice and each polynomial order
   // [BX, BY, BZ, MOD][slice number][polynomial order]
   vector<vector<vector<TH2*> > >fhBPolynomial;
   // Grid approximation for each component and each slice
   // [BX, BY, BZ][slice number]
   vector<vector<TH2*> > fhBGrid;

   // Error histograms for polynomial approximation
   // [BX, BY, BZ, MOD][slice number][polynom number]
   vector<vector<vector<TH2*> > >fhBPolynomialErrH2; // 2D absolute error error distribution in (X, Y)
   vector<vector<vector<TH1*> > >fhBPolynomialErrH1; // Absolute error
   vector<vector<vector<TH1*> > >fhBPolynomialRelErrH1; // Relative error
   vector<vector<vector<TH2*> > >fhBPolynomialRelErrH2; // 2D relative error distribution in (X, Y)

   // Error histograms for grid approximation
   // [BX, BY, BZ, MOD][slice number]
   vector<vector<TH2*> > fhBGridErrH2; // 2D absolute error error distribution in (X, Y)
   vector<vector<TH1*> > fhBGridErrH1; // Absolute error
   vector<vector<TH1*> > fhBGridRelErrH1; // Relative error
   vector<vector<TH2*> > fhBGridRelErrH2; // 2D relative error distribution in (X, Y)

   // Field map values histograms along Z
   // [BX, BY, BZ][polar angle]
   vector<vector<TH1*> >fhBAlongZGraph;

   Bool_t fCheckFieldMap; // If true than field map is checked
   Bool_t fCheckFieldApproximation; // If true than field approximation is checked
   Bool_t fCheckGridCreator; // If true than field creator is checked

   // Drawing options
   Bool_t fDrawFieldMap; // Draw field map
   Bool_t fDrawFieldApproximation; // Draw field approximation
   Bool_t fDrawGridCreator; // Draw grid creator histograms
   Bool_t fDrawBx; // Draw Bx field component histograms
   Bool_t fDrawBy; // Draw By field component histograms
   Bool_t fDrawBz; // Draw Bz field component histograms
   Bool_t fDrawMod; // Draw field module histograms

   Bool_t fFixedBounds; // Fixed bounds for error histograms

   // Indexes of the magnetic field components for convenience
   static const Int_t BX = 0; // Bx
   static const Int_t BY = 1; // By
   static const Int_t BZ = 2; // Bz
   static const Int_t MOD = 3; // Mod = std::sqrt(Bx*Bx + By*By + Bz*Bz)

   ClassDef(CbmLitFieldQa, 1);
};

#endif /* CBMLITFIELDQA_H_ */
