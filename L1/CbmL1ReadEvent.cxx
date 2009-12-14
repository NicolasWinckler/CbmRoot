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
 *  Read Event information to L1
 *
 *====================================================================
 */

#include "CbmL1.h"
#include "L1Algo/L1Algo.h"
#include "CbmKF.h"
#include "CbmMvdHitMatch.h"

#include "CbmStsStation.h" // only for get z position of stantions

#include <iostream>
#include <vector>

using std::cout;
using std::endl;
using std::vector;

struct TmpHitSort{
  int ID;
  double z;
  int StsHit;
  CbmL1MCPoint *MCPoint;
  bool eff;
  static bool compareIDz( const TmpHitSort &a, const TmpHitSort &b )
  {
    return ( a.ID < b.ID ) || ( ( a.ID == b.ID ) && (a.z < b.z) );
  }
};

struct TmpHit{
  int iStripF, iStripB;
  int indStripF, indStripB;
  int iSector, iStation;
  int ExtIndex;
  bool isStrip;
  double u_front, u_back;
  double x, y;
  int iMC;
  static bool Compare( const TmpHit &a, const TmpHit &b )
  {
    return ( a.iStation < b.iStation ) ||
      ( ( a.iStation == b.iStation ) && ( a.y < b.y ) );
  }  
};

struct TmpStrip{
  fscal u;
  int iStation;
  int iSector, iStrip;
  bool isStrip;
  int effIndex;
};


void CbmL1::ReadEvent()
{
  vMCPoints.clear();
  vMCTracks.clear();
  vRTracks.clear();
  vHitMCRef.clear();
  vHitStore.clear();
  algo->vStsHits.clear();
  algo->vStsStrips.clear();
  algo->vStsStripsB.clear();
  algo->vSFlag.clear();
  algo->vSFlagB.clear();

  vector<TmpHit> tmpHits;
  vector<TmpStrip> tmpStrips;
  vector<TmpStrip> tmpStripsB;

  // produce Sts hits from space points

  for(int i = 0; i < NStation; i++){
    algo->StsHitsStartIndex[i] = -1;
    algo->StsHitsStopIndex[i]  = -2;
  }
  
  // get MVD hits

  int nMvdHits=0;
  if( listMvdHits ){ 
    Int_t nEnt  = listMvdHits->GetEntries();
    for (int j=0; j <nEnt; j++ )
      {
	//CbmMvdHit *sh = (CbmMvdHit*) listMvdHits->At(j);	
	TmpHit th;
	{
	  CbmMvdHit *mh = (CbmMvdHit*) listMvdHits->At(j);
	  th.ExtIndex = -(1+j);	  
	  th.iStation = mh->GetStationNr() - 1;
	  th.iSector  = 0;
	  th.isStrip  = 0;
	  th.iStripF = j;
	  th.iStripB = -1;
	  if( th.iStripF<0 ) continue;
	  if( th.iStripF>=0 && th.iStripB>=0 ) th.isStrip  = 1;
	  if( th.iStripB <0 ) th.iStripB = th.iStripF;
  
	  TVector3 pos, err;
	  mh->Position(pos);
	  mh->PositionError(err);

	  th.x = pos.X();
	  th.y = pos.Y();
  
	  L1Station &st = algo->vStations[th.iStation];
	  th.u_front = th.x*st.frontInfo.cos_phi[0] + th.y*st.frontInfo.sin_phi[0];
	  th.u_back  = th.x* st.backInfo.cos_phi[0] + th.y* st.backInfo.sin_phi[0];
	}
	th.iMC=-1;
	int iMC = -1;
	if( listMvdHitMatches ){
	  CbmMvdHitMatch *match = (CbmMvdHitMatch*) listMvdHitMatches->At(j);
	  if( match){
	    iMC = match->GetPointId();
	  }
	}
	if( listStsPts && iMC>=0 ){
	  CbmL1MCPoint MC;
	  if( ! ReadMCPoint( &MC, iMC, 1 ) ){
	    MC.iStation = th.iStation;
	    vMCPoints.push_back( MC );
	    th.iMC = vMCPoints.size()-1;	    
	  }
	}
	//if(  h.MC_Point >=0 ) // DEBUG !!!!
	{
	  tmpHits.push_back(th);
	  nMvdHits++;
	}
      }
  }
  
  int nStsHits=0;
  if( listStsHits ){ 
    Int_t nEnt = listStsHits->GetEntries();
    //cout<<"nEnt = "<<nEnt<<endl;
    Int_t nMC = listStsPts->GetEntries();
    int negF=0;
    for (int j=0; j < nEnt; j++ )
      {
	CbmStsHit *sh = (CbmStsHit*) listStsHits->At(j);
	TmpHit th;
	{
	  CbmStsHit *mh = (CbmStsHit*) listStsHits->At(j);
	  th.ExtIndex = j;
	  th.iStation = NMvdStations + mh->GetStationNr() - 1;
	  th.iSector  = mh->GetSectorNr(); 
	  th.isStrip  = 0;
	  th.iStripF = mh->GetDigi(0);
	  th.iStripB = mh->GetDigi(1);
	  if( th.iStripF<0 ){ negF++; continue;}
	  if( th.iStripF>=0 && th.iStripB>=0 ) th.isStrip  = 1;
	  if( th.iStripB <0 ) th.iStripB = th.iStripF;
	  
	  th.iStripF += nMvdHits;
	  th.iStripB += nMvdHits;

	  TVector3 pos, err;
	  mh->Position(pos);
	  mh->PositionError(err);

	  th.x = pos.X();
	  th.y = pos.Y();
	  
	  L1Station &st = algo->vStations[th.iStation];
	  th.u_front = th.x*st.frontInfo.cos_phi[0] + th.y*st.frontInfo.sin_phi[0];
	  th.u_back  = th.x* st.backInfo.cos_phi[0] + th.y* st.backInfo.sin_phi[0];
	}
	th.iMC=-1;
	int iMC = sh->GetRefIndex();
	if( listStsPts && iMC>=0 && iMC<nMC){
	  CbmL1MCPoint MC;
	  if( ! ReadMCPoint( &MC, iMC, 0 ) ){
	    MC.iStation = th.iStation;
	    vMCPoints.push_back( MC );
	    th.iMC = vMCPoints.size()-1;	    
	  }
	}
	//if(  th.iMC >=0 ) // DEBUG !!!!
	{
	  tmpHits.push_back(th);
	  nStsHits++;
	}
      }
    //cout<<"negF="<<negF<<endl;
  }
 
  int nHits = nMvdHits + nStsHits;

  //cout<<" L1 Mvd+Sts hits :"<< nHits<<endl;

  sort( tmpHits.begin(), tmpHits.end(), TmpHit::Compare );      

  //cout<<"Making strips"<<endl;

  int NStrips = 0, NStripsB = 0;

  for ( int ih = 0; ih<nHits; ih++ ){
    TmpHit &th = tmpHits[ih];
    th.indStripF = -1;
    th.indStripB = -1;
    for( int is = 0; is<NStrips; is++ ){
      TmpStrip &s = tmpStrips[is];
      if( s.iStation!=th.iStation ||  s.iSector!=th.iSector ) continue;
      if( s.iStrip!=th.iStripF ) continue; 
      th.indStripF = is;
    }
    for( int is = 0; is<NStripsB; is++ ){
      TmpStrip &s = tmpStripsB[is];
      if( s.iStation!=th.iStation ||  s.iSector!=th.iSector ) continue;
      if( s.iStrip!=th.iStripB ) continue;
      if( fabs(s.u - th.u_back)>1.e-4 ) continue;
      th.indStripB = is;      
    }
    if( th.indStripF<0 ){
      TmpStrip tmp;
      tmp.iStation = th.iStation;
      tmp.iSector = th.iSector;
      tmp.iStrip = th.iStripF;
      tmp.u = th.u_front;
      tmp.isStrip = th.isStrip;
      tmpStrips.push_back(tmp);
      th.indStripF = NStrips++;
    }
    if( th.indStripB<0 ){
      TmpStrip tmp;
      tmp.iStation = th.iStation;
      tmp.iSector = th.iSector;
      tmp.iStrip = th.iStripB;
      tmp.isStrip = th.isStrip;
      tmp.u = th.u_back;
      tmpStripsB.push_back(tmp);
      th.indStripB = NStripsB++;
    }
  }
  
  //cout <<"N strips = "<<NStrips<<" "<<NStripsB<<endl;
  for( int i=0; i<NStrips; i++ ) tmpStrips[i].effIndex = -1;  
  for( int i=0; i<NStripsB; i++ ) tmpStripsB[i].effIndex = -1;
  

  if(1){ // space point unefficiency
    for( int i=0; i<nHits; i++ ){
      TmpHit &th = tmpHits[i];
      if( th.indStripF <0 || th.indStripF >= NStrips ) continue;
      if( th.indStripB <0 || th.indStripB >= NStripsB ) continue;
      if( th.iMC<0 ) continue;
      if( gRandom->Uniform(1)>fDetectorEfficiency ){
	tmpStrips [th.indStripF].effIndex = -2;
	tmpStripsB[th.indStripB].effIndex = -2;
      }
    }
  } // space point unefficiency
  else { // strip unefficiency

    for( int i=0; i<NStrips; i++ ){
      if( gRandom->Uniform(1)>fDetectorEfficiency ) tmpStrips[i].effIndex = -2;
    }
    
    for( int i=0; i<NStripsB; i++ ){
      TmpStrip &ts = tmpStripsB[i];
      if( ts.isStrip && gRandom->Uniform(1)>fDetectorEfficiency ) ts.effIndex = -2;
    }
  }// strip unefficiency

  Int_t NEffStrips = 0, NEffStripsB = 0;

  for( int i=0; i<NStrips; i++ ){
    TmpStrip &ts = tmpStrips[i];
    if( ts.effIndex == -1 ){
      ts.effIndex = NEffStrips++;
      char flag = ts.iStation*4;
      algo->vStsStrips.push_back(ts.u);
      algo->vSFlag.push_back(flag);
   } 
  }

  for( int i=0; i<NStripsB; i++ ){
    TmpStrip &ts = tmpStripsB[i];
    if( ts.effIndex == -1 ){
      ts.effIndex = NEffStripsB++;
      char flag = ts.iStation*4;
      algo->vStsStripsB.push_back(ts.u);
      algo->vSFlagB.push_back(flag);
    }
  }
  
  vector<int> vUnEffHitMCRef;
  vector<CbmL1HitStore> vUnEffHitStore;

  int nEffHits = 0;
  vector<float> vStsZPos_temp; // temp array for unsorted z positions of detectors segments
  vStsZPos_temp.clear();
  for( int i=0; i<nHits; i++ ){
    TmpHit &th = tmpHits[i];
 
    CbmL1HitStore s;
    s.ExtIndex = th.ExtIndex;
    s.iStation = th.iStation;
    s.x        = th.x;
    s.y        = th.y;

    if( th.indStripF <0 || th.indStripF >= NStrips ) continue;
    if( th.indStripB <0 || th.indStripB >= NStripsB ) continue;

    TmpStrip &tsF = tmpStrips [th.indStripF];
    TmpStrip &tsB = tmpStripsB[th.indStripB];
    if( tsF.effIndex <0 || tsB.effIndex <0 ){
      vUnEffHitStore.push_back(s);     
      vUnEffHitMCRef.push_back(th.iMC);
      continue;
    }

    L1StsHit h;
    h.f = tsF.effIndex;
    h.b = tsB.effIndex;
      // find and save z positions
    float z_tmp;
    int ist = th.iStation;
    if (ist < NMvdStations){
      CbmKFTube &t = CbmKF::Instance()->vMvdMaterial[ist];
      z_tmp = t.z;
    }
    else {
      CbmStsHit *mh_m = (CbmStsHit*) (CbmL1::Instance()->listStsHits->At(s.ExtIndex));
      z_tmp = mh_m->GetZ();
    }
//     h.z = z_tmp;
    
    int k;
    for (k = 0; k < vStsZPos_temp.size(); k++){
      if (vStsZPos_temp[k] == z_tmp){
        h.iz = k;
        break;
      }
    }
    if (k == vStsZPos_temp.size()){
      h.iz = vStsZPos_temp.size();
      vStsZPos_temp.push_back(z_tmp);
    }
    
      // save hit
    algo->vStsHits.push_back(h);
    
    int sta = th.iStation;
    if (algo->StsHitsStartIndex[sta] < 0) algo->StsHitsStartIndex[sta] = nEffHits;    
    algo->StsHitsStopIndex[sta] = nEffHits;
    nEffHits ++;

    vHitStore.push_back(s);
    vHitMCRef.push_back(th.iMC);
  }
  
//   if(0){ // check index z-pos befor sort
//     for (int k = 0; k < algo->vStsHits.size(); k++){
//       cout << algo->vStsHits[k].z << "  " << vStsZPos_temp[algo->vStsHits[k].iz] << endl;
//     }
//     int kk; std::cin >> kk;
//   }
  
    // sort z-pos
  vector<float> vStsZPos_temp2;
  vStsZPos_temp2.clear();
  vStsZPos_temp2.push_back(vStsZPos_temp[0]);
  vector<int> newToOldIndex;
  newToOldIndex.clear();
  newToOldIndex.push_back(0);
  for (int k = 1; k < vStsZPos_temp.size(); k++){
    vector<float>::iterator itpos = vStsZPos_temp2.begin()+1;
    vector<int>::iterator iti = newToOldIndex.begin()+1;
    for (; itpos < vStsZPos_temp2.end(); itpos++, iti++){
      if (vStsZPos_temp[k] < *itpos){
        vStsZPos_temp2.insert(itpos,vStsZPos_temp[k]);
        newToOldIndex.insert(iti,k);
        break;
      }
    }
    if (itpos == vStsZPos_temp2.end()){
      vStsZPos_temp2.push_back(vStsZPos_temp[k]);
      newToOldIndex.push_back(k);
    }
  } // k
    // save z-pos
  for (int k = 0; k < vStsZPos_temp2.size(); k++)
    algo->vStsZPos.push_back(vStsZPos_temp2[k]);
    // correct index of z-pos in hits array
  int size_nto_tmp = newToOldIndex.size();
  vector<int> oldToNewIndex;  
  oldToNewIndex.clear();
  oldToNewIndex.resize(size_nto_tmp);
  for (int k = 0; k < size_nto_tmp; k++)
    oldToNewIndex[newToOldIndex[k]] = k;
  
  int size_hs_tmp = vHitStore.size();
  for (int k = 0; k < size_hs_tmp; k++)
    algo->vStsHits[k].iz = oldToNewIndex[algo->vStsHits[k].iz];
  
//   if(0){ // check index z-pos
//     for (int k = 0; k < algo->vStsHits.size(); k++){
//       cout << algo->vStsHits[k].z << "  " << vStsZPos_temp2[algo->vStsHits[k].iz] << endl;
//     }
//   }
  if(0){ // check z-pos
    cout  << endl << endl << endl << "!!!!!!!!!!!!!!!!!!!!!Z-POS OLD!!!!!!!!!!!!!!!!" << endl << endl << endl;
    for (int k = 0; k < vStsZPos_temp.size(); k++)
      cout << vStsZPos_temp[k] << " " << k << " " << oldToNewIndex[k] << "     ";
    cout << endl;

    cout  << "Z-POS  NEW " << endl << endl << endl;
    for (int k = 0; k < vStsZPos_temp2.size(); k++)
      cout << vStsZPos_temp2[k] << " " << newToOldIndex[k] << " " << k << "     ";
    cout << endl;
  }
  
  
  //cout<<" sorting MC points.."<<endl;
  vector<TmpHitSort> vTmp;

  for ( int i=0; i<nEffHits; i++ ){
    int iMC = vHitMCRef[i];
    if( iMC < 0 ) continue;
    CbmL1MCPoint &MC = vMCPoints[iMC];
    TmpHitSort tmp;
    tmp.ID = MC.ID;
    tmp.z = MC.z;
    tmp.StsHit = i;
    tmp.MCPoint = &MC;
    tmp.eff = 1;
    vTmp.push_back(tmp);
  }

  for ( unsigned int i=0; i<vUnEffHitStore.size(); i++ ){
    int iMC = vUnEffHitMCRef[i];
    if( iMC < 0 ) continue;
    CbmL1MCPoint &MC = vMCPoints[iMC];
    TmpHitSort tmp;
    tmp.ID = MC.ID;
    tmp.z = MC.z;
    tmp.StsHit = i;
    tmp.MCPoint = &MC;
    tmp.eff = 0;
    vTmp.push_back(tmp);
  }
  
  sort( vTmp.begin(), vTmp.end(), TmpHitSort::compareIDz );
  //cout<<"Fill MC tracks.."<<endl;

  // fill MC tracks

  PrimVtx.MC_ID = 999;

  {
    CbmL1Vtx Vtxcurr;
    int nvtracks=0, nvtrackscurr=0;
    int ID = -10000;
    for ( vector<TmpHitSort>::iterator i= vTmp.begin(); i!=vTmp.end(); ++i)
      {
	if( vMCTracks.empty() || i->ID!= ID )
	  {
	    //cout<<endl;
	    ID = i->ID;
	    CbmL1MCTrack T;
	    T.ID = ID;
	    if( listMCTracks )
	      {
		T.q = i->MCPoint->q;
		T.mass = i->MCPoint->mass;
		T.pdg  = i->MCPoint->pdg;
		CbmMCTrack *MCTrack = (CbmMCTrack*) listMCTracks->At( T.ID );
		T.mother_ID = MCTrack->GetMotherId();
		TVector3 vr;
		MCTrack->GetStartVertex(vr);
		TLorentzVector vp;
		MCTrack->Get4Momentum(vp);
		T.px = vp.Px();
		T.py = vp.Py();
		T.pz = vp.Pz();
		T.x = vr.X();
		T.y = vr.Y();
		T.z = vr.Z();
		T.p = sqrt( fabs(T.px*T.px + T.py*T.py + T.pz*T.pz ));
	      }
	    vMCTracks.push_back( T );	      
	    if ( T.mother_ID ==-1 ){ // vertex track

	      if (  PrimVtx.MC_ID == 999 || fabs(T.z-Vtxcurr.MC_z)>1.e-7 ){// new vertex
		if( nvtrackscurr > nvtracks ){
		  PrimVtx = Vtxcurr;
		  nvtracks = nvtrackscurr;
		}
		Vtxcurr.MC_x  = T.x;
		Vtxcurr.MC_y  = T.y;
		Vtxcurr.MC_z  = T.z;
		Vtxcurr.MC_ID = T.mother_ID;
		nvtrackscurr = 1;
	      }else nvtrackscurr++;
	    }
	  }
	vMCTracks.back().Points.push_back(i->MCPoint);
	if( i->eff ) vMCTracks.back().StsHits.push_back(i->StsHit);
      }
    if( nvtrackscurr > nvtracks ) PrimVtx = Vtxcurr;
    
    //cout<<endl;
  } // fill MC tracks


  for( vector<CbmL1MCTrack>::iterator it = vMCTracks.begin(); it != vMCTracks.end(); ++it){
    CbmL1MCTrack &T = *it;    
    int nPoints = T.Points.size();
    T.nMCContStations = 0;  
    int istaold = -1, ncont=0;
    for( int ih=0; ih<nPoints; ih++ ){    
      CbmL1MCPoint &h = *T.Points[ih];      
      int ista = h.iStation;
      if (ista - istaold == 1) ncont++;
      else {
	if( T.nMCContStations<ncont ) T.nMCContStations=ncont;
	ncont = 1;
      }
      if (ista <= istaold ) continue; // backward direction
      istaold = ista;
    }
    if( T.nMCContStations<ncont ) T.nMCContStations=ncont;

    int nhits = T.StsHits.size();
    T.nHitContStations = 0;
    istaold = -1;
    ncont=0;
    for( int ih=0; ih<nhits; ih++ ){
      int jh = T.StsHits[ih];
      L1StsHit &h = algo->vStsHits[jh];      
      int ista = algo->vSFlag[h.f]/4; 
      if (ista - istaold == 1) ncont++;
      else {
	if( T.nHitContStations<ncont ) T.nHitContStations=ncont;
	ncont = 1;
      }
      if (ista <= istaold ) continue; // backward direction    
      istaold = ista;
    }
    if( T.nHitContStations<ncont ) T.nHitContStations=ncont;
  }
  
  if ( fVerbose && PrimVtx.MC_ID == 999 ) cout<<"No primary vertex !!!"<<endl;

}


bool CbmL1::ReadMCPoint( CbmL1MCPoint *MC, int iPoint, bool MVD )
{
  TVector3 xyzI,PI, xyzO,PO;
  Int_t mcID=-1;
  if( MVD && listMvdPts ){
    CbmMvdPoint *pt = (CbmMvdPoint*) listMvdPts->At(iPoint);
    if ( !pt ) return 1;
    pt->Position(xyzI);
    pt->Momentum(PI);
    pt->PositionOut(xyzO);
    pt->MomentumOut(PO);
    mcID = pt->GetTrackID();
  }else if( listStsPts ){
    CbmStsPoint *pt = (CbmStsPoint*) listStsPts->At(iPoint);
    if ( !pt ) return 1;
    pt->Position(xyzI);
    pt->Momentum(PI);
    pt->PositionOut(xyzO);
    pt->MomentumOut(PO);
    mcID = pt->GetTrackID();
  }
  TVector3 xyz = .5*(xyzI + xyzO );
  TVector3 P = .5*(PI + PO );
  MC->x  = xyz.X();
  MC->y  = xyz.Y();
  MC->z  = xyz.Z();
  MC->px = P.X();
  MC->py = P.Y();
  MC->pz = P.Z();  
  MC->p = sqrt( fabs( MC->px*MC->px + MC->py*MC->py + MC->pz*MC->pz ) );
  MC->ID = mcID;

  CbmMCTrack *MCTrack = (CbmMCTrack*) listMCTracks->At( MC->ID );
  if ( !MCTrack ) return 1;	  
  MC->pdg  = MCTrack->GetPdgCode();	  
  MC->mother_ID = MCTrack->GetMotherId(); 
  if ( abs(MC->pdg) >= 10000 ) return 1;
  MC->q = TDatabasePDG::Instance()->GetParticle(MC->pdg)->Charge()/3.0;
  MC->mass = TDatabasePDG::Instance()->GetParticle(MC->pdg)->Mass();      

  return 0;
}
