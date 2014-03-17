/** @file CbmTofSimpClusterizer.cxx
 ** @author Pierre-Alain Loizeau <loizeau@physi.uni-heidelberg.de>
 ** @date 23.08.2013
 **/

#include "CbmTofSimpClusterizer.h"

// TOF Classes and includes
#include "CbmTofPoint.h"      // in cbmdata/tof
#include "CbmTofDigi.h"       // in cbmdata/tof
#include "CbmTofDigiExp.h"    // in cbmdata/tof
#include "CbmTofHit.h"        // in cbmdata/tof
#include "CbmTofGeoHandler.h" // in tof/TofTools
#include "CbmTofDetectorId_v12b.h" // in cbmdata/tof
#include "CbmTofCell.h"       // in tof/TofData
#include "CbmTofDigiPar.h"    // in tof/TofParam
#include "CbmTofDigiBdfPar.h" // in tof/TofParam
#include "CbmTofAddress.h"    // in cbmdata/tof

// CBMroot classes and includes
#include "CbmMCTrack.h"
#include "CbmMatch.h"

// FAIR classes and includes
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairLogger.h"

// ROOT Classes and includes
#include "TClonesArray.h"
#include "TMath.h"
#include "TLine.h"
#include "TRandom3.h"
#include "TF2.h"
#include "TVector3.h"
#include "TH1.h"
#include "TH2.h"
#include "TDirectory.h"
#include "TROOT.h"

// C++ Classes and includes

/************************************************************************************/
CbmTofSimpClusterizer::CbmTofSimpClusterizer():
   FairTask("CbmTofSimpClusterizer"),
   fGeoHandler(new CbmTofGeoHandler()),
   fTofId(NULL),
   fDigiPar(NULL),
   fChannelInfo(NULL),
   fDigiBdfPar(NULL),
   fTofPointsColl(NULL),
   fMcTracksColl(NULL),
   fTofDigisColl(NULL),
   fbWriteHitsInOut(kTRUE),
   fTofHitsColl(NULL),
   fiNbHits(0),
   fVerbose(1),
   fStorDigi(),
   fStorDigiExp(),
   fviClusterSize(),
   fviTrkMul(),
   fvdX(),
   fvdY(),
   fvdDifX(),
   fvdDifY(),
   fvdDifCh(),
   fhClustBuildTime(NULL),
   fhHitsPerTracks(NULL),
   fhPtsPerHit(NULL),
   fhTimeResSingHits(NULL),
   fhTimeResSingHitsB(NULL),
   fhTimePtVsHits(NULL),
   fhClusterSize(NULL),
   fhClusterSizeType(NULL),
   fhTrackMul(NULL),
   fhClusterSizeMulti(NULL),
   fhTrk1MulPos(NULL),
   fhHiTrkMulPos(NULL),
   fhAllTrkMulPos(NULL),
   fhMultiTrkProbPos(NULL),
   fhDigSpacDifClust(NULL),
   fhDigTimeDifClust(NULL),
   fhDigDistClust(NULL),
   fhClustSizeDifX(NULL),
   fhClustSizeDifY(NULL),
   fhChDifDifX(NULL),
   fhChDifDifY(NULL),
   fiNbSameSide(0),
   fhNbSameSide(NULL),
   fhNbDigiPerChan(NULL),
   fStart(),
   fStop()
{
}

CbmTofSimpClusterizer::CbmTofSimpClusterizer(const char *name, Int_t verbose, Bool_t writeDataInOut):
   FairTask(TString(name),verbose),
   fGeoHandler(new CbmTofGeoHandler()),
   fTofId(NULL),
   fDigiPar(NULL),
   fChannelInfo(NULL),
   fDigiBdfPar(NULL),
   fTofPointsColl(NULL),
   fMcTracksColl(NULL),
   fTofDigisColl(NULL),
   fbWriteHitsInOut(writeDataInOut),
   fTofHitsColl(NULL),
   fiNbHits(0),
   fVerbose(verbose),
   fStorDigi(),
   fStorDigiExp(),
   fviClusterSize(),
   fviTrkMul(),
   fvdX(),
   fvdY(),
   fvdDifX(),
   fvdDifY(),
   fvdDifCh(),
   fhClustBuildTime(NULL),
   fhHitsPerTracks(NULL),
   fhPtsPerHit(NULL),
   fhTimeResSingHits(NULL),
   fhTimeResSingHitsB(NULL),
   fhTimePtVsHits(NULL),
   fhClusterSize(NULL),
   fhClusterSizeType(NULL),
   fhTrackMul(NULL),
   fhClusterSizeMulti(NULL),
   fhTrk1MulPos(NULL),
   fhHiTrkMulPos(NULL),
   fhAllTrkMulPos(NULL),
   fhMultiTrkProbPos(NULL),
   fhDigSpacDifClust(NULL),
   fhDigTimeDifClust(NULL),
   fhDigDistClust(NULL),
   fhClustSizeDifX(NULL),
   fhClustSizeDifY(NULL),
   fhChDifDifX(NULL),
   fhChDifDifY(NULL),
   fiNbSameSide(0),
   fhNbSameSide(NULL),
   fhNbDigiPerChan(NULL),
   fStart(),
   fStop()
{
}

CbmTofSimpClusterizer::~CbmTofSimpClusterizer()
{
   if( fGeoHandler )
      delete fGeoHandler;
//   DeleteHistos(); // <-- if needed  ?
}

/************************************************************************************/
// FairTasks inherited functions
InitStatus CbmTofSimpClusterizer::Init()
{
   if( kFALSE == RegisterInputs() )
      return kFATAL;

   if( kFALSE == RegisterOutputs() )
      return kFATAL;

   if( kFALSE == InitParameters() )
      return kFATAL;

   if( kFALSE == LoadGeometry() )
      return kFATAL;

   if( kFALSE == CreateHistos() )
      return kFATAL;

   return kSUCCESS;
}

void CbmTofSimpClusterizer::SetParContainers()
{
   LOG(INFO)<<" CbmTofSimpClusterizer => Get the digi parameters for tof"<<FairLogger::endl;

   // Get Base Container
   FairRunAna* ana = FairRunAna::Instance();
   FairRuntimeDb* rtdb=ana->GetRuntimeDb();

   fDigiPar = (CbmTofDigiPar*)
              (rtdb->getContainer("CbmTofDigiPar"));

   fDigiBdfPar = (CbmTofDigiBdfPar*)
              (rtdb->getContainer("CbmTofDigiBdfPar"));
}

void CbmTofSimpClusterizer::Exec(Option_t * option)
{
   fTofHitsColl->Clear("C");

   fiNbHits = 0;

   LOG(DEBUG)<<" CbmTofSimpClusterizer => New event"<<FairLogger::endl;
   fStart.Set();

   BuildClusters();

   fStop.Set();

   FillHistos();
}

void CbmTofSimpClusterizer::Finish()
{
   WriteHistos();
   // Prevent them from being sucked in by the CbmHadronAnalysis WriteHistograms method
   DeleteHistos();
}

/************************************************************************************/
// Functions common for all clusters approximations
Bool_t   CbmTofSimpClusterizer::RegisterInputs()
{
   FairRootManager *fManager = FairRootManager::Instance();

   fTofPointsColl  = (TClonesArray *) fManager->GetObject("TofPoint");
   if( NULL == fTofPointsColl)
   {
      LOG(ERROR)<<"CbmTofSimpClusterizer::RegisterInputs => Could not get the TofPoint TClonesArray!!!"<<FairLogger::endl;
      return kFALSE;
   } // if( NULL == fTofPointsColl)

   fMcTracksColl   = (TClonesArray *) fManager->GetObject("MCTrack");
   if( NULL == fMcTracksColl)
   {
      LOG(ERROR)<<"CbmTofSimpClusterizer::RegisterInputs => Could not get the MCTrack TClonesArray!!!"<<FairLogger::endl;
      return kFALSE;
   } // if( NULL == fMcTracksColl)

   fTofDigisColl   = (TClonesArray *) fManager->GetObject("TofDigi");
   if( NULL == fTofDigisColl)
   {
      LOG(ERROR)<<"CbmTofSimpClusterizer::RegisterInputs => Could not get the TofDigi TClonesArray!!!"<<FairLogger::endl;
      return kFALSE;
   } // if( NULL == fTofDigisColl)

   return kTRUE;
}
Bool_t   CbmTofSimpClusterizer::RegisterOutputs()
{
   FairRootManager* rootMgr = FairRootManager::Instance();

   fTofHitsColl = new TClonesArray("CbmTofHit");

   // Flag check to control whether digis are written in ouput root file
   rootMgr->Register( "TofHit","Tof", fTofHitsColl, fbWriteHitsInOut);
   return kTRUE;
}
Bool_t   CbmTofSimpClusterizer::InitParameters()
{
   // Initialize the TOF GeoHandler
   Bool_t isSimulation=kFALSE;
   Int_t iGeoVersion = fGeoHandler->Init(isSimulation);
   if( k12b > iGeoVersion )
   {
      LOG(ERROR)<<"CbmTofSimpClusterizer::InitParameters => Only compatible with geometries after v12b !!!"
                <<FairLogger::endl;
      return kFALSE;
   }

   fTofId = new CbmTofDetectorId_v12b();

   return kTRUE;
}
/************************************************************************************/
Bool_t   CbmTofSimpClusterizer::LoadGeometry()
{
   Int_t iNbSmTypes = fDigiBdfPar->GetNbSmTypes();
   if( kTRUE == fDigiBdfPar->UseExpandedDigi() )
   {
      fStorDigiExp.resize( iNbSmTypes );
      fviClusterSize.resize( iNbSmTypes );
      fviTrkMul.resize( iNbSmTypes );
      fvdX.resize( iNbSmTypes );
      fvdY.resize( iNbSmTypes );
      fvdDifX.resize( iNbSmTypes );
      fvdDifY.resize( iNbSmTypes );
      fvdDifCh.resize( iNbSmTypes );
      for( Int_t iSmType = 0; iSmType < iNbSmTypes; iSmType++ )
      {
         Int_t iNbSm  = fDigiBdfPar->GetNbSm(  iSmType);
         Int_t iNbRpc = fDigiBdfPar->GetNbRpc( iSmType);
//         fStorDigiExp[iSmType].resize( iNbSm );
         fStorDigiExp[iSmType].resize( iNbSm*iNbRpc );
         fviClusterSize[iSmType].resize( iNbRpc );
         fviTrkMul[iSmType].resize( iNbRpc );
         fvdX[iSmType].resize( iNbRpc );
         fvdY[iSmType].resize( iNbRpc );
         fvdDifX[iSmType].resize( iNbRpc );
         fvdDifY[iSmType].resize( iNbRpc );
         fvdDifCh[iSmType].resize( iNbRpc );
         for( Int_t iSm = 0; iSm < iNbSm; iSm++ )
         {
//            fStorDigiExp[iSmType][iSm].resize( iNbRpc );
            for( Int_t iRpc = 0; iRpc < iNbRpc; iRpc++ )
            {
               Int_t iNbChan = fDigiBdfPar->GetNbChan( iSmType, iRpc );
//               fStorDigiExp[iSmType][iSm][iRpc].resize( iNbChan );
               fStorDigiExp[iSmType][iSm*iNbRpc+iRpc].resize( iNbChan );
            } // for( Int_t iRpc = 0; iRpc < iNbRpc; iRpc++ )
         } // for( Int_t iSm = 0; iSm < iNbSm; iSm++ )
      } // for( Int_t iSmType = 0; iSmType < iNbSmTypes; iSmType++ )
   } // if( kTRUE == fDigiBdfPar->UseExpandedDigi() )
      else
      {
         fStorDigi.resize( iNbSmTypes );
         fviClusterSize.resize( iNbSmTypes );
         fviTrkMul.resize( iNbSmTypes );
         fvdX.resize( iNbSmTypes );
         fvdY.resize( iNbSmTypes );
         fvdDifX.resize( iNbSmTypes );
         fvdDifY.resize( iNbSmTypes );
         fvdDifCh.resize( iNbSmTypes );
         for( Int_t iSmType = 0; iSmType < iNbSmTypes; iSmType++ )
         {
            Int_t iNbSm  = fDigiBdfPar->GetNbSm(  iSmType);
            Int_t iNbRpc = fDigiBdfPar->GetNbRpc( iSmType);
//            fStorDigi[iSmType].resize( iNbSm );
            fStorDigi[iSmType].resize( iNbSm*iNbRpc );
            fviClusterSize[iSmType].resize( iNbRpc );
            fviTrkMul[iSmType].resize( iNbRpc );
            fvdX[iSmType].resize( iNbRpc );
            fvdY[iSmType].resize( iNbRpc );
            fvdDifX[iSmType].resize( iNbRpc );
            fvdDifY[iSmType].resize( iNbRpc );
            fvdDifCh[iSmType].resize( iNbRpc );
            for( Int_t iSm = 0; iSm < iNbSm; iSm++ )
            {
//               fStorDigi[iSmType][iSm].resize( iNbRpc );
               for( Int_t iRpc = 0; iRpc < iNbRpc; iRpc++ )
               {
                  Int_t iNbChan = fDigiBdfPar->GetNbChan( iSmType, iRpc );
                  fStorDigi[iSmType][iSm*iNbRpc+iRpc].resize( iNbChan );
//                  fStorDigi[iSmType][iSm][iRpc].resize( iNbChan );
               } // for( Int_t iRpc = 0; iRpc < iNbRpc; iRpc++ )
            } // for( Int_t iSm = 0; iSm < iNbSm; iSm++ )
         } // for( Int_t iSmType = 0; iSmType < iNbSmTypes; iSmType++ )
      } // else of if( kTRUE == fDigiBdfPar->UseExpandedDigi() )
   return kTRUE;
}
Bool_t   CbmTofSimpClusterizer::DeleteGeometry()
{
   Int_t iNbSmTypes = fDigiBdfPar->GetNbSmTypes();
   if( kTRUE == fDigiBdfPar->UseExpandedDigi() )
   {
      for( Int_t iSmType = 0; iSmType < iNbSmTypes; iSmType++ )
      {
         Int_t iNbSm  = fDigiBdfPar->GetNbSm(  iSmType);
         Int_t iNbRpc = fDigiBdfPar->GetNbRpc( iSmType);
         for( Int_t iSm = 0; iSm < iNbSm; iSm++ )
         {
            for( Int_t iRpc = 0; iRpc < iNbRpc; iRpc++ )
//               fStorDigiExp[iSmType][iSm][iRpc].clear();
               fStorDigiExp[iSmType][iSm*iNbRpc+iRpc].clear();
//            fStorDigiExp[iSmType][iSm].clear();
         } // for( Int_t iSm = 0; iSm < iNbSm; iSm++ )
         fStorDigiExp[iSmType].clear();
      } // for( Int_t iSmType = 0; iSmType < iNbSmTypes; iSmType++ )
      fStorDigiExp.clear();
   } // if( kTRUE == fDigiBdfPar->UseExpandedDigi() )
      else
      {
         for( Int_t iSmType = 0; iSmType < iNbSmTypes; iSmType++ )
         {
            Int_t iNbSm  = fDigiBdfPar->GetNbSm(  iSmType);
            Int_t iNbRpc = fDigiBdfPar->GetNbRpc( iSmType);
            for( Int_t iSm = 0; iSm < iNbSm; iSm++ )
            {
               for( Int_t iRpc = 0; iRpc < iNbRpc; iRpc++ )
//                  fStorDigi[iSmType][iSm][iRpc].clear();
                  fStorDigi[iSmType][iSm*iNbRpc+iRpc].clear();
//               fStorDigi[iSmType][iSm].clear();
            } // for( Int_t iSm = 0; iSm < iNbSm; iSm++ )
            fStorDigi[iSmType].clear();
         } // for( Int_t iSmType = 0; iSmType < iNbSmTypes; iSmType++ )
         fStorDigi.clear();
      } // else of if( kTRUE == fDigiBdfPar->UseExpandedDigi() )
   return kTRUE;
}
/************************************************************************************/
// Histogramming functions
Bool_t   CbmTofSimpClusterizer::CreateHistos()
{
   TDirectory * oldir = gDirectory; // <= To prevent histos from being sucked in by the param file of the TRootManager!
   gROOT->cd(); // <= To prevent histos from being sucked in by the param file of the TRootManager !
   fhClustBuildTime = new TH1I( "TofSimpClus_ClustBuildTime", "Time needed to build clusters in each event; Time [s]",
                                  4000, 0.0, 4.0 );
   fhHitsPerTracks = new TH1I( "TofSimpClus_TofHitPerTrk", "Mean Number of TofHit per Mc Track; Nb TofHits/Nb MC Tracks []",
         2000, 0.0, 20.0 );
   if( kFALSE == fDigiBdfPar->ClustUseTrackId() )
      fhPtsPerHit = new TH1I( "TofSimpClus_TofPtsPerHit", "Distribution of the Number of MCPoints associated to each TofHit; Nb MCPoint []",
         20, 0.0, 20.0 );
   if( kTRUE == fDigiBdfPar->ClustUseTrackId() )
   {
      fhTimeResSingHits = new TH1I( "TofSimpClus_TofTimeResClust", "Time resolution for TofHits containing Digis from a single MC Track; t(1st Mc Point) -tTofHit [ns]",
            10000, -25.0, 25.0 );
      fhTimeResSingHitsB = new TH2I( "TofSimpClus_TofTimeResClustB", "Time resolution for TofHits containing Digis from a single MC Track; (1st Mc Point) -tTofHit [ns]",
            5000, -25.0, 25.0,
            6, 0, 6);
      fhTimePtVsHits = new TH2I( "TofSimpClus_TofTimePtVsHit", "Time resolution for TofHits containing Digis from a single MC Track; t(1st Mc Point) -tTofHit [ps]",
            2000, 0.0, 50000.0,
            2000, 0.0, 50000.0 );
   }
      else
      {
         fhTimeResSingHits = new TH1I( "TofSimpClus_TofTimeResClust", "Time resolution for TofHits containing Digis from a single TofPoint; tMcPoint -tTofHit [ns]",
               10000, -25.0, 25.0 );
         fhTimeResSingHitsB = new TH2I( "TofSimpClus_TofTimeResClustB", "Time resolution for TofHits containing Digis from a single TofPoint; tMcPoint -tTofHit [ns]",
               5000, -25.0, 25.0,
               6, 0, 6);
         fhTimePtVsHits = new TH2I( "TofSimpClus_TofTimePtVsHit", "Time resolution for TofHits containing Digis from a single TofPoint; tMcPoint -tTofHit [ps]",
               2000, 0.0, 50000.0,
               2000, 0.0, 50000.0 );
      } // else of if( kTRUE == fDigiBdfPar->ClustUseTrackId() )
   fhClusterSize = new TH1I( "TofSimpClus_ClusterSize", "Cluster Size distribution; Cluster Size [Strips]",
         100, 0.5, 100.5 );
   fhClusterSizeType = new TH2I( "TofSimpClus_ClusterSizeType", "Cluster Size distribution in each (SM type, Rpc) pair; Cluster Size [Strips]; 10*SM Type + Rpc Index []",
         100, 0.5, 100.5,
         40*fDigiBdfPar->GetNbSmTypes(), 0.0, 40*fDigiBdfPar->GetNbSmTypes() );
   if( kTRUE == fDigiBdfPar->ClustUseTrackId() )
   {
      fhTrackMul = new TH1I( "TofSimpClus_TrackMul", "Number of MC tracks generating the cluster; MC Tracks multiplicity []",
            100, 0.5, 100.5 );
      fhClusterSizeMulti = new TH2I( "TofSimpClus_ClusterSizeMulti",
            "Cluster Size distribution as function of Number of MC tracks generating the cluster; Cluster Size [Strips]; MC tracks mul. []",
            100, 0.5, 100.5,
            100, 0.5, 100.5 );
      fhTrk1MulPos = new TH2D( "TofSimpClus_Trk1MulPos",
            "Position of Clusters with only 1 MC tracks generating the cluster; X [cm]; Y [cm]",
            1500, -750, 750,
            1000, -500, 500 );
      fhHiTrkMulPos = new TH2D( "TofSimpClus_HiTrkMulPos",
            "Position of Clusters with >1 MC tracks generating the cluster; X [cm]; Y [cm]",
            1500, -750, 750,
            1000, -500, 500 );
      fhAllTrkMulPos = new TH2D( "TofSimpClus_AllTrkMulPos",
            "Position of all clusters generating the cluster; X [cm]; Y [cm]",
            1500, -750, 750,
            1000, -500, 500 );
      fhMultiTrkProbPos = new TH2D( "TofSimpClus_MultiTrkProbPos",
            "Probability of having a cluster with multiple tracks as function of position; X [cm]; Y [cm]; Prob. [%]",
            1500, -750, 750,
            1000, -500, 500 );
   } // if( kTRUE == fDigiBdfPar->ClustUseTrackId() )

   fhDigSpacDifClust = new TH1I( "TofSimpClus_DigSpacDifClust",
         "Space difference along channel direction between Digi pairs on adjacent channels; PosCh i - Pos Ch i+1 [cm]",
         5000, -10.0, 10.0 );
   fhDigTimeDifClust = new TH1I( "TofSimpClus_DigTimeDifClust",
         "Time difference between Digi pairs on adjacent channels; 0.5*(tDigiA + tDigiA)chi - 0.5*(tDigiA + tDigiA)chi+1 [ns]",
         5000, -5.0, 5.0 );
   fhDigDistClust = new TH2I( "TofSimpClus_DigDistClust",
         "Distance between Digi pairs on adjacent channels; PosCh i - Pos Ch i+1 [cm along ch]; 0.5*(tDigiA + tDigiA)chi - 0.5*(tDigiA + tDigiA)chi+1 [ns]",
         5000, -10.0, 10.0,
         2000, -5.0, 5.0 );

   fhClustSizeDifX = new TH2I( "TofSimpClus_ClustSizeDifX", "Position difference between Point and Hit as function of Cluster Size; Cluster Size [Strips]; dX [cm]",
         100, 0.5, 100.5,
         500,-50,50);
   fhClustSizeDifY = new TH2I( "TofSimpClus_ClustSizeDifY", "Position difference between Point and Hit as function of Cluster Size; Cluster Size [Strips]; dY [cm]",
         100, 0.5, 100.5,
         500,-50,50);
   fhChDifDifX = new TH2I( "TofSimpClus_ChDifDifX", "Position difference between Point and Hit as function of Channel dif; Ch Dif [Strips]; dX [cm]",
         101, -50.5, 50.5,
         500,-50,50);
   fhChDifDifY = new TH2I( "TofSimpClus_ChDifDifY", "Position difference between Point and Hit as function of Channel Dif; Ch Dif [Strips]; dY [cm]",
         101, -50.5, 50.5,
         500,-50,50);

   fhNbSameSide = new TH1I( "TofSimpClus_NbSameSide",
         "How many time per event the 2 digis on a channel were of the same side ; Counts/Event []",
         500, 0.0, 500.0 );
   fhNbDigiPerChan = new TH1I( "TofSimpClus_NbDigiPerChan",
         "Nb of Digis per channel; Nb Digis []",
         100, 0.0, 100.0 );

   gDirectory->cd( oldir->GetPath() ); // <= To prevent histos from being sucked in by the param file of the TRootManager!

   return kTRUE;
}
Bool_t   CbmTofSimpClusterizer::FillHistos()
{
   fhClustBuildTime->Fill( fStop.GetSec() - fStart.GetSec()
                           + (fStop.GetNanoSec() - fStart.GetNanoSec())/1e9 );
   Int_t iNbTofHits  = fTofHitsColl->GetEntries();
   Int_t iNbTracks   = fMcTracksColl->GetEntries();

   // Trakcs Info
   Int_t iNbTofTracks     = 0;
   Int_t iNbTofTracksPrim = 0;
   CbmMCTrack  *pMcTrk;
   for(Int_t iTrkInd = 0; iTrkInd < iNbTracks; iTrkInd++)
   {
      pMcTrk = (CbmMCTrack*) fMcTracksColl->At( iTrkInd );

      if( 0 < pMcTrk->GetNPoints(kTOF) )
      {
         iNbTofTracks++;
      }
      if( 0 < pMcTrk->GetNPoints(kTOF) && -1 == pMcTrk->GetMotherId() )
         iNbTofTracksPrim++;

   } // for(Int_t iTrkInd = 0; iTrkInd < nMcTracks; iTrkInd++)

   if( 0 < iNbTofTracks )
      fhHitsPerTracks->Fill( (Double_t)iNbTofHits/(Double_t)iNbTofTracks );

   CbmTofHit  *pHit;
   for( Int_t iHitInd = 0; iHitInd < iNbTofHits; iHitInd++)
   {
      pHit = (CbmTofHit*) fTofHitsColl->At( iHitInd );
      if( kFALSE == fDigiBdfPar->ClustUseTrackId() )
         fhPtsPerHit->Fill(pHit->GetFlag());
      if( 1 == pHit->GetFlag() )
      {
//         CbmTofPoint* pPt = (CbmTofPoint*)pHit->GetRefId();
         // Using the SetLinks/GetLinks of the TofHit class seems to conflict
         // with something in littrack QA
//         CbmTofPoint* pPt = (CbmTofPoint*)(pHit->GetLinks());
         // Use instead the index
         CbmTofPoint* pPt = (CbmTofPoint*)fTofPointsColl->At( pHit->GetRefId() );
         fhTimePtVsHits->Fill( pPt->GetTime(), pHit->GetTime() );
         fhTimeResSingHits->Fill( pHit->GetTime() - pPt->GetTime() );
         fhTimeResSingHitsB->Fill( pHit->GetTime() - pPt->GetTime(),
               fGeoHandler->GetSMType(pPt->GetDetectorID()) );
      } // if( 1 == pHit->GetFlag() )
   } // for( Int_t iHitInd = 0; iHitInd < iNbTofHits; iHitInd++)

   for( Int_t iSmType = 0; iSmType < fDigiBdfPar->GetNbSmTypes(); iSmType++ )
      for( Int_t iRpc = 0; iRpc < fDigiBdfPar->GetNbRpc( iSmType); iRpc++ )
      {
         for( Int_t iCluster = 0; iCluster < fviClusterSize[iSmType][iRpc].size(); iCluster++ )
         {
            fhClusterSize->Fill( fviClusterSize[iSmType][iRpc][iCluster]);
            fhClusterSizeType->Fill( fviClusterSize[iSmType][iRpc][iCluster], 40*iSmType + iRpc );
            if( kTRUE == fDigiBdfPar->ClustUseTrackId() )
            {
               fhTrackMul->Fill( fviTrkMul[iSmType][iRpc][iCluster] );
               fhClusterSizeMulti->Fill( fviClusterSize[iSmType][iRpc][iCluster],
                                         fviTrkMul[iSmType][iRpc][iCluster] );
               if( 1 == fviTrkMul[iSmType][iRpc][iCluster] )
                  fhTrk1MulPos->Fill( fvdX[iSmType][iRpc][iCluster], fvdY[iSmType][iRpc][iCluster] );
               if( 1 < fviTrkMul[iSmType][iRpc][iCluster] )
                  fhHiTrkMulPos->Fill(fvdX[iSmType][iRpc][iCluster], fvdY[iSmType][iRpc][iCluster] );
               fhAllTrkMulPos->Fill(fvdX[iSmType][iRpc][iCluster], fvdY[iSmType][iRpc][iCluster] );
            } // if( kTRUE == fDigiBdfPar->ClustUseTrackId() )
            if( 1 == fviTrkMul[iSmType][iRpc][iCluster] )
            {
               fhClustSizeDifX->Fill( fviClusterSize[iSmType][iRpc][iCluster], fvdDifX[iSmType][iRpc][iCluster]);
               fhClustSizeDifY->Fill( fviClusterSize[iSmType][iRpc][iCluster], fvdDifY[iSmType][iRpc][iCluster]);
               if( 1 == fviClusterSize[iSmType][iRpc][iCluster] )
               {
                  fhChDifDifX->Fill( fvdDifCh[iSmType][iRpc][iCluster], fvdDifX[iSmType][iRpc][iCluster]);
                  fhChDifDifY->Fill( fvdDifCh[iSmType][iRpc][iCluster], fvdDifY[iSmType][iRpc][iCluster]);
               }
            }
         } // for( Int_t iCluster = 0; iCluster < fviClusterSize[iSmType][iRpc].size(); iCluster++ )
         fviClusterSize[iSmType][iRpc].clear();
         fviTrkMul[iSmType][iRpc].clear();
         fvdX[iSmType][iRpc].clear();
         fvdY[iSmType][iRpc].clear();
         fvdDifX[iSmType][iRpc].clear();
         fvdDifY[iSmType][iRpc].clear();
         fvdDifCh[iSmType][iRpc].clear();
      } // for( Int_t iRpc = 0; iRpc < fDigiBdfPar->GetNbRpc( iSmType); iRpc++ )

   fhNbSameSide->Fill(fiNbSameSide);

   return kTRUE;
}
Bool_t   CbmTofSimpClusterizer::WriteHistos()
{
   TDirectory * oldir = gDirectory;
   TFile *fHist = new TFile("./tofSimpClust.hst.root","RECREATE");

   fHist->cd();
   fhClustBuildTime->Write();
   fhHitsPerTracks->Write();
   if( kFALSE == fDigiBdfPar->ClustUseTrackId() )
      fhPtsPerHit->Write();
   fhTimeResSingHits->Write();
   fhTimeResSingHitsB->Write();
   fhTimePtVsHits->Write();
   fhClusterSize->Write();
   fhClusterSizeType->Write();
   if( kTRUE == fDigiBdfPar->ClustUseTrackId() )
   {
      fhTrackMul->Write();
      fhClusterSizeMulti->Write();
      fhTrk1MulPos->Write();
      fhHiTrkMulPos->Write();
      fhAllTrkMulPos->Write();
      fhMultiTrkProbPos->Divide( fhHiTrkMulPos, fhAllTrkMulPos);
      fhMultiTrkProbPos->Scale( 100.0 );
      fhMultiTrkProbPos->Write();
   } // if( kTRUE == fDigiBdfPar->ClustUseTrackId() )
   fhDigSpacDifClust->Write();
   fhDigTimeDifClust->Write();
   fhDigDistClust->Write();

   fhClustSizeDifX->Write();
   fhClustSizeDifY->Write();

   fhChDifDifX->Write();
   fhChDifDifY->Write();

   fhNbSameSide->Write();
   fhNbDigiPerChan->Write();

   gDirectory->cd( oldir->GetPath() );

   fHist->Close();

   return kTRUE;
}
Bool_t   CbmTofSimpClusterizer::DeleteHistos()
{
   delete fhClustBuildTime;
   delete fhHitsPerTracks;
   delete fhPtsPerHit;
   delete fhTimeResSingHits;
   delete fhTimeResSingHitsB;
   delete fhTimePtVsHits;
   delete fhClusterSize;
   delete fhClusterSizeType;

   if( kTRUE == fDigiBdfPar->ClustUseTrackId() )
   {
      delete fhTrackMul;
      delete fhClusterSizeMulti;
      delete fhTrk1MulPos;
      delete fhHiTrkMulPos;
      delete fhAllTrkMulPos;
      delete fhMultiTrkProbPos;
   }
   delete fhDigSpacDifClust;
   delete fhDigTimeDifClust;
   delete fhDigDistClust;

   delete fhClustSizeDifX;
   delete fhClustSizeDifY;

   delete fhChDifDifX;
   delete fhChDifDifY;

   delete fhNbSameSide;
   delete fhNbDigiPerChan;

   return kTRUE;
}
/************************************************************************************/
Bool_t   CbmTofSimpClusterizer::BuildClusters()
{
   /*
    * FIXME: maybe use the 2D distance (X/Y) as criteria instead of the distance long channel
    * direction
    */
   Double_t dMaxTimeDist  = fDigiBdfPar->GetMaxTimeDist();
   Double_t dMaxSpaceDist = fDigiBdfPar->GetMaxDistAlongCh();

   // First Time order the Digis array
//   fTofDigisColl->Sort();

   // Then loop over the digis array and store the Digis in separate vectors for
   // each RPC modules
   Int_t iNbTofDigi  = fTofDigisColl->GetEntries();
   if( kTRUE == fDigiBdfPar->UseExpandedDigi() )
   {
      CbmTofDigiExp *pDigi;
      for( Int_t iDigInd = 0; iDigInd < iNbTofDigi; iDigInd++ )
      {
         pDigi = (CbmTofDigiExp*) fTofDigisColl->At( iDigInd );
//         cout<<iDigInd<<" "<<pDigi<<" "
//             <<pDigi->GetType()<<" "
//             <<pDigi->GetSm()<<" "
//             <<pDigi->GetRpc()<<" "
//             <<pDigi->GetChannel()<<" "
//             <<pDigi->GetTime()<<" "
//             <<pDigi->GetTot()
//             <<endl;

//         fStorDigiExp[pDigi->GetType()][pDigi->GetSm()][pDigi->GetRpc()][pDigi->GetChannel()].push_back(pDigi);
         fStorDigiExp[pDigi->GetType()]
                      [pDigi->GetSm()*fDigiBdfPar->GetNbRpc( pDigi->GetType()) + pDigi->GetRpc()]
                        [pDigi->GetChannel()].push_back(pDigi);
      } // for( Int_t iDigInd = 0; iDigInd < nTofDigi; iDigInd++ )
   } // if( kTRUE == fDigiBdfPar->UseExpandedDigi() )
      else
      {
         CbmTofDigi *pDigi;
         for( Int_t iDigInd = 0; iDigInd < iNbTofDigi; iDigInd++ )
         {
            pDigi = (CbmTofDigi*) fTofDigisColl->At( iDigInd );
//            fStorDigi[pDigi->GetType()][pDigi->GetSm()][pDigi->GetRpc()][pDigi->GetChannel()].push_back(pDigi);
            fStorDigi[pDigi->GetType()]
                      [pDigi->GetSm()*fDigiBdfPar->GetNbRpc( pDigi->GetType()) + pDigi->GetRpc()]
                        [pDigi->GetChannel()].push_back(pDigi);
         } // for( Int_t iDigInd = 0; iDigInd < nTofDigi; iDigInd++ )
      } // else of if( kTRUE == fDigiBdfPar->UseExpandedDigi() )

   // Then build clusters inside each RPC module
   // Assume only 0 or 1 Digi per channel/side in each event
   // Use simplest method possible, scan direction independent:
   // a) Loop over channels in the RPC starting from 0
   //   * If strips
   //     i) Loop over Digis to check if both ends of the channel have a Digi
   //    ii) Reconstruct a mean channel time and a mean position
   //     + If a Hit is currently filled & the mean position (space, time) is less than XXX from last channel position
   //   iii) Add the mean channel time and the mean position to the ones of the hit
   //     + else
   //   iii) Use nb of strips in cluster to cal. the hit mean time and pos (charge/tot weighting)
   //    iv) Save the hit
   //     v) Start a new hit with current channel
   //   * else (pads)
   //     i) Loop over Digis to find if this channel fired
   //    ii) FIXME: either scan all other channels to check for matching Digis or have more than 1 hit open
   Int_t iNbSmTypes = fDigiBdfPar->GetNbSmTypes();
   // Hit variables
   Double_t dWeightedTime = 0.0;
   Double_t dWeightedPosX = 0.0;
   Double_t dWeightedPosY = 0.0;
   Double_t dWeightedPosZ = 0.0;
   Double_t dWeightsSum   = 0.0;
   std::vector< CbmTofPoint* > vPtsRef;
   Int_t    iNbChanInHit  = 0;
   // Last Channel Temp variables
   Int_t    iLastChan = -1;
   Double_t dLastPosX = 0.0;
   Double_t dLastPosY = 0.0;
   Double_t dLastTime = 0.0;
   // Channel Temp variables
   Double_t dPosX = 0.0;
   Double_t dPosY = 0.0;
   Double_t dPosZ = 0.0;
   Double_t dTime = 0.0;
   Double_t dTotS = 0.0;
   fiNbSameSide = 0;
   if( kTRUE == fDigiBdfPar->UseExpandedDigi() )
   {
      for( Int_t iSmType = 0; iSmType < iNbSmTypes; iSmType++ )
      {
         Int_t iNbSm  = fDigiBdfPar->GetNbSm(  iSmType);
         Int_t iNbRpc = fDigiBdfPar->GetNbRpc( iSmType);
         for( Int_t iSm = 0; iSm < iNbSm; iSm++ )
            for( Int_t iRpc = 0; iRpc < iNbRpc; iRpc++ )
            {
               Int_t    iChType = fDigiBdfPar->GetChanType( iSmType, iRpc );
               if( 0 == iChType )
               {
                  Int_t iNbCh = fDigiBdfPar->GetNbChan( iSmType, iRpc );
                  // Don't spread clusters over RPCs!!!
                  dWeightedTime = 0.0;
                  dWeightedPosX = 0.0;
                  dWeightedPosY = 0.0;
                  dWeightedPosZ = 0.0;
                  dWeightsSum   = 0.0;
                  iNbChanInHit  = 0;
                  vPtsRef.clear();
                  // For safety reinitialize everything
                  iLastChan = -1;
                  dLastPosX = 0.0;
                  dLastPosY = 0.0;
                  dLastTime = 0.0;
                  if( 1 == fDigiBdfPar->GetChanOrient( iSmType, iRpc ) )
                  {
                     // Horizontal strips => X comes from left right time difference
                     for( Int_t iCh = 0; iCh < iNbCh; iCh++ )
                     {
                        if( 0 < fStorDigiExp[iSmType][iSm*iNbRpc+iRpc][iCh].size() )
                           fhNbDigiPerChan->Fill( fStorDigiExp[iSmType][iSm*iNbRpc+iRpc][iCh].size() );
//                        if( 2 == fStorDigiExp[iSmType][iSm][iRpc][iCh].size() )
                        if( 2 == fStorDigiExp[iSmType][iSm*iNbRpc+iRpc][iCh].size() )
                        {
                           if( (fStorDigiExp[iSmType][iSm*iNbRpc+iRpc][iCh][0])->GetSide() ==
                               (fStorDigiExp[iSmType][iSm*iNbRpc+iRpc][iCh][1])->GetSide() )
                           {
                              // Not one Digi of each end!
                              fiNbSameSide++;
                              continue;
                           }

                           // 2 Digis = both sides present
                           CbmTofDetectorInfo xDetInfo(kTOF, iSmType, iSm, iRpc, 0, iCh + 1);
                           Int_t iChId = fTofId->SetDetectorInfo( xDetInfo );
                           fChannelInfo = fDigiPar->GetCell( iChId );

//                           CbmTofDigiExp * xDigiA = fStorDigiExp[iSmType][iSm][iRpc][iCh][0];
//                           CbmTofDigiExp * xDigiB = fStorDigiExp[iSmType][iSm][iRpc][iCh][1];

                           CbmTofDigiExp * xDigiA = fStorDigiExp[iSmType][iSm*iNbRpc+iRpc][iCh][0];
                           CbmTofDigiExp * xDigiB = fStorDigiExp[iSmType][iSm*iNbRpc+iRpc][iCh][1];
						   CbmTofPoint* pointA = (CbmTofPoint*)fTofPointsColl->At(xDigiA->GetMatch()->GetMatchedLink().GetIndex());
						   CbmTofPoint* pointB = (CbmTofPoint*)fTofPointsColl->At(xDigiB->GetMatch()->GetMatchedLink().GetIndex());

                           // The "Strip" time is the mean time between each end
                           dTime = xDigiA->GetTime() + xDigiB->GetTime();
                           dTime /= 2.0;

                           // Weight is the total charge => sum of both ends ToT
                           dTotS = xDigiA->GetTot() + xDigiB->GetTot();

                           // Y position is just the center of the channel
                           dPosY = fChannelInfo->GetY();

                           // X position form strip ends time difference
                           dPosX = fChannelInfo->GetX();
                           if( 1 == xDigiA->GetSide() )
                              // 0 is the right side, 1 is the left side
                              dPosX += fDigiBdfPar->GetSignalSpeed() *
                                       ( xDigiA->GetTime() - xDigiB->GetTime() )/2.0;
                           else
                              // 0 is the left side, 1 is the right side
                              dPosX += fDigiBdfPar->GetSignalSpeed() *
                                       ( xDigiB->GetTime() - xDigiA->GetTime() )/2.0;

                           // For Z always just take the one of the channel itself( in fact its gap one)
                           dPosZ = fChannelInfo->GetZ();

                           // Now check if a hit/cluster is already started
                           if( 0 < iNbChanInHit)
                           {
                              if( iLastChan == iCh - 1 )
                              {
                                 fhDigTimeDifClust->Fill( dTime - dLastTime );
                                 fhDigSpacDifClust->Fill( dPosX - dLastPosX );
                                 fhDigDistClust->Fill( dPosX - dLastPosX,
                                                       dTime - dLastTime );
                              } // if( iLastChan == iCh - 1 )
                              // a cluster is already started => check distance in space/time
                              // For simplicity, just check along strip direction for now
                              // and break cluster when a not fired strip is found
                              if( TMath::Abs( dTime - dLastTime) < dMaxTimeDist &&
                                   iLastChan == iCh - 1 &&
                                   TMath::Abs( dPosX - dLastPosX) < dMaxSpaceDist  )
                              {
                                 // Add to cluster/hit
                                 dWeightedTime += dTime*dTotS;
                                 dWeightedPosX += dPosX*dTotS;
                                 dWeightedPosY += dPosY*dTotS;
                                 dWeightedPosZ += dPosZ*dTotS;
                                 dWeightsSum   += dTotS;
                                 iNbChanInHit  += 1;
                                 // if one of the side digi comes from a CbmTofPoint not already found
                                 // in this cluster, save its pointer
                                 Bool_t bFoundA = kFALSE;
                                 Bool_t bFoundB = kFALSE;

                                 if( kTRUE == fDigiBdfPar->ClustUseTrackId() )
                                    for( Int_t iPtRef = 0; iPtRef < vPtsRef.size(); iPtRef++)
                                    {
                                       if( ((CbmTofPoint*)(vPtsRef[iPtRef]))->GetTrackID() == pointA->GetTrackID() )
                                          bFoundA = kTRUE;
                                       if( ((CbmTofPoint*)(vPtsRef[iPtRef]))->GetTrackID() == pointB->GetTrackID() )
                                          bFoundB = kTRUE;
                                    } // for( Int
                                    else for( Int_t iPtRef = 0; iPtRef < vPtsRef.size(); iPtRef++)
                                    {
                                       if( vPtsRef[iPtRef] == pointA )
                                          bFoundA = kTRUE;
                                       if( vPtsRef[iPtRef] == pointB )
                                          bFoundB = kTRUE;
                                    } // for( Int_t iPtRef = 0; iPtRef < vPtsRef.size(); iPtRef++)

                                 // CbmTofPoint pointer for 1st digi not found => save it
                                 if( kFALSE == bFoundA )
                                    vPtsRef.push_back( pointA );
                                 // CbmTofPoint pointer for 2nd digi not found => save it
                                 if( kFALSE == bFoundB )
                                    vPtsRef.push_back( pointB );
                              } // if current Digis compatible with last fired chan
                              else
                              {
                                 // Save Hit
                                 dWeightedTime /= dWeightsSum;
                                 dWeightedPosX /= dWeightsSum;
                                 dWeightedPosY /= dWeightsSum;
                                 dWeightedPosZ /= dWeightsSum;
                                 TVector3 hitPos(dWeightedPosX, dWeightedPosY, dWeightedPosZ);
                                 // Simple errors, not properly done at all for now
                                 // Right way of doing it should take into account the weight distribution
                                 // and real system time resolution
                                 TVector3 hitPosErr( fDigiBdfPar->GetFeeTimeRes() * fDigiBdfPar->GetSignalSpeed(), // Use the electronics resolution
                                                     fChannelInfo->GetSizey()/TMath::Sqrt(12.0),   // Single strips approximation
                                                     fDigiBdfPar->GetNbGaps( iSmType, iRpc)*
                                                     fDigiBdfPar->GetGapSize( iSmType, iRpc)/10.0 / // Change gap size in cm
                                                     TMath::Sqrt(12.0) ); // Use full RPC thickness as "Channel" Z size

                                 Int_t iDetId = vPtsRef[0]->GetDetectorID();// detID = pt->GetDetectorID() <= from TofPoint
//                                 Int_t iDetId = 0;
                                 Int_t iRefId = fTofPointsColl->IndexOf( vPtsRef[0] );
//                                 Int_t iRefId = 0; // Index of the correspondng TofPoint
                                 new((*fTofHitsColl)[fiNbHits]) CbmTofHit( iDetId,
                                                     hitPos, hitPosErr,
                                                     iRefId,
                                                     dWeightedTime,
                                                     vPtsRef.size() ); // flag  = number of TofPoints generating the cluster
                                 // Using the SetLinks/GetLinks of the TofHit class seems to conflict
                                 // with something in littrack QA
//                                 ((CbmTofHit*)fTofHitsColl->At(fiNbHits))->SetLinks(vPtsRef[0]);
                                 fiNbHits++;
                                 // For Histogramming
                                 fviClusterSize[iSmType][iRpc].push_back(iNbChanInHit);
                                 fviTrkMul[iSmType][iRpc].push_back( vPtsRef.size() );
                                 fvdX[iSmType][iRpc].push_back(dWeightedPosX);
                                 fvdY[iSmType][iRpc].push_back(dWeightedPosY);
                                 fvdDifX[iSmType][iRpc].push_back( vPtsRef[0]->GetX() - dWeightedPosX);
                                 fvdDifY[iSmType][iRpc].push_back( vPtsRef[0]->GetY() - dWeightedPosY);

                                 fvdDifCh[iSmType][iRpc].push_back( fGeoHandler->GetCell( vPtsRef[0]->GetDetectorID() ) -1 -iLastChan );
                                 vPtsRef.clear();

                                 // Start a new hit
                                 dWeightedTime = dTime*dTotS;
                                 dWeightedPosX = dPosX*dTotS;
                                 dWeightedPosY = dPosY*dTotS;
                                 dWeightedPosZ = dPosZ*dTotS;
                                 dWeightsSum   = dTotS;
                                 iNbChanInHit  = 1;
                                 // Save pointer on CbmTofPoint
                                 vPtsRef.push_back( pointA );

                                 if( kTRUE == fDigiBdfPar->ClustUseTrackId() )
                                 {
                                    if( pointA->GetTrackID() !=
                                          pointB->GetTrackID() )
                                       // if other side come from a different Track,
                                       // also save the pointer on CbmTofPoint
                                       vPtsRef.push_back( pointB );
                                 } // if( kTRUE == fDigiBdfPar->ClustUseTrackId() )
                                    else if( pointA != pointB )
                                       // if other side come from a different TofPoint,
                                       // also save the pointer on CbmTofPoint
                                       vPtsRef.push_back( pointB );
                              } // else of if current Digis compatible with last fired chan
                           } // if( 0 < iNbChanInHit)
                              else
                              {
                                 // first fired strip in this RPC
                                 dWeightedTime = dTime*dTotS;
                                 dWeightedPosX = dPosX*dTotS;
                                 dWeightedPosY = dPosY*dTotS;
                                 dWeightedPosZ = dPosZ*dTotS;
                                 dWeightsSum   = dTotS;
                                 iNbChanInHit  = 1;
                                 // Save pointer on CbmTofPoint
                                 vPtsRef.push_back( pointA );

                                 if( kTRUE == fDigiBdfPar->ClustUseTrackId() )
                                 {
                                    if( pointA->GetTrackID() !=
                                          pointB->GetTrackID() )
                                       // if other side come from a different Track,
                                       // also save the pointer on CbmTofPoint
                                       vPtsRef.push_back( pointB );
                                 } // if( kTRUE == fDigiBdfPar->ClustUseTrackId() )
                                    else if( pointA != pointB )
                                       // if other side come from a different TofPoint,
                                       // also save the pointer on CbmTofPoint
                                       vPtsRef.push_back( pointB );
                              } // else of if( 0 < iNbChanInHit)
                           iLastChan = iCh;
                           dLastPosX = dPosX;
                           dLastPosY = dPosY;
                           dLastTime = dTime;
                        } // if( 2 <= fStorDigiExp[iSmType][iSm][iRpc][iCh].size() )
//                           fStorDigiExp[iSmType][iSm][iRpc][iCh].clear();
                        fStorDigiExp[iSmType][iSm*iNbRpc+iRpc][iCh].clear();
                     } // for( Int_t iCh = 0; iCh < iNbCh; iCh++ )
                  } // if( 1 == fDigiBdfPar->GetChanOrient( iSmType, iRpc ) )
                     else
                     {
                        // Vertical strips => Y comes from bottom top time difference
                        for( Int_t iCh = 0; iCh < iNbCh; iCh++ )
                        {
                           if( 0 < fStorDigiExp[iSmType][iSm*iNbRpc+iRpc][iCh].size() )
                              fhNbDigiPerChan->Fill( fStorDigiExp[iSmType][iSm*iNbRpc+iRpc][iCh].size() );
//                           if( 2 == fStorDigiExp[iSmType][iSm][iRpc][iCh].size() )
                           if( 2 == fStorDigiExp[iSmType][iSm*iNbRpc+iRpc][iCh].size() )
                           {
                              if( (fStorDigiExp[iSmType][iSm*iNbRpc+iRpc][iCh][0])->GetSide() ==
                                  (fStorDigiExp[iSmType][iSm*iNbRpc+iRpc][iCh][1])->GetSide() )
                              {
                                 // Not one Digi of each end!
                                 fiNbSameSide++;
                                 continue;
                              }

                              // 2 Digis = both sides present
                              CbmTofDetectorInfo xDetInfo(kTOF, iSmType, iSm, iRpc, 0, iCh + 1);
                              Int_t iChId = fTofId->SetDetectorInfo( xDetInfo );
                              fChannelInfo = fDigiPar->GetCell( iChId );

//                              CbmTofDigiExp * xDigiA = fStorDigiExp[iSmType][iSm][iRpc][iCh][0];
//                              CbmTofDigiExp * xDigiB = fStorDigiExp[iSmType][iSm][iRpc][iCh][1];

                              CbmTofDigiExp * xDigiA = fStorDigiExp[iSmType][iSm*iNbRpc+iRpc][iCh][0];
                              CbmTofDigiExp * xDigiB = fStorDigiExp[iSmType][iSm*iNbRpc+iRpc][iCh][1];
							  CbmTofPoint* pointA = (CbmTofPoint*)fTofPointsColl->At(xDigiA->GetMatch()->GetMatchedLink().GetIndex());
							  CbmTofPoint* pointB = (CbmTofPoint*)fTofPointsColl->At(xDigiB->GetMatch()->GetMatchedLink().GetIndex());

                              // The "Strip" time is the mean time between each end
                              dTime = xDigiA->GetTime() + xDigiB->GetTime();
                              dTime /= 2.0;

                              // Weight is the total charge => sum of both ends ToT
                              dTotS = xDigiA->GetTot() + xDigiB->GetTot();

                              // X position is just the center of the channel
                              dPosX = fChannelInfo->GetX();

                              // Y position form strip ends time difference
                              dPosY = fChannelInfo->GetY();
                              if( 1 == xDigiA->GetSide() )
                                 // 0 is the top side, 1 is the bottom side
                                 dPosY += fDigiBdfPar->GetSignalSpeed() *
                                          ( xDigiA->GetTime() - xDigiB->GetTime() )/2.0;
                              else
                                 // 0 is the bottom side, 1 is the top side
                                 dPosY += fDigiBdfPar->GetSignalSpeed() *
                                          ( xDigiB->GetTime() - xDigiA->GetTime() )/2.0;

                              // For Z always just take the one of the channel itself( in fact its gap one)
                              dPosZ = fChannelInfo->GetZ();

                              // Now check if a hit/cluster is already started
                              if( 0 < iNbChanInHit)
                              {
                                 if( iLastChan == iCh - 1 )
                                 {
                                    fhDigTimeDifClust->Fill( dTime - dLastTime );
                                    fhDigSpacDifClust->Fill( dPosY - dLastPosY );
                                    fhDigDistClust->Fill( dPosY - dLastPosY,
                                                          dTime - dLastTime );
                                 } // if( iLastChan == iCh - 1 )
                                 // a cluster is already started => check distance in space/time
                                 // For simplicity, just check along strip direction for now
                                 // and break cluster when a not fired strip is found
                                 if( TMath::Abs( dTime - dLastTime) < dMaxTimeDist &&
                                      iLastChan == iCh - 1 &&
                                      TMath::Abs( dPosY - dLastPosY) < dMaxSpaceDist  )
                                 {
                                    // Add to cluster/hit
                                    dWeightedTime += dTime*dTotS;
                                    dWeightedPosX += dPosX*dTotS;
                                    dWeightedPosY += dPosY*dTotS;
                                    dWeightedPosZ += dPosZ*dTotS;
                                    dWeightsSum   += dTotS;
                                    iNbChanInHit  += 1;
                                    // if one of the side digi comes from a CbmTofPoint not already found
                                    // in this cluster, save its pointer
                                    Bool_t bFoundA = kFALSE;
                                    Bool_t bFoundB = kFALSE;
                                    if( kTRUE == fDigiBdfPar->ClustUseTrackId() )
                                       for( Int_t iPtRef = 0; iPtRef < vPtsRef.size(); iPtRef++)
                                       {
                                          if( ((CbmTofPoint*)(vPtsRef[iPtRef]))->GetTrackID() == pointA->GetTrackID() )
                                             bFoundA = kTRUE;
                                          if( ((CbmTofPoint*)(vPtsRef[iPtRef]))->GetTrackID() == pointB->GetTrackID() )
                                             bFoundB = kTRUE;
                                       } // for( Int
                                       else for( Int_t iPtRef = 0; iPtRef < vPtsRef.size(); iPtRef++)
                                       {
                                          if( vPtsRef[iPtRef] == pointA )
                                             bFoundA = kTRUE;
                                          if( vPtsRef[iPtRef] == pointB )
                                             bFoundB = kTRUE;
                                       } // for( Int_t iPtRef = 0; iPtRef < vPtsRef.size(); iPtRef++)

                                    // CbmTofPoint pointer for 1st digi not found => save it
                                    if( kFALSE == bFoundA )
                                       vPtsRef.push_back( pointA );
                                    // CbmTofPoint pointer for 2nd digi not found => save it
                                    if( kFALSE == bFoundB )
                                       vPtsRef.push_back( pointB );
                                 } // if current Digis compatible with last fired chan
                                 else
                                 {
                                    // Save Hit
                                    dWeightedTime /= dWeightsSum;
                                    dWeightedPosX /= dWeightsSum;
                                    dWeightedPosY /= dWeightsSum;
                                    dWeightedPosZ /= dWeightsSum;
                                    TVector3 hitPos(dWeightedPosX, dWeightedPosY, dWeightedPosZ);
                                    // Simple errors, not properly done at all for now
                                    // Right way of doing it should take into account the weight distribution
                                    // and real system time resolution
                                    TVector3 hitPosErr( fChannelInfo->GetSizex()/TMath::Sqrt(12.0),   // Single strips approximation
                                                        fDigiBdfPar->GetFeeTimeRes() * fDigiBdfPar->GetSignalSpeed(), // Use the electronics resolution
                                                        fDigiBdfPar->GetNbGaps( iSmType, iRpc)*
                                                        fDigiBdfPar->GetGapSize( iSmType, iRpc)/10.0 / // Change gap size in cm
                                                        TMath::Sqrt(12.0) ); // Use full RPC thickness as "Channel" Z size
                                    Int_t iDetId = vPtsRef[0]->GetDetectorID();// detID = pt->GetDetectorID() <= from TofPoint
//                                    Int_t iDetId = 0;
                                    Int_t iRefId = fTofPointsColl->IndexOf( vPtsRef[0] );
//                                    Int_t iRefId = 0; // Index of the correspondng TofPoint
                                    new((*fTofHitsColl)[fiNbHits]) CbmTofHit( iDetId,
                                                        hitPos, hitPosErr,
                                                        iRefId,
                                                        dWeightedTime,
                                                        vPtsRef.size() ); // flag  = number of TofPoints generating the cluster
                                    // Using the SetLinks/GetLinks of the TofHit class seems to conflict
                                    // with something in littrack QA
//                                    ((CbmTofHit*)fTofHitsColl->At(fiNbHits))->SetLinks(vPtsRef[0]);
                                    fiNbHits++;
                                    // For Histogramming
                                    fviClusterSize[iSmType][iRpc].push_back(iNbChanInHit);
                                    fviTrkMul[iSmType][iRpc].push_back( vPtsRef.size() );
                                    fvdX[iSmType][iRpc].push_back(dWeightedPosX);
                                    fvdY[iSmType][iRpc].push_back(dWeightedPosY);
                                    fvdDifX[iSmType][iRpc].push_back( vPtsRef[0]->GetX() - dWeightedPosX);
                                    fvdDifY[iSmType][iRpc].push_back( vPtsRef[0]->GetY() - dWeightedPosY);
                                    fvdDifCh[iSmType][iRpc].push_back( fGeoHandler->GetCell( vPtsRef[0]->GetDetectorID() ) -1 -iLastChan );
                                    vPtsRef.clear();

                                    // Start a new hit
                                    dWeightedTime = dTime*dTotS;
                                    dWeightedPosX = dPosX*dTotS;
                                    dWeightedPosY = dPosY*dTotS;
                                    dWeightedPosZ = dPosZ*dTotS;
                                    dWeightsSum   = dTotS;
                                    iNbChanInHit  = 1;
                                    // Save pointer on CbmTofPoint
                                    vPtsRef.push_back( pointA );

                                    if( kTRUE == fDigiBdfPar->ClustUseTrackId() )
                                    {
                                       if( pointA->GetTrackID() !=
                                             pointB->GetTrackID() )
                                          // if other side come from a different Track,
                                          // also save the pointer on CbmTofPoint
                                          vPtsRef.push_back( pointB );
                                    } // if( kTRUE == fDigiBdfPar->ClustUseTrackId() )
                                       else if( pointA != pointB )
                                          // if other side come from a different TofPoint,
                                          // also save the pointer on CbmTofPoint
                                          vPtsRef.push_back( pointB );
                                 } // else of if current Digis compatible with last fired chan
                              } // if( 0 < iNbChanInHit)
                                 else
                                 {
                                    // first fired strip in this RPC
                                    dWeightedTime = dTime*dTotS;
                                    dWeightedPosX = dPosX*dTotS;
                                    dWeightedPosY = dPosY*dTotS;
                                    dWeightedPosZ = dPosZ*dTotS;
                                    dWeightsSum   = dTotS;
                                    iNbChanInHit  = 1;
                                    // Save pointer on CbmTofPoint
                                    vPtsRef.push_back( pointA );

                                    if( kTRUE == fDigiBdfPar->ClustUseTrackId() )
                                    {
                                       if( pointA->GetTrackID() !=
                                             pointB->GetTrackID() )
                                          // if other side come from a different Track,
                                          // also save the pointer on CbmTofPoint
                                          vPtsRef.push_back( pointB );
                                    } // if( kTRUE == fDigiBdfPar->ClustUseTrackId() )
                                       else if( pointA != pointB )
                                          // if other side come from a different TofPoint,
                                          // also save the pointer on CbmTofPoint
                                          vPtsRef.push_back( pointB );
                                 } // else of if( 0 < iNbChanInHit)
                              iLastChan = iCh;
                              dLastPosX = dPosX;
                              dLastPosY = dPosY;
                              dLastTime = dTime;
                           } // if( 2 == fStorDigiExp[iSmType][iSm*iNbRpc+iRpc][iCh].size() )
//                              fStorDigiExp[iSmType][iSm][iRpc][iCh].clear();
                           fStorDigiExp[iSmType][iSm*iNbRpc+iRpc][iCh].clear();
                        } // for( Int_t iCh = 0; iCh < iNbCh; iCh++ )
                     } // else of if( 1 == fDigiBdfPar->GetChanOrient( iSmType, iRpc ) )
               } // if( 0 == iChType)
                  else
                  {
                     LOG(ERROR)<<"CbmTofSimpClusterizer::BuildClusters => Cluster building "
                           <<"from digis to hits not implemented for pads, Sm type "
                           <<iSmType<<" Rpc "<<iRpc<<FairLogger::endl;
                     return kFALSE;
                  } // else of if( 0 == iChType)

               // Now check if a hit/cluster is started
               // and save it if it's the case
               if( 0 < iNbChanInHit)
               {
                  // Check orientation to properly assign errors
                  if( 1 == fDigiBdfPar->GetChanOrient( iSmType, iRpc ) )
                  {
                     // Save Hit
                     dWeightedTime /= dWeightsSum;
                     dWeightedPosX /= dWeightsSum;
                     dWeightedPosY /= dWeightsSum;
                     dWeightedPosZ /= dWeightsSum;
                     TVector3 hitPos(dWeightedPosX, dWeightedPosY, dWeightedPosZ);
                     // Simple errors, not properly done at all for now
                     // Right way of doing it should take into account the weight distribution
                     // and real system time resolution
                     TVector3 hitPosErr( fDigiBdfPar->GetFeeTimeRes() * fDigiBdfPar->GetSignalSpeed(), // Use the electronics resolution
                                         fChannelInfo->GetSizey()/TMath::Sqrt(12.0),   // Single strips approximation
                                         fDigiBdfPar->GetNbGaps( iSmType, iRpc)*
                                         fDigiBdfPar->GetGapSize( iSmType, iRpc)/10.0 / // Change gap size in cm
                                         TMath::Sqrt(12.0) ); // Use full RPC thickness as "Channel" Z size
                     Int_t iDetId = vPtsRef[0]->GetDetectorID();// detID = pt->GetDetectorID() <= from TofPoint
//                     Int_t iDetId = 0;
                     Int_t iRefId = fTofPointsColl->IndexOf( vPtsRef[0] );
//                                 Int_t iRefId = 0; // Index of the correspondng TofPoint
                     new((*fTofHitsColl)[fiNbHits]) CbmTofHit( iDetId,
                                         hitPos, hitPosErr,
                                         iRefId,
                                         dWeightedTime,
                                         vPtsRef.size() ); // flag  = number of TofPoints generating the cluster
//                     ((CbmTofHit*)fTofHitsColl->At(fiNbHits))->SetLinks(vPtsRef[0]);
                     fiNbHits++;
                     // For Histogramming
                     fviClusterSize[iSmType][iRpc].push_back(iNbChanInHit);
                     fviTrkMul[iSmType][iRpc].push_back( vPtsRef.size() );
                     fvdX[iSmType][iRpc].push_back(dWeightedPosX);
                     fvdY[iSmType][iRpc].push_back(dWeightedPosY);
                     fvdDifX[iSmType][iRpc].push_back( vPtsRef[0]->GetX() - dWeightedPosX);
                     fvdDifY[iSmType][iRpc].push_back( vPtsRef[0]->GetY() - dWeightedPosY);
                     fvdDifCh[iSmType][iRpc].push_back( fGeoHandler->GetCell( vPtsRef[0]->GetDetectorID() ) -1 -iLastChan );
                     vPtsRef.clear();
                  } // if( 1 == fDigiBdfPar->GetChanOrient( iSmType, iRpc ) )
                  else
                  {
                     // Save Hit
                     dWeightedTime /= dWeightsSum;
                     dWeightedPosX /= dWeightsSum;
                     dWeightedPosY /= dWeightsSum;
                     dWeightedPosZ /= dWeightsSum;
                     TVector3 hitPos(dWeightedPosX, dWeightedPosY, dWeightedPosZ);
                     // Simple errors, not properly done at all for now
                     // Right way of doing it should take into account the weight distribution
                     // and real system time resolution
                     TVector3 hitPosErr( fChannelInfo->GetSizex()/TMath::Sqrt(12.0),   // Single strips approximation
                                         fDigiBdfPar->GetFeeTimeRes() * fDigiBdfPar->GetSignalSpeed(), // Use the electronics resolution
                                         fDigiBdfPar->GetNbGaps( iSmType, iRpc)*
                                         fDigiBdfPar->GetGapSize( iSmType, iRpc)/10.0 / // Change gap size in cm
                                         TMath::Sqrt(12.0) ); // Use full RPC thickness as "Channel" Z size
//                     cout<<"a "<<vPtsRef.size()<<endl;
//                     cout<<"b "<<vPtsRef[0]<<endl;
//                     cout<<"c "<<vPtsRef[0]->GetDetectorID()<<endl;
                     Int_t iDetId = vPtsRef[0]->GetDetectorID();// detID = pt->GetDetectorID() <= from TofPoint
//                     Int_t iDetId = 0;
                     Int_t iRefId = fTofPointsColl->IndexOf( vPtsRef[0] );
//                                 Int_t iRefId = 0; // Index of the correspondng TofPoint

                     new((*fTofHitsColl)[fiNbHits]) CbmTofHit( iDetId,
                                         hitPos, hitPosErr,
                                         iRefId,
                                         dWeightedTime,
                                         vPtsRef.size() ); // flag  = number of TofPoints generating the cluster
                     // Using the SetLinks/GetLinks of the TofHit class seems to conflict
                     // with something in littrack QA
//                     ((CbmTofHit*)fTofHitsColl->At(fiNbHits))->SetLinks(vPtsRef[0]);
                     fiNbHits++;
                     // For Histogramming
                     fviClusterSize[iSmType][iRpc].push_back(iNbChanInHit);
                     fviTrkMul[iSmType][iRpc].push_back( vPtsRef.size() );
                     fvdX[iSmType][iRpc].push_back(dWeightedPosX);
                     fvdY[iSmType][iRpc].push_back(dWeightedPosY);
                     fvdDifX[iSmType][iRpc].push_back( vPtsRef[0]->GetX() - dWeightedPosX);
                     fvdDifY[iSmType][iRpc].push_back( vPtsRef[0]->GetY() - dWeightedPosY);
                     fvdDifCh[iSmType][iRpc].push_back( fGeoHandler->GetCell( vPtsRef[0]->GetDetectorID() ) -1 -iLastChan );
                     vPtsRef.clear();
                  } // else of if( 1 == fDigiBdfPar->GetChanOrient( iSmType, iRpc ) )
               } // if( 0 < iNbChanInHit)
            } // for each sm/rpc pair
      } // for( Int_t iSmType = 0; iSmType < iNbSmTypes; iSmType++ )
   } // if( kTRUE == fDigiBdfPar->UseExpandedDigi() )
      else
      {
         for( Int_t iSmType = 0; iSmType < iNbSmTypes; iSmType++ )
         {
            Int_t iNbSm  = fDigiBdfPar->GetNbSm(  iSmType);
            Int_t iNbRpc = fDigiBdfPar->GetNbRpc( iSmType);
            for( Int_t iSm = 0; iSm < iNbSm; iSm++ )
               for( Int_t iRpc = 0; iRpc < iNbRpc; iRpc++ )
               {
                  Int_t    iChType = fDigiBdfPar->GetChanType( iSmType, iRpc );
                  if( 0 == iChType )
                  {
                     Int_t iNbCh = fDigiBdfPar->GetNbChan( iSmType, iRpc );
                     // Don't spread clusters over RPCs!!!
                     dWeightedTime = 0.0;
                     dWeightedPosX = 0.0;
                     dWeightedPosY = 0.0;
                     dWeightedPosZ = 0.0;
                     dWeightsSum   = 0.0;
                     iNbChanInHit  = 0;
                     vPtsRef.clear();
                     // For safety reinitialize everything
                     iLastChan = -1;
                     dLastPosX = 0.0;
                     dLastPosY = 0.0;
                     dLastTime = 0.0;
                     if( 1 == fDigiBdfPar->GetChanOrient( iSmType, iRpc ) )
                     {
                        // Horizontal strips => X comes from left right time difference
                        for( Int_t iCh = 0; iCh < iNbCh; iCh++ )
                        {
                           if( 0 < fStorDigi[iSmType][iSm*iNbRpc+iRpc][iCh].size() )
                              fhNbDigiPerChan->Fill( fStorDigi[iSmType][iSm*iNbRpc+iRpc][iCh].size() );
//                           if( 2 == fStorDigi[iSmType][iSm][iRpc][iCh].size() )
                           if( 2 == fStorDigi[iSmType][iSm*iNbRpc+iRpc][iCh].size() )
                           {
                              if( (fStorDigi[iSmType][iSm*iNbRpc+iRpc][iCh][0])->GetSide() ==
                                    (fStorDigi[iSmType][iSm*iNbRpc+iRpc][iCh][1])->GetSide() )
                              {
                                 // Not one Digi of each end!
                                 fiNbSameSide++;
                                 continue;
                              }

                              CbmTofDetectorInfo xDetInfo(kTOF, iSmType, iSm, iRpc, 0, iCh + 1);
                              Int_t iChId = fTofId->SetDetectorInfo( xDetInfo );
                              fChannelInfo = fDigiPar->GetCell( iChId );

//                              CbmTofDigi * xDigiA = fStorDigi[iSmType][iSm][iRpc][iCh][0];
//                              CbmTofDigi * xDigiB = fStorDigi[iSmType][iSm][iRpc][iCh][1];

                              CbmTofDigi * xDigiA = fStorDigi[iSmType][iSm*iNbRpc+iRpc][iCh][0];
                              CbmTofDigi * xDigiB = fStorDigi[iSmType][iSm*iNbRpc+iRpc][iCh][1];
							  CbmTofPoint* pointA = (CbmTofPoint*)fTofPointsColl->At(xDigiA->GetMatch()->GetMatchedLink().GetIndex());
							  CbmTofPoint* pointB = (CbmTofPoint*)fTofPointsColl->At(xDigiB->GetMatch()->GetMatchedLink().GetIndex());
                             // The "Strip" time is the mean time between each end
                              dTime = xDigiA->GetTime() + xDigiB->GetTime();
                              dTime /= 2.0;

                              // Weight is the total charge => sum of both ends ToT
                              dTotS = xDigiA->GetTot() + xDigiB->GetTot();

                              // Y position is just the center of the channel
                              dPosY = fChannelInfo->GetY();

                              // X position form strip ends time difference
                              dPosX = fChannelInfo->GetX();
                              if( 1 == xDigiA->GetSide() )
                                 // 0 is the right side, 1 is the left side
                                 dPosX += fDigiBdfPar->GetSignalSpeed() *
                                          ( xDigiA->GetTime() - xDigiB->GetTime() )/2.0;
                              else
                                 // 0 is the left side, 1 is the right side
                                 dPosX += fDigiBdfPar->GetSignalSpeed() *
                                          ( xDigiB->GetTime() - xDigiA->GetTime() )/2.0;

                              // For Z always just take the one of the channel itself( in fact its gap one)
                              dPosZ = fChannelInfo->GetZ();

                              // Now check if a hit/cluster is already started
                              if( 0 < iNbChanInHit)
                              {
                                 if( iLastChan == iCh - 1 )
                                 {
                                    fhDigTimeDifClust->Fill( dTime - dLastTime );
                                    fhDigSpacDifClust->Fill( dPosX - dLastPosX );
                                    fhDigDistClust->Fill( dPosX - dLastPosX,
                                                          dTime - dLastTime );
                                 } // if( iLastChan == iCh - 1 )
                                 // a cluster is already started => check distance in space/time
                                 // For simplicity, just check along strip direction for now
                                 if( TMath::Abs( dTime - dLastTime) < dMaxTimeDist &&
                                      iLastChan == iCh - 1 &&
                                      TMath::Abs( dPosX - dLastPosX) < dMaxSpaceDist  )
                                 {
                                    // Add to cluster/hit
                                    dWeightedTime += dTime*dTotS;
                                    dWeightedPosX += dPosX*dTotS;
                                    dWeightedPosY += dPosY*dTotS;
                                    dWeightedPosZ += dPosZ*dTotS;
                                    dWeightsSum   += dTotS;
                                    iNbChanInHit  += 1;
                                    // if one of the side digi comes from a CbmTofPoint not already found
                                    // in this cluster, save its pointer
                                    Bool_t bFoundA = kFALSE;
                                    Bool_t bFoundB = kFALSE;
                                    if( kTRUE == fDigiBdfPar->ClustUseTrackId() )
                                       for( Int_t iPtRef = 0; iPtRef < vPtsRef.size(); iPtRef++)
                                       {
                                          if( ((CbmTofPoint*)(vPtsRef[iPtRef]))->GetTrackID() == pointA->GetTrackID() )
                                             bFoundA = kTRUE;
                                          if( ((CbmTofPoint*)(vPtsRef[iPtRef]))->GetTrackID() == pointB->GetTrackID() )
                                             bFoundB = kTRUE;
                                       } // for( Int
                                       else for( Int_t iPtRef = 0; iPtRef < vPtsRef.size(); iPtRef++)
                                       {
                                          if( vPtsRef[iPtRef] == pointA )
                                             bFoundA = kTRUE;
                                          if( vPtsRef[iPtRef] == pointB )
                                             bFoundB = kTRUE;
                                       } // for( Int_t iPtRef = 0; iPtRef < vPtsRef.size(); iPtRef++)

                                    // CbmTofPoint pointer for 1st digi not found => save it
                                    if( kFALSE == bFoundA )
                                       vPtsRef.push_back( pointA );
                                    // CbmTofPoint pointer for 2nd digi not found => save it
                                    if( kFALSE == bFoundB )
                                       vPtsRef.push_back( pointB );
                                 } // if current Digis compatible with last fired chan
                                 else
                                 {
                                    // Save Hit
                                    dWeightedTime /= dWeightsSum;
                                    dWeightedPosX /= dWeightsSum;
                                    dWeightedPosY /= dWeightsSum;
                                    dWeightedPosZ /= dWeightsSum;
                                    TVector3 hitPos(dWeightedPosX, dWeightedPosY, dWeightedPosZ);
                                    // Simple errors, not properly done at all for now
                                    // Right way of doing it should take into account the weight distribution
                                    // and real system time resolution
                                    TVector3 hitPosErr( fDigiBdfPar->GetFeeTimeRes() * fDigiBdfPar->GetSignalSpeed(), // Use the electronics resolution
                                                        fChannelInfo->GetSizey()/TMath::Sqrt(12.0),   // Single strips approximation
                                                        fDigiBdfPar->GetNbGaps( iSmType, iRpc)*
                                                        fDigiBdfPar->GetGapSize( iSmType, iRpc)/10.0 / // Change gap size in cm
                                                        TMath::Sqrt(12.0) ); // Use full RPC thickness as "Channel" Z size
                                    Int_t iDetId = vPtsRef[0]->GetDetectorID();// detID = pt->GetDetectorID() <= from TofPoint
//                                    Int_t iDetId = 0;
                                    Int_t iRefId = fTofPointsColl->IndexOf( vPtsRef[0] );
//                                 Int_t iRefId = 0; // Index of the correspondng TofPoint

                                    new((*fTofHitsColl)[fiNbHits]) CbmTofHit( iDetId,
                                                        hitPos, hitPosErr,
                                                        iRefId,
                                                        dWeightedTime,
                                                        vPtsRef.size() ); // flag  = number of TofPoints generating the cluster
                                    // Using the SetLinks/GetLinks of the TofHit class seems to conflict
                                    // with something in littrack QA
//                                    ((CbmTofHit*)fTofHitsColl->At(fiNbHits))->SetLinks(vPtsRef[0]);
                                    fiNbHits++;
                                    // For Histogramming
                                    fviClusterSize[iSmType][iRpc].push_back(iNbChanInHit);
                                    fviTrkMul[iSmType][iRpc].push_back( vPtsRef.size() );
                                    fvdX[iSmType][iRpc].push_back(dWeightedPosX);
                                    fvdY[iSmType][iRpc].push_back(dWeightedPosY);
                                    fvdDifX[iSmType][iRpc].push_back( vPtsRef[0]->GetX() - dWeightedPosX);
                                    fvdDifY[iSmType][iRpc].push_back( vPtsRef[0]->GetY() - dWeightedPosY);
                                    fvdDifCh[iSmType][iRpc].push_back( fGeoHandler->GetCell( vPtsRef[0]->GetDetectorID() ) -1 -iLastChan );
                                    vPtsRef.clear();

                                    // Start a new hit
                                    dWeightedTime = dTime*dTotS;
                                    dWeightedPosX = dPosX*dTotS;
                                    dWeightedPosY = dPosY*dTotS;
                                    dWeightedPosZ = dPosZ*dTotS;
                                    dWeightsSum   = dTotS;
                                    iNbChanInHit  = 1;
                                    // Save pointer on CbmTofPoint
                                    vPtsRef.push_back( pointA );

                                    if( kTRUE == fDigiBdfPar->ClustUseTrackId() )
                                    {
                                       if( pointA->GetTrackID() !=
                                             pointB->GetTrackID() )
                                          // if other side come from a different Track,
                                          // also save the pointer on CbmTofPoint
                                          vPtsRef.push_back( pointB );
                                    } // if( kTRUE == fDigiBdfPar->ClustUseTrackId() )
                                       else if( pointA != pointB )
                                          // if other side come from a different TofPoint,
                                          // also save the pointer on CbmTofPoint
                                          vPtsRef.push_back( pointB );
                                 } // else of if current Digis compatible with last fired chan
                              } // if( 0 < iNbChanInHit)
                                 else
                                 {
                                    // first fired strip in this RPC
                                    dWeightedTime = dTime*dTotS;
                                    dWeightedPosX = dPosX*dTotS;
                                    dWeightedPosY = dPosY*dTotS;
                                    dWeightedPosZ = dPosZ*dTotS;
                                    dWeightsSum   = dTotS;
                                    iNbChanInHit  = 1;
                                    // Save pointer on CbmTofPoint
                                    vPtsRef.push_back( pointA );

                                    if( kTRUE == fDigiBdfPar->ClustUseTrackId() )
                                    {
                                       if( pointA->GetTrackID() !=
                                             pointB->GetTrackID() )
                                          // if other side come from a different Track,
                                          // also save the pointer on CbmTofPoint
                                          vPtsRef.push_back( pointB );
                                    } // if( kTRUE == fDigiBdfPar->ClustUseTrackId() )
                                       else if( pointA != pointB )
                                          // if other side come from a different TofPoint,
                                          // also save the pointer on CbmTofPoint
                                          vPtsRef.push_back( pointB );
                                 } // else of if( 0 < iNbChanInHit)
                              iLastChan = iCh;
                              dLastPosX = dPosX;
                              dLastPosY = dPosY;
                              dLastTime = dTime;
                           } // if( 2 == fStorDigi[iSmType][iSm][iRpc][iCh].size() )
//                              fStorDigi[iSmType][iSm][iRpc][iCh].clear();
                           fStorDigi[iSmType][iSm*iNbRpc+iRpc][iCh].clear();
                        } // for( Int_t iCh = 0; iCh < iNbCh; iCh++ )
                     } // if( 1 == fDigiBdfPar->GetChanOrient( iSmType, iRpc ) )
                        else
                        {
                           // Vertical strips => Y comes from bottom top time difference
                           for( Int_t iCh = 0; iCh < iNbCh; iCh++ )
                           {
                              if( 0 < fStorDigi[iSmType][iSm*iNbRpc+iRpc][iCh].size() )
                                 fhNbDigiPerChan->Fill( fStorDigi[iSmType][iSm*iNbRpc+iRpc][iCh].size() );
//                              if( 2 == fStorDigi[iSmType][iSm][iRpc][iCh].size() )
                              if( 2 == fStorDigi[iSmType][iSm*iNbRpc+iRpc][iCh].size() )
                              {
                                 if( (fStorDigi[iSmType][iSm*iNbRpc+iRpc][iCh][0])->GetSide() ==
                                       (fStorDigi[iSmType][iSm*iNbRpc+iRpc][iCh][1])->GetSide() )
                                 {
                                    // Not one Digi of each end!
                                    fiNbSameSide++;
                                    continue;
                                 }

                                 // 2 Digis = both sides present
                                 CbmTofDetectorInfo xDetInfo(kTOF, iSmType, iSm, iRpc, 0, iCh + 1);
                                 Int_t iChId = fTofId->SetDetectorInfo( xDetInfo );
                                 fChannelInfo = fDigiPar->GetCell( iChId );

//                                 CbmTofDigi * xDigiA = fStorDigi[iSmType][iSm][iRpc][iCh][0];
//                                 CbmTofDigi * xDigiB = fStorDigi[iSmType][iSm][iRpc][iCh][1];

                                 CbmTofDigi * xDigiA = fStorDigi[iSmType][iSm*iNbRpc+iRpc][iCh][0];
                                 CbmTofDigi * xDigiB = fStorDigi[iSmType][iSm*iNbRpc+iRpc][iCh][1];
								 CbmTofPoint* pointA = (CbmTofPoint*)fTofPointsColl->At(xDigiA->GetMatch()->GetMatchedLink().GetIndex());
								 CbmTofPoint* pointB = (CbmTofPoint*)fTofPointsColl->At(xDigiB->GetMatch()->GetMatchedLink().GetIndex());

                                 // The "Strip" time is the mean time between each end
                                 dTime = xDigiA->GetTime() + xDigiB->GetTime();
                                 dTime /= 2.0;

                                 // Weight is the total charge => sum of both ends ToT
                                 dTotS = xDigiA->GetTot() + xDigiB->GetTot();

                                 // X position is just the center of the channel
                                 dPosX = fChannelInfo->GetX();

                                 // Y position form strip ends time difference
                                 dPosY = fChannelInfo->GetY();
                                 if( 1 == xDigiA->GetSide() )
                                    // 0 is the top side, 1 is the bottom side
                                    dPosY += fDigiBdfPar->GetSignalSpeed() *
                                             ( xDigiA->GetTime() - xDigiB->GetTime() )/2.0;
                                 else
                                    // 0 is the bottom side, 1 is the top side
                                    dPosY += fDigiBdfPar->GetSignalSpeed() *
                                             ( xDigiB->GetTime() - xDigiA->GetTime() )/2.0;

                                 // For Z always just take the one of the channel itself( in fact its gap one)
                                 dPosZ = fChannelInfo->GetZ();

                                 // Now check if a hit/cluster is already started
                                 if( 0 < iNbChanInHit)
                                 {
                                    if( iLastChan == iCh - 1 )
                                    {
                                       fhDigTimeDifClust->Fill( dTime - dLastTime );
                                       fhDigSpacDifClust->Fill( dPosY - dLastPosY );
                                       fhDigDistClust->Fill( dPosY - dLastPosY,
                                                             dTime - dLastTime );
                                    } // if( iLastChan == iCh - 1 )
                                    // a cluster is already started => check distance in space/time
                                    // For simplicity, just check along strip direction for now
                                    if( TMath::Abs( dTime - dLastTime) < dMaxTimeDist &&
                                         iLastChan == iCh - 1 &&
                                         TMath::Abs( dPosY - dLastPosY) < dMaxSpaceDist  )
                                    {
                                       // Add to cluster/hit
                                       dWeightedTime += dTime*dTotS;
                                       dWeightedPosX += dPosX*dTotS;
                                       dWeightedPosY += dPosY*dTotS;
                                       dWeightedPosZ += dPosZ*dTotS;
                                       dWeightsSum   += dTotS;
                                       iNbChanInHit  += 1;
                                       // if one of the side digi comes from a CbmTofPoint not already found
                                       // in this cluster, save its pointer
                                       Bool_t bFoundA = kFALSE;
                                       Bool_t bFoundB = kFALSE;
                                       if( kTRUE == fDigiBdfPar->ClustUseTrackId() )
                                          for( Int_t iPtRef = 0; iPtRef < vPtsRef.size(); iPtRef++)
                                          {
                                             if( ((CbmTofPoint*)(vPtsRef[iPtRef]))->GetTrackID() == pointA->GetTrackID() )
                                                bFoundA = kTRUE;
                                             if( ((CbmTofPoint*)(vPtsRef[iPtRef]))->GetTrackID() == pointB->GetTrackID() )
                                                bFoundB = kTRUE;
                                          } // for( Int
                                          else for( Int_t iPtRef = 0; iPtRef < vPtsRef.size(); iPtRef++)
                                          {
                                             if( vPtsRef[iPtRef] == pointA )
                                                bFoundA = kTRUE;
                                             if( vPtsRef[iPtRef] == pointB )
                                                bFoundB = kTRUE;
                                          } // for( Int_t iPtRef = 0; iPtRef < vPtsRef.size(); iPtRef++)

                                       // CbmTofPoint pointer for 1st digi not found => save it
                                       if( kFALSE == bFoundA )
                                          vPtsRef.push_back( pointA );
                                       // CbmTofPoint pointer for 2nd digi not found => save it
                                       if( kFALSE == bFoundB )
                                          vPtsRef.push_back( pointB );
                                    } // if current Digis compatible with last fired chan
                                    else
                                    {
                                       // Save Hit
                                       dWeightedTime /= dWeightsSum;
                                       dWeightedPosX /= dWeightsSum;
                                       dWeightedPosY /= dWeightsSum;
                                       dWeightedPosZ /= dWeightsSum;
                                       TVector3 hitPos(dWeightedPosX, dWeightedPosY, dWeightedPosZ);
                                       // Simple errors, not properly done at all for now
                                       // Right way of doing it should take into account the weight distribution
                                       // and real system time resolution
                                       TVector3 hitPosErr( fChannelInfo->GetSizey()/TMath::Sqrt(12.0),   // Single strips approximation
                                                           fDigiBdfPar->GetFeeTimeRes() * fDigiBdfPar->GetSignalSpeed(), // Use the electronics resolution
                                                           fDigiBdfPar->GetNbGaps( iSmType, iRpc)*
                                                           fDigiBdfPar->GetGapSize( iSmType, iRpc)/10.0 / // Change gap size in cm
                                                           TMath::Sqrt(12.0) ); // Use full RPC thickness as "Channel" Z size
                                       Int_t iDetId = vPtsRef[0]->GetDetectorID();// detID = pt->GetDetectorID() <= from TofPoint
//                                       Int_t iDetId = 0;
                                       Int_t iRefId = fTofPointsColl->IndexOf( vPtsRef[0] );
//                                 Int_t iRefId = 0; // Index of the correspondng TofPoint

                                       new((*fTofHitsColl)[fiNbHits]) CbmTofHit( iDetId,
                                                           hitPos, hitPosErr,
                                                           iRefId,
                                                           dWeightedTime,
                                                           vPtsRef.size() ); // flag  = number of TofPoints generating the cluster
                                       // Using the SetLinks/GetLinks of the TofHit class seems to conflict
                                       // with something in littrack QA
//                                       ((CbmTofHit*)fTofHitsColl->At(fiNbHits))->SetLinks(vPtsRef[0]);
                                       fiNbHits++;
                                       // For Histogramming
                                       fviClusterSize[iSmType][iRpc].push_back(iNbChanInHit);
                                       fviTrkMul[iSmType][iRpc].push_back( vPtsRef.size() );
                                       fvdX[iSmType][iRpc].push_back(dWeightedPosX);
                                       fvdY[iSmType][iRpc].push_back(dWeightedPosY);
                                       fvdDifX[iSmType][iRpc].push_back( vPtsRef[0]->GetX() - dWeightedPosX);
                                       fvdDifY[iSmType][iRpc].push_back( vPtsRef[0]->GetY() - dWeightedPosY);
                                       fvdDifCh[iSmType][iRpc].push_back( fGeoHandler->GetCell( vPtsRef[0]->GetDetectorID() ) -1 -iLastChan );
                                       vPtsRef.clear();

                                       // Start a new hit
                                       dWeightedTime = dTime*dTotS;
                                       dWeightedPosX = dPosX*dTotS;
                                       dWeightedPosY = dPosY*dTotS;
                                       dWeightedPosZ = dPosZ*dTotS;
                                       dWeightsSum   = dTotS;
                                       iNbChanInHit  = 1;
                                       // Save pointer on CbmTofPoint
                                       vPtsRef.push_back( pointA );

                                       if( kTRUE == fDigiBdfPar->ClustUseTrackId() )
                                       {
                                          if( pointA->GetTrackID() !=
                                                pointB->GetTrackID() )
                                             // if other side come from a different Track,
                                             // also save the pointer on CbmTofPoint
                                             vPtsRef.push_back( pointB );
                                       } // if( kTRUE == fDigiBdfPar->ClustUseTrackId() )
                                          else if( pointA != pointB )
                                             // if other side come from a different TofPoint,
                                             // also save the pointer on CbmTofPoint
                                             vPtsRef.push_back( pointB );
                                    } // else of if current Digis compatible with last fired chan
                                 } // if( 0 < iNbChanInHit)
                                    else
                                    {
                                       // first fired strip in this RPC
                                       dWeightedTime = dTime*dTotS;
                                       dWeightedPosX = dPosX*dTotS;
                                       dWeightedPosY = dPosY*dTotS;
                                       dWeightedPosZ = dPosZ*dTotS;
                                       dWeightsSum   = dTotS;
                                       iNbChanInHit  = 1;
                                       // Save pointer on CbmTofPoint
                                       vPtsRef.push_back( pointA );

                                       if( kTRUE == fDigiBdfPar->ClustUseTrackId() )
                                       {
                                          if( pointA->GetTrackID() !=
                                                pointB->GetTrackID() )
                                             // if other side come from a different Track,
                                             // also save the pointer on CbmTofPoint
                                             vPtsRef.push_back( pointB );
                                       } // if( kTRUE == fDigiBdfPar->ClustUseTrackId() )
                                          else if( pointA != pointB )
                                             // if other side come from a different TofPoint,
                                             // also save the pointer on CbmTofPoint
                                             vPtsRef.push_back( pointB );
                                    } // else of if( 0 < iNbChanInHit)
                                 iLastChan = iCh;
                                 dLastPosX = dPosX;
                                 dLastPosY = dPosY;
                                 dLastTime = dTime;
                              } // if( 2 <= fStorDigi[iSmType][iSm][iRpc][iCh].size() )
//                                 fStorDigi[iSmType][iSm][iRpc][iCh].clear();
                                 fStorDigi[iSmType][iSm*iNbRpc+iRpc][iCh].clear();
                           } // for( Int_t iCh = 0; iCh < iNbCh; iCh++ )
                        } // else of if( 1 == fDigiBdfPar->GetChanOrient( iSmType, iRpc ) )
                  } // if( 0 == iChType)
                     else
                     {
                        LOG(ERROR)<<"CbmTofSimpClusterizer::BuildClusters => Cluster building "
                              <<"from digis to hits not implemented for pads, Sm type "
                              <<iSmType<<" Rpc "<<iRpc<<FairLogger::endl;
                        return kFALSE;
                     } // else of if( 0 == iChType)

                  // Now check if a hit/cluster is started
                  // and save it if it's the case
                  if( 0 < iNbChanInHit)
                  {
                     // Check orientation to properly assign errors
                     if( 1 == fDigiBdfPar->GetChanOrient( iSmType, iRpc ) )
                     {
                        // Save Hit
                        dWeightedTime /= dWeightsSum;
                        dWeightedPosX /= dWeightsSum;
                        dWeightedPosY /= dWeightsSum;
                        dWeightedPosZ /= dWeightsSum;
                        TVector3 hitPos(dWeightedPosX, dWeightedPosY, dWeightedPosZ);
                        // Simple errors, not properly done at all for now
                        // Right way of doing it should take into account the weight distribution
                        // and real system time resolution
                        TVector3 hitPosErr( fDigiBdfPar->GetFeeTimeRes() * fDigiBdfPar->GetSignalSpeed(), // Use the electronics resolution
                                            fChannelInfo->GetSizey()/TMath::Sqrt(12.0),   // Single strips approximation
                                            fDigiBdfPar->GetNbGaps( iSmType, iRpc)*
                                            fDigiBdfPar->GetGapSize( iSmType, iRpc)/10.0 / // Change gap size in cm
                                            TMath::Sqrt(12.0) ); // Use full RPC thickness as "Channel" Z size
                        Int_t iDetId = vPtsRef[0]->GetDetectorID();// detID = pt->GetDetectorID() <= from TofPoint
//                        Int_t iDetId = 0;
                        Int_t iRefId = fTofPointsColl->IndexOf( vPtsRef[0] );
//                                 Int_t iRefId = 0; // Index of the correspondng TofPoint

                        new((*fTofHitsColl)[fiNbHits]) CbmTofHit( iDetId,
                                            hitPos, hitPosErr,
                                            iRefId,
                                            dWeightedTime,
                                            vPtsRef.size() ); // flag  = number of TofPoints generating the cluster
                        // Using the SetLinks/GetLinks of the TofHit class seems to conflict
                        // with something in littrack QA
//                        ((CbmTofHit*)fTofHitsColl->At(fiNbHits))->SetLinks(vPtsRef[0]);
                        fiNbHits++;
                        // For Histogramming
                        fviClusterSize[iSmType][iRpc].push_back(iNbChanInHit);
                        fviTrkMul[iSmType][iRpc].push_back( vPtsRef.size() );
                        fvdX[iSmType][iRpc].push_back(dWeightedPosX);
                        fvdY[iSmType][iRpc].push_back(dWeightedPosY);
                        fvdDifX[iSmType][iRpc].push_back( vPtsRef[0]->GetX() - dWeightedPosX);
                        fvdDifY[iSmType][iRpc].push_back( vPtsRef[0]->GetY() - dWeightedPosY);
                        fvdDifCh[iSmType][iRpc].push_back( fGeoHandler->GetCell( vPtsRef[0]->GetDetectorID() ) -1 -iLastChan );
                        vPtsRef.clear();
                     } // if( 1 == fDigiBdfPar->GetChanOrient( iSmType, iRpc ) )
                     else
                     {
                        // Save Hit
                        dWeightedTime /= dWeightsSum;
                        dWeightedPosX /= dWeightsSum;
                        dWeightedPosY /= dWeightsSum;
                        dWeightedPosZ /= dWeightsSum;
                        TVector3 hitPos(dWeightedPosX, dWeightedPosY, dWeightedPosZ);
                        // Simple errors, not properly done at all for now
                        // Right way of doing it should take into account the weight distribution
                        // and real system time resolution
                        TVector3 hitPosErr( fChannelInfo->GetSizex()/TMath::Sqrt(12.0),   // Single strips approximation
                                            fDigiBdfPar->GetFeeTimeRes() * fDigiBdfPar->GetSignalSpeed(), // Use the electronics resolution
                                            fDigiBdfPar->GetNbGaps( iSmType, iRpc)*
                                            fDigiBdfPar->GetGapSize( iSmType, iRpc)/10.0 / // Change gap size in cm
                                            TMath::Sqrt(12.0) ); // Use full RPC thickness as "Channel" Z size
                        Int_t iDetId = vPtsRef[0]->GetDetectorID();// detID = pt->GetDetectorID() <= from TofPoint
//                        Int_t iDetId = 0;
                        Int_t iRefId = fTofPointsColl->IndexOf( vPtsRef[0] );
//                                 Int_t iRefId = 0; // Index of the correspondng TofPoint

                        new((*fTofHitsColl)[fiNbHits]) CbmTofHit( iDetId,
                                            hitPos, hitPosErr,
                                            iRefId,
                                            dWeightedTime,
                                            vPtsRef.size() ); // flag  = number of TofPoints generating the cluster
                        // Using the SetLinks/GetLinks of the TofHit class seems to conflict
                        // with something in littrack QA
//                        ((CbmTofHit*)fTofHitsColl->At(fiNbHits))->SetLinks(vPtsRef[0]);
                        fiNbHits++;
                        // For Histogramming
                        fviClusterSize[iSmType][iRpc].push_back(iNbChanInHit);
                        fviTrkMul[iSmType][iRpc].push_back( vPtsRef.size() );
                        fvdX[iSmType][iRpc].push_back(dWeightedPosX);
                        fvdY[iSmType][iRpc].push_back(dWeightedPosY);
                        fvdDifX[iSmType][iRpc].push_back( vPtsRef[0]->GetX() - dWeightedPosX);
                        fvdDifY[iSmType][iRpc].push_back( vPtsRef[0]->GetY() - dWeightedPosY);
                        fvdDifCh[iSmType][iRpc].push_back( fGeoHandler->GetCell( vPtsRef[0]->GetDetectorID() ) -1 -iLastChan );
                        vPtsRef.clear();
                     } // else of if( 1 == fDigiBdfPar->GetChanOrient( iSmType, iRpc ) )
                  } // if( 0 < iNbChanInHit)
               } // for each sm/rpc pair
         } // for( Int_t iSmType = 0; iSmType < iNbSmTypes; iSmType++ )
      } // else of if( kTRUE == fDigiBdfPar->UseExpandedDigi() )
//cout<<"test"<<endl;
   return kTRUE;
}
