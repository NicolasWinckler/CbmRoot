/**
 * \file CbmLitNearestHitTofMerger.h
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2013
 **/

#include "finder/CbmLitNearestHitTofMerger.h"

#include "data/CbmLitHit.h"
#include "data/CbmLitTrack.h"
#include "data/CbmLitTrackParam.h"
#include "interface/CbmLitTrackPropagator.h"
#include "interface/CbmLitTrackUpdate.h"
#include "utils/CbmLitMath.h"

#include <iostream>
#include <map>

using std::map;

CbmLitNearestHitTofMerger::CbmLitNearestHitTofMerger():
   fChiSqCut(25.)
{
}

CbmLitNearestHitTofMerger::~CbmLitNearestHitTofMerger()
{
}

LitStatus CbmLitNearestHitTofMerger::DoMerge(
   HitPtrVector& hits,
   TrackPtrVector& tracks)
{
   // First find hit with minimum Z position and build map Zhit position to track param
   // to improve the calculation speed.
   litfloat zMin = std::numeric_limits<litfloat>::max();
   map<litfloat, CbmLitTrackParam> zParamMap;
   for (HitPtrIterator it = hits.begin(); it != hits.end(); it++) {
      const CbmLitHit* hit = *it;
      zMin = std::min(zMin, hit->GetZ());
      zParamMap[hit->GetZ()] = CbmLitTrackParam();
   }


   std::cout << "-DoMerge: zMin=" << zMin << " nofZ=" << zParamMap.size() << std::endl;

   // Loop over input tracks
   for (TrackPtrIterator it = tracks.begin(); it != tracks.end(); it++) {
         CbmLitTrack* track = *it;
         if (track->GetQuality() != kLITGOODMERGE) { continue; }
         CbmLitTrackParam par(*track->GetParamLast());

         // Extrapolate track to minimum hit Z position using magnetic field propagator.
         fFieldPropagator->Propagate(&par, zMin, fPDG);

         for (map<litfloat, CbmLitTrackParam>::iterator it2 = zParamMap.begin(); it2 != zParamMap.end(); it2++) {
            (*it2).second = par;
            fLinePropagator->Propagate(&(*it2).second, (*it2).first, fPDG);
         }

        // std::cout << "-DoMerge: par=" << par.ToString();

         // Loop over hits
         litfloat minChiSq = std::numeric_limits<litfloat>::max(); // minimum chi-square of hit
         const CbmLitHit* minHit = NULL; // Pointer to hit with minimum chi-square
         CbmLitTrackParam minPar; // Track parameters for closest hit
         for (HitPtrIterator it = hits.begin(); it != hits.end(); it++) {
            const CbmLitHit* hit = *it;
            // Propagate track to Z position of the hit using line extrapolation.
            //CbmLitTrackParam tpar(par);
            //fLinePropagator->Propagate(&tpar, hit->GetZ(), fPDG);
            if (zParamMap.find(hit->GetZ()) == zParamMap.end()) {
               std::cout << "CbmLitNearestHitTofMerger::DoMerge: Z position " << hit->GetZ() << " not found in map. Something is wrong.\n";
            }
            CbmLitTrackParam tpar(zParamMap[hit->GetZ()]);
            litfloat chi = 0.;
            fFilter->Update(&tpar, hit, chi);
            if (chi < fChiSqCut && chi < minChiSq) { // Check if hit is inside validation gate and closer to the track.
               minChiSq = chi;
               minHit = hit;
               minPar = tpar;
            }
         }

         if (minHit != NULL) { // Check if hit was added
            track->AddHit(minHit);
            track->SetParamLast(&minPar);
            track->SetChi2(track->GetChi2() + minChiSq);
            track->SetNDF(lit::NDF(track));
         }
   }
   return kLITSUCCESS;
}
