#include "CbmTrdClusterizer.h"

#include "CbmTrdDigiPar.h"
#include "CbmTrdPoint.h"
#include "CbmTrdDigi.h"
#include "CbmTrdDigiMatch.h"
#include "CbmTrdModule.h"
#include "CbmTrdRadiator.h"

#include "CbmMCTrack.h"

#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairBaseParSet.h"

#include "TRandom.h"
#include "TMath.h"
#include "TVector3.h"
#include "TF1.h"
#include "TH2F.h"
#include "TH1F.h"
#include "TProfile.h"
#include "TCanvas.h"
#include "TImage.h"
#include "TLine.h"
#include "TClonesArray.h"
#include "TGeoManager.h"
#include "TGeoVolume.h"
#include "TGeoMaterial.h"
#include "TGeoNode.h"
#include "TGeoMatrix.h"
#include "TGeoBBox.h"
#include "TPRegexp.h"
#if ROOT_VERSION_CODE < ROOT_VERSION(5,20,0)
#include "CbmTrdStringToken.h"
#endif

//#include "sqlite/sqlite3.h" // used for the lookup table option

#include <iostream>
#include <cmath>
using std::cout;
using std::cin;
using std::endl;
using std::pair;
using std::fabs;
// ---- Default constructor -------------------------------------------
CbmTrdClusterizer::CbmTrdClusterizer()
    :FairTask("TrdCluster")
	//:fRef(0)
{
   fDigiCollection = new TClonesArray("CbmTrdDigi");
  fEfficiency = 1;
}
// --------------------------------------------------------------------

// ---- Constructor ----------------------------------------------------
CbmTrdClusterizer::CbmTrdClusterizer(const char *name, const char *title,
                 CbmTrdRadiator *radiator)
	:FairTask(name)
{
  fRadiators = radiator;
  fEfficiency = 1;
}
// --------------------------------------------------------------------

// ---- Destructor ----------------------------------------------------
CbmTrdClusterizer::~CbmTrdClusterizer()
{
  //    FairRootManager *ioman =FairRootManager::Instance();
  //ioman->Write();
  fDigiCollection->Clear("C");
  delete fDigiCollection;
  fDigiMatchCollection->Clear("C");
  delete fDigiMatchCollection;

}
// --------------------------------------------------------------------

// ----  Initialisation  ----------------------------------------------
void CbmTrdClusterizer::SetParContainers()
{
    cout<<" * CbmTrdClusterizer * :: SetParContainers() "<<endl;


    // Get Base Container
    FairRunAna* ana = FairRunAna::Instance();
    FairRuntimeDb* rtdb=ana->GetRuntimeDb();

    fDigiPar = (CbmTrdDigiPar*)
               (rtdb->getContainer("CbmTrdDigiPar"));

}
// --------------------------------------------------------------------

// ---- ReInit  -------------------------------------------------------
InitStatus CbmTrdClusterizer::ReInit(){

  cout<<" * CbmTrdClusterizer * :: ReInit() "<<endl;


  FairRunAna* ana = FairRunAna::Instance();
  FairRuntimeDb* rtdb=ana->GetRuntimeDb();

  fDigiPar = (CbmTrdDigiPar*)
      (rtdb->getContainer("CbmTrdDigiPar"));
  
  return kSUCCESS;
}
// --------------------------------------------------------------------

// ---- Init ----------------------------------------------------------
InitStatus CbmTrdClusterizer::Init()
{

    cout<<" * CbmTrdCluster * :: Init() "<<endl;

    FairRootManager *ioman = FairRootManager::Instance();
    if ( ! ioman ) Fatal("Init", "No FairRootManager");
    
    fTrdPoints=(TClonesArray *)ioman->ActivateBranch("TrdPoint"); 
    if ( ! fTrdPoints ) {
      cout << "-W CbmTrdCluster::Init: No TrdPoints array!" << endl;
      cout << "                            Task will be inactive" << endl;
      return kERROR;
    }

    fMCStacks = (TClonesArray*)ioman->ActivateBranch("MCTrack");
  if ( ! fMCStacks ) {
      cout << "-W CbmTrdCluster::Init: No MCTrack array!" << endl;
      cout << "                            Task will be inactive" << endl;
      return kERROR;
    }

    fDigiCollection = new TClonesArray("CbmTrdDigi", 100);
    ioman->Register("TrdDigi","TRD Digis",fDigiCollection,kTRUE);

    fDigiMatchCollection = new TClonesArray("CbmTrdDigiMatch", 100);
    ioman->Register("TrdDigiMatch","TRD Digis",fDigiMatchCollection,kTRUE);

    fRadiators->Init();

    return kSUCCESS;

}
// --------------------------------------------------------------------


// ---- Exec ----------------------------------------------------------
void CbmTrdClusterizer::Exec(Option_t * option)
{
  Digicounter = 0;
  //printf("Exec...\n");
  //gStyle->SetPalette(1,0);
  CbmTrdPoint *pt=NULL;
  
  TVector3 mom;
  Double_t ELoss;
  Double_t ELossTR;     // TR energy loss for e- & e+
  Double_t ELossdEdX;   // ionization energy loss
  
  //const Bool_t Reconst = true;
  //const Bool_t Reconst = false;
  const Bool_t Histo = true;
  //const Bool_t Histo = false;
  const Bool_t TEST = true;
  //const Bool_t TEST = false;
  const Bool_t Sector = true;
  //const Bool_t Sector = false;
  
  if (Histo){
    printf("Finale Histograms are Created and Saved to Pics/*.png\n");
  }
  if (TEST){
    printf("Long path tracks are Exported to Pics/*.png\n");
  }
  
  TH1F* TRDalpha = NULL;
  TH1F* TRDbeta = NULL;
  TH1F* XMC = NULL;
  TH1F* Xreco = NULL;
  TH1F* RMC = NULL;
  TH1F* PR = NULL;
  TH1F* hDeltaX = NULL;
  TH1F* hDeltaY = NULL;
  TH1F* PadX = NULL;
  TH1F* PadY = NULL;
  TH1F* ClusterWidth = NULL;
  TH1F* Charge = NULL;
  TH1F* TestIntegration = NULL;
  TH1F* ChargedEdX = NULL;
  TH1F* ChargeTR = NULL;
  TH1F* ELossSprectra = NULL;
  TH1F* ELossdEdXSprectra = NULL;
  TH1F* ELossTRSprectra = NULL;

  TH2F* TRD1 = NULL;   
  TH2F* TRDin_out = NULL;  
  TH2F* localTRD1 = NULL;     
  TH2F* globalTRD1 = NULL;
  TH2F* localModule = NULL;
  TH2F* padTRD1 = NULL;
  TH2F* recoTRD1 = NULL;
  TH2F* padTRD2 = NULL;
  TH2F* recoTRD2 = NULL;
  TH2F* DeltaSlice = NULL;
  TH2F* PRF = NULL;

  TH2F* deltarecoTRD2 = NULL;
  TH2F* rdTRD2 = NULL;
     
  TProfile* zdTRD = NULL;
  TProfile* rdTRD = NULL;
  TProfile* deltarecoTRD1 = NULL;
  TProfile* deltarecoPad = NULL;
  TProfile* PRF2 = NULL;

  TF1 *mathieson = NULL;
  
  if (Histo)
    {
      printf("Init Histograms\n");
      TRDalpha = new TH1F("TRDalpha","acos(deltaZ(MC) / deltaX(MC)) [deg]",110,-10,100);
      TRDbeta = new TH1F("TRDbeta","acos(deltaZ(MC) / deltaY(MC)) [deg]",110,-10,100);
      XMC = new TH1F("XMC","x_mean(MC) [pad unit]",100,-1,1);
      Xreco = new TH1F("Xreco","x_mean(reco) [pad unit]",100,-1,1);
      RMC = new TH1F("RMC","r_in(MC) - r_out(MC) [mm]",200,0,100);
      PR = new TH1F("PR","x_mean(MC) - x_mean(reco) [mm]",400,-20,20);
      hDeltaX = new TH1F("DeltaX","fabs(x_in - x_out) [mm]",200,0,100);
      hDeltaY = new TH1F("DeltaY","fabs(y_in - y_out) [mm]",200,0,100);
      PadX = new TH1F("PadX","pads between x_in and x_out [pad unit]",50,0,50);
      PadY = new TH1F("PadY","pads between y_in and y_out [pad unit]",50,0,50);
      ClusterWidth = new TH1F("ClusterWidth","Cluster Width 'Charge > 0' [pad unit]",100,0,100);
      //Charge = new TH1F("ChargeSpectra","Charge Spectra [a.u.]",100,0,2);
      TestIntegration = new TH1F("IntegrSpectra","Integral Spectra [a.u.]", 100, 1E3, 4 * 1E4);
      Charge = new TH1F("ChargeSpectra","Charge Spectra [a.u.]",200,0,2);
      ChargedEdX = new TH1F("ChargedEdXSpectra","Charge dEdX Spectra [a.u.]",200,0,2);
      ChargeTR = new TH1F("ChargeTRSpectra","Charge TR Spectra [a.u.]",200,0,2);
      ELossSprectra = new TH1F("ELossSprectra","ELossSprectra",1000,0,0.002);
      ELossdEdXSprectra = new TH1F("ELossdEdXSprectra","ELossdEdX Sprectra",1000,0,0.002);
      ELossTRSprectra = new TH1F("ELossTRSprectra","ELossTR Sprectra",1000,0,0.002);

      TRD1 = new TH2F("TRD1","TRD1",1000,-10000,10000,1000,-10000,10000);
      TRD1->SetContour(99);
      TRDin_out = new TH2F("TRDin_out","TRDin_out",1000,-10000,10000,1000,-10000,10000);
      TRDin_out->SetContour(99);
      localTRD1 = new TH2F("TRDlocal","TRDlocal",1000,-1000,1000,1000,-1000,1000);
      localTRD1->SetContour(99);
      globalTRD1 = new TH2F("TRDglobal","TRDglobal",1000,-10000,10000,1000,-10000,10000);
      globalTRD1->SetContour(99);
      localModule = new TH2F("Modulelocal","Modulelocal",1000,-1000,1000,1000,-1000,1000);
      globalTRD1->SetContour(99);
      padTRD1 = new TH2F("TRDpad","x_mean(MC) vs. y_mean(MC) [mm] pad 'C'-CS",50,-10,10,2800,-70,70);
      padTRD1->SetContour(99);
      recoTRD1 = new TH2F("TRD1reco","x_mean(reco) vs y_mean(MC) [mm] pad 'C'-CS",50,-10,10,280,-70,70);
      recoTRD1->SetContour(99);
      padTRD2 = new TH2F("TRD2pad","x_mean(MC) vs. y_mean(MC) [pad units] pad 'C'-CS",100,-1,1,100,-1,1);
      padTRD2->SetContour(99);
      recoTRD2 = new TH2F("TRD2reco","x_mean(reco) vs y_mean(MC) [pad units] pad 'C'-CS",100,-1,1,100,-1,1);
      recoTRD2->SetContour(99);
      DeltaSlice = new TH2F("Delta1","x_in(MC) - x_out(MC) vs. y_in(MC) - y_out(MC) [pad units]",100,-10,10,100,-5,5);
      DeltaSlice->SetContour(99);
      deltarecoTRD2 = new TH2F("deltarecoTRD2","r_in(MC) vs. fabs(x_mean(MC) - x_mean(reco)) [mm] pad 'C'-CS", 50, 0,10000,200,0,0.5/**20*/);
      deltarecoTRD2->SetContour(99);
      PRF = new TH2F("PRF","PRF",200, -2 * 5, 2 * 5, 100, 0, 1);
      PRF->SetContour(99);
      rdTRD2 = new TH2F("rdTRD2","r_in(MC) vs. fabs(r_in(MC) - r_out(MC)) [mm]", 50, 0,10000,100,0,10);
      rdTRD2->SetContour(99);

      zdTRD = new TProfile("zdTRD","Layer+(Station-1)*4 vs. fabs(r_in(MC) - r_out(MC)) [mm]", 15, 0,15);
      rdTRD = new TProfile("rdTRD","r_in(MC) vs. fabs(r_in(MC) - r_out(MC)) [mm]", 50, 0,10000);
      deltarecoTRD1 = new TProfile("deltarecoTRD","r_in(MC) vs. fabs(x_mean(MC) - x_mean(reco)) [mm] pad 'C'-CS", 50, 0,10000);
      deltarecoPad = new TProfile("deltarecoPad","x_mean(MC) vs. (x_mean(MC) - x_mean(reco)) [pad units] pad 'C'-CS", 100, -1,1);
      PRF2 = new TProfile("PRF2","PRF2",150,-1.5 * 5,1.5 * 5);
      Float_t K_3 = 0.525;
      Float_t pW = 5;
      Float_t h = 3;
      Float_t par = 1;
      Char_t Mathieson[500];
      sprintf(Mathieson," -1. / (2. * atan(sqrt(%f))) * (atan(sqrt(%f) *tanh(3.14159265 * (-2. + sqrt(%f) ) * (%f + 2.* x * %f) / (8.* %f) )) +  atan(sqrt(%f) *  tanh(3.14159265 * (-2. + sqrt(%f) ) * (%f - 2.* x * %f) / (8.* %f) )) )",K_3,K_3,K_3,pW,par,h,K_3,K_3,pW,par,h);
      mathieson = new TF1("mathieson", Mathieson, -1.5 * pW, 1.5 * pW);
      mathieson->SetLineColor(2);
    }


  Int_t nEntries = fTrdPoints->GetEntriesFast();
  if (TEST)
    {
      //nEntries = nEntries * 10 / 100;//5;
    }
  for (int j = 0; j < nEntries ; j++ ) 
    {
      if (int(j * 10 / float(nEntries)) - int((j-1) * 10 / float(nEntries)) == 1)
	{
	  printf("   %3d \n",(int)(j * 100 / float(nEntries)));
	}

      TH2F* DeltaSlice2 = NULL;
      TH2F* Reco = NULL;
      TH2F* In = NULL;
      TH2F* Out = NULL;
      TH2F* Clusterposition = NULL;

      // if random value above fEfficency reject point
      if (gRandom->Rndm() > fEfficiency ) continue;
 
      pt = (CbmTrdPoint*) fTrdPoints->At(j);
  
      if(NULL == pt) continue;

      pt->Momentum(mom);
      fMCindex=pt->GetTrackID();

      CbmMCTrack *p= (CbmMCTrack*) fMCStacks->At(fMCindex);
      if(NULL == p) continue;

      Int_t pdgCode = p->GetPdgCode();

      fELossdEdX = pt->GetEnergyLoss();
      fELoss     = fELossdEdX;

      // Calculate TR
      // Sorry, Electrons & Positrons only
      //	if(TMath::Abs(pdgCode) == 11 && mom.Z() > 0.5){  //0.5
      if(TMath::Abs(pdgCode) == 11){ 

	fELossTR = fRadiators->GetTR(mom);

	fELoss += fELossTR;
      }
      //fELoss = 1.0;

      // Calculate point in the middle of the detector. This is
      // for safety reasons, because the point at exit is slightly
      // outside of the active volume. If one does not use a point
      // in the detector one will get a wrong volume from the
      // geomanager. Navigate to the correct volume to get all
      // necessary information about this volume
      // --------------->[cm]<---------------
      Double_t x_in   = pt->GetXIn();
      Double_t x_out  = pt->GetXOut();
      Double_t y_in   = pt->GetYIn();
      Double_t y_out  = pt->GetYOut();
      Double_t z_in   = pt->GetZIn();
      Double_t z_out  = pt->GetZOut();
      Double_t x_mean = (x_in + x_out)/2.;
      Double_t y_mean = (y_in + y_out)/2.;
      Double_t z_mean = (z_in + z_out)/2.;
      // --------------->[mm]<---------------
 
      gGeoManager->FindNode(x_mean, y_mean, z_mean);

      // Get the local point in local MC coordinates from
      // the geomanager. This coordinate system is rotated
      // if the chamber is rotated. This is corrected in 
      // GetModuleInformationFromDigiPar(..) to have a
      // the same local coordinate system in all the chambers
      const Double_t *global_point = gGeoManager->GetCurrentPoint();
      Double_t local_point[3];
      /*
	global_meanC= {x_mean, y_mean , z_mean};//[cm]      
	global_inC  = {x_in  , y_in   , z_in  };//[cm]      
	global_outC = {x_out , y_out  , z_out };//[cm]
      */
      global_meanC[0]= x_mean;//[cm]      
      global_inC[0]  = x_in  ;//[cm]      
      global_outC[0] = x_out ;//[cm]

      global_meanC[1]= y_mean;//[cm]      
      global_inC[1]  = y_in  ;//[cm]      
      global_outC[1] = y_out ;//[cm]

      global_meanC[2]= z_mean;//[cm]      
      global_inC[2]  = z_in  ;//[cm]      
      global_outC[2] = z_out ;//[cm]

      gGeoManager->MasterToLocal(global_point, local_point);
      gGeoManager->MasterToLocal(global_meanC, local_meanC);
      gGeoManager->MasterToLocal(global_inC, local_inC);
      gGeoManager->MasterToLocal(global_outC, local_outC);

      for ( Int_t idim = 0; idim < 3; idim++)
	{
	  local_meanC[idim]  *= 10; //[cm]->[mm]
	  local_inC[idim]    *= 10; //[cm]->[mm]
	  local_outC[idim]   *= 10; //[cm]->[mm]

	  global_meanC[idim] *= 10; //[cm]->[mm]
	  global_inC[idim]   *= 10; //[cm]->[mm]
	  global_outC[idim]  *= 10; //[cm]->[mm]
	}

      fModuleID = pt->GetDetectorID();

      GetModuleInformationFromDigiPar(Sector, fModuleID);

      TransformC2LL(local_meanC, local_meanLL, modulesize);
      TransformC2LL(local_inC,   local_inLL,   modulesize);
      TransformC2LL(local_outC,  local_outLL,  modulesize);

     
      fDeltax = 0.5 * fabs(local_inC[0] - local_outC[0]); //[mm]
      fDeltay = 0.5 * fabs(local_inC[1] - local_outC[1]); //[mm]

      Float_t alpha = 0;
      Float_t beta = 0;
      if (Histo)
	{
	  alpha = TMath::ACos(fabs(local_outC[2] - local_inC[2]) / fabs(local_outC[0] - local_inC[0])) * (180. / TMath::Pi());
	  beta  = TMath::ACos(fabs(local_outC[2] - local_inC[2]) / fabs(local_outC[1] - local_inC[1])) * (180. / TMath::Pi());
	}
      CalculatePixel(Sector);

      CalcPixelCoordinate(); 

      GetIntegrationArea(Histo, PadX , PadY);
      
      const Int_t nCol = (Int_t)(modulesize[0] / (int(padsize[0] * 100) / 100.0));   
      Int_t temp = 0;
      for (Int_t i = 0; i < fNoSectors; i++)
	{
	  temp += sectorrows[i];
	}
      const Int_t nRow = temp;
  
      Double_t PadChargeModule[nRow * nCol];     
      Double_t padW[nCol];
      Double_t padH[nRow];
      Int_t iSector = 0;

      Int_t secCounter = 0;
      Int_t tempRow = sectorrows[secCounter];
     
      for (Int_t iRow = 0; iRow < nRow; iRow++)
	{
	  if (iRow == tempRow)
	    {
	      secCounter++;
	      tempRow += sectorrows[secCounter];
	    }
	  padH[iRow] = GetPadHeight(secCounter);
	  for (Int_t iCol = 0; iCol < nCol; iCol++)
	    {
	      PadChargeModule[iRow * nCol + iCol] = 0.0;	      
	      padW[iCol] = padsize[0];
	    }
	}   
      
      if (TEST)
	{
	  Reco = new TH2F("Reco","Reco [mm]", (nCol) * 100, 0, (nCol), (nRow) * 100, 0, (nRow)); 
	}
      
      SplitPathSlices(Sector, Histo, TEST, DeltaSlice2, In, Out, Clusterposition, PadChargeModule, nCol, nRow, j, padW, padH, Reco, recoTRD1, recoTRD2, deltarecoTRD1, deltarecoTRD2, deltarecoPad, Xreco, PR, PRF, PRF2, TestIntegration);
 
      if (TEST)
	{
	  if (DeltaSlice2)
	    {
	      delete DeltaSlice2;
	    }
	  if (Reco)
	    {
	      delete Reco;
	    }
	  if ( In )
	    {
	      delete In;
	    }
	  if ( Out)
	    {
	      delete Out;
	    }
	  if (Clusterposition)
	    {
	      delete Clusterposition;
	    }
	}
      if (Histo)
	{
	  Double_t tempCharge = 0.0;
	  Int_t counter = 0;
	  for (Int_t iRow = 0; iRow < nRow; iRow++)
	    {
	      for (Int_t iCol = 0; iCol < nCol; iCol++)
		{
		  tempCharge +=  PadChargeModule[iRow * nCol + iCol];
		  if (PadChargeModule[iRow * nCol + iCol] > 0)
		    {
		      counter++;
		    }
		}
	    } 
	  //printf("%.1E\n",tempCharge/fELoss);
	  Double_t Rin  = sqrt(pow(global_inC[0] ,2) + pow(global_inC[1] ,2));
	  Double_t Rout = sqrt(pow(global_outC[0],2) + pow(global_outC[1],2));

	  ClusterWidth->Fill(counter);

	  ELossSprectra->Fill(fELoss);
	  Charge->Fill(tempCharge);
	  //TestIntegration->Fill(tempCharge/fELoss+5);
	  hDeltaX->Fill(fabs(local_inC[0] - local_outC[0]));
	  hDeltaY->Fill(fabs(local_inC[1] - local_outC[1]));
	  TRDalpha->Fill(alpha);
	  TRDbeta->Fill(beta);
	  TRD1->Fill(global_meanC[0], global_meanC[1]);
  
	  TRDin_out->Fill(local_inC[0],local_inC[1]);
	  TRDin_out->Fill(local_outC[0],local_outC[1]);
	  
	  rdTRD->Fill(fabs(Rin), fabs(Rin-Rout));
	  rdTRD2->Fill(Rin, fabs(Rin-Rout));

	  RMC->Fill( fabs(Rin-Rout));
	  globalTRD1->Fill(global_point[0],global_point[1]); 
	  localTRD1->Fill(local_inLL[0],local_inLL[1]);

	  if ((local_meanLL[0]+fDeltax > modulesize[0]) || (local_meanLL[1]+fDeltay > modulesize[1]))
	    {
	      localModule->Fill(local_meanLL[0]+fDeltax,local_meanLL[1]+fDeltay);
	    }
	  if((local_meanLL[0]-fDeltax < 0) || (local_meanLL[1]-fDeltay < 0))
	    {	
	      localModule->Fill(local_meanLL[0]-fDeltax,local_meanLL[1]-fDeltay);
	    }	
	  zdTRD->Fill(fLayer+(fStation-1)*4, fabs(Rin-Rout));
	  padTRD1->Fill(fPadPosxC, fPadPosyC);
	  padTRD2->Fill(fPadPosxC/padsize[0], fPadPosyC/padsize[1]);
	  DeltaSlice->Fill((global_inC[0] - global_outC[0])/padsize[0], (global_inC[1] - global_outC[1])/padsize[1]); 
	  XMC->Fill(fPadPosxC/padsize[0]);
	}
    }
  printf("Added %d TRD Digis to Collection\n%.2f Digis per Hit in Average\n",Digicounter,float(Digicounter/float(nEntries)));
  if (Histo)
    {
      Char_t Canfile1[100];
      Char_t Canfile2[100];
      sprintf(Canfile1,"Pics/C1.png");
      TCanvas *c1 = new TCanvas("c1","c1",1200,900);	
      c1->Divide(4,3);
      c1->cd(1);
      zdTRD->Draw();			 
      c1->cd(2)->SetLogy();
      TRDalpha->Draw();                      
      c1->cd(3)->SetLogy();
      TRDbeta->Draw();
      c1->cd(4);
      DeltaSlice->Draw("colz");
      c1->cd(5)->SetLogy();
      TestIntegration->Draw();
      c1->cd(6);
      padTRD2->Draw("colz");
      c1->cd(7);
      recoTRD2->Draw("colz");
      c1->cd(8);
      rdTRD->Draw();
      c1->cd(9);
      deltarecoTRD1->Draw();
      c1->cd(10);
      XMC->Draw();
      c1->cd(11);
      Xreco->Draw();
      c1->cd(12)->SetLogy();
      RMC->Draw();
      c1->Update();
      TImage *Outimage1 = TImage::Create();
      Outimage1->FromPad(c1);
      Outimage1->WriteImage(Canfile1);
     
      TCanvas *c2 = new TCanvas("c2","c2",1200,900);
      sprintf(Canfile2,"Pics/C2.png");
      c2->Divide(4,3);
      c2->cd(1)->SetLogy();
      PR->Draw();
      c2->cd(2)->SetLogy();
      PadX->Draw();
      c2->cd(3)->SetLogy();
      PadY->Draw();
      c2->cd(4)->SetLogy();
      hDeltaX->Draw();
      c2->cd(5)->SetLogy();
      hDeltaY->Draw();
      c2->cd(6);
      rdTRD2->Draw("colz");
      c2->cd(7);
      deltarecoTRD2->Draw("colz");
      c2->cd(8)->SetLogz();
      if (TEST)
	{
	  if (PRF)
	    {
	      PRF->Draw("colz");
	      PRF2->Draw("same");
	      mathieson->Draw("same");                   // TODO: implement thereotcal Mathieson function to be compared with reconstructed PRF
	    }
	  else
	    {
	      printf(" bad PRF in Draw\n");
	    }
	}
      c2->cd(9);
      localTRD1->Draw("colz");
      c2->cd(10)->SetLogy();
      ClusterWidth->Draw();   
      c2->cd(11)->SetLogy();
      Charge->Draw();
      c2->cd(12)->SetLogy();
      ELossSprectra->Draw();
      c2->Update();
      TImage *Outimage2 = TImage::Create();
      Outimage2->FromPad(c2);
      Outimage2->WriteImage(Canfile2);
      delete Outimage1;     
      delete Outimage2;

    }

  Int_t iDigi=0; 

  for ( fDigiMapIt=fDigiMap.begin() ; fDigiMapIt != fDigiMap.end(); fDigiMapIt++ ){
    if (fDigiMapIt->second->GetDetId() == 111003) {
      cout<<"Add ModID 111003 to TClonesArray"<<endl;
    }

    new ((*fDigiCollection)[iDigi]) CbmTrdDigi(fDigiMapIt->second->GetDetId(), fDigiMapIt->second->GetCol(), fDigiMapIt->second->GetRow(), fDigiMapIt->second->GetCharge(),fDigiMapIt->second->GetTime());
    ;
    CbmTrdDigiMatch *p = new ((*fDigiMatchCollection)[iDigi]) CbmTrdDigiMatch(); 
    std::vector<int> arr=fDigiMapIt->second->GetMCIndex();
    std::vector<int>::iterator it;

    for (it=arr.begin() ; it <arr.end(); it++  ) {
      Int_t bla = p->AddPoint((Int_t)*it);
    }
    iDigi++;
  }
}
   // --------------------------------------------------------------------

    // ---- FinishTask-----------------------------------------------------
    void CbmTrdClusterizer::FinishEvent()
    {
      fDigiMap.clear();
      if ( fDigiCollection ) fDigiCollection->Clear();
      if ( fDigiMatchCollection ) fDigiMatchCollection->Clear();
    }

  // --------------------------------------------------------------------
  // ---- CoordinateTransformation-----------------------------------------------------
void CbmTrdClusterizer::TransformLL2C(Double_t* LLCoordinate, Double_t* CCoordinate, Double_t* StrucDim)
{
  for (Int_t idim = 0; idim < 2; idim++)
    {
      CCoordinate[idim] = LLCoordinate[idim] - 0.5 * StrucDim[idim];
    }
}
  // --------------------------------------------------------------------
  // ---- CoordinateTransformation-----------------------------------------------------
  void CbmTrdClusterizer::TransformC2LL(Double_t* CCoordinate, Double_t* LLCoordinate, Double_t* StrucDim)
  {
  for (Int_t idim = 0; idim < 2; idim++)
    {
      LLCoordinate[idim] = CCoordinate[idim] + 0.5 * StrucDim[idim];
    }
  }

double CbmTrdClusterizer::GetPadHeight(Int_t Sector)
{
  Double_t fpadsizex; //pixel widh in x;
  Double_t fpadsizey; //pixel width in y;
  Double_t temp;
  fpadsizex    = (fModuleInfo->GetPadSizex(Sector)) * 10.; // [mm]
  fpadsizey    = (fModuleInfo->GetPadSizey(Sector)) * 10.;
  Double_t fsectorsizex = (fModuleInfo->GetSectorSizex(Sector)) * 10; // [mm]
  Double_t fsectorsizey = (fModuleInfo->GetSectorSizey(Sector)) * 10;
  if (fLayer%2 == 0) //un-rotate
    {
      temp      = fpadsizex;
      fpadsizex = fpadsizey;
      fpadsizey = temp;
    }
  return fpadsizey;
}
  // --------------------------------------------------------------------
  // ----GetModuleInformation ------------------------------------------
  void CbmTrdClusterizer::GetModuleInformation()
  {
    // Extract the information about station, layer, module type
    // and cpoy number of the module from the full path to the
    // node.
    // The full path is tokenized at the "/" which diide the different
    // levels of the geometry.
    // Knowing the nameing scheme of the volumes one gets the required
    // information with simple string manipulation.
    // This is probably not the fastes way, but the speed has to be checked.
    // The methode works only for versions of Root > 5.20.0, before the
    // class TStringTocken is not implemented

    TString path = gGeoManager->GetPath();
    cout<<"Path: "<<path<<endl;
#if ROOT_VERSION_CODE >= ROOT_VERSION(5,20,0)
    TStringToken* bla = new TStringToken(path,"/");
#else
    CbmTrdStringToken* bla = new CbmTrdStringToken(path,"/");
#endif

    while (bla->NextToken()) {
      if (bla->Contains("layer")) {
	TString bla3 = (TString) *bla;
	Ssiz_t pos = bla3.Last('_');
	Ssiz_t substringLength=bla3.Length()-pos-1;
	TString bla2 = bla3((bla3.Last('_')+1),substringLength);
	TString bla1 = bla3(3,1);
	fStation=bla1.Atoi();
	fLayer=bla2.Atoi();
      }
      if (bla->Contains("mod")){
	TString bla3 = (TString) *bla;
	Ssiz_t pos = bla3.Last('_');
	Ssiz_t substringLength=bla3.Length()-pos-1;
	TString bla2 = bla3(pos+1,substringLength);
	substringLength=pos-7;
	TString bla1 = bla3(7,substringLength);     
	fModuleType = bla1.Atoi();
	fModuleCopy = bla2.Atoi();
	break;
      } 
    }

  }
  // --------------------------------------------------------------------
  // ----GetModuleInformationFromDigiPar ------------------------------------------
void CbmTrdClusterizer::GetModuleInformationFromDigiPar(Bool_t Sector, Int_t VolumeID)
{
  // fPos is >0 for x and y and not rotated
  // origin of the local coordinate system in 
  // the lower left corner of the chamber, 
  // x to the right (small side of the pads), y up
  fModuleInfo = fDigiPar->GetModule(VolumeID);
  if (fModuleInfo != NULL)
    {
      Int_t detID = fModuleInfo->GetDetectorId();

      if (detID != VolumeID ){
	cout<<" -E- This is wrong!!!!!!!!!!!!!!!!!!!!!"<<endl;
      }

      Double_t fpadsizex; //pixel  width in x;
      Double_t fpadsizey; //pixel  width in y;
      Double_t fsizex;    //module width in x;
      Double_t fsizey;    //module width in y;
      Double_t fsectorsizex;
      Double_t fsectorsizey;
      Double_t temp = 0.0;

      Int_t* detInfo = fTrdId.GetDetectorInfo(VolumeID); 
      fStation    = detInfo[1];
      fLayer      = detInfo[2];     
      fModuleType = detInfo[3];
      fModuleCopy = detInfo[4];
      //static const Int_t fNoSectors  = (fModuleInfo->GetNoSectors);
      for (Int_t iSector = 0; iSector < fNoSectors; iSector++)
	{
	  //fSector      = (fModuleInfo->GetSector(local_inLL)  );
	  fsectorsizex = (fModuleInfo->GetSectorSizex(iSector)) * 10; // [mm]
	  fsectorsizey = (fModuleInfo->GetSectorSizey(iSector)) * 10;
	  fpadsizex    = (fModuleInfo->GetPadSizex(iSector))    * 10.; // [mm]
	  fpadsizey    = (fModuleInfo->GetPadSizey(iSector))    * 10.;
	  if (fLayer%2 == 0) //un-rotate
	    {
	      temp = fsectorsizex;
	      fsectorsizex = fsectorsizey;
	      fsectorsizey = temp;

	      temp      = fpadsizex;
	      fpadsizex = fpadsizey;
	      fpadsizey = temp;
	    }
	  sectorsize[iSector] =  fsectorsizey;
	  sectorrows[iSector] =  int(fsectorsizey / fpadsizey);
	}
      Double_t temp2 = local_inLL[1];
      for (Int_t i = 0; i < fNoSectors; i++)
	{
	  if (local_inLL[1] > sectorsize[i])
	    {
	      temp2 -= sectorsize[i];
	    }
	  else
	    {
	      fSector = i;
	    }
	}
      fpadsizex    = (fModuleInfo->GetPadSizex(fSector))    * 10.; // [mm]
      fpadsizey    = (fModuleInfo->GetPadSizey(fSector))    * 10.;
      fsizex       = (fModuleInfo->GetSizex())              * 10. * 2;
      fsizey       = (fModuleInfo->GetSizey())              * 10. * 2;
      if (fLayer%2 == 0) //un-rotate
	{
	  temp      = fpadsizex;
	  fpadsizex = fpadsizey;
	  fpadsizey = temp;

	  temp   = fsizex;
	  fsizex = fsizey;
	  fsizey = temp;
	}
      /*
	padsize    = {fpadsizex, fpadsizey, 0.};
	modulesize = {fsizex,    fsizey,    0.};
      */
      padsize[0]    = fpadsizex;
      modulesize[0] = fsizex;

      padsize[1]    = fpadsizey;
      modulesize[1] = fsizey;

      padsize[2]    = 0.;
      modulesize[2] = 0.;

    }
  else
    {
      printf("fModuleInfo == NULL\n");
    }
}
  // ---------------CalculatePixel-----------------------------------------------------
void CbmTrdClusterizer::CalculatePixel(Bool_t Sector) 
{
  // fCol(x) & fRow(y) is >0 and not rotated
  // origin of the local coordinate system in 
  // the lower left corner of the chamber, 
  // x to the right (small side of the pads), y up
  fCol_mean = GetCol(local_meanLL[0]);
  fRow_mean = GetRow(local_meanLL[1]);

  fCol_in   = GetCol(  local_inLL[0]);
  fRow_in   = GetRow(  local_inLL[1]);

  fCol_out  = GetCol( local_outLL[0]);
  fRow_out  = GetRow( local_outLL[1]);

}
  // ---------------CalculatePixelCoordinate-----------------------------------------------------
void CbmTrdClusterizer::CalcPixelCoordinate() 
{
  // fPadPosLL is >0 for x and y and not rotated
  // origin of the local coordinate system 'LL' in 
  // the 'L'ower 'L'eft corner of the chamber.
  fPadPosxLL = (local_meanLL[0] - padsize[0] * fCol_mean); //[mm]
  fPadPosyLL = (GetFloatPositionY(local_meanLL[1]) - int(GetFloatPositionY(local_meanLL[1]))) * padsize[1];//[mm]

  // fPadPosC is not rotated
  // origin of the local coordinate system 'C' is 
  // the 'C'enter of the pad
  fPadPosxC = fPadPosxLL - 0.5 * padsize[0]; //[mm]
  fPadPosyC = fPadPosyLL - 0.5 * padsize[1]; //[mm]
}
  // --------------------------------------------------------------------
  // --------------------------------------------------------------------
float CbmTrdClusterizer::GetFloatPositionY(Double_t tempPosY)
{
  Float_t floatRow = 0;
  for (Int_t iSector = 0; iSector < fNoSectors; iSector++)
    {
      if (tempPosY > sectorsize[iSector])
	{
	  tempPosY -= sectorsize[iSector];
	  floatRow += sectorrows[iSector];
	}
      else
	{
	  floatRow += tempPosY / float(GetPadHeight(iSector));
	  break;
	}
    }
  return floatRow;
}
  // --------------------------------------------------------------------
  // --------------------------------------------------------------------
int CbmTrdClusterizer::GetCol(Double_t tempPosX)/*tempPosX has to be in LL module coordinate-systems in [mm]*/
{
  Int_t iCol   = (Int_t)((tempPosX) / padsize[0]);
  return iCol;
}
  // --------------------------------------------------------------------
  // --------------------------------------------------------------------
int CbmTrdClusterizer::GetRow(Double_t tempPosY)/*tempPosY has to be in LL module coordinate-systems in [mm]*/
{
  Int_t iRow = 0;
  for (Int_t iSector = 0; iSector < fNoSectors; iSector++)
    {
      if (tempPosY > sectorsize[iSector])
	{
	  tempPosY -= sectorsize[iSector];
	  iRow += sectorrows[iSector];
	}
      else
	{
	  iRow += int(tempPosY / float(GetPadHeight(iSector)));
	  break;
	}
    }
  return iRow;
}
  // --------------------------------------------------------------------
  // --------------------------------------------------------------------
int CbmTrdClusterizer::GetSector(Double_t tempPosY)/*tempPosY has to be in LL module coordinate-systems in [mm]*/
{
   for (Int_t iSector = 0; iSector < fNoSectors; iSector++)
    {
      if (tempPosY > sectorsize[iSector])
	{
	  tempPosY -= sectorsize[iSector];
	}
      else
	{
	  return iSector;
	  break;//?? nötig ??
	}
    }
  
}
  // --------------------------------------------------------------------
  // ---- SplitPathSlices -------------------------------------------------
void CbmTrdClusterizer::SplitPathSlices(Bool_t Sector, Bool_t Histo, Bool_t TEST, TH2F* DeltaSlice2, TH2F* In, TH2F* Out, TH2F* Clusterposition, Double_t* PadChargeModule, Int_t nCol, Int_t nRow, Int_t j, Double_t* padW, Double_t* padH,TH2F* Reco, TH2F* recoTRD1, TH2F* recoTRD2, TProfile* deltarecoTRD1, TH2F*  deltarecoTRD2, TProfile* deltarecoPad, TH1F* Xreco, TH1F* PR, TH2F* PRF, TProfile* PRF2, TH1F* TestIntegration)
{
  Int_t DrawTH = 200;//15;
  Float_t ClusterDistance = 5; //[mm]
  Double_t deltaR = sqrt(pow((local_inLL[0] - local_outLL[0]),2) + pow((local_inLL[1] - local_outLL[1]),2)/* + pow((fz_in - fz_out),2)*/);
  Int_t nPathSlice = int(deltaR / ClusterDistance) + 1;                       

  Double_t W[fPadNrX];
  Double_t H[fPadNrY];

  Double_t delta[2];
  delta[0] = (local_outLL[0] - local_inLL[0]) / double(nPathSlice); //[mm]
  delta[1] = (local_outLL[1] - local_inLL[1]) / double(nPathSlice); //[mm]

  //Double_t deltaZ = (local_out[2] - local_in[2]) / double(nTimeSlice); //[mm]
  Double_t SliceELoss = fELossdEdX / float(nPathSlice);
  if (nPathSlice == 1)
    {
      SliceELoss += fELossTR;
    }
  if (nPathSlice > DrawTH)
    {
      if (TEST)
	{
	  printf("Event %d    CN %d\n",j,nPathSlice);
	  DeltaSlice2 = new TH2F("Delta","Inpact (white) Exit (black) Cluster (gray) ClusterCharge (color) [pad units]",(nCol),0,(nCol),(nRow),0,(nRow));
	  DeltaSlice2->SetContour(99);
	  Reco->SetMarkerStyle(20);
	  Reco->SetMarkerColor(7);

	  In = new TH2F("in","in [mm]", (nCol) * 100, 0, (nCol), (nRow) * 100, 0, (nRow));
	  In->SetMarkerStyle(24);
	  In->SetMarkerColor(0);
	  Out = new TH2F("out","out [mm]", (nCol) * 100, 0, (nCol), nRow * 100, 0, (nRow));
	  Out->SetMarkerStyle(24);
	  Out->SetMarkerColor(1);
	  In ->Fill(local_inLL[0] /padsize[0], GetFloatPositionY(local_inLL[1] )); 
	  Out->Fill(local_outLL[0]/padsize[0], GetFloatPositionY(local_outLL[1]));
	  Clusterposition = new TH2F("ClusterPosition","Cluster Positions [mm]", (nCol) * 100, 0, (nCol),(nRow) * 100, 0, (nRow));
	  Clusterposition->SetMarkerStyle(2);
	  Clusterposition->SetMarkerColor(16);
	}
    }
  Int_t Col_slice = 0;
  Int_t Row_slice = 0;
  Double_t ClusterMLL[2];
  //Double_t ClusterPosLL[2];//Cluster position in pad coordinates (not rotated)
  Double_t ClusterPosC[2];//Cluster position in pad coordinates (not rotated)

  for ( Int_t iPathSlice = 0; iPathSlice < nPathSlice; iPathSlice++)
    {
      if (nPathSlice > 1 && iPathSlice == 0)
	{
	  SliceELoss += fELossTR;
	}
      if (nPathSlice > 1 && iPathSlice > 0)
	{
	  SliceELoss = fELossdEdX / float(nPathSlice);
	}
      // Cluster position has to be known also in module cs to calc if cluster has traversed a pad boundery
      ClusterMLL[0]    = local_inLL[0] + (0.5 * delta[0]) + (delta[0] * iPathSlice); 
      ClusterMLL[1]    = local_inLL[1] + (0.5 * delta[1]) + (delta[1] * iPathSlice); 

      Col_slice = GetCol(ClusterMLL[0]);
      Row_slice = GetRow(ClusterMLL[1]);

      ClusterPosC[0] = (ClusterMLL[0] - padsize[0] * Col_slice) - 0.5 * padsize[0]; //[mm]
      Double_t tempClusterPosCy = ClusterMLL[1];
      Int_t tempSecRows = Row_slice;
      Int_t iSector;
      for ( iSector = 0; iSector < fNoSectors; iSector++)
	{
	  if (tempClusterPosCy > sectorsize[iSector])
	    {
	      tempClusterPosCy -= sectorsize[iSector];
	      tempSecRows -= sectorrows[iSector];
	    }
	  else
	    {
	      ClusterPosC[1] = tempClusterPosCy - tempSecRows * GetPadHeight(iSector) - 0.5 * GetPadHeight(iSector);
	      break;
	    }
	}

      if (nPathSlice > DrawTH)
	{
	  if (TEST)
	    {
	      Clusterposition->Fill(ClusterMLL[0]/padsize[0], GetFloatPositionY(ClusterMLL[1]));
	    }
	}

      GetPadSizeMatrix( H, W, padH, padW, Row_slice, Col_slice, nRow, nCol);

      CalcMathieson(TEST, ClusterPosC[0], ClusterPosC[1], SliceELoss, W ,H);

      if (Histo)
	{
	  Float_t tempCharge = 0;
	  for (Int_t iPadRow = 0; iPadRow < fPadNrY; iPadRow++)
	    { 
	      for (Int_t iPadCol = 0; iPadCol < fPadNrX; iPadCol++)		
		{ 
		  tempCharge += fPadCharge[iPadRow][iPadCol];
		}
	    }
	  TestIntegration->Fill(tempCharge/SliceELoss);
	  //printf("%.1E\n",tempCharge/SliceELoss);
	}

      ChargeConservation(Row_slice, Col_slice,  nCol, nRow);

      ClusterMapping( nCol, nRow, Col_slice, Row_slice, PadChargeModule);

    }

  for (Int_t iRow = 0; iRow < nRow; iRow++)
    {
      for (Int_t iCol = 0; iCol < nCol; iCol++)
	{
	  if (PadChargeModule[iRow * nCol + iCol] > 0)
	    {
	      AddDigi(iCol, iRow, float(PadChargeModule[iRow * nCol + iCol]));
	      Digicounter++;
	      if (nPathSlice > DrawTH)
		{
		  if(TEST)
		    {			      		
		      DeltaSlice2->SetBinContent(iCol+1,iRow+1,float(PadChargeModule[iRow * nCol + iCol]));
		    }
		}
	    }
	}
    }

  if (Histo || TEST)
    {
      CalcPRF(TEST, Histo, nRow, nCol, PadChargeModule, Reco, recoTRD1, recoTRD2, deltarecoTRD1, deltarecoTRD2, deltarecoPad, Xreco, PR, PRF, PRF2);
    }
      
  if (nPathSlice > DrawTH)
    {
      if (TEST)
	{
	  TCanvas *c = new TCanvas("c","c",900,675);	
	  c->Divide(1,1);	       
	  c->cd(1);
	  DeltaSlice2->Draw("colz");
	  In->Draw("same");
	  Out->Draw("same");
	  Clusterposition->Draw("same");
	  Reco->Draw("same");
	  Char_t Picfile[200];
	  sprintf(Picfile,"Pics/S%d__L%d__pw%.1f__ph%.1f__mw%.1f_mh%.1f__in%.2f_%.2f__out%.2f_%.2f__cn%d_EN%05d.png",fStation,fLayer,padsize[0],padsize[1],modulesize[0],modulesize[1],local_inLL[0],local_inLL[1],local_outLL[0],local_outLL[1],nPathSlice,j);
	  TImage *Padimage = TImage::Create();
	  Padimage->FromPad(c);
	  Padimage->WriteImage(Picfile);
	      
	  if (Padimage)
	    {
	      delete Padimage;
	    }
	  if (c)
	    {
	      delete c;
	    }
	  if (DeltaSlice2)
	    {
	      delete DeltaSlice2;
	    }
	  if (Reco)
	    {
	      //delete Reco;
	    }
	  if (In)
	    {
	      delete In;
	    }
	  if (Out)
	    {
	      delete Out;
	    }
	  if (Clusterposition)
	    {
	      delete Clusterposition;
	    }
	}
    }
}
  // --------------------------------------------------------------------
  // --------------------------------------------------------------------
void CbmTrdClusterizer::GetPadSizeMatrix(Double_t* H, Double_t* W, Double_t* padH, Double_t* padW, Int_t Row_slice, Int_t Col_slice, Int_t nRow, Int_t nCol)
{  
  for (Int_t iRow = 0; iRow <= fPadNrY / 2; iRow++)
    {
      if (iRow == 0)
	{
	  H[(fPadNrY / 2)] = padH[Row_slice];
	}
      else
	{
	  if (Row_slice + iRow < nRow)
	    {
	      H[(fPadNrY / 2) + iRow] = padH[Row_slice + iRow];
	    }
	  else
	    {
	      H[(fPadNrY / 2) + iRow] = H[iRow - 1];
	    }
	  if (Row_slice - iRow > 0)
	    {
	      H[(fPadNrY / 2) - iRow] = padH[Row_slice - iRow];
	    }
	  else
	    {
	      H[(fPadNrY / 2) - iRow] = H[iRow + 1];
	    }
	}
    }
  for (Int_t iCol = 0; iCol <= fPadNrX / 2; iCol++)
    {	   
      if (iCol == 0)
	{
	  W[(fPadNrX / 2)] = padW[Col_slice];
	}
      else
	{
	  if (Col_slice + iCol < nCol)
	    {   
	      W[(fPadNrX / 2) + iCol] = padW[Col_slice + iCol];
	    }
	  else
	    {
	      W[(fPadNrX / 2) + iCol] = W[iCol - 1];
	    }
	  if (Col_slice - iCol > 0)
	    {
	      W[(fPadNrX / 2) - iCol] = padW[Col_slice - iCol];
	    }
	  else
	    {
	      W[(fPadNrX / 2) - iCol] = W[iCol + 1];
	    }
	}
    }  
}
  // --------------------------------------------------------------------
  // --------------------------------------------------------------------
void CbmTrdClusterizer::ChargeConservation(Int_t Row_slice, Int_t Col_slice, Int_t nCol, Int_t nRow)
{
  /*
    the integration area of CalcMathieson() is not necessary coverd by the module size. Therefore the charge induced outside of the module borders is added to the charge on the first pads inside of the module ranges.
   */
  for (Int_t iRow = 0; iRow <= fPadNrY / 2; iRow++)
    {
      for (Int_t iCol = 0; iCol <= fPadNrX / 2; iCol++)
	{	      
	  if (Row_slice - (fPadNrY / 2) + iRow < 0)
	    {
	      fPadCharge[iRow + 1][iCol] += fPadCharge[iRow][iCol];
	      fPadCharge[iRow    ][iCol]  = 0.0;

	      fPadCharge[iRow + 1][(fPadNrX-1) - iCol] += fPadCharge[iRow][(fPadNrX-1) - iCol];
	      fPadCharge[iRow    ][(fPadNrX-1) - iCol]  = 0.0;
	    }
	      
	  if (Row_slice + (fPadNrY / 2)  - iRow > nRow)
	    {
	      fPadCharge[(fPadNrY-1) - iRow - 1][iCol] += fPadCharge[(fPadNrY-1) - iRow][iCol];
	      fPadCharge[(fPadNrY-1) - iRow    ][iCol]  = 0.0;

	      fPadCharge[(fPadNrY-1) - iRow - 1][(fPadNrX-1) - iCol] += fPadCharge[(fPadNrY) - iRow][(fPadNrX-1) - iCol];
	      fPadCharge[(fPadNrY-1) - iRow    ][(fPadNrX-1) - iCol]  = 0.0;
	    }
	      
	  if (Col_slice - (fPadNrX / 2) + iCol < 0)
	    { 
	      fPadCharge[iRow][iCol + 1] += fPadCharge[iRow][iCol];
	      fPadCharge[iRow][iCol    ]  = 0.0;
	    }
	      
	  if (Col_slice + (fPadNrX / 2) - iCol > nCol)
	    {
	      fPadCharge[iRow][(fPadNrX-1) - iCol - 1] += fPadCharge[iRow][(fPadNrX-1) - iCol];
	      fPadCharge[iRow][(fPadNrX-1) - iCol    ]  = 0.0;
	    }	      	     
	}
    }  
}
  // --------------------------------------------------------------------
  // --------------------------------------------------------------------
void CbmTrdClusterizer::ClusterMapping(Int_t nCol, Int_t nRow, Int_t Col_slice, Int_t Row_slice, Double_t* PadChargeModule)
{
  /*
    Associates the integration are used within CalcMathieson() to the pad area of the module
   */  
  for (Int_t iRow = 0; iRow < nRow; iRow++)
    {
      for (Int_t iCol = 0; iCol < nCol; iCol++)
	{	
	  if (iCol >= Col_slice - (fPadNrX / 2) && iCol <= Col_slice + (fPadNrX / 2))
	    { 		  
	      if (iRow >= Row_slice - (fPadNrY / 2) && iRow <= Row_slice + (fPadNrY / 2))
		{
		  PadChargeModule[iRow * nCol + iCol] += fPadCharge[iRow - (Row_slice - (fPadNrY / 2))][iCol - (Col_slice - (fPadNrX / 2))];
		}
	    }
	}
    }  
}
  // --------------------------------------------------------------------
  // ---- GetIntegrationArea -------------------------------------------------
  void CbmTrdClusterizer::GetIntegrationArea(Bool_t Histo, TH1F* PadX, TH1F* PadY)
  {
    /*
      First iteration of the area on which the charge of one track is induced
     */
    Int_t Padx = fabs(fCol_in - fCol_out) + 1;
    Int_t Pady = fabs(fRow_in - fRow_out) + 1;

    if (Histo)
      {
	PadX->Fill(Padx);
	PadY->Fill(Pady);
      }
  }
  // --------------------------------------------------------------------
  // ---- CalcMathieson -------------------------------------------------
void CbmTrdClusterizer::CalcMathieson( Bool_t TEST, Double_t x_mean, Double_t y_mean, Double_t SliceELoss,Double_t* W, Double_t* H)
{
  /*
    Calculates the induced charge on tha area defind by 'fPadNrX' aand 'fPadNrY'  by using the mathieson formula for each cluster
  */
  //printf("CalcMathieson...\n");
  //Int_t accuracy = 1;    // '1/accuracy' integration step width [mm]
  Float_t h = 3;         //anode-cathode gap [mm]
  Float_t s = 3;         //anode wire spacing [mm]
  Float_t r_a = 12.5E-3/2.; //anode wire radius [mm]
  Float_t q_a = 1700;    //anode neto charge [??] ???

  for (Int_t iPadRow = 0; iPadRow < fPadNrY; iPadRow++)
    {
      for (Int_t iPadCol = 0; iPadCol < fPadNrX; iPadCol++)
	{
	  fPadCharge[iPadRow][iPadCol] = 0.0;
	}
    }
  Float_t par = 1.0;   // normalization factor
  Float_t r = 0.0;     // local pad cylindrical coordinates in anode wire direction; r = sqrt(x^2+y^2) [mm]
  Float_t value = 0.0;
  Double_t rho = 0.0;   //charge at position x
  Float_t K_3 = 0.525; //Mathieson parameter for 2nd MuBu prototype -> Parametrisation for chamber parameter
  //Float_t K_3 = -0.24 * (h / s) + 0.7 + (-0.75 * log(r_a / s) - 3.64);// aproximation of 'E. Mathieson 'Cathode Charge Distributions in Multiwire Chambers' Nuclear Instruments and Methods in Physics Research A270,1988
  Float_t K_2 = 3.14159265 / 2.* ( 1. - sqrt(K_3)/2.);
  Float_t K_1 = (K_2 * sqrt(K_3)) / (4. * atan(sqrt(K_3)));
  TH2F* Test = NULL;
  TH2F* TestPos = NULL;
  TH2F* Test2 = NULL;
  
  for (Int_t iPadRow = 0; iPadRow < fPadNrY; iPadRow++)
    { 
      for (Int_t iPadCol = 0; iPadCol < fPadNrX; iPadCol++)		
	{ 
	  for (Int_t yi = 0; yi < H[iPadRow] * accuracy; yi++)
	    {
	      for (Int_t xi = 0; xi < W[iPadCol] * accuracy; xi++)
		{
		  //Mathieson coordinate system ist centered in the center of the hit pad 
		  r = sqrt(
			   pow(((iPadCol - int(fPadNrX/2)) * W[iPadCol] + (xi + 0.5) / float(accuracy) - 0.5 * W[iPadCol]) - x_mean,2) + 
			   pow(((iPadRow - int(fPadNrY/2)) * H[iPadRow] + (yi + 0.5) / float(accuracy) - 0.5 * H[iPadRow]) - y_mean,2)
			 
			   );
		  value = K_2 * r / h;
		  //rho = r;
		  //rho = (iPadRow * fPadNrX + iPadCol) / (H[iPadRow] * accuracy * W[iPadCol] * accuracy);
		  //rho = (1.) / (H[iPadRow] * accuracy * W[iPadCol] * accuracy);
		  /*
		  rho = (q_a * K_1 * (1. - tanh(K_2 * r / h) * tanh(K_2 * r / h)) /
			 (1. + K_3 * tanh(K_2 * r / h) * tanh(K_2 * r / h))) / float((accuracy * accuracy));  
		  */ 
		  rho = (q_a * K_1 * (1. - tanh(value) * tanh(value)) /
			 (1. + K_3 * tanh(value) * tanh(value))) / float((accuracy * accuracy));         
		    
		  if (rho > 0.00)
		    {
		      fPadCharge[iPadRow][iPadCol] += rho * SliceELoss;
		    }
		}
	    }
	}
    }
}
  // --------------------------------------------------------------------
  int CbmTrdClusterizer::GetPadMax(Int_t iRow, Int_t nCol, Double_t* PadChargeModule)
  {
    /*
      Find Pad with maximum induced charge per row
     */
    Int_t PadMax = -1;
    Int_t PadPlateau = 1;
    Double_t TempCharge = 0;
    for (Int_t iCol = 1; iCol < nCol; iCol++)
      {
	if (PadChargeModule[iRow * nCol + iCol] > 0)
	  {
	    if (PadChargeModule[iRow * nCol + iCol] > TempCharge )
	      {
		TempCharge = PadChargeModule[iRow * nCol + iCol];
		PadMax = iCol;
	      }
	  }
      }
    return PadMax;
  }
  // --------------------------------------------------------------------

  // ---- CalcCenterOfGravity -------------------------------------------
  void CbmTrdClusterizer::CalcCenterOfGravity()
  {
  }
  // --------------------------------------------------------------------
 
  // ---- CalcPRF------------ -------------------------------------------
void CbmTrdClusterizer::CalcPRF(Bool_t TEST, Bool_t Histo, Int_t nRow, Int_t nCol, Double_t* PadChargeModule, TH2F* Reco, TH2F* recoTRD1, TH2F* recoTRD2, TProfile* deltarecoTRD1, TH2F*  deltarecoTRD2, TProfile* deltarecoPad, TH1F* Xreco, TH1F* PR, TH2F* PRF, TProfile* PRF2)
{
  /*
    Uses the non weighted Pad Responce Function to reconstructed the cluster position for each row
   */
  if ( recoTRD1 && recoTRD2 && deltarecoTRD1 && deltarecoTRD2 && deltarecoPad && Xreco && PR && PRF && PRF2)
    {
      if (TEST)
	{
	  if(Reco==NULL)
	    {
	      printf("bad Reco! CalcPRF1\n");
	    }
	  else
	    {
	      for (Int_t iRow = 0; iRow < nRow; iRow++)
		{     
		  Int_t PadMax = GetPadMax(iRow, nCol, PadChargeModule);
		  if (PadMax > -1)
		    {
		      fPRFHitPositionC = 0.0;
		      Int_t max    = PadMax;
		      Int_t left   = PadMax - 1;
		      Int_t right  = PadMax + 1;
		      if (PadChargeModule[iRow * nCol + left]  > 0 && 
			  PadChargeModule[iRow * nCol + max]   > 0 && 
			  PadChargeModule[iRow * nCol + right] > 0)
			{
			  if (PadChargeModule[iRow * nCol + left ] != PadChargeModule[iRow * nCol + max]   && 
			      PadChargeModule[iRow * nCol + max]   != PadChargeModule[iRow * nCol + right] && 
			      PadChargeModule[iRow * nCol + right] != PadChargeModule[iRow * nCol + left] )
			    {
			      fPRFHitPositionC = 0.5 * padsize[0] * log( PadChargeModule[iRow * nCol + right] / PadChargeModule[iRow * nCol + left] ) /
				( log( PadChargeModule[iRow * nCol + max] * PadChargeModule[iRow * nCol + max]
				       / ( PadChargeModule[iRow * nCol + left] * PadChargeModule[iRow * nCol + right] )));
   
			      fPRFHitPositionLL = fPRFHitPositionC + 0.5 * padsize[0];
			    }
			  if (TEST)// is this right?????
			    {
			      if(Reco==NULL)printf("bad Reco! CalcPRF2\n");
			      else
				{
				  Reco->Fill(fPRFHitPositionLL/padsize[0]+PadMax,iRow+0.5);
				}
			    }
			  if (Histo)
			    {
			      recoTRD1->Fill(fPRFHitPositionC, fPadPosyC);
			      recoTRD2->Fill(fPRFHitPositionC/padsize[0], fPadPosyC/padsize[1]);
			      deltarecoTRD1->Fill(sqrt(pow(global_inC[0],2) + pow(global_inC[1],2)),fabs(fPRFHitPositionC-fPadPosxC));
			      deltarecoTRD2->Fill(sqrt(pow(global_inC[0],2) + pow(global_inC[1],2)),fabs(fPRFHitPositionC-fPadPosxC));
			      deltarecoPad->Fill(fPadPosxC/padsize[0],(fPRFHitPositionC-fPadPosxC)/padsize[0]);	 
			      Xreco->Fill(fPRFHitPositionC/padsize[0]);
			      PR->Fill(fPadPosxC-fPRFHitPositionC);
			      if(PRF != NULL)
				{
				  PRF->Fill(fPRFHitPositionC, PadChargeModule[iRow * nCol + max]/
					    (PadChargeModule[iRow * nCol + left] + PadChargeModule[iRow * nCol + max] + PadChargeModule[iRow * nCol + right]));
				  PRF->Fill( + 1 * padsize[0] - fPRFHitPositionC, PadChargeModule[iRow * nCol + right]/
					     (PadChargeModule[iRow * nCol + left] + PadChargeModule[iRow * nCol + max] + PadChargeModule[iRow * nCol + right]));
				  PRF->Fill( - 1 * padsize[0] - fPRFHitPositionC, PadChargeModule[iRow * nCol + left]/
					     (PadChargeModule[iRow * nCol + left] + PadChargeModule[iRow * nCol + max] + PadChargeModule[iRow * nCol + right]));

				  PRF2->Fill(fPRFHitPositionC, PadChargeModule[iRow * nCol + max]/
					     (PadChargeModule[iRow * nCol + left] + PadChargeModule[iRow * nCol + max] + PadChargeModule[iRow * nCol + right]));
				  PRF2->Fill( + 1 * padsize[0] - fPRFHitPositionC, PadChargeModule[iRow * nCol + right]/
					      (PadChargeModule[iRow * nCol + left] + PadChargeModule[iRow * nCol + max] + PadChargeModule[iRow * nCol + right]));
				  PRF2->Fill( - 1 * padsize[0] - fPRFHitPositionC, PadChargeModule[iRow * nCol + left]/
					      (PadChargeModule[iRow * nCol + left] + PadChargeModule[iRow * nCol + max] + PadChargeModule[iRow * nCol + right]));
				}
			      else 
				{
				  printf("bad PRF\n");
				}
			    }
			}
		    }
		}	  
	    }
	}
    }
  else
    {
      printf(":( CalcPRF Histos NULL\n");
    }
}

  // -------------------------------------------------------------------
  // ------AddDigi--------------------------------------------------------------

  //void CbmTrdClusterizer::AddDigi() 
  void CbmTrdClusterizer::AddDigi(Int_t iCol, Int_t iRow, Float_t iCharge) 
  {

    // Add digi for pixel(x,y) in module to map for intermediate storage
    // In case the pixel for this pixel/module combination does not exists
    // it is added to the map.
    // In case it exists already the information about another hit in this
    // pixel is added. Also the additional energy loss is added to the pixel.

  
    // Look for pixel in charge map
    //pair<Int_t, Int_t> a(fCol_mean, fRow_mean);
    pair<Int_t, Int_t> a(iCol, iRow);
    pair<Int_t, pair<Int_t,Int_t> > b(fModuleID, a);
    fDigiMapIt = fDigiMap.find(b);

    //    cout<<"DetID: "<<fModuleID<<endl;

    // Pixel not yet in map -> Add new pixel
    if ( fDigiMapIt == fDigiMap.end() ) {
      /*
      if (fModuleID == 111003) {
	cout<<"Create ModID 111003 in Map"<<endl;
      }
      */
      //  CbmTrdDigi* digi = new CbmTrdDigi(fModuleID, fCol_mean, fRow_mean, fELoss, fMCindex);
      CbmTrdDigi* digi = new CbmTrdDigi(fModuleID, iCol, iRow, iCharge,/* fTime,*/ fMCindex);
      fDigiMap[b] = digi;
    }

    // Pixel already in map -> Add charge
    else {
      /*
      if (fModuleID == 111003) {
	cout<<"Modify ModID 111003 in Map"<<endl;
      }
      */
      CbmTrdDigi* digi = (CbmTrdDigi*)fDigiMapIt->second;
      if ( ! digi ) Fatal("AddChargeToPixel", "Zero pointer in digi map!");
      digi->AddCharge(iCharge);
      digi->AddMCIndex(fMCindex);
      //if( fTime > (digi->GetTime()) ) digi->SetTime(fTime);
    }
  
  }

  // ---- Register ------------------------------------------------------
  void CbmTrdClusterizer::Register()
  {
    FairRootManager::Instance()->Register("TrdDigi","Trd Digi", fDigiCollection, kTRUE);
    FairRootManager::Instance()->Register("TrdDigiMatch","Trd Digi Match", fDigiMatchCollection, kTRUE);
  }
  // --------------------------------------------------------------------

  ClassImp(CbmTrdClusterizer)
