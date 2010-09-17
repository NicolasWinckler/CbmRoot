#include "CbmL1RichRingQa.h"

#include "FairTask.h"
#include "FairRootManager.h"
#include "CbmMCTrack.h"
#include "CbmRichPoint.h"
#include "CbmRichHit.h"
#include "CbmRichRing.h"
#include "CbmMCTrack.h"
#include "CbmRichRingMatch.h"
//#include "CbmStsTrackMatch.h"

#include "TH1.h"
#include "TH2.h"
#include "TProfile.h"
#include "TFile.h"
#include "TClonesArray.h"
#include "TROOT.h"

#include <iostream>
#include <list>
#include <vector>
#include <map>
#include <cmath>

using std::cout;
using std::endl;
using std::ios;
using std::list;
using std::pair;
using std::vector;
using std::map;
using std::fabs;

ClassImp(CbmL1RichRingQa)


//------------  standard constructor (with verbosity level)  ---------------------------------
CbmL1RichRingQa::CbmL1RichRingQa(const char *name, const char *title, Int_t verbose)
  :FairTask(name),
  fRingArray(0),        // Array of CbmRichRings
  fMCPointArray(0),       // Array of FairMCPoints
  fMCTrackArray(0),       // Array of CbmMCTracks
  fHitArray(0)         // Array of CbmRichHits
{
  fVerbose = verbose;
}

  // -----   Destructor   ----------------------------------------------------
CbmL1RichRingQa::~CbmL1RichRingQa()
{
}

InitStatus CbmL1RichRingQa::Init()
{

 // Get and check FairRootManager
  FairRootManager* ioman = FairRootManager::Instance();
  if (! ioman) {
    cout << "-E- CbmL1RichRingQa::Init: "
	 << "RootManager not instantised!" << endl;
    return kERROR;
  }

  // Get hit Array
  fHitArray = (TClonesArray*) ioman->GetObject("RichHit");
  if ( ! fHitArray) {
    cout << "-W- CbmL1RichRingQa::Init: No RichHit array!"
	 << endl;
  }

  // Get RichRing Array
  fRingArray = (TClonesArray*) ioman->GetObject("RichRing");
  if ( ! fRingArray ) {
    cout << "-E- CbmL1RichRingQa::Init: No RichRing array!" << endl;
    return kERROR;
  }

  // Get MC Point array
  fMCPointArray = (TClonesArray*) ioman->GetObject("RichPoint");
  if ( ! fMCPointArray ) {
    cout << "-E- CbmL1RichRingQa::Init: No RichPoint array!" << endl;
    return kERROR;
  }

  // Get MC Point array
  fMCTrackArray = (TClonesArray*) ioman->GetObject("MCTrack");
  if ( ! fMCTrackArray ) {
    cout << "-E- CbmL1RichRingQa::Init: No MCTrack array!" << endl;
    return kERROR;
  }

  return kSUCCESS;
}


void CbmL1RichRingQa::CirFit( list<pair<Double_t,Double_t> > &P, Double_t *X, Double_t *Y, Double_t *R )
{
  Double_t S[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
  Int_t n=0;
  for( list<pair<Double_t,Double_t> >::iterator i=P.begin(); i!=P.end(); ++i )
    {
      Double_t &x = i->first;
      Double_t &y = i->second;
      Double_t r2 = x*x+y*y;
      S[0] +=  x * x;
      S[1] +=  y * y;
      S[2] +=  x * y;
      S[3] +=  x * r2 ;
      S[4] += y * r2 ;
      S[5] += x;
      S[6] += y;
      S[7] += r2;
      n++;
    }
  Double_t s0 = S[6]*S[0]-S[2]*S[5];
  Double_t s1 = S[0]*S[1]-S[2]*S[2];
  Double_t s2 = S[0]*S[4]-S[2]*S[3];
  *X = *Y = *R = 0.;
  if( fabs(s0)<1.E-6 || fabs(s1)<1.E-6 ) return;
  Double_t tmp = s1*(S[5]*S[5]-n*S[0])+s0*s0;
  Double_t A = ( ( S[0]*S[7]-S[3]*S[5] )*s1-s2*s0 ) / tmp;
  *Y = (s2 + s0*A )/s1/2;
  *X = ( S[3] + S[5]*A - S[2]*(*Y)*2 )/S[0]/2;
  Double_t R2 = (*X)*(*X)+(*Y)*(*Y)-A;
  if( R2<0 ) return;
  *R = sqrt( R2 );
}

void CbmL1RichRingQa::Exec(Option_t* option)
{
  // histogramms

  static TH1F
    *h_MC_radius,
    *h_MC_nhits,
    *h_MC_primary_nhits,
    *h_MC_momentum,
    *h_MC_primary_momentum,
    *h_MC_resolution,
    *h_MC_ref_resolution,
    *h_MC_extra_resolution,
    *h_ghost_nhits;

  static TH2F
    *h_MC_primary_res_vs_momentum;

  static TProfile
    *p_ref_eff_vs_nhits,
    *p_extra_eff_vs_nhits;

  static TList *listHisto;
  static bool first_call_performance = 1;

  if ( first_call_performance )
    {
      first_call_performance = 0;
      TDirectory *curdir = gDirectory;
      TDirectory *histodir = gROOT->mkdir("CbmL1RichRingQaHisto");
      histodir->cd();

      h_MC_radius = new TH1F("h_MC_radius", "MC ring radius (cm)", 100, 0.0, 10.);
      h_MC_nhits  = new TH1F("h_MC_nhits", "Hits per MC ring", 50, 0.0, 50.);
      h_MC_primary_nhits  = new TH1F("h_MC_primary_nhits", "Hits per primary MC ring", 50, 0.0, 50.);
      h_MC_momentum = new TH1F("h_MC_momentum", "MC track momentum (GeV)", 100, 0.0, 15.);
      h_MC_primary_momentum = new TH1F("h_MC_primary_momentum", "MC primary track momentum (GeV)", 100, 0.0, 15.);
      h_MC_resolution =
	new TH1F("h_MC_resolution", "Hit deviation from MC ring (cm)", 500, -5.0, 5.0);
      h_MC_ref_resolution =
	new TH1F("h_MC_ref_resolution", "Hit deviation from REF MC ring (cm)", 500, -5.0, 5.0);
      h_MC_extra_resolution =
	new TH1F("h_MC_extra_resolution", "Hit deviation from EXTRA MC ring (cm)", 500, -5.0, 5.0);

      h_ghost_nhits = new TH1F("h_ghost_nhits", "Hits per ghost ring", 50, 0.0, 50.);

      h_MC_primary_res_vs_momentum =
	new TH2F("h_MC_primary_res_vs_momentum", "Hit deviation from ptimary MC ring (cm) vs P",
		  100, 0., 15., 500, -5.0, 5.0 );

      p_ref_eff_vs_nhits =
	new TProfile("p_ref_eff_vs_nhits", "Refset efficiency vs N Hits", 100, 0.0, 50.0, 0.0, 1.0 );

      p_extra_eff_vs_nhits =
	new TProfile("p_extra_eff_vs_nhits", "Extraset efficiency vs N Hits", 100, 0.0, 50.0, 0.0, 1.0 );

      // ----- Create list of all histogram pointers
      listHisto = gDirectory->GetList();

      curdir->cd();
    }

  // Create hit vector

  if( !fHitArray || !fMCTrackArray || !fMCPointArray || !fRingArray ) return;

  int NHits = fHitArray->GetEntriesFast();
  PerfHit Hits[NHits];

  map < void*, int > pHitIndex;

  for( Int_t i = 0; i<NHits; i++ ){

    PerfHit &hit = Hits[i];
    hit.index = i;
    hit.x = 0;
    hit.y = 0;
    hit.MCTrackID = - 1;

    CbmRichHit * phit = (CbmRichHit*) fHitArray->At( i );
    if ( !phit ) continue;
    pHitIndex.insert(pair<void*, int>(phit,i));
    hit.x = phit->GetX();
    hit.y = phit->GetY();
    Int_t pointID = phit->GetRefId();
    if (pointID < 0) continue;
    CbmRichPoint* point = (CbmRichPoint*) (fMCPointArray->At(pointID));
    if ( !point ) continue;
    Int_t trackID = point->GetTrackID();
    if( trackID< 0 ) continue;
    CbmMCTrack *track= (CbmMCTrack*) fMCTrackArray->At(trackID);
    if( !track || track->GetPdgCode() != 50000050 ) continue; // select only Cherenkov photons
    hit.MCTrackID = track->GetMotherId();
  }

  // Create map of MC rings

  map < Int_t, MCRing> MCRingMap;

  // match hits

  for( int ih = 0; ih<NHits; ih++ ){
    int ID =  Hits[ih].MCTrackID;
    if ( ID>=0 && MCRingMap.find( ID )==MCRingMap.end() )
      {
	MCRing tmp;
	tmp.NHits = 0;
	MCRingMap.insert(pair<Int_t, MCRing>( ID, tmp));
      }
    MCRingMap[ID].NHits++;
    MCRingMap[ID].Hits.push_back(ih);
  }

  // fit MC rings & set parameters

  for( map<Int_t,MCRing>::iterator i=MCRingMap.begin(); i!=MCRingMap.end(); ++i )
    {
      MCRing &ring = i->second;
      ring.MCTrackID = i->first;
      ring.primary = 0;
      ring.P = 0;
      ring.PDG = 0;
      ring.Reconstructed = 0;
      ring.kind = 0;

      // find momentum & vertex
      if( !fMCTrackArray || ring.MCTrackID < 0 ) continue;

      CbmMCTrack *pm= (CbmMCTrack*) fMCTrackArray->At( ring.MCTrackID );
      if ( pm ){
	ring.PDG = pm->GetPdgCode();  // get PDG code of mother

	Double_t vx = pm->GetStartX();
	Double_t vy = pm->GetStartY();
	Double_t vz = pm->GetStartZ();

	if (fabs(vx)<0.1 && fabs(vy)<0.1 && fabs(vz)<0.1) ring.primary = 1;

	ring.P = pm->GetP();
      }

      // fit the MC ring

      list<pair<Double_t,Double_t> > L;
      for( vector<int>::iterator j=ring.Hits.begin(); j!=ring.Hits.end(); ++j ){
	L.push_back(pair<Double_t,Double_t>( Hits[(*j)].x, Hits[(*j)].y) );
      }
      CirFit( L, &ring.x, &ring.y, &ring.r );

      // define the kind of the ring

      if ( ring.r>3. && ring.r<7. && ring.NHits >=5 )
	{
	  if ( ring.NHits >=15 && ring.primary )
	    {
	      ring.kind = 2;
	    }
	  else ring.kind = 1;
	}
      else ring.kind = 0;
    }


  // match reconstructed->simulated rings, calc. ghost

  Int_t NGhost = 0;
  Int_t NReco = fRingArray->GetEntriesFast();

  for( Int_t ir = 0; ir<NReco; ir++ ){

    map< Int_t, Int_t > hitmap;
    CbmRichRing *r = (CbmRichRing*) fRingArray->At(ir);
    Int_t nh = r->GetNofHits();

    for( int ih=0; ih<nh; ih++ )
      {

	CbmRichHit *h = (CbmRichHit *)fHitArray->At(r->GetHit(ih));
	Int_t jh = pHitIndex[h];
	int ID = Hits[jh].MCTrackID;
	if( hitmap.find( ID )==hitmap.end() ) hitmap.insert( pair<Int_t,Int_t>( ID, 0 ) );
	hitmap[ ID ]++;
      }
    Bool_t ghost = 1;
    for( map<Int_t,Int_t>::iterator j=hitmap.begin(); j!=hitmap.end(); ++j )
      {
	if ( ( (Double_t) j->second ) < 0.7 * nh ) continue;
	MCRingMap[ j->first ].Reconstructed++;
	ghost = 0;
	break;
      }
    if ( ghost )
      {
	h_ghost_nhits->Fill( nh );
	NGhost ++;
      }
  }

  // get statistics from MC rings

  Int_t
    NAll = 0, NRef = 0, NExtra = 0,
    NAllReco = 0, NRefReco = 0, NExtraReco = 0,
    NClone = 0;

  for( map<Int_t,MCRing>::iterator i=MCRingMap.begin(); i!=MCRingMap.end(); ++i )
    {
      MCRing &ring = i->second;
      if ( ring.kind==0 ) continue;
      NAll++;
      if ( ring.kind==1 ) NExtra++;
      if ( ring.kind==2 ) NRef++;
      if ( ring.Reconstructed )
	{
	  NAllReco++;
	  NClone += i->second.Reconstructed - 1 ;
	  if ( ring.kind==2 )  NRefReco++;
	  else NExtraReco++;
	}
    }


  // accumulated statistics

  static Int_t
    S_NAll = 0, S_NRef = 0, S_NExtra = 0, S_NReco = 0,
    S_NAllReco = 0, S_NRefReco = 0, S_NExtraReco = 0,
    S_NClone = 0, S_NGhost = 0,
    S_NEvents = 0;

  S_NAll       += NAll;
  S_NRef       += NRef;
  S_NExtra     += NExtra;
  S_NReco      += NReco;
  S_NAllReco   += NAllReco;
  S_NRefReco   += NRefReco;
  S_NExtraReco += NExtraReco;
  S_NClone     += NClone;
  S_NGhost     += NGhost;

  S_NEvents++ ;

  // write statistics

  cout.setf(ios::fixed);
  cout.setf(ios::showpoint);
  cout.precision(4);
  /*

  Double_t p_all   = (NAll   >0)  ? (Double_t) NAllReco / NAll : 0.;
  Double_t p_ref   = (NRef   >0)  ? (Double_t) NRefReco / NRef : 0.;
  Double_t p_extra = (NExtra >0)  ? (Double_t) NExtraReco / NExtra : 0.;
  Double_t p_clone = (NReco  >0)  ? (Double_t) NClone / NReco : 0.;
  Double_t p_ghost = (NReco  >0)  ? (Double_t) NGhost / NReco : 0.;

  cout                                               << endl;
  cout << "L1ENN PER EVENT STAT   : "                << endl;
  cout << "MC Refset              : " << NRef        << endl;
  cout << "MC Extras              : " << NExtra      << endl;
  cout << "ALL SIMULATED          : " << NAll        << endl;
  cout                                               << endl;
  cout << "RC Refset              : " << NRefReco    << endl;
  cout << "RC Extras              : " << NExtraReco  << endl;
  cout << "clones                 : " << NClone      << endl;
  cout << "ghosts                 : " << NGhost      << endl;
  cout << "ALL RECONSTRUCTED      : " << NAllReco    << endl;
  cout                                               << endl;
  cout << "Refset efficiency      : " << p_ref       << endl;
  cout << "Allset efficiency      : " << p_all       << endl;
  cout << "Extra  efficiency      : " << p_extra     << endl;
  cout << "clone  probability     : " << p_clone     << endl;
  cout << "ghost  probability     : " << p_ghost         << endl;
  cout << endl;
  cout << "Reco time (ms)         : " << fRecoTime/1000. << endl;

  cout << endl;
  */

  Double_t S_p_all   = (S_NAll   >0)  ? (Double_t) S_NAllReco / S_NAll : 0.;
  Double_t S_p_ref   = (S_NRef   >0)  ? (Double_t) S_NRefReco / S_NRef : 0.;
  Double_t S_p_extra = (S_NExtra >0)  ? (Double_t) S_NExtraReco / S_NExtra : 0.;
  Double_t S_p_clone = (S_NReco  >0)  ? (Double_t) S_NClone / S_NReco : 0.;
  Double_t S_p_ghost = (S_NReco  >0)  ? (Double_t) S_NGhost / S_NReco : 0.;

  cout << "ACCUMULATED STAT : " << S_NEvents << " EVENTS "               << endl << endl;

  cout << "Refset efficiency      : " << S_p_ref       <<" | "<< S_NRef << endl;
  cout << "Allset efficiency      : " << S_p_all       <<" | "<< S_NAll << endl;
  cout << "Extra  efficiency      : " << S_p_extra     <<" | "<< S_NExtra << endl;
  cout << "clone  probability     : " << S_p_clone     <<" | "<< S_NClone << endl;
  cout << "ghost  probability     : " << S_p_ghost     <<" | "<< S_NGhost << endl;
  cout << "MC rings/event found   : " << Int_t(Double_t(S_NAllReco)/Double_t(S_NEvents)) << endl;
  cout << endl;
  cout << "Reco time (ms)         : " << 0.*1000./S_NEvents
       << endl;

  cout << endl;


  // fill histogramms

  for( map<Int_t,MCRing>::iterator i=MCRingMap.begin(); i!=MCRingMap.end(); ++i )
    {
      MCRing &ring = i->second;

      h_MC_radius->Fill( ring.r );
      h_MC_nhits->Fill( ring.NHits );
      h_MC_momentum->Fill( ring.P );
      if ( ring.primary )
	{
	  h_MC_primary_nhits->Fill( ring.NHits );
	  h_MC_primary_momentum->Fill( ring.P );
	}

      for( vector<int>::iterator j=ring.Hits.begin(); j!=ring.Hits.end(); ++j )
	{
	  Double_t dx = Hits[(*j)].x - ring.x;
	  Double_t dy = Hits[(*j)].y - ring.y;
	  Double_t res = sqrt( dx*dx + dy*dy) - ring.r;
	  h_MC_resolution->Fill( res );
	  if ( ring.kind==1 ) h_MC_extra_resolution->Fill( res );
	  if ( ring.kind==2 ) h_MC_ref_resolution->Fill( res );
	  if ( ring.primary ) h_MC_primary_res_vs_momentum->Fill( ring.P, res );
	}
      Double_t entry = ( ring.Reconstructed ) ? 1.0  : 0.0 ;
      if ( ring.kind==1 ) p_extra_eff_vs_nhits->Fill( ring.NHits, entry );
      if ( ring.kind==2 ) p_ref_eff_vs_nhits->Fill  ( ring.NHits, entry );
    }

  // Open output file and write histograms
  TDirectory *curr = gDirectory;
  TFile* outfile = new TFile("L1RingQaHisto.root","RECREATE");
  outfile->cd();
  TIter hiter(listHisto);
  while (TObject* obj = hiter()) obj->Write();
  outfile->Close();
  curr->cd();
}

void CbmL1RichRingQa::Finish()
{
}


