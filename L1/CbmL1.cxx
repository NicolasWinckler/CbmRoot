/*
 *====================================================================
 *
 *  CBM Level 1 Reconstruction 
 *  
 *  Authors: I.Kisel,  S.Gorbunov
 *
 *  e-mail : ikisel@kip.uni-heidelberg.de 
 *
 *====================================================================
 *
 *  L1 main program
 *
 *====================================================================
 */
#include "CbmL1.h"

#include "L1Algo/L1Algo.h"
#include "L1Algo/L1StsHit.h"
#include "L1Algo/L1Branch.h"

#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "CbmGeoStsPar.h"
#include "CbmStsStation.h"
#include "CbmStsSector.h"
#include "CbmStsDigiScheme.h"
#include "CbmStsFindTracks.h"
#include "CbmKF.h"

#include "TVector3.h"
#include "TVectorD.h"
#include "TMatrixD.h"
#include "TROOT.h"

#include <iostream>
#include <vector>

using std::cout;
using std::endl;
using std::vector;

ClassImp(CbmL1)


static L1Algo algo_static _fvecalignment;

CbmL1 *CbmL1::fInstance = 0;


CbmL1::CbmL1()
{
  if( !fInstance ) fInstance = this;
  fTrackingLevel = 2;
  //fMomentumCutOff = 0.2;
  fMomentumCutOff = 0.1;
  fGhostSuppression = 1;
  fDetectorEfficiency = 1.;
}

CbmL1::CbmL1(const char *name, Int_t iVerbose ):FairTask(name,iVerbose)
{
  if( !fInstance ) fInstance = this;
  fTrackingLevel = 2;
  //fMomentumCutOff = 0.2;
  fMomentumCutOff = 0.1;
  fGhostSuppression = 1;
  fDetectorEfficiency = 1.;
}

CbmL1::~CbmL1()
{
  if( fInstance==this ) fInstance = 0;
}



void CbmL1::SetParContainers()
{
  FairRunAna* ana = FairRunAna::Instance();
  FairRuntimeDb* rtdb=ana->GetRuntimeDb();
  rtdb->getContainer("CbmGeoPassivePar");
  rtdb->getContainer("CbmGeoStsPar");
  rtdb->getContainer("CbmStsDigiPar");
}

InitStatus CbmL1::ReInit()
{
  SetParContainers();
  return Init();
}

InitStatus CbmL1::Init()
{
  if( fVerbose>1 ){
    char y[20] = " [0;33;44m"; // yellow 
    char Y[20] = " [1;33;44m"; // yellow bold     
    char W[20] = " [1;37;44m"; // white bold 
    char o[20] = " [0m\n";       // color off
    Y[0] = y[0] = W[0] = o[0] = 0x1B;    // escape character
    
    cout<<endl<<endl;
    cout<<"  "<<W<<"                                                                 "<<o;
    cout<<"  "<<W<<"  ===////======================================================  "<<o;
    cout<<"  "<<W<<"  =                                                           =  "<<o;
    cout<<"  "<<W<<"  =                   "
	<<Y                      <<"L1 on-line finder"
	<<W                                       <<"                       =  "<<o;
    cout<<"  "<<W<<"  =                                                           =  "<<o;
    cout<<"  "<<W<<"  =  "
	<<W     <<"Cellular Automaton 3.1 Vector"
	<<y                                  <<" with "
	<<W                                        <<"KF Quadro"
	<<y                                                 <<" technology"
	<<W                                                            <<"  =  "<<o;
    cout<<"  "<<W<<"  =                                                           =  "<<o;
    cout<<"  "<<W<<"  =  "
	<<y     <<"Designed for CBM collaboration"
	<<W                                   <<"                           =  "<<o;
    cout<<"  "<<W<<"  =  "
	<<y     <<"All rights reserved"
	<<W                        <<"                                      =  "<<o;
    cout<<"  "<<W<<"  =                                                           =  "<<o;
    cout<<"  "<<W<<"  ========================================================////=  "<<o;
    cout<<"  "<<W<<"                                                                 "<<o;
    cout<<endl<<endl;
  }

  FairRootManager *fManger = FairRootManager::Instance();

  FairRunAna *Run = FairRunAna::Instance();
  FairRuntimeDb *RunDB = Run->GetRuntimeDb();
  {
    CbmGeoStsPar* StsPar = (CbmGeoStsPar*) (RunDB->findContainer("CbmGeoStsPar"));
    CbmStsDigiPar *digiPar = (CbmStsDigiPar*)(RunDB->findContainer("CbmStsDigiPar"));
    StsDigi.Init(StsPar, digiPar);
  }
  {
    fUseMVD = 1;
    CbmStsFindTracks * FindTask = (CbmStsFindTracks*) Run->GetTask("STSFindTracks");
    if( FindTask ) fUseMVD = FindTask->MvdUsage();
  }

  histodir = gROOT->mkdir("L1");

  listStsPts = (TClonesArray *)  fManger->GetObject("StsPoint");
  listStsHits = (TClonesArray *)  fManger->GetObject("StsHit");
  listMCTracks = (TClonesArray*) fManger->GetObject("MCTrack");  

  if( !fUseMVD ){
    listMvdPts = 0;
    listMvdHits = 0;
    listMvdHitMatches = 0;
  } else {
    listMvdPts = (TClonesArray *)  fManger->GetObject("MvdPoint");
    listMvdHits = (TClonesArray *)  fManger->GetObject("MvdHit");
    listMvdHitMatches = (TClonesArray *)  fManger->GetObject("MvdHitMatch");
  }

  NMvdStations = 0;
  NStsStations = 0;
  NStation = 0;
  algo = & algo_static;

  fscal geo[10000];

  int ind = 0;
  for( int i=0; i<3; i++ ){
    Double_t point[3] = { 0,0,2.5*i};
    Double_t B[3] = {0,0,0};
    if( CbmKF::Instance()->GetMagneticField() ) CbmKF::Instance()->GetMagneticField()->GetFieldValue( point, B );	
    geo[ind++] = 2.5*i;
    geo[ind++] = B[0];
    geo[ind++] = B[1];
    geo[ind++] = B[2];
  }
      
  NMvdStations = CbmKF::Instance()->vMvdMaterial.size();
  NStsStations = StsDigi.GetNStations();
  NStation = NMvdStations + NStsStations;
  geo[ind++] = NStation;

  // field
  const int M=3; // polinom order
  const int N=(M+1)*(M+2)/2;

  for ( Int_t ist = 0; ist<NStation; ist++ )
    {
      double C[3][N];
      double z = 0;
      double R = 0;
      if( ist<NMvdStations ){
	CbmKFTube &t = CbmKF::Instance()->vMvdMaterial[ist];
	geo[ind++] = t.z;
	geo[ind++] = t.dz;
	geo[ind++] = t.r;
	geo[ind++] = t.R;
	geo[ind++] = t.RadLength;	
	fscal f_phi=0, f_sigma=5.e-4, b_phi=3.14159265358/2., b_sigma=5.e-4;
	geo[ind++] = f_phi;
	geo[ind++] = f_sigma;
	geo[ind++] = b_phi;
	geo[ind++] = b_sigma;      
	z = t.z;
	R = t.R;
      }else{
	CbmStsStation *st = StsDigi.GetStation(ist - NMvdStations);
	
	geo[ind++] = st->GetZ();
	geo[ind++] = st->GetD();
	geo[ind++] = st->GetRmin();
	geo[ind++] = st->GetRmax();
	geo[ind++] = st->GetRadLength();
	
	CbmStsSector* sector = st->GetSector(0);
	fscal f_phi, f_sigma, b_phi, b_sigma; // angle and sigma front/back  side
	f_phi = sector->GetRotation();
	b_phi = sector->GetRotation();
	if( sector->GetType()==1 ){
	  b_phi += 3.14159265358/2.;
	  b_sigma = sector->GetDy()/sqrt(12.);
	}
	else{
          f_phi +=sector->GetStereoF();
          b_phi +=sector->GetStereoB();
          f_sigma = sector->GetDx() / TMath::Sqrt(12);
          b_sigma  = f_sigma;
            }

	//if( sector->GetType()==2 ){ //!! DEBUG !!
	//b_phi = sector->GetRotation() + 3.14159265358/2.;
	//b_sigma = 12./sqrt(12.);
	//}
	geo[ind++] = f_phi;
	geo[ind++] = f_sigma;
	geo[ind++] = b_phi;
	geo[ind++] = b_sigma;
	//cout<<"ist="<<ist
	//<<", f_phi="<<f_phi<<", b_phi="<<b_phi
	//<<", f_sigma="<<f_sigma <<", b_sigma="<<b_sigma<<endl;
	z = st->GetZ();
	R = st->GetRmax();
      }
      double d = 1.;
      if( d > R/N/2 ) d = R/N/4.;
      for( int i=0; i<3; i++)
	for( int k=0; k<N; k++) C[i][k] = 0;
      TMatrixD A(N,N);
      TVectorD b0(N), b1(N), b2(N);
      for( int i=0; i<N; i++){
	for( int j=0; j<N; j++) A(i,j)==0.;
	b0(i)=b1(i)=b2(i) = 0.;
      }
      for( double x=-R; x<=R; x+=d )
	for( double y=-R; y<=R; y+=d )
	  {
	    double r = sqrt(fabs(x*x+y*y));
	    if( r>R ) continue;
	    Double_t w = 1./(r*r+1);
	    Double_t p[3] = { x, y, z};
	    Double_t B[3] = {0.,0.,0.};
	    CbmKF::Instance()->GetMagneticField()->GetFieldValue(p, B);
	    TVectorD m(N);
	    m(0)=1; 
	    for( int i=1; i<=M; i++){
	      int k = (i-1)*(i)/2;
	      int l = i*(i+1)/2;
	      for( int j=0; j<i; j++ ) m(l+j) = x*m(k+j);
	      m(l+i) = y*m(k+i-1);
	    }
	    
	    TVectorD mt = m;
	    for( int i=0; i<N; i++){
	      for( int j=0; j<N;j++) A(i,j)+=w*m(i)*m(j);
	      b0(i)+=w*B[0]*m(i);
	      b1(i)+=w*B[1]*m(i);
	      b2(i)+=w*B[2]*m(i);
	    }	
	  }	  
      double det;
      A.Invert(&det);
      TVectorD c0 = A*b0, c1 = A*b1, c2 = A*b2;
      for(int i=0; i<N; i++){
	C[0][i] = c0(i);
	C[1][i] = c1(i);
	C[2][i] = c2(i);
      }

      geo[ind++] = N;
      for( int k=0; k<3; k++ ){	  
	for( int j=0; j<N; j++) geo[ind++] = C[k][j];
      }    
    }

  geo[ind++] = fTrackingLevel;
  geo[ind++] = fMomentumCutOff;
  geo[ind++] = fGhostSuppression;

  algo->Init(geo);
  return kSUCCESS;
}


void CbmL1::Exec(Option_t * option)
{
  static int nevent=1;
  if( fVerbose>1 ) cout << endl << "CbmL1::Exec event " << nevent++ << " ..." << endl << endl;

  ReadEvent();  

  if( fVerbose>1 ) cout<<"L1 Track finder..."<<endl;
  algo->CATrackFinder();
  if( fVerbose>1 ) cout<<"L1 Track finder ok"<<endl;
  //algo->KFTrackFitter();
  //cout<<"L1 Track fitter  ok"<<endl;
  vRTracks.clear();
  int start_hit = 0;
  for(vector<L1Track>::iterator it = algo->vTracks.begin(); it!=algo->vTracks.end(); it++){
    CbmL1Track t;
    for( int i=0; i<6; i++) t.T[i] = it->TFirst[i];
    for( int i=0; i<15; i++) t.C[i] = it->CFirst[i];
    for( int i=0; i<6; i++) t.TLast[i] = it->TLast[i];
    for( int i=0; i<15; i++) t.CLast[i] = it->CLast[i];
    t.chi2 = it->chi2;
    t.NDF = it->NDF;
    //t.T[4] = it->Momentum;
    for( int i=0; i<it->NHits; i++ ){ 
      t.StsHits.push_back( algo->vRecoHits[start_hit++]);
    }
    t.mass = 0.1395679; // pion mass
    t.is_electron = 0;
    vRTracks.push_back(t);
  }

  if( fVerbose>1 ) cout<<"Performance..."<<endl;
  Performance();

  if( fVerbose>1 ) cout<<"End of L1"<<endl;

  static bool ask = 0; 
  char symbol;
  if (ask){
    std::cout << std::endl << "L1 run (any/r/q) > ";
    do{
      std::cin.get(symbol);
      if (symbol == 'r')
	ask = false;
      if ((symbol == 'e')||(symbol == 'q'))
	exit(0);
    } while (symbol != '\n');
  }
}

// -----   Finish CbmStsFitPerformanceTask task   -----------------------------
void CbmL1::Finish(){
  TDirectory *curr = gDirectory;
  // Open output file and write histograms
  TFile* outfile = new TFile("L1_histo.root","RECREATE");
  outfile->cd();
  writedir2current(histodir);
  outfile->Close();
  curr->cd();
}


void CbmL1::writedir2current( TObject *obj ){
  if( !obj->IsFolder() ) obj->Write();
  else{
    TDirectory *cur = gDirectory;
    TDirectory *sub = cur->mkdir(obj->GetName());
    sub->cd();
    TList *listSub = ((TDirectory*)obj)->GetList();
    TIter it(listSub);
    while( TObject *obj1=it() ) writedir2current(obj1);
    cur->cd();
  }
}
