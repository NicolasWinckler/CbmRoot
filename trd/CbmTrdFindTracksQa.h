// -------------------------------------------------------------------------
// -----                  CbmTrdFindTracksQa header file               -----
// -----                  Created 06/07/06  by A. Lebedev               -----
// -------------------------------------------------------------------------


/** CbmTrdFindTracksQa.h
 *@author A.Lebedev <alebedev@jinr.ru>
 **
 ** Quality check task for CbmTrdFindTracks
 **/


#ifndef CBMTRDTRACKFINDERQAA_H
#define CBMTRDTRACKFINDERQAA_H 1


#include "CbmTask.h"

#include "TVector3.h"

#include <set>
#include <map>

class TClonesArray;
class TH1;
class TH1F;
class TList;
class CbmGeoPassivePar;




class CbmTrdFindTracksQa : public CbmTask
{

 public:

  /** Default constructor **/
  CbmTrdFindTracksQa();


  /** Standard constructor
  *@param minPoints   Minimal number of MCPoints for considered MCTracks
  *@param quota       True/all hits for track to be considered reconstructed
  *@param iVerbose    Verbosity level
  **/
  CbmTrdFindTracksQa(Int_t minPoints, Double_t quota, Int_t iVerbose = 1);


  /** Destructor **/
  virtual ~CbmTrdFindTracksQa();


  /** Set parameter containers **/
  virtual void SetParContainers();


  /** Initialisation **/
  virtual InitStatus Init();


  /** Reinitialisation **/
  virtual InitStatus ReInit();


  /** Execution **/
  virtual void Exec(Option_t* opt);

  /** Accessors **/
  Int_t GetNormType() {return fNormType;}
  Int_t GetMinPoints() {return fMinPoints;}

  /** Modifiers **/
  void SetNormType(Int_t normType) { fNormType = normType;}
  void SetMinPoints(Int_t minPoints) { fMinPoints = minPoints;}

 private:

  /** Finish **/
  virtual void Finish();

  /** Read the geometry parameters **/
  InitStatus GetGeometry();


  /** Create histograms **/
  void CreateHistos();


  /** Reset histograms and counters **/
  void Reset();


  /** Divide histograms (reco/all) with correct error for the efficiency
   *@param histo1  reconstructed tracks
   *@param histo2  all tracks (normalisation)
   *@param histo3  efficiency
   **/
  void DivideHistos(TH1* histo1, TH1* histo2, TH1* histo3);


  /** Map from MCTrack index to matched TrdTrack index **/
  std::map<Int_t, Int_t> fMatchMap;

  /** Set MCTrack index  **/
  std::set<Int_t> fMatchSetSts;


  /** Map from MCTrack index to percentage of matched hits **/
  std::map<Int_t, Double_t> fQualiMap;


  /** Pointers to data arrays **/
  TClonesArray* fMCTracks;           // MCtrack
  TClonesArray* fTrdTracks;          // TrdTrack
  TClonesArray* fStsTracks;          // StsTrack
  TClonesArray* fTrdMatches;         // TrdTrackMatch
  TClonesArray* fStsMatches;         // StsTrackMatch


  /** Normalisation type.
     ** 1 - by number of reconstructable in TRD MC tracks.
     ** 2 - by number of STS tracks, which match to reco MC tracks
     ** Default value - 1.
     **/
  Int_t fNormType;



  /** Geometry parameters **/
  CbmGeoPassivePar* fPassGeo;             // Passive geometry parameters
  TVector3 fTargetPos;                    // Target centre position



  /** Task parameters **/
  Int_t fMinPoints; // Minimal number of MCPoints for considered MCTrack
  Double_t fQuota;  // True/all hits for track to be considered reconstructed


  /** Histograms **/
  TH1F* fhMomAccAll,  *fhMomRecAll,  *fhMomEffAll;   // eff. vs. p, all
  TH1F* fhMomAccPrim, *fhMomRecPrim, *fhMomEffPrim;  // eff. vs. p, vertex
  TH1F* fhMomAccSec,  *fhMomRecSec,  *fhMomEffSec;   // eff. vs. p, non-vertex
  TH1F* fhNpAccAll,   *fhNpRecAll,   *fhNpEffAll;    // eff. vs. np, all
  TH1F* fhNpAccPrim,  *fhNpRecPrim,  *fhNpEffPrim;   // eff. vs. np, vertex
  TH1F* fhNpAccSec,   *fhNpRecSec,   *fhNpEffSec;    // eff. vs. np, non-vertex
  TH1F* fhZAccSec,    *fhZRecSec,    *fhZEffSec;     // eff. vs. z, non-vertex
  TH1F* fhNhClones,   *fhNhGhosts;              // # hits of clones and ghosts   


  /** List of histograms **/
  TList* fHistoList;


  /** Counters **/
  Int_t fNAccAll, fNAccPrim, fNAccRef, fNAccSec;
  Int_t fNRecAll, fNRecPrim, fNRecRef, fNRecSec;
  Int_t fNGhosts, fNClones;
  Int_t fNEvents;


  ClassDef(CbmTrdFindTracksQa,1);

};


#endif
				 

