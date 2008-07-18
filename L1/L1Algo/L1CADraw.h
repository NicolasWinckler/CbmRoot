#ifndef L1CADraw_h
#define L1CADraw_h 1

#define DRAW

#include "CbmL1.h"
#include "CbmKF.h"

#include "TApplication.h"
#include "TStyle.h"
#include "TCanvas.h"
#include "TPad.h"
#include "TFrame.h"
#include "TMarker.h"
#include "TPolyMarker.h"
#include "TPaveText.h"
#include "TEllipse.h"
#include "TText.h"
#include "TLatex.h"
#include "TPolyLine.h"

//static TApplication *myapp;
static TCanvas *YZ, *YX, *XZ;

static bool ask = true;
//static bool ask = false;


void InitL1Draw(){

  static bool first_call_of_draw = 1;
  if( first_call_of_draw ){
    first_call_of_draw = 0;

    //myapp = new TApplication("myapp", 0, 0);
    
    gStyle->SetCanvasBorderMode(0);
    gStyle->SetCanvasBorderSize(1);
    gStyle->SetCanvasColor(0);
    
    
    YZ = new TCanvas ("YZ", "YZ Side View", -1, 0, 500, 500);
    YZ->Range(-5.0, -50.0, 105.0, 50.0);
    YZ->Draw();
    YZ->Update();
    
    XZ = new TCanvas ("XZ", "XZ Top View", -1, 500, 500, 500);
    XZ->Range(-5.0, -50.0, 105.0, 50.0);
    XZ->Draw();
    XZ->Update();
    
    YX = new TCanvas ("YX", "YX Front View (Tracks)", -506, 0, 500, 500);
    YX->Range(-50.0, -50.0, 50.0, 50.0);
    YX->Draw();
    YX->Update();    

  }
}

void DrawAsk(){
  char symbol;
  if (ask){
    std::cout << "ask>";
    do{
      std::cin.get(symbol);
      if (symbol == 'r')
	ask = false;
    } while (symbol != '\n');
    std::cout << endl;
  }
}

void Draw1(){

  L1Algo *algo = CbmL1::Instance()->algo;

  TLatex latex;
  latex.SetTextFont(132);
  latex.SetTextAlign(12);
  latex.SetTextSize(0.035);

  YZ->cd(); YZ->Clear(); latex.DrawLatex(0.0, 45.0, "YZ Side View"); YZ->Draw();  
  XZ->cd(); XZ->Clear(); latex.DrawLatex(0.0, 45.0, "XZ Top View");  XZ->Draw();
  YX->cd(); YX->Clear(); latex.DrawLatex(-45.0, 45.0, "YX Front View"); YX->Draw(); 


  int nhits = algo->vStsHits.size();
  Double_t x_poly[nhits], y_poly[nhits], z_poly[nhits];
  Double_t x_poly_fake[nhits], y_poly_fake[nhits], z_poly_fake[nhits];

  int mcolor[10] = {5, 7, 3, 8, 6, 2, 4, 1, 9, 10};

  for (int ista = algo->NStations-1; ista>=0; ista--){//  //start downstream chambers
    L1Station &st = algo->vStations[ista];
    Int_t n_poly = 0;
    Int_t n_poly_fake = 0;
    for (int ih = algo->StsHitsStartIndex[ista]; ih<=algo->StsHitsStopIndex[ista]; ih++){
      L1StsHit &h = algo->vStsHits[ih];
      int iMC = CbmL1::Instance()->vHitMCRef[ih];
      //if( (vSFlag[h.f] | vSFlagB[h.b] )&0x02 ) continue; // if used

      double x = algo->vStsStrips[h.f];
      double v = algo->vStsStripsB[h.b];
      double y = (st.yInfo.cos_phi*x + st.yInfo.sin_phi*v)[0];
      double z = st.z[0];
      if( iMC>=0 ){
	x_poly[n_poly] = x;
	y_poly[n_poly] = y;
	z_poly[n_poly] = z;
	n_poly++;
      }else{
	x_poly_fake[n_poly_fake] = x;
	y_poly_fake[n_poly_fake] = y;
	z_poly_fake[n_poly_fake] = z;
	n_poly_fake++;
      }
    }

    YZ->cd();  

    TLine *line = new TLine();
    line->SetLineColor(17);
    line->DrawLine(st.z[0], -st.Rmax[0], st.z[0], st.Rmax[0]);

    TPolyMarker *pmyz = new TPolyMarker(n_poly, z_poly, y_poly); 
    pmyz->SetMarkerColor(mcolor[ista]);
    pmyz->SetMarkerStyle(20);
    pmyz->SetMarkerSize(0.4);
    pmyz->Draw();
    
    TPolyMarker *pmyz_fake = new TPolyMarker(n_poly_fake, z_poly_fake, y_poly_fake); 
    pmyz_fake->SetMarkerColor(mcolor[ista]);
    pmyz_fake->SetMarkerStyle(20);
    pmyz_fake->SetMarkerSize(0.4);
    pmyz_fake->Draw();
    
    XZ->cd();  

    line->DrawLine(st.z[0], -st.Rmax[0], st.z[0], st.Rmax[0]);

    TPolyMarker *pmxz = new TPolyMarker(n_poly, z_poly, x_poly); 
    pmxz->SetMarkerColor(mcolor[ista]);
    pmxz->SetMarkerStyle(20);
    pmxz->SetMarkerSize(0.4);
    pmxz->Draw();
    
    TPolyMarker *pmxz_fake = new TPolyMarker(n_poly_fake, z_poly_fake, x_poly_fake); 
    pmxz_fake->SetMarkerColor(mcolor[ista]);
    pmxz_fake->SetMarkerStyle(20);
    pmxz_fake->SetMarkerSize(0.4);
    pmxz_fake->Draw();
    
    YX->cd(); 

    TEllipse *ellipse = new TEllipse(0.0, 0.0, st.Rmax[0]);
    ellipse->SetLineColor(17);
    ellipse->Draw();

    TPolyMarker *pmyx = new TPolyMarker(n_poly, x_poly, y_poly); 
    pmyx->SetMarkerColor(mcolor[ista]);
    pmyx->SetMarkerStyle(20);
    pmyx->SetMarkerSize(0.4);
    pmyx->Draw();

    TPolyMarker *pmyx_fake = new TPolyMarker(n_poly_fake, x_poly_fake, y_poly_fake); 
    pmyx_fake->SetMarkerColor(mcolor[ista]);
    pmyx_fake->SetMarkerStyle(20);
    pmyx_fake->SetMarkerSize(0.4);
    pmyx_fake->Draw();
  }

  int NRegMCTracks = 0;
  CbmL1 &L1 = *CbmL1::Instance();
  TPolyLine pline;
  pline.SetLineColor(kRed);

  for( vector<CbmL1MCTrack>::iterator it = L1.vMCTracks.begin(); it != L1.vMCTracks.end(); ++it){
    CbmL1MCTrack &T = *it;
    //draw reconstructable tracks only
    if( T.nMCContStations<4 ) continue;
    //if (( T.mother_ID< 0 )&&( T.nContStations<3 )) continue;
    //if (( T.mother_ID>=0 )&&( T.nContStations<4 )) continue;
    //if( T.p<.2 ) continue;
    if( T.p<.1 ) continue;
    double par[6];
    par[0] = T.x;
    par[1] = T.y;
    //if( fabs(T.pz)<0.1 ) continue;
    par[2] = T.px/T.pz;
    par[3] = T.py/T.pz;
    par[4] = T.q/T.p;
    par[5] = T.z;

    int npoints = T.Points.size();
    if( npoints<1 ) continue;

    vector<double> lx, ly, lz;
    lx.push_back(par[0]);
    ly.push_back(par[1]);
    lz.push_back(par[5]);

    bool ok = true;

    for( int ip=0; ip<npoints; ip++){
      CbmL1MCPoint &p = *(T.Points[ip]);
      double par1[6];
      //if( fabs(p.pz)<0.05 ) continue;
      par1[0] = p.x;
      par1[1] = p.y;
      par1[2] = p.px/p.pz;
      par1[3] = p.py/p.pz;
      par1[4] = p.q/p.p;
      par1[5] = p.z;
     
      double Zfrst = par[5];
      double Zlast = par1[5];
      double step = .5;
      if( step>fabs(Zfrst-Zlast)/5 ) step = fabs(Zfrst-Zlast)/5;
      if( Zlast<par[5] ) step = -step;
      while( fabs( par[5] - Zlast) > fabs(step) ){
	double znxt = par[5]+step;
	CbmKF::Instance()->Propagate( par1, 0, znxt, par1[4] );
	CbmKF::Instance()->Propagate( par, 0, znxt, par[4] );
	double w = fabs(znxt-Zfrst);
	double w1 = fabs(znxt-Zlast);
	if( w+w1 < 1.e-3 ){ w=1; w1=0; }
	float xl = ( w1*par[0] + w*par1[0])/(w+w1);
	float yl = ( w1*par[1] + w*par1[1])/(w+w1);
	float zl = ( w1*par[5] + w*par1[5])/(w+w1);
	if ((fabs(xl) > 50.0)||(fabs(yl) > 50.0)){
	  //cout << "*** track " << NRegMCTracks+1 << " xl = " << xl << ", zl = " << zl << endl;
	  //cout << "*** track " << NRegMCTracks+1 << " yl = " << yl << ", zl = " << zl << endl;
	  ok = false;
	  continue;
	}
	lx.push_back( ( w1*par[0] + w*par1[0])/(w+w1) );
	ly.push_back( ( w1*par[1] + w*par1[1])/(w+w1) );
	lz.push_back( ( w1*par[5] + w*par1[5])/(w+w1) );
      }

      par[0] = p.x;
      par[1] = p.y;
      par[2] = p.px/p.pz;
      par[3] = p.py/p.pz;
      par[4] = p.q/p.p;
      par[5] = p.z;
      lx.push_back(par[0]);
      ly.push_back(par[1]);
      lz.push_back(par[5]);
    }      
    
    /*
      double lx[1000], ly[1000], lz[1000];
      lx[0] = T.x;
      ly[0] = T.y;
      lz[0] = T.z;
      int nhits = T.StsHits.size();
      for( int ih=0; ih<nhits; ih++ ){
      int jh = T.StsHits[ih];
      L1StsHit &h = algo->vStsHits[jh];      
      int ista = algo->vSFlag[h.f]/4;
      L1Station &st = algo->vStations[ista];
      
      double x = algo->vStsStrips[h.f];
      double v = algo->vStsStripsB[h.b];
      double y = (st.yInfo.cos_phi*x + st.yInfo.sin_phi*v)[0];
      double z = st.z[0];
      
      lx[ih+1] = x;
      ly[ih+1] = y;
      lz[ih+1] = z;
      }
    */

    if (ok){

      NRegMCTracks++;

      YZ->cd(); 
      pline.DrawPolyLine(lx.size(), &(lz[0]), &(ly[0]) );
      XZ->cd(); 
      pline.DrawPolyLine(lx.size(), &(lz[0]), &(lx[0]) );
      YX->cd(); 
      pline.DrawPolyLine(lx.size(), &(lx[0]), &(ly[0]) );
    }
  }

  cout <<"L1CADraw: number of registered MC tracks: " << NRegMCTracks << endl;

  YZ->cd(); YZ->Update();
  XZ->cd(); XZ->Update();
  YX->cd(); YX->Update();

  YX->cd(); 
  YX->SaveAs("YXView.eps");
  YX->SaveAs("YXView.pdf");

  XZ->cd(); 
  XZ->SaveAs("XZView.eps");
  XZ->SaveAs("XZView.pdf");

  YZ->cd(); 
  YZ->SaveAs("YZView.eps");
  YZ->SaveAs("YZView.pdf");

}


#endif
