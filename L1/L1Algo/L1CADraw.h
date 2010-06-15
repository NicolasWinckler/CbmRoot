#ifndef L1CADraw_h
#define L1CADraw_h 1

#define DRAW

#include "L1Algo/L1Algo.h"
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

class L1AlgoDraw{
  struct Point{
    double x,y,z;
  };
 public:
  L1AlgoDraw();
  void InitL1Draw(L1Algo *algo_);

  void DrawMCTracks();
  void DrawRecoTracks();
  
  void DrawTriplets(vector <L1Triplet> &triplets, unsigned int *realIHit);
  void DrawDoublets(unsigned short int* Duplets_hits, unsigned short int*  Duplets_start, const int MaxArrSize,
                    int* StsHitsStartIndex, unsigned int *realIHit);
  void DrawDoubletsOnSta(int iSta, unsigned short int* Duplets_hits, unsigned short int*  Duplets_start, const int MaxArrSize, int* StsRestHitsStartIndex, unsigned int *realIHit);
  
  void DrawInputHits(); // draw all hits, which TF have gotten
  void DrawRestHits(int *StsRestHitsStartIndex, int *StsRestHitsStopIndex, unsigned int *realIHit); // draw only hits which leave on current iteration.
  
  void DrawInfo();
  void ClearVeiw();
  void SaveCanvas(TString name);
  void DrawAsk();
 private:
  Point GetHitCoor(int ih);

  void DrawTriplet(int il, int im, int ir);
  void DrawDoublet(int il, int ir);


  L1Algo *algo;

  vector< fscal > vStsStrips, vStsStripsB;
  vector< L1StsHit   > vStsHits;
  int StsHitsStartIndex[20], StsHitsStopIndex[20];
  
  int NStations;
  L1Station vStations[20];

  int mcolor[10]; // color for hits on i-station
  int StaColor; // color for stantions
  int hitsMStyle; // style for hits
  int fakesMStyle; // style for fakes
  
  double HitSize; // size of hits

  
  /*static*/ TCanvas *YZ, *YX, *XZ;
  bool ask;
};

L1AlgoDraw::L1AlgoDraw()
{
  
  int tmp[10] = {5, 7, 3, 8, 6, 2, 4, 1, 9, 14}; // color for hits on i-station
  for (int i = 0; i < 10; i++){
    mcolor[i] = tmp[i];
  }
  StaColor = 17;
  hitsMStyle = 20;
  fakesMStyle = 24;
      
  HitSize = 1;

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

  YX = new TCanvas ("YX", "YX Front View", -506, 0, 500, 500);
  YX->Range(-50.0, -50.0, 50.0, 50.0);
  YX->Draw();
  YX->Update();
  
  ask = true;
}

void L1AlgoDraw::InitL1Draw(L1Algo *algo_)
{
  //   algo = CbmL1::Instance()->algo;
  algo = algo_;
  vStsStrips.resize(algo->vStsStrips.size());
  for (int i = 0; i < algo->vStsStrips.size(); i++){
    vStsStrips[i] = algo->vStsStrips[i];
  }
  vStsStripsB.resize(algo->vStsStripsB.size());
  for (int i = 0; i < algo->vStsStripsB.size(); i++){
    vStsStripsB[i] = algo->vStsStripsB[i];
  }
  vStsHits.resize(algo->vStsHits.size());
  for (int i = 0; i < algo->vStsHits.size(); i++){
    vStsHits[i] = algo->vStsHits[i];
  }
  NStations = algo->NStations;
  for (int i = 0; i < NStations; i++){
    StsHitsStartIndex[i] = algo->StsHitsStartIndex[i];
    StsHitsStopIndex[i] = algo->StsHitsStopIndex[i];
    vStations[i] = algo->vStations[i];
  }
}

void L1AlgoDraw::DrawMCTracks()
{
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
    if( T.p<0.1 ) continue;
    if( T.p<0.5 ) pline.SetLineColor(kBlue);
    if( T.mother_ID != -1) pline.SetLineColor(8);
    if(( T.mother_ID != -1) && ( T.p<0.5 )) pline.SetLineColor(12);
    cout << "MC Track: p = " << T.p << "  mother_ID = " << T.mother_ID << "  PDG = " << T.pdg << endl;
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
 
    if (ok){

      NRegMCTracks++;

      YZ->cd(); 
      pline.DrawPolyLine(lx.size(), &(lz[0]), &(ly[0]) );
      XZ->cd(); 
      pline.DrawPolyLine(lx.size(), &(lz[0]), &(lx[0]) );
      YX->cd(); 
      pline.DrawPolyLine(lx.size(), &(lx[0]), &(ly[0]) );

//       YZ->cd(); YZ->Update();
//       XZ->cd(); XZ->Update();
//       YX->cd(); YX->Update();
//       DrawAsk();
    }
  }

  cout <<"L1CADraw: number of registered MC tracks: " << NRegMCTracks << endl;

  YZ->cd(); YZ->Update();
  XZ->cd(); XZ->Update();
  YX->cd(); YX->Update();

}

void L1AlgoDraw::DrawRecoTracks()
{

  int NRecTracks = 0;
//   CbmL1 &L1 = *CbmL1::Instance();
  TPolyLine pline;
  pline.SetLineColor(kBlue);

  int curRecoHit = 0;
  vector< unsigned short int > &recoHits = algo->vRecoHits;
  for( vector<L1Track>::iterator it = algo->vTracks.begin(); it != algo->vTracks.end(); ++it){
    L1Track &T = *it;

    vector<double> lx, ly, lz;

    int nHits = T.NHits;
    for( int iHit=0; iHit<nHits; iHit++){
      unsigned int ih = recoHits[curRecoHit++];
      Point p = GetHitCoor(ih);
      lx.push_back(p.x);
      ly.push_back(p.y);
      lz.push_back(p.z);
    }

    if (1){

      NRecTracks++;

      YZ->cd();
      pline.DrawPolyLine(lx.size(), &(lz[0]), &(ly[0]) );
      XZ->cd();
      pline.DrawPolyLine(lx.size(), &(lz[0]), &(lx[0]) );
      YX->cd();
      pline.DrawPolyLine(lx.size(), &(lx[0]), &(ly[0]) );
    }
  }

  cout <<"L1CADraw: number of reconstructed tracks: " << NRecTracks << endl;

  YZ->cd(); YZ->Update();
  XZ->cd(); XZ->Update();
  YX->cd(); YX->Update();
}

void L1AlgoDraw::DrawTriplets(vector <L1Triplet> &triplets, unsigned int *realIHit)
{
//   vector <L1Triplet> triplets = algo->vTriplets;
  for (int iTrip = 0; iTrip < triplets.size(); iTrip++){
    L1Triplet &trip = triplets[iTrip];

    unsigned int iLHit = trip.GetLHit();
    iLHit = realIHit[iLHit];
    unsigned int iMHit = trip.GetMHit();
    iMHit = realIHit[iMHit];
    unsigned int iRHit = trip.GetRHit();
    iRHit = realIHit[iRHit];
    
    DrawTriplet(iLHit, iMHit, iRHit);
  }

  YZ->cd(); YZ->Update();
  XZ->cd(); XZ->Update();
  YX->cd(); YX->Update();
};

void L1AlgoDraw::DrawTriplet(int il, int im, int ir)
{
  TPolyLine pline;
  pline.SetLineColor(kBlack);
  TMarker marker;
  marker.SetMarkerColor(kBlack);
  marker.SetMarkerStyle(26);
  marker.SetMarkerSize(HitSize*2);
  
  vector<double> lx, ly, lz;
  
  Point coor;
  
  coor = GetHitCoor(il);
  lx.push_back(coor.x);
  ly.push_back(coor.y);
  lz.push_back(coor.z);
  
  coor = GetHitCoor(im);
  lx.push_back(coor.x);
  ly.push_back(coor.y);
  lz.push_back(coor.z);
  
  coor = GetHitCoor(ir);
  lx.push_back(coor.x);
  ly.push_back(coor.y);
  lz.push_back(coor.z);

  const int nHits = 3;
  YZ->cd();
  pline.DrawPolyLine(nHits, &(lz[0]), &(ly[0]) );
  marker.DrawMarker(lz[nHits-1],ly[nHits-1]);
  XZ->cd();
  pline.DrawPolyLine(nHits, &(lz[0]), &(lx[0]) );
  marker.DrawMarker(lz[nHits-1],lx[nHits-1]);
  YX->cd();
  pline.DrawPolyLine(nHits, &(lx[0]), &(ly[0]) );
  marker.DrawMarker(lx[nHits-1],ly[nHits-1]);
}

void L1AlgoDraw::DrawDoublets(unsigned short int* Duplets_hits, unsigned short int*  Duplets_start, const int MaxArrSize, int* StsRestHitsStartIndex, unsigned int *realIHit)
{
  for (int iSta = 0; iSta < NStations-1; iSta++){
    const int firstHitOnSta = StsRestHitsStartIndex[iSta];
    const int firstHitOnNextSta = StsRestHitsStartIndex[iSta+1];
    unsigned short int* staDupletsHits = Duplets_hits + MaxArrSize*iSta;
    unsigned short int*  staDupletsStart = Duplets_start + MaxArrSize*iSta;
    
    for (int iRestLHit = firstHitOnSta; iRestLHit < firstHitOnNextSta; iRestLHit++){
      const int ilh = iRestLHit - firstHitOnSta;
      const int iirhFirst = staDupletsStart[ilh];
      const int iirhLast = staDupletsStart[ilh+1]-1;
      
      for (int iirh = iirhFirst; iirh <= iirhLast; iirh++){
        const int iRestRHit = staDupletsHits[iirh] + firstHitOnNextSta;
      
        const int iLHit = realIHit[iRestLHit];
        const int iRHit = realIHit[iRestRHit];
    
        DrawDoublet(iLHit, iRHit);
      }
    }
  }

  YZ->cd(); YZ->Update();
  XZ->cd(); XZ->Update();
  YX->cd(); YX->Update();
};

void L1AlgoDraw::DrawDoubletsOnSta(int iSta, unsigned short int* Duplets_hits, unsigned short int*  Duplets_start, const int MaxArrSize, int* StsRestHitsStartIndex, unsigned int *realIHit)
{
  const int firstHitOnSta = StsRestHitsStartIndex[iSta];
  const int firstHitOnNextSta = StsRestHitsStartIndex[iSta+1];
  unsigned short int* staDupletsHits = Duplets_hits + MaxArrSize*iSta;
  unsigned short int*  staDupletsStart = Duplets_start + MaxArrSize*iSta;

  for (int iRestLHit = firstHitOnSta; iRestLHit < firstHitOnNextSta; iRestLHit++){
    const int ilh = iRestLHit - firstHitOnSta;
    const int iirhFirst = staDupletsStart[ilh];
    const int iirhLast = staDupletsStart[ilh+1]-1;

    for (int iirh = iirhFirst; iirh <= iirhLast; iirh++){
      const int iRestRHit = staDupletsHits[iirh] + firstHitOnNextSta;

      const int iLHit = realIHit[iRestLHit];
      const int iRHit = realIHit[iRestRHit];

      DrawDoublet(iLHit, iRHit);
    }
  }

  YZ->cd(); YZ->Update();
  XZ->cd(); XZ->Update();
  YX->cd(); YX->Update();
};

void L1AlgoDraw::DrawDoublet(int il, int ir)
{
  TPolyLine pline;
  pline.SetLineColor(kBlue);
  TMarker marker;
  marker.SetMarkerColor(kBlue);
  marker.SetMarkerStyle(27);
  marker.SetMarkerSize(HitSize*2);
  
  vector<double> lx, ly, lz;
  
  Point coor;
  
  coor = GetHitCoor(il);
  lx.push_back(coor.x);
  ly.push_back(coor.y);
  lz.push_back(coor.z);

  coor = GetHitCoor(ir);
  lx.push_back(coor.x);
  ly.push_back(coor.y);
  lz.push_back(coor.z);

  const int nHits = 2;
  YZ->cd();
  pline.DrawPolyLine(nHits, &(lz[0]), &(ly[0]) );
  marker.DrawMarker(lz[nHits-1],ly[nHits-1]);
  XZ->cd();
  pline.DrawPolyLine(nHits, &(lz[0]), &(lx[0]) );
  marker.DrawMarker(lz[nHits-1],lx[nHits-1]);
  YX->cd();
  pline.DrawPolyLine(nHits, &(lx[0]), &(ly[0]) );
  marker.DrawMarker(lx[nHits-1],ly[nHits-1]);
}


void L1AlgoDraw::DrawInfo()
{
  cout << " vStsHits.size = " << algo->vStsHits.size() << endl;
  cout << " vRecoHits.size = " << algo->vRecoHits.size() << endl;
  cout << " vTracks.size = " << algo->vTracks.size() << endl;
}

void L1AlgoDraw::DrawInputHits()
{
  
  TLatex latex;
  latex.SetTextFont(132);
  latex.SetTextAlign(12);
  latex.SetTextSize(0.035);

  YZ->cd(); latex.DrawLatex(0.0, 45.0, "YZ Side View"); YZ->Draw();
  XZ->cd(); latex.DrawLatex(0.0, 45.0, "XZ Top View");  XZ->Draw();
  YX->cd(); latex.DrawLatex(-45.0, 45.0, "YX Front View"); YX->Draw();


  int nhits = vStsHits.size();
  Double_t x_poly[nhits], y_poly[nhits], z_poly[nhits];
  Double_t x_poly_fake[nhits], y_poly_fake[nhits], z_poly_fake[nhits];


  for (int ista = NStations-1; ista>=0; ista--){//  //start downstream chambers
    L1Station &st = vStations[ista];
    Int_t n_poly = 0;
    Int_t n_poly_fake = 0;
    for (int ih = StsHitsStartIndex[ista]; ih<=StsHitsStopIndex[ista]; ih++){
      L1StsHit &h = vStsHits[ih];
      int iMC = CbmL1::Instance()->vHitMCRef[ih];
      //if( (vSFlag[h.f] | vSFlagB[h.b] )&0x02 ) continue; // if used

      double x = vStsStrips[h.f];
      double v = vStsStripsB[h.b];
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
    line->SetLineColor(StaColor);
    line->DrawLine(st.z[0], -st.Rmax[0], st.z[0], st.Rmax[0]);

    TPolyMarker *pmyz = new TPolyMarker(n_poly, z_poly, y_poly);
    pmyz->SetMarkerColor(mcolor[ista]);
    pmyz->SetMarkerStyle(hitsMStyle);
    pmyz->SetMarkerSize(HitSize);
    pmyz->Draw();
    
    TPolyMarker *pmyz_fake = new TPolyMarker(n_poly_fake, z_poly_fake, y_poly_fake);
    pmyz_fake->SetMarkerColor(mcolor[ista]);
    pmyz_fake->SetMarkerStyle(fakesMStyle);
    pmyz_fake->SetMarkerSize(HitSize);
    pmyz_fake->Draw();
    
    XZ->cd();

    line->DrawLine(st.z[0], -st.Rmax[0], st.z[0], st.Rmax[0]);

    TPolyMarker *pmxz = new TPolyMarker(n_poly, z_poly, x_poly);
    pmxz->SetMarkerColor(mcolor[ista]);
    pmxz->SetMarkerStyle(hitsMStyle);
    pmxz->SetMarkerSize(HitSize);
    pmxz->Draw();
    
    TPolyMarker *pmxz_fake = new TPolyMarker(n_poly_fake, z_poly_fake, x_poly_fake);
    pmxz_fake->SetMarkerColor(mcolor[ista]);
    pmxz_fake->SetMarkerStyle(fakesMStyle);
    pmxz_fake->SetMarkerSize(HitSize);
    pmxz_fake->Draw();
    
    YX->cd();

    TEllipse *ellipse = new TEllipse(0.0, 0.0, st.Rmax[0]);
    ellipse->SetLineColor(StaColor);
    ellipse->SetFillStyle(0);
    ellipse->Draw();

    TPolyMarker *pmyx = new TPolyMarker(n_poly, x_poly, y_poly);
    pmyx->SetMarkerColor(mcolor[ista]);
    pmyx->SetMarkerStyle(hitsMStyle);
    pmyx->SetMarkerSize(HitSize);
    pmyx->Draw();

    TPolyMarker *pmyx_fake = new TPolyMarker(n_poly_fake, x_poly_fake, y_poly_fake);
    pmyx_fake->SetMarkerColor(mcolor[ista]);
    pmyx_fake->SetMarkerStyle(fakesMStyle);
    pmyx_fake->SetMarkerSize(HitSize);
    pmyx_fake->Draw();
  }

} // DrawInputHits

void L1AlgoDraw::DrawRestHits(int *StsRestHitsStartIndex, int *StsRestHitsStopIndex, unsigned int *realIHit)
{
  
  TLatex latex;
  latex.SetTextFont(132);
  latex.SetTextAlign(12);
  latex.SetTextSize(0.035);

  YZ->cd(); latex.DrawLatex(0.0, 45.0, "YZ Side View"); YZ->Draw();
  XZ->cd(); latex.DrawLatex(0.0, 45.0, "XZ Top View");  XZ->Draw();
  YX->cd(); latex.DrawLatex(-45.0, 45.0, "YX Front View"); YX->Draw();


  int nhits = vStsHits.size();
  Double_t x_poly[nhits], y_poly[nhits], z_poly[nhits];
  Double_t x_poly_fake[nhits], y_poly_fake[nhits], z_poly_fake[nhits];


  for (int ista = NStations-1; ista>=0; ista--){//  //start downstream chambers
    L1Station &st = vStations[ista];
    Int_t n_poly = 0;
    Int_t n_poly_fake = 0;
    for (int iRestHit = StsRestHitsStartIndex[ista]; iRestHit<=StsRestHitsStopIndex[ista]; iRestHit++){
      int ih = realIHit[iRestHit];
      L1StsHit &h = vStsHits[ih];
      int iMC = CbmL1::Instance()->vHitMCRef[ih];
      //if( (vSFlag[h.f] | vSFlagB[h.b] )&0x02 ) continue; // if used

      double x = vStsStrips[h.f];
      double v = vStsStripsB[h.b];
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
    line->SetLineColor(StaColor);
    line->DrawLine(st.z[0], -st.Rmax[0], st.z[0], st.Rmax[0]);

    TPolyMarker *pmyz = new TPolyMarker(n_poly, z_poly, y_poly);
    pmyz->SetMarkerColor(mcolor[ista]);
    pmyz->SetMarkerStyle(hitsMStyle);
    pmyz->SetMarkerSize(HitSize);
    pmyz->Draw();
    
    TPolyMarker *pmyz_fake = new TPolyMarker(n_poly_fake, z_poly_fake, y_poly_fake);
    pmyz_fake->SetMarkerColor(mcolor[ista]);
    pmyz_fake->SetMarkerStyle(fakesMStyle);
    pmyz_fake->SetMarkerSize(HitSize);
    pmyz_fake->Draw();
    
    XZ->cd();

    line->DrawLine(st.z[0], -st.Rmax[0], st.z[0], st.Rmax[0]);

    TPolyMarker *pmxz = new TPolyMarker(n_poly, z_poly, x_poly);
    pmxz->SetMarkerColor(mcolor[ista]);
    pmxz->SetMarkerStyle(hitsMStyle);
    pmxz->SetMarkerSize(HitSize);
    pmxz->Draw();
    
    TPolyMarker *pmxz_fake = new TPolyMarker(n_poly_fake, z_poly_fake, x_poly_fake);
    pmxz_fake->SetMarkerColor(mcolor[ista]);
    pmxz_fake->SetMarkerStyle(fakesMStyle);
    pmxz_fake->SetMarkerSize(HitSize);
    pmxz_fake->Draw();
    
    YX->cd();

    TEllipse *ellipse = new TEllipse(0.0, 0.0, st.Rmax[0]);
    ellipse->SetLineColor(StaColor);
    ellipse->SetFillStyle(0);
    ellipse->Draw();

    TPolyMarker *pmyx = new TPolyMarker(n_poly, x_poly, y_poly);
    pmyx->SetMarkerColor(mcolor[ista]);
    pmyx->SetMarkerStyle(hitsMStyle);
    pmyx->SetMarkerSize(HitSize);
    pmyx->Draw();

    TPolyMarker *pmyx_fake = new TPolyMarker(n_poly_fake, x_poly_fake, y_poly_fake);
    pmyx_fake->SetMarkerColor(mcolor[ista]);
    pmyx_fake->SetMarkerStyle(fakesMStyle);
    pmyx_fake->SetMarkerSize(HitSize);
    pmyx_fake->Draw();
  }

} // DrawCurHits

void L1AlgoDraw::DrawAsk()
{
  char symbol;
  if (ask){
    std::cout << "ask>";
    do{
      std::cin.get(symbol);
      if (symbol == 'r')
        ask = false;
      if (symbol == 'q')
        exit;
    } while (symbol != '\n');
    std::cout << endl;
  }
}

void L1AlgoDraw::ClearVeiw()
{
  YZ->Clear();
  XZ->Clear();
  YX->Clear();
}

L1AlgoDraw::Point L1AlgoDraw::GetHitCoor(int ih)
{
  L1StsHit &hit = vStsHits[ ih ];
        // find station
  int ista = 0;
  for (int i = 0; i < NStations; i++){
    if ( (StsHitsStartIndex[i] <= ih) && (StsHitsStopIndex[i] >= ih) ){
      ista = i;
      break;
    }
  }
  L1Station &sta = vStations[ista];
  float &x = vStsStrips[hit.f];
  float &v = vStsStripsB[hit.b];
  Point out;
  out.x = x;
  out.y = (sta.yInfo.cos_phi*x + sta.yInfo.sin_phi*v)[0];
  out.z = sta.z[0];
  return out;
};

void L1AlgoDraw::SaveCanvas(TString name)
{
  TString tmp = name;
  tmp += "YXView.pdf";
  YX->cd(); 
//   YX->SaveAs("YXView.eps");
  YX->SaveAs(tmp);

  tmp = name;
  tmp += "XZView.pdf";
  XZ->cd();
//   XZ->SaveAs("XZView.eps");
  XZ->SaveAs(tmp);

  tmp = name;
  tmp += "YZView.pdf";
  YZ->cd();
//   YZ->SaveAs("YZView.eps");
  YZ->SaveAs(tmp);
}

#endif
