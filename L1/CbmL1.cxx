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


CbmL1::CbmL1(int fStAPDataMode_, TString fStAPDataDir_)
{
  fStAPDataMode = fStAPDataMode_;
  fStAPDataDir = fStAPDataDir_;
  if( !fInstance ) fInstance = this;
  fTrackingLevel = 2;
  //fMomentumCutOff = 0.2;
  fMomentumCutOff = 0.1;
  fGhostSuppression = 1;
  fDetectorEfficiency = 1.;
}

CbmL1::CbmL1(const char *name, Int_t iVerbose, int fStAPDataMode_, TString fStAPDataDir_):FairTask(name,iVerbose)
{
  fStAPDataMode = fStAPDataMode_;
  fStAPDataDir = fStAPDataDir_;
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

  listStsPts = (TClonesArray *)  fManger->GetObject("STSPoint");
  listStsHits = (TClonesArray *)  fManger->GetObject("STSHit");
  listMCTracks = (TClonesArray*) fManger->GetObject("MCTrack");  

  if( !fUseMVD ){
    listMvdPts = 0;
    listMvdHits = 0;
    listMvdHitMatches = 0;
  } else {
    listMvdPts = (TClonesArray *)  fManger->GetObject("MVDPoint");
    listMvdHits = (TClonesArray *)  fManger->GetObject("MVDHit");
    listMvdHitMatches = (TClonesArray *)  fManger->GetObject("MVDHitMatch");
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
  geo[ind++] = NMvdStations;

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

  {
    if(fStAPDataMode%2 == 1){ // 1,3
      WriteStAPGeoData( (void*)geo, ind);
    };
    if(fStAPDataMode >= 2){ // 2,3
      int ind2; 
      ReadStAPGeoData( (void*)geo, ind2); 
      if (ind2 != ind)  cout << "-E- CbmL1: Read geometry from file " << fStAPDataDir + "geo_algo.txt was NOT successful." << endl;
    };
  }
  
  algo->Init(geo);
  return kSUCCESS;
}



void CbmL1::WriteStAPGeoData(void* geo_, int size)
{
  fscal *geo = (fscal*) geo_;
    // write geo in file
  TString fgeo_name = fStAPDataDir + "geo_algo.txt";
  ofstream fgeo(fgeo_name);
  for (int i = 0; i < size; i++){
    fgeo << geo[i] << endl;
    cout << geo[i] << endl;
  };
  fgeo.close();
  cout << "-I- CbmL1: Geometry data has been written in " << fgeo_name << endl;
}

void CbmL1::WriteStAPAlgoData()  // must be called after ReadEvent
{
    // write algo data in file
  static int vNEvent = 1; 
  fstream fadata;
  
  TString fadata_name = fStAPDataDir + "data_algo.txt";
//    if ( vNEvent <= maxNEvent ) {
  if ( 1 ) {
 
    if (vNEvent == 1)
      fadata.open(fadata_name,fstream::out);  // begin new file
    else
      fadata.open(fadata_name,fstream::out | fstream::app);
        
    fadata << "Event:" << " ";
    fadata << vNEvent << endl;
      // write vStsStrips
    int n = algo->vStsStrips.size();  // number of elements
    fadata << n << endl;  
    for (int i = 0; i < n; i++){
      fadata << algo->vStsStrips[i] << endl;
    };
      // write vStsStripsB
    n = algo->vStsStripsB.size();
    fadata << n << endl;  
    for (int i = 0; i < n; i++){
      fadata << algo->vStsStripsB[i] << endl;
    };    
      // write vStsZPos
    n = algo->vStsZPos.size();
    fadata << n << endl;  
    for (int i = 0; i < n; i++){
      fadata << algo->vStsZPos[i] << endl;
    };    
      // write vSFlag
    n = algo->vSFlag.size();
    fadata << n << endl;  
    unsigned char element;
    for (int i = 0; i < n; i++){
      element = algo->vSFlag[i];
      fadata << (int)element << endl;
    };
      // write vSFlagB
    n = algo->vSFlagB.size();
    fadata << n << endl;  
    for (int i = 0; i < n; i++){
      element = algo->vSFlagB[i];
      fadata << (int)element << endl;
    };
      // write vStsHits
    n = algo->vStsHits.size();
    fadata << n << endl;
    unsigned short int element2;
    char element3;
    for (int i = 0; i < n; i++){
      element2 = algo->vStsHits[i].f;
      fadata  << (int)element2 << " "; 
      element2 = algo->vStsHits[i].b;
      fadata  << (int)element2 << " ";
      element3 = algo->vStsHits[i].iz;
      fadata  << (int)element3 << endl;
    };
      // write StsHitsStartIndex and StsHitsStopIndex
    n = 20;
    for (int i = 0; i < n; i++){
      fadata  << algo->StsHitsStartIndex[i] << endl;
    };
    for (int i = 0; i < n; i++){
      fadata  << algo->StsHitsStopIndex[i] << endl;
    };
    
    fadata.close();
  }
  cout << "-I- CbmL1: CATrackFinder data for event number " << vNEvent << " have been written in file " << fadata_name << endl;
  vNEvent++;    
}

void CbmL1::WriteStAPPerfData()
{
  fstream fpdata;

  static int vNEvent = 1; 

  TString fpdata_name = fStAPDataDir + "data_perfo.txt";
        // write data for performance in file
  //   if ( vNEvent <= maxNEvent )  {
  if ( 1 ) {

    if (vNEvent == 1)
      fpdata.open(fpdata_name,fstream::out);  // begin new file
    else
      fpdata.open(fpdata_name,fstream::out | fstream::app);
  
    fpdata << "Event: " ;
    fpdata << vNEvent << endl;
      // write vMCPoints
    Int_t n = vMCPoints.size();  // number of elements
    fpdata << n << endl;  
    for (int i = 0; i < n; i++){
      fpdata << vMCPoints[i].x << " ";
      fpdata << vMCPoints[i].y << " ";
      fpdata << vMCPoints[i].z << "  ";
      fpdata << vMCPoints[i].px << " ";
      fpdata << vMCPoints[i].py << " ";
      fpdata << vMCPoints[i].pz << " ";
      fpdata << vMCPoints[i].p << "  ";
      fpdata << vMCPoints[i].q << " ";
      fpdata << vMCPoints[i].mass << "   ";
      
      fpdata << vMCPoints[i].pdg << " ";
      fpdata << vMCPoints[i].ID << " ";
      fpdata << vMCPoints[i].mother_ID << " ";
      fpdata << vMCPoints[i].iStation << endl;
    };
    
          // write vMCTracks  . without Points
    n = vMCTracks.size();  // number of elements
    fpdata << n << endl;  
    for (int i = 0; i < n; i++){
      fpdata << vMCTracks[i].x << " ";
      fpdata << vMCTracks[i].y << " ";
      fpdata << vMCTracks[i].z << "  ";
      fpdata << vMCTracks[i].px << " ";
      fpdata << vMCTracks[i].py << " ";
      fpdata << vMCTracks[i].pz << " ";
      fpdata << vMCTracks[i].p << "  ";
      fpdata << vMCTracks[i].q << " ";
      fpdata << vMCTracks[i].mass << "   ";
      
      fpdata << vMCTracks[i].pdg << " ";
      fpdata << vMCTracks[i].ID << " ";
      fpdata << vMCTracks[i].mother_ID << endl;
      
      int nhits = vMCTracks[i].StsHits.size();
      fpdata  << "   " << nhits << endl << "   ";
      for (int k = 0; k < nhits; k++){
        fpdata << vMCTracks[i].StsHits[k] << " ";
      };
      fpdata << endl;

      fpdata << vMCTracks[i].nMCContStations << " ";
      fpdata << vMCTracks[i].nHitContStations << endl;
    };
    
        // write vHitMCRef
    n = vHitMCRef.size();  // number of elements
    fpdata << n << endl;  
    for (int i = 0; i < n; i++){
      fpdata << vHitMCRef[i] << endl;
    };
        
      // write vHitStore
    n = vHitStore.size();  // number of elements
    fpdata << n << endl;  
    for (int i = 0; i < n; i++){
      fpdata << vHitStore[i].ExtIndex << "  ";
      fpdata << vHitStore[i].iStation << "  ";
      
      fpdata << vHitStore[i].x << " ";
      fpdata << vHitStore[i].y << endl;
    };

    fpdata.close();
  }
  cout << "-I- CbmL1: Data for performance of event number " << vNEvent << " have been written in file " << fpdata_name << endl;
  vNEvent++;  
    
}


void CbmL1::ReadStAPGeoData(void* geo_, int &size)
{
  
  fscal *geo = (fscal*)geo_;
  TString fgeo_name = fStAPDataDir + "geo_algo.txt";
  ifstream fgeo(fgeo_name);

  cout << "-I- CbmL1: Read geometry from file " << fgeo_name << endl;
  int i;
  for (int i = 0; !fgeo.eof(); i++){
    fgeo >> geo[i];
  };
  size = i;
  fgeo.close();

}

void CbmL1::ReadStAPAlgoData()
{
  static int nEvent = 1; 
  static fstream fadata;
  TString fadata_name = fStAPDataDir + "data_algo.txt";
//  if (nEvent <= maxNEvent){
  if (1){ 
    if ( nEvent == 1 ){
      fadata.open(fadata_name,fstream::in);
    };
    
    algo->vStsHits.clear();
    algo->vStsStrips.clear();
    algo->vStsStripsB.clear();
    algo->vStsZPos.clear();
    algo->vSFlag.clear();
    algo->vSFlagB.clear();
    
      // check correct position in file
    char s[] = "Event:  ";
    int nEv;
    fadata >> s;
    fadata >> nEv;
    if (nEv != nEvent)  cout << "-E- CbmL1: Can't read event number " << nEvent << " from file " << fadata_name << endl;
    
    int n;  // number of elements
      // read algo->vStsStrips
    fadata >> n; 
    for (int i = 0; i < n; i++){
      fscal element;
      fadata >> element;
      algo->vStsStrips.push_back(element);
    };
      // read algo->vStsStripsB
    fadata >> n; 
    for (int i = 0; i < n; i++){
      fscal element;
      fadata >> element;
      algo->vStsStripsB.push_back(element);
    };
      // read algo->vStsZPos
    fadata >> n; 
    for (int i = 0; i < n; i++){
      fscal element;
      fadata >> element;
      algo->vStsZPos.push_back(element);
    };
      // read algo->vSFlag
    fadata >> n; 
    for (int i = 0; i < n; i++){
      int element;
      fadata >> element;
      algo->vSFlag.push_back((unsigned char)element);
    };    
      // read algo->vSFlagB
    fadata >> n; 
    for (int i = 0; i < n; i++){
      int element;
      fadata >> element;
      algo->vSFlagB.push_back((unsigned char)element);
    };
      // read algo->vStsHits
    fadata >> n; 
    int element_f;  // for convert
    int element_b;
    int element_iz;
    for (int i = 0; i < n; i++){
      L1StsHit element;
      fadata >> element_f >> element_b >> element_iz;
      element.f = (unsigned short int) element_f;
      element.b = (unsigned short int) element_b;
      element.iz = (char) element_iz;
      algo->vStsHits.push_back(element);
    };
      // read StsHitsStartIndex and StsHitsStopIndex
    n = 20;
    for (int i = 0; i < n; i++){
      fadata >> algo->StsHitsStartIndex[i];
    };
    for (int i = 0; i < n; i++){
      fadata >> algo->StsHitsStopIndex[i];
    };

    cout << nEvent << "-I- CbmL1: CATrackFinder data has been read from file " << fadata_name << " successfully." << endl;
//    if (nEvent == maxNEvent) fadata.close();  
  }
  nEvent++;
};

void CbmL1::ReadStAPPerfData()
{
  static int nEvent = 1; 
  static fstream fpdata;
  TString fpdata_name = fStAPDataDir + "data_perfo.txt";
//  if (nEvent <= maxNEvent){
  if (1){   
    if ( nEvent == 1 ){
      fpdata.open(fpdata_name,fstream::in);
    };

    vMCPoints.clear();
    vMCTracks.clear();
    vHitMCRef.clear();
    vHitStore.clear();

      // check if it is right position in file
    char s[] = "EVENT:     ";  // buffer
    int nEv=0;                // event number
    fpdata >> s;
    fpdata >> nEv;  
    if (nEv != nEvent)  cout << "-E- CbmL1: Performance: can't read event number " << nEvent << " from file " << "data_perfo.txt" << endl;
      // vMCPoints
    int n;  // number of elements
    fpdata >> n;
    for (int i = 0; i < n; i++){
      CbmL1MCPoint element;
  
      fpdata >> element.x; 
      fpdata >> element.y;
      fpdata >> element.z;
      fpdata >> element.px;
      fpdata >> element.py;
      fpdata >> element.pz;
      fpdata >> element.p;
      fpdata >> element.q;
      fpdata >> element.mass;
      
      fpdata >> element.pdg;
      fpdata >> element.ID;
      fpdata >> element.mother_ID;
      fpdata >> element.iStation; 
      vMCPoints.push_back(element);
    };
    
      // vMCTracks . without Points
    fpdata >> n;  
    for (int i = 0; i < n; i++){
      CbmL1MCTrack element;
  
      fpdata >> element.x;        
      fpdata >> element.y;        
      fpdata >> element.z;        
      fpdata >> element.px;       
      fpdata >> element.py;       
      fpdata >> element.pz;       
      fpdata >> element.p;        
      fpdata >> element.q;        
      fpdata >> element.mass;     
      
      fpdata >> element.pdg;      
      fpdata >> element.ID;       
      fpdata >> element.mother_ID;
      
      int nhits;
      fpdata >> nhits;    
      for (int k = 0; k < nhits; k++){
        int helement;
        fpdata >> helement;
        element.StsHits.push_back(helement);
      };
      
      fpdata >> element.nMCContStations; 
      fpdata >> element.nHitContStations; 
      vMCTracks.push_back(element);
    };
        
        // find the corrspondece between MCPoint MCTrack
    
    for (unsigned k = 0; k < vMCPoints.size(); k++){
          // find track with ID
      int itr = 0;
      for (;vMCTracks[itr].ID != vMCPoints[k].ID;itr++); 
      vMCTracks[itr].Points.push_back(&vMCPoints[k]);
    };
    
        // vHitMCRef
    fpdata >> n;
    for (int i = 0; i < n; i++){
      int element;
      fpdata >> element; 
      vHitMCRef.push_back(element);
    };

      // vHitStore
    fpdata >> n;                  
    for (int i = 0; i < n; i++){
      CbmL1HitStore element;
      fpdata >> element.ExtIndex; 
      fpdata >> element.iStation; 
      
      fpdata >> element.x;  
      fpdata >> element.y; 
      vHitStore.push_back(element);
//  cout <<   element.ExtIndex << " "<<   element.iStation << " "<<   element.x << " "<<  element.y << " " <<  endl;  
    };
      
//    if (nEvent == maxNEvent) { // file open on begin of all work class and close at end
//       fpdata.close(); 
//       cout << " -I- Performance: data read from file " << "data_perfo.txt" << " successfully"<< endl;
//     }
    cout << nEvent << "-I- CbmL1: event data has been read from file " << fpdata_name << " successfully." << endl;

  } // if (nEvent <= maxNEvent)
  nEvent++;
};



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
