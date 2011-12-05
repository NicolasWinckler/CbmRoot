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
   void SetSliceZPosition(const std::vector<Double_t>& zPos) { fZpos = zPos; }
   void SetPolynomDegrees(const std::vector<UInt_t>& degrees) { fPolynomDegrees = degrees; }
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
   void SetOutputDir(const std::string& dir) {fOutputDir = dir;}
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
         const std::string& opt);

   /**
    * \brief Draw comparison for different polynomial orders for each slice.
    * \param[in] opt "rel" for relative errors or "abs" for absolute errors.
    */
   void DrawPoly(
         const std::string& opt);

   /**
    * \brief Draw field map components for each slice.
    */
   void DrawFieldSlices();

   /**
    * \brief Draw field map components along Z coordinate.
    */
   void DrawFieldAlongZ();

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
   std::vector<UInt_t> fPolynomDegrees;
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
   // Grid field graph for each component and each slice
   // [BX, BY, BZ][slice number]
   std::vector<std::vector<TGraph2D*> > fhBGridGraph;

   // Error histograms for polynomial approximation
   // [BX, BY, BZ, MOD][slice number][polynom number]
   std::vector<std::vector<std::vector<TH2D*> > >fhBErrH2D; // 2D absolute error error distribution in (X, Y)
   std::vector<std::vector<std::vector<TH1D*> > >fhBErrH1D; // Absolute error
   std::vector<std::vector<std::vector<TH1D*> > >fhBRelErrH1D; // Relative error
   std::vector<std::vector<std::vector<TH2D*> > >fhBRelErrH2D; // 2D relative error distribution in (X, Y)

   // Error histograms for grid creator tool
   // [BX, BY, BZ, MOD][slice number]
   std::vector<std::vector<TH2D*> > fhBGridErrH2D; // 2D absolute error error distribution in (X, Y)
   std::vector<std::vector<TH1D*> > fhBGridErrH1D; // Absolute error
   std::vector<std::vector<TH1D*> > fhBGridRelErrH1D; // Relative error
   std::vector<std::vector<TH2D*> > fhBGridRelErrH2D; // 2D relative error distribution in (X, Y)

   // Field map values histograms along Z
   // [BX, BY, BZ][polar angle]
   std::vector<std::vector<TGraph*> >fhBAlongZGraph;

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

   // Store image names
   std::vector<std::string> fImageList;

   ClassDef(CbmLitFieldQa, 1);
};

#endif /* CBMLITFIELDQA_H_ */
