/** @file CbmTofSimpClusterizer.h
 ** @author Pierre-Alain Loizeau <loizeau@physi.uni-heidelberg.de>
 ** @date 23.08.2013
 **/

/** @class CbmTofSimpClusterizer
 ** @brief Simple Cluster building and hit producing for CBM ToF using Digis as input
 ** @author Pierre-Alain Loizeau <loizeau@physi.uni-heidelberg.de>
 ** @version 1.0
 **/
#ifndef CBMTOFSIMPCLUSTERIZER_H
#define CBMTOFSIMPCLUSTERIZER_H  1

// TOF Classes and includes
   // Input/Output
//class CbmTofPoint;
class CbmTofDigi;
class CbmTofDigiExp;
   // Geometry
class CbmTofGeoHandler;
class CbmTofDetectorId;
class CbmTofDigiPar;
class CbmTofDigiBdfPar;
class CbmTofCell;

// FAIR classes and includes
#include "FairTask.h"

// ROOT Classes and includes
class TClonesArray;
class TH1;
class TH2;
#include "TTimeStamp.h"

// C++ Classes and includes
#include <vector>

class CbmTofSimpClusterizer : public FairTask
{
   public:

      /**
       ** @brief Constructor.
       **/
      CbmTofSimpClusterizer();

      /**
       ** @brief Constructor.
       **/
      CbmTofSimpClusterizer(const char *name, Int_t verbose = 1, Bool_t writeDataInOut = kTRUE);
      /**
       ** @brief Destructor.
       **/
      virtual ~CbmTofSimpClusterizer();

      /**
       ** @brief Inherited from FairTask.
       **/
      virtual InitStatus Init();

      /**
       ** @brief Inherited from FairTask.
       **/
      virtual void SetParContainers();

      /**
       ** @brief Inherited from FairTask.
       **/
      virtual void Exec(Option_t * option);

      /**
       ** @brief Inherited from FairTask.
       **/
      virtual void Finish();

   protected:

   private:
      /**
       ** @brief Copy constructor.
       **/
      CbmTofSimpClusterizer(const CbmTofSimpClusterizer&);
      /**
       ** @brief Copy operator.
       **/
      CbmTofSimpClusterizer& operator=(const CbmTofSimpClusterizer&);

      // Functions common for all clusters approximations
      /**
       ** @brief Recover pointer on input TClonesArray: TofPoints, TofDigis...
       **/
      Bool_t   RegisterInputs();
      /**
       ** @brief Create and register output TClonesArray of Tof Hits.
       **/
      Bool_t   RegisterOutputs();
      /**
       ** @brief Initialize other parameters not included in parameter classes.
       **/
      Bool_t   InitParameters();
      /**
       ** @brief Load the geometry: for now just resizing the Digis temporary vectors
       **/
      Bool_t   LoadGeometry();
      /**
       ** @brief Delete the geometry related arrays: for now just clearing the Digis temporary vectors
       **/
      Bool_t   DeleteGeometry();

      // Histogramming functions
      Bool_t   CreateHistos();
      Bool_t   FillHistos();
      Bool_t   WriteHistos();
      Bool_t   DeleteHistos();

      /**
       ** @brief Build clusters out of ToF Digis and store the resulting info in a TofHit.
       **/
      Bool_t   BuildClusters();

      // ToF geometry variables
      CbmTofGeoHandler      * fGeoHandler;
      CbmTofDetectorId      * fTofId;
      CbmTofDigiPar         * fDigiPar;
      CbmTofCell            * fChannelInfo;
      CbmTofDigiBdfPar      * fDigiBdfPar;

      // Input variables
      TClonesArray          * fTofPointsColl; // TOF MC points
      TClonesArray          * fMcTracksColl;  // MC tracks
      TClonesArray          * fTofDigisColl;  // TOF Digis

      // Output variables
      Bool_t                  fbWriteHitsInOut;
      TClonesArray          * fTofHitsColl; // TOF hits
      Int_t  fiNbHits;                       // Index of the CbmTofHit TClonesArray

      // Generic
      Int_t fVerbose;

      // Intermediate storage variables
      std::vector< std::vector< std::vector< std::vector< CbmTofDigi* > > > >
               fStorDigi; //[nbType][nbSm*nbRpc][nbCh][nDigis]
      std::vector< std::vector< std::vector< std::vector< CbmTofDigiExp* > > > >
               fStorDigiExp; //[nbType][nbSm*nbRpc][nbCh][nDigis]
      /*
      std::vector< std::vector< std::vector< std::vector< std::vector< CbmTofDigi* > > > > >
               fStorDigi; //[nbType][nbSm][nbRpc][nbCh][nDigis]
      std::vector< std::vector< std::vector< std::vector< std::vector< CbmTofDigiExp* > > > > >
               fStorDigiExp; //[nbType][nbSm][nbRpc][nbCh][nDigis]
      */
      std::vector< std::vector< std::vector< Int_t > > > fviClusterSize; //[nbType][nbRpc][nClusters]
      std::vector< std::vector< std::vector< Int_t > > > fviTrkMul; //[nbType][nbRpc][nClusters]
      std::vector< std::vector< std::vector< Double_t > > > fvdX; //[nbType][nbRpc][nClusters]
      std::vector< std::vector< std::vector< Double_t > > > fvdY; //[nbType][nbRpc][nClusters]
      std::vector< std::vector< std::vector< Double_t > > > fvdDifX; //[nbType][nbRpc][nClusters]
      std::vector< std::vector< std::vector< Double_t > > > fvdDifY; //[nbType][nbRpc][nClusters]
      std::vector< std::vector< std::vector< Double_t > > > fvdDifCh; //[nbType][nbRpc][nClusters]

      // Histograms
      TH1* fhClustBuildTime;
      TH1* fhHitsPerTracks;
      TH1* fhPtsPerHit;
      TH1* fhTimeResSingHits;
      TH2* fhTimeResSingHitsB;
      TH2* fhTimePtVsHits;
      TH1* fhClusterSize;
      TH2* fhClusterSizeType;
      TH1* fhTrackMul;
      TH2* fhClusterSizeMulti;
      TH2* fhTrk1MulPos;
      TH2* fhHiTrkMulPos;
      TH2* fhAllTrkMulPos;
      TH2* fhMultiTrkProbPos;
      TH1* fhDigSpacDifClust;
      TH1* fhDigTimeDifClust;
      TH2* fhDigDistClust;
      TH2* fhClustSizeDifX;
      TH2* fhClustSizeDifY;
      TH2* fhChDifDifX;
      TH2* fhChDifDifY;

         // Digis quality
      Int_t fiNbSameSide;
      TH1* fhNbSameSide;
      TH1* fhNbDigiPerChan;

      // Control
      TTimeStamp fStart;
      TTimeStamp fStop;

   ClassDef(CbmTofSimpClusterizer, 1);
};

#endif // CBMTOFSIMPCLUSTERIZER_H
