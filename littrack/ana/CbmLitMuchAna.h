 
// -------------------------------------------------------------------------
// -----                   CbmLitMuchAna header file                   -----
// -----                  Created 01/10/07  by A. Lebedev              -----
// -----                                                               -----
// -------------------------------------------------------------------------


/** CbmLitMuchAna
 *@author A.Lebedev <Andrey.Lebedev@gsi.de>
 **
 **
 **/

#ifndef CBMLITMUCHANA_H
#define CBMLITMUCHANA_H 1

#include "CbmTask.h"

#include <vector>

class CbmLitTrackUpdate;
class CbmLitTrackExtrapolator;
class CbmLitTrackPropagator;
class CbmLitTrackFitter;
class CbmLitTrackParam;
class CbmMCPoint;
class CbmMCTrack;
class TClonesArray;
class TH1F;

class CbmLitMuchAna : public CbmTask
{

public:

   CbmLitMuchAna(); 
   virtual ~CbmLitMuchAna();

   virtual InitStatus Init();

   virtual void Exec(Option_t* opt);

   virtual void Finish();

   virtual void SetParContainers();
  
private:

      // Arrays of MC information 
   TClonesArray* fMCTrackArray;
   TClonesArray* fMCPointArray;
   TClonesArray* fMuchHits;
   TClonesArray* fMuchTracks;
   TClonesArray* fMuchDigis;
   TClonesArray* fMuchDigiMatches;
   
   // Propagation and filter tools
   CbmLitTrackExtrapolator *fExtrapolator;
   CbmLitTrackPropagator *fPropagator;
   CbmLitTrackUpdate *fFilter;
   CbmLitTrackFitter *fFitter;
   CbmLitTrackFitter *fSmoother; 
      
   Int_t fNofLayers;
   
   std::vector<TH1F *> fh_srx;
   std::vector<TH1F *> fh_sry;
   
   std::vector<TH1F *> fh_resx;
   std::vector<TH1F *> fh_resy;
   std::vector<TH1F *> fh_restx;
   std::vector<TH1F *> fh_resty;
   std::vector<TH1F *> fh_resqp;
   
   std::vector<TH1F *> fh_pullx;
   std::vector<TH1F *> fh_pully;
   std::vector<TH1F *> fh_pulltx;
   std::vector<TH1F *> fh_pullty;
   std::vector<TH1F *> fh_pullqp;
   
   std::vector<TH1F *> fh_resp;
   
   TH1F* fh_pullx_last, *fh_pullx_first;
   TH1F* fh_pully_last, *fh_pully_first;
   TH1F* fh_pulltx_last, *fh_pulltx_first;
   TH1F* fh_pullty_last, *fh_pullty_first;
   TH1F* fh_pullqp_last, *fh_pullqp_first;

   TH1F* fh_resx_last, *fh_resx_first;
   TH1F* fh_resy_last, *fh_resy_first;
   TH1F* fh_restx_last, *fh_restx_first;
   TH1F* fh_resty_last, *fh_resty_first;
   TH1F* fh_resqp_last, *fh_resqp_first;
   
   TH1F* fh_resp_last, *fh_resp_first;
   
   Int_t fEvents;
   
   Int_t fVerbose;
   
   void FillParam(
		   const CbmMCPoint* point,
		   CbmLitTrackParam* par);
   
   void CreateHistograms();
   
   void CalcResAndPull(
   		const CbmMCTrack* mcTrack,
   		const CbmMCPoint* point,
   		const CbmLitTrackParam* par,
   		std::vector<Double_t>& res,
   		std::vector<Double_t>& pull,
   		Double_t& resp);
                  
   ClassDef(CbmLitMuchAna,1);

};

#endif
