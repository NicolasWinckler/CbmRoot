#include "LxEff.h"
#include "Lx.h"
#include <map>
#include <iostream>

#ifdef MAKE_DIFF_FIT_HISTOS
extern TH1F* stsXDiffFitHisto;
extern TH1F* stsYDiffFitHisto;
extern TH1F* stsTXDiffFitHisto;
extern TH1F* stsTYDiffFitHisto;
#endif//MAKE_DIFF_FIT_HISTOS

#ifdef MAKE_DIFF_FIRST_HISTOS
extern TH1F* stsXDiffFirstHisto[8];
extern TH1F* stsYDiffFirstHisto[8];
extern TH1F* stsTXDiffFirstHisto[8];
extern TH1F* stsTYDiffFirstHisto[8];
#endif//MAKE_DIFF_FIRST_HISTOS

#ifdef MAKE_FIT_COEFFS_HISTOS
extern TH1F* bXSigHisto;
extern TH1F* bYSigHisto;
extern TH1F* bXBackHisto;
extern TH1F* bYBackHisto;
#endif//MAKE_FIT_COEFFS_HISTOS

#ifdef MAKE_LAST_STS_NEAREST_Y
extern TH1F* rightLastStsYHisto;
#endif//MAKE_LAST_STS_NEAREST_Y

using namespace std;

static Double_t stsXDiffLimits[] = { 14.0, 12.0, 11.0, 10.0, 8.0, 7.0, 6.0, 5.7 };
static Double_t stsYDiffLimits[] = { 9.6, 8.7, 8.1, 7.5, 7.1, 6.5, 5.8, 5.3 };
static Double_t stsTXDiffLimits[] = { 0.249, 0.225, 0.202, 0.180, 0.159, 0.139, 0.122, 0.106 };
static Double_t stsTYDiffLimits[] = { 0.065, 0.065, 0.063, 0.063, 0.064, 0.065, 0.065, 0.064 };

LxEff::LxEff(LxFinder& owner) : finder(owner)
{
}

Double_t LxEff::CalcRecoEff(bool joinExt)
{
  Double_t result = 0;
  static Int_t foundTracks = 0;
  static Int_t reconstructableTracks = 0;
  Int_t foundNow = 0;
  Int_t mcNow = 0;
  bool thisRcbl = false;// Is this track reconstructable?

  for (vector<LxMCTrack>::iterator i = finder.MCTracks.begin(); i != finder.MCTracks.end(); ++i)
  {
    thisRcbl = false;
    LxMCTrack& mcTrack = *i;
    Int_t pdgCode = mcTrack.pdg;
    map<LxTrack*, int> recoTracks;
    map<Double_t, pair<Int_t, Int_t> >::iterator effByMomentumIter;

    for (vector<LxMCPoint*>::iterator j = mcTrack.Points.begin(); j != mcTrack.Points.end(); ++j)
    {
      LxMCPoint* pMCPoint = *j;

      for (list<LxPoint*>::iterator k = pMCPoint->lxPoints.begin(); k != pMCPoint->lxPoints.end(); ++k)
      {
        LxPoint* point = *k;
        LxTrack* track = point->track;

        if (0 == track)
          continue;
        
        if (track->matched)
          continue;

        map<LxTrack*, int>::iterator l = recoTracks.find(track);
        
        if (l != recoTracks.end())
          ++(l->second);
        else
          recoTracks[track] = 1;
      }
    }

    LxTrack* matchTrack = 0;
    int matchedPoints = 0;
    
    for (map<LxTrack*, int>::iterator j = recoTracks.begin(); j != recoTracks.end(); ++j)
    {
      if (0 == matchTrack || matchedPoints < j->second + j->first->restoredPoints)
      {
        matchTrack = j->first;
        matchedPoints = j->second + j->first->restoredPoints;
      }
    }

    int numberOfMuchMCPoints = LXLAYERS * (LXSTATIONS - LXFIRSTSTATION);

#ifdef CALC_LINK_WITH_STS_EFF
    if (0 != matchTrack && matchedPoints >= 0.7 * numberOfMuchMCPoints)
      matchTrack->mcTracks.push_back(&mcTrack);
#endif//CALC_LINK_WITH_STS_EFF

    bool mcPointOnSta[18] = { false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false };

    for (vector<LxMCPoint*>::iterator j = mcTrack.Points.begin(); j != mcTrack.Points.end(); ++j)
    {
      LxMCPoint* pMCPoint = *j;
      mcPointOnSta[pMCPoint->stationNumber * 3 + pMCPoint->layerNumber] = true;
    }

    bool isRefTrack = true;

    for (int j = 0; j < 18; ++j)
    {
      if (!mcPointOnSta[j])
      {
        isRefTrack = false;
      }
    }

    if (!isRefTrack)
      continue;

    Double_t pt2 = mcTrack.px * mcTrack.px + mcTrack.py * mcTrack.py;

    if (mcTrack.mother_ID < 0 && (pdgCode != 13 || pdgCode != -13) && mcTrack.p > 3 && pt2 > 1)
    {
      ++reconstructableTracks;
      ++mcNow;
      thisRcbl = true;
    }

    if (0 == matchTrack)
    {
      if (thisRcbl)
        LxFinder::effByMomentumProfile->Fill(mcTrack.p, 0.);
#ifdef MAKE_EFF_CALC
      if (thisRcbl)
      {
        finder.incomplete_events << finder.eventNumber << " matched points: " << matchedPoints << " ; MC points: " << mcTrack.Points.size() << endl;
        finder.caSpace.CheckArray(finder.incomplete_events, mcTrack);
      }
#endif//MAKE_EFF_CALC
      continue;
    }

    matchTrack->mcTrack = &mcTrack;

    if (matchedPoints < 0.7 * numberOfMuchMCPoints)
    {
      if (thisRcbl)
        LxFinder::effByMomentumProfile->Fill(mcTrack.p, 0.);
#ifdef MAKE_EFF_CALC
      if (thisRcbl)
      {
        finder.incomplete_events << finder.eventNumber << " matched points: " << matchedPoints << " ; MC points: " << mcTrack.Points.size() << endl;
        finder.caSpace.CheckArray(finder.incomplete_events, mcTrack);
      }
#endif//MAKE_EFF_CALC
      continue;
    }

#ifdef MAKE_DIFF_FIRST_HISTOS
    // Now we reconstructed track and can investigate its properties in STS.
    LxPoint* firstMuchPoint = matchTrack->rays[0]->end;
    Double_t firstMuchTx = matchTrack->rays[0]->tx;
    Double_t firstMuchTy = matchTrack->rays[0]->ty;

    for (int j = 0; j < 8; ++j)
    {
      for (list<LxStsMCPoint*>::iterator k = mcTrack.stsPoints[j].begin(); k != mcTrack.stsPoints[j].end(); ++k)
      {
        LxStsMCPoint* stsPoint = *k;
        Double_t diffZ = stsPoint->z - firstMuchPoint->z;
        Double_t diffX = stsPoint->x - firstMuchTx* diffZ - firstMuchPoint->x;
        stsXDiffFirstHisto[j]->Fill(diffX);
        Double_t diffY = stsPoint->y - firstMuchTy* diffZ - firstMuchPoint->y;
        stsYDiffFirstHisto[j]->Fill(diffY);

        if (j > 0)
        {
          for (list<LxStsMCPoint*>::iterator l = mcTrack.stsPoints[j - 1].begin(); l != mcTrack.stsPoints[j - 1].end(); ++l)
          {
            LxStsMCPoint* stsPointPrev = *l;
            Double_t tx = (stsPoint->x - stsPointPrev->x) / (stsPoint->z - stsPointPrev->z);
            //Double_t diffTx = atan(tx - firstMuchTx) * 180 / 3.1415926535;
            Double_t diffTx = tx - firstMuchTx;
            stsTXDiffFirstHisto[j]->Fill(diffTx);
            Double_t ty = (stsPoint->y - stsPointPrev->y) / (stsPoint->z - stsPointPrev->z);
            //Double_t diffTy = atan(ty - firstMuchTy) * 180 / 3.1415926535;
            Double_t diffTy = ty - firstMuchTy;
            stsTYDiffFirstHisto[j]->Fill(diffTy);
          }
        }
      }
    }
#endif//MAKE_DIFF_FIRST_HISTOS

#ifdef MAKE_FIT_COEFFS_HISTOS
    if (mcTrack.mother_ID < 0 && (pdgCode != 13 || pdgCode != -13) && mcTrack.p > 3 && pt2 > 1)
    {
      bXSigHisto->Fill(matchTrack->bX);
      bYSigHisto->Fill(matchTrack->bY);
    }
    else
    {
      bXBackHisto->Fill(matchTrack->bX);
      bYBackHisto->Fill(matchTrack->bY);
    }
#endif//MAKE_FIT_COEFFS_HISTOS

    //if (mcTrack.mother_ID >= 0 || (pdgCode != 13 && pdgCode != -13) || mcTrack.p <= 3 || pt2 <= 1)
    if (!thisRcbl)
      continue;

#ifdef MAKE_LAST_STS_NEAREST_Y
    LxStsMCPoint* lastStsNearestMC = 0;
    Double_t lastStsMinChi2 = 0;
    LxPoint* firstMuchPoint = matchTrack->rays[0]->end;
    Double_t firstMuchTx = matchTrack->rays[0]->tx;
    Double_t firstMuchTy = matchTrack->rays[0]->ty;

    for (list<LxStsMCPoint*>::iterator j = finder.MCStsPointsByStations[7].begin(); j != finder.MCStsPointsByStations[7].end(); ++j)
    {
      LxStsMCPoint* stsPoint = *j;
      Double_t diffZ = stsPoint->z - firstMuchPoint->z;
      Double_t diffX = stsPoint->x - firstMuchTx * diffZ - firstMuchPoint->x;
      Double_t diffY = stsPoint->y - firstMuchTy * diffZ - firstMuchPoint->y;
      //list<LxStsMCPoint*>& prevStPoints = stsPoint->mcTrack->stsPoints[7];

      //for (list<LxStsMCPoint*>::iterator k = prevStPoints.begin(); k != prevStPoints.end(); ++k)
      //{
        //LxStsMCPoint* prevStsPoint = *k;
        //Double_t dZ = stsPoint->z - prevStsPoint->z;
        //Double_t tx = (stsPoint->x - prevStsPoint->x) / dZ;
        //Double_t ty = (stsPoint->y - prevStsPoint->y) / dZ;
      Double_t tx = stsPoint->px / stsPoint->pz;
      Double_t ty = stsPoint->py / stsPoint->pz;
        Double_t diffTx = tx - firstMuchTx;
        Double_t diffTy = ty - firstMuchTy;
        Double_t chi2 = diffX * diffX / 2.03 + diffY * diffY / 1.77 + diffTx * diffTx / 0.000702 + diffTy * diffTy / 0.000257;

        if (0 == lastStsNearestMC || lastStsMinChi2 > chi2)
        {
          lastStsNearestMC = stsPoint;
          lastStsMinChi2 = chi2;
        }
      //}
    }

    if (0 == lastStsNearestMC)
      rightLastStsYHisto->Fill(0.);
    else
    {
      bool found = false;

      for (list<LxStsMCPoint*>::iterator j = mcTrack.stsPoints[7].begin(); j != mcTrack.stsPoints[7].end(); ++j)
      {
        if (lastStsNearestMC == *j)
        {
          found = true;
          break;
        }
      }

      if (found)
        rightLastStsYHisto->Fill(1.);
      else
        rightLastStsYHisto->Fill(0.);
    }
#endif//MAKE_LAST_STS_NEAREST_Y

    if (!joinExt// Don't demand joining. So track is considered as found.
         || (0 != matchTrack->externalTrack.track && matchTrack->externalTrack.track == mcTrack.externalTrack)// Both STS and we matched the same MC-track.
       )
    {
      matchTrack->matched = true;
      ++foundTracks;
      ++foundNow;
      LxFinder::effByMomentumProfile->Fill(mcTrack.p, 100.);
    }
    else
      LxFinder::effByMomentumProfile->Fill(mcTrack.p, 0.);
  }
  
  result = 100 * foundTracks;
  result /= reconstructableTracks;
  
#ifdef MAKE_EFF_CALC
  //if (foundNow != mcNow)
    //finder.incomplete_events << finder.eventNumber << endl;
#endif//MAKE_EFF_CALC

  cout << "LxEff::CalcRecoEff() returning: " << result << "( " << foundTracks << " / " << reconstructableTracks << " )" << endl;

  return result;
}

#ifdef CALC_LINK_WITH_STS_EFF
Double_t LxEff::CalcLinkEff()
{
  static Int_t trueTracks = 0;
  static Int_t allTracks = 0;

  for (list<LxTrack*>::iterator i = finder.caSpace.tracks.begin(); i != finder.caSpace.tracks.end(); ++i)
  {
    LxTrack* track = *i;

    if (0 == track->externalTrack.mcTrack)
      continue;

    ++allTracks;

    for (list<LxMCTrack*>::iterator j = track->mcTracks.begin(); j != track->mcTracks.end(); ++j)
    {
      LxMCTrack* mcTrack = *j;

      if (mcTrack == track->externalTrack.mcTrack)
      {
        ++trueTracks;
        break;
      }
    }
  }

  Double_t result = 100 * trueTracks;
  result /= allTracks;
  cout << "LxEff::CalcLinkEff() returning: " << result << "( " << trueTracks << " / " << allTracks << " )" << endl;
  return result;
}
#endif//CALC_LINK_WITH_STS_EFF
