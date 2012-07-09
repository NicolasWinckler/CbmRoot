/**
 * \file CbmLitFieldQa.h
 * \brief Field map QA.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2009
 **/

#ifndef CBMLITFIELDQA_H_
#define CBMLITFIELDQA_H_

#include "FairTask.h"
#include <vector>
#include <string>

class FairField;
class TGraph;
class TGraph2D;
class TList;

using std::vector;
using std::string;

/**
 * \class CbmLitFieldQa
 * \brief Field map QA.
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

   /* Setters */
   void SetSliceZPosition(const vector<Double_t>& zPos) { fZSlicePosition = zPos; }
   void SetAcceptanceAngleX(Double_t xangle) { fAcceptanceAngleX = xangle; }
   void SetAcceptanceAngleY(Double_t yangle) { fAcceptanceAngleY = yangle; }
   void SetNofBinsX(Int_t nofBinsX) { fNofBinsX = nofBinsX; }
   void SetNofBinsY(Int_t nofBinsY) { fNofBinsY = nofBinsY; }
   void SetZMin(Double_t zMin) { fZMin = zMin; }
   void SetZMax(Double_t zMax) { fZMax = zMax; }
   void SetZStep(Double_t zStep) { fZStep = zStep; }
   void SetOutputDir(const string& dir) { fOutputDir = dir; }

private:
   /**
    * \brief Create histograms.
    */
   void CreateHistos();

   /**
    * \brief Fill graphs and histos for field map for each field component (Bx, By, Bz, |B|).
    */
   void FillBHistos();

   /**
    * \brief Create property tree and serialize it to JSON.
    */
   void CreatePropertyTree();

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

   // Slice is defined as rectangle at a certain Z position
   // with upper left corner coordinate equals to [-X, -Y]
   // and lower right corner coordinate equals to [X, Y].
   // Z, X, Y coordinates for each slice.
   // [slice number]
   vector<Double_t> fZSlicePosition; // Z position of the slice
   vector<Double_t> fXSlicePosition; // X coordinate of slice
   vector<Double_t> fYSlicePosition; // Y coordinate of slice

   // Number of slices along Z for field approximation
   Int_t fNofSlices;
   // Acceptance angle for X
   Double_t fAcceptanceAngleX;
   // Acceptance angle for Y
   Double_t fAcceptanceAngleY;
   // Number of bins for X
   Int_t fNofBinsX;
   // Number of bins for Y
   Int_t fNofBinsY;

   vector<Double_t> fAlongZAngles; // Polar angles [grad]
   vector<Double_t> fAlongZXPosition; // X position for plotting field along Z
   vector<Double_t> fAlongZYPosition; // Y position for plotting field along Z
   Double_t fZMin; // Minimum Z position [cm]
   Double_t fZMax; // Maximum Z position [cm]
   Double_t fZStep; // Step size [cm]

   // Field map graph for each component and each slice
   // [BX, BY, BZ, MOD][slice number]
   vector<vector<TGraph2D*> >fgB;

   // Field map values histograms along Z
   // [BX, BY, BZ][polar angle]
   vector<vector<TGraph*> >fgBAlongZAngle;

   // Field map values histograms along Z
   // [BX, BY, BZ][x position]
   vector<vector<TGraph*> >fgBAlongZXPosition;

   // Field map values histograms along Z
   // [BX, BY, BZ][y position]
   vector<vector<TGraph*> >fgBAlongZYPosition;

   // Output directory for images
   string fOutputDir;

   // Indexes of the magnetic field components for convenience
   static const Int_t BX = 0; // Bx
   static const Int_t BY = 1; // By
   static const Int_t BZ = 2; // Bz
   static const Int_t MOD = 3; // Mod = sqrt(Bx*Bx + By*By + Bz*Bz)

   ClassDef(CbmLitFieldQa, 1);
};

#endif /* CBMLITFIELDQA_H_ */
