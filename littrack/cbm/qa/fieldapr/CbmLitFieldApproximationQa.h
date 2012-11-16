/**
 * \file CbmLitFieldApproximationQa.h
 * \brief Field map approximation QA.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2009
 **/

#ifndef CBMLITFIELDAPPROXIMATIONQA_H_
#define CBMLITFIELDAPPROXIMATIONQA_H_

#include "FairTask.h"
#include <vector>
#include <string>

class FairField;
class TH1;
class TH2;
class TGraph;
class TGraph2D;
class TList;
class CbmLitFieldFitter;
class CbmLitFieldGridCreator;
class CbmHistManager;

using std::vector;
using std::string;

/**
 * \class CbmLitFieldApproximationQa
 * \brief Field map approximation QA.
 *
 * Task checks magnetic field map.
 * Compares field map with its polynomial approximation
 * and with approximation based on grid.
 *
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2009
 **/
class CbmLitFieldApproximationQa: public FairTask {
public:
   /**
    * \brief Constructor.
    */
   CbmLitFieldApproximationQa();

   /**
    * \brief Destructor.
    */
   virtual ~CbmLitFieldApproximationQa();

   /**
    * \brief Inherited from FairTask.
    */
   virtual InitStatus Init();

   /**
    * \brief Inherited from FairTask.
    */
   virtual void Exec(Option_t* opt);

   /**
    * \brief Inherited from FairTask.
    */
   virtual void Finish();

   /* Setters */
   void SetSliceZPosition(const vector<Double_t>& zPos) {
      fZSlicePosition = zPos;
   }
   void SetPolynomDegrees(const vector<UInt_t>& degrees) {
      fPolynomDegrees = degrees;
   }
   void IsFixedBounds(Bool_t fixedBounds) {
      fFixedBounds = fixedBounds;
   }
   void SetAcceptanceAngleX(double xangle) {
      fAcceptanceAngleX = xangle;
   }
   void SetAcceptanceAngleY(double yangle) {
      fAcceptanceAngleY = yangle;
   }
   void SetNofBinsX(int nofBinsX) {
      fNofBinsX = nofBinsX;
   }
   void SetNofBinsY(int nofBinsY) {
      fNofBinsY = nofBinsY;
   }
   void SetUseEllipseAcc(bool useEllipseAcc) {
      fUseEllipseAcc = useEllipseAcc;
   }
   void SetOutputDir(const string& dir) {
      fOutputDir = dir;
   }
   void SetPolynomDegreeIndex(unsigned int degreeIndex) {
      fPolynomDegreeIndex = degreeIndex;
   }

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
    * \brief Create simulation QA report.
    */
   void CreateSimulationReport();

   /**
    * \brief Draw canvas with histograms for each approximated slice.
    * \param[in] v Defines field component (BX, BY, BZ, MOD).
    * \param[in] opt Drawing option:
    *                   "grid" to plot results for grid creator
    *                   "apr" to plot results for field fitter.
    */
   void DrawSlices(Int_t v, const string& opt);

   /**
    * \brief Draw comparison for different polynomial orders for each slice.
    * \param[in] opt "rel" for relative errors or "abs" for absolute errors.
    */
   void DrawPoly(const string& opt);

   // Pointer to the magnetic field map
   FairField* fField;
//   // List of histograms and graphs
//   TList* fHistoList;

   // Number of slices along Z for field approximation
   Int_t fNofSlices;
   // Slice is defined as rectangle at a certain Z position
   // with upper left corner coordinate equals to [-X, -Y]
   // and lower right corner coordinate equals to [X, Y].
   // Z, X, Y coordinates for each slice.
   // [slice number]
   vector<Double_t> fZSlicePosition; // Z position of the slice
   vector<Double_t> fXSlicePosition; // X coordinate
   vector<Double_t> fYSlicePosition; // Y coordinate

   // Output directory for images
   string fOutputDir;

   //
   // Field approximation variables
   //
   // Acceptance angle for X
   Double_t fAcceptanceAngleX;
   // Acceptance angle for Y
   Double_t fAcceptanceAngleY;
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

   CbmHistManager* fHM; // Histogram manager

//   //
//   // Histograms and graphs
//   //
//   // Field map graph for each component and each slice
//   // [BX, BY, BZ, MOD][slice number]
//   vector<vector<TGraph2D*> > fhBGraph;
//   // Approximated field graph for each component, each slice and each polynom order
//   // [BX, BY, BZ, MOD][slice number][polynom order]
//   vector<vector<vector<TGraph2D*> > > fhBAprGraph;
//   // Grid field graph for each component and each slice
//   // [BX, BY, BZ][slice number]
//   vector<vector<TGraph2D*> > fhBGridGraph;
//
//   // Error histograms for polynomial approximation
//   // [BX, BY, BZ, MOD][slice number][polynom number]
//   vector<vector<vector<TH2*> > > fhBPolynomialErrH2; // 2D absolute error error distribution in (X, Y)
//   vector<vector<vector<TH1*> > > fhBPolynomialErrH1; // Absolute error
//   vector<vector<vector<TH1*> > > fhBPolynomialRelErrH1; // Relative error
//   vector<vector<vector<TH2*> > > fhBPolynomialRelErrH2; // 2D relative error distribution in (X, Y)
//
//   // Error histograms for grid creator tool
//   // [BX, BY, BZ, MOD][slice number]
//   vector<vector<TH2*> > fhBGridErrH2; // 2D absolute error error distribution in (X, Y)
//   vector<vector<TH1*> > fhBGridErrH1; // Absolute error
//   vector<vector<TH1*> > fhBGridRelErrH1; // Relative error
//   vector<vector<TH2*> > fhBGridRelErrH2; // 2D relative error distribution in (X, Y)

   Bool_t fFixedBounds; // Fixed bounds for error histograms

//   // Indexes of the magnetic field components for convenience
//   static const Int_t BX = 0; // Bx
//   static const Int_t BY = 1; // By
//   static const Int_t BZ = 2; // Bz
//   static const Int_t MOD = 3; // Mod = sqrt(Bx*Bx + By*By + Bz*Bz)

ClassDef(CbmLitFieldApproximationQa, 1);
};

#endif /* CBMLITFIELDAPPROXIMATIONQA_H_ */
