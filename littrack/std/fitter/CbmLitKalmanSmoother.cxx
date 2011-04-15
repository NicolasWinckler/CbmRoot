/** CbmLitKalmanSmoother.cxx
 *@author A.Lebedev <alebedev@jinr.ru>
 *@since 2007
 **/

#include "fitter/CbmLitKalmanSmoother.h"

#include "data/CbmLitTrack.h"
#include "utils/CbmLitMath.h"
#include "utils/CbmLitMatrixMath.h"

#include <iostream>

CbmLitKalmanSmoother::CbmLitKalmanSmoother()
{
}

CbmLitKalmanSmoother::~CbmLitKalmanSmoother()
{
}

LitStatus CbmLitKalmanSmoother::Initialize()
{
   return kLITSUCCESS;
}

LitStatus CbmLitKalmanSmoother::Finalize()
{
   return kLITSUCCESS;
}

LitStatus CbmLitKalmanSmoother::Fit(
   CbmLitTrack* track,
   bool downstream)
{
   int n = track->GetNofHits();

   std::vector<CbmLitFitNode> nodes = track->GetFitNodes();
   nodes[n-1].SetSmoothedParam(nodes[n-1].GetUpdatedParam());

   // start with the before the last detector plane
   for (int i = n - 1; i > 0; i--) {
      Smooth(&nodes[i - 1], &nodes[i]);
   }

   // Calculate the chi2 of the track
   track->SetChi2(0.);
   for (int i = 0; i < n; i++) {
      myf chi2Hit = lit::ChiSq(nodes[i].GetSmoothedParam(), track->GetHit(i));
      nodes[i].SetChiSqSmoothed(chi2Hit);
      track->SetChi2(track->GetChi2() + chi2Hit);
   }

   track->SetParamFirst(nodes[0].GetSmoothedParam());
   track->SetFitNodes(nodes);
   track->SetNDF(lit::NDF(track));

   return kLITSUCCESS;
}

// We are going in the upstream direction
// this Node (k) , prevNode (k+1)
void CbmLitKalmanSmoother::Smooth(
   CbmLitFitNode* thisNode,
   const CbmLitFitNode* prevNode)
{
// TMatrixDSym invPrevPredC(5);
// prevNode->GetPredictedParam()->GetCovMatrix(invPrevPredC);
// invPrevPredC.Invert();
   std::vector<myf> invPrevPredC(prevNode->GetPredictedParam()->GetCovMatrix());
   InvSym15(invPrevPredC);

// TMatrixD Ft(5, 5);
// prevNode->GetF(Ft);
// Ft.T();
   std::vector<myf> Ft(prevNode->GetF());
   Transpose25(Ft);

// TMatrixDSym thisUpdC(5);
// thisNode->GetUpdatedParam()->GetCovMatrix(thisUpdC);
   const std::vector<myf>& thisUpdC = thisNode->GetUpdatedParam()->GetCovMatrix();

// TMatrixD A(5, 5);
// A = thisUpdC * Ft * invPrevPredC;
   std::vector<myf> A(25);
   std::vector<myf> temp1(25);
   Mult15On25(thisUpdC, Ft, temp1);
   Mult25On15(temp1, invPrevPredC, A);

// TVectorD thisUpdX(5), prevSmoothedX(5), prevPredX(5);
// thisNode->GetUpdatedParam()->GetStateVector(thisUpdX);
// prevNode->GetSmoothedParam()->GetStateVector(prevSmoothedX);
// prevNode->GetPredictedParam()->GetStateVector(prevPredX);
// TVectorD thisSmoothedX(thisUpdX + A * (prevSmoothedX - prevPredX));

   const std::vector<myf>& thisUpdX = thisNode->GetUpdatedParam()->GetStateVector();
   const std::vector<myf>& prevSmoothedX = prevNode->GetSmoothedParam()->GetStateVector();
   const std::vector<myf>& prevPredX = prevNode->GetPredictedParam()->GetStateVector();

   std::vector<myf> temp2(5), temp3(5);
   Subtract(prevSmoothedX, prevPredX, temp2);
   Mult25On5(A, temp2, temp3);
   std::vector<myf> thisSmoothedX(5);
   Add(thisUpdX, temp3, thisSmoothedX);


// TMatrixDSym prevSmoothedC(5), prevPredC(5), Cdiff(5);
// prevNode->GetSmoothedParam()->GetCovMatrix(prevSmoothedC);
// prevNode->GetPredictedParam()->GetCovMatrix(prevPredC);
// Cdiff = prevSmoothedC - prevPredC;

   const std::vector<myf>& prevSmoothedC = prevNode->GetSmoothedParam()->GetCovMatrix();
   const std::vector<myf>& prevPredC = prevNode->GetPredictedParam()->GetCovMatrix();
   std::vector<myf> temp4(15);
   Subtract(prevSmoothedC, prevPredC, temp4);


// TMatrixDSym thisSmoothedC(5);
// thisSmoothedC = thisUpdC + Cdiff.Similarity(A);
   std::vector<myf> temp5(15);
   Similarity(A, temp4, temp5);
   std::vector<myf> thisSmoothedC(15);
   Add(thisUpdC, temp5, thisSmoothedC);

   CbmLitTrackParam par;

   par.SetStateVector(thisSmoothedX);
   par.SetCovMatrix(thisSmoothedC);
   par.SetZ(thisNode->GetUpdatedParam()->GetZ());

   thisNode->SetSmoothedParam(&par);
}
