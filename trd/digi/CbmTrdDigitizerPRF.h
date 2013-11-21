#ifndef CBMTRDDIGITIZERPRF_H
#define CBMTRDDIGITIZERPRF_H

#include "FairTask.h"

#include <map>
using std::map;
using std::pair;

class CbmTrdPoint;
class CbmTrdDigiPar;
class CbmTrdModule;
class CbmTrdRadiator;
class CbmTrdGeoHandler;
class CbmMatch;
class CbmTrdDigi;

class TClonesArray;

class CbmTrdDigitizerPRF : public FairTask
{
public:

   /**
   * \brief Standard constructor.
   **/
   CbmTrdDigitizerPRF(CbmTrdRadiator* radiator = NULL);

   /**
   * \brief Destructor.
   **/
   virtual ~CbmTrdDigitizerPRF();

   /**
   * \brief Inherited from FairTask
   **/
   virtual InitStatus Init();

   /**
   * \brief Inherited from FairTask
   **/
   virtual void SetParContainers();

   /**
   * \brief Inherited from FairTask
   **/
   virtual void Exec(Option_t * option);

private:

   CbmTrdDigitizerPRF& operator=(const CbmTrdDigitizerPRF&);
   CbmTrdDigitizerPRF(const CbmTrdDigitizerPRF&);

   Double_t CalcPRF(Double_t x, Double_t W, Double_t h);

   void ScanPadPlane(const Double_t* local_point, Double_t clusterELoss);

   void SplitTrackPath(const CbmTrdPoint* point, Double_t ELoss);

   void AddDigi(Int_t pointId, Int_t address, Double_t charge, Double_t time);

   Double_t fTime;
   Int_t fnRow;
   Int_t fnCol;
   Int_t fnSec;
   Int_t fLayerId;
   Int_t fModuleId;
   Int_t fModuleAddress;

   Int_t fModuleType;
   Int_t fModuleCopy;
   Int_t fMCPointId;

   TClonesArray* fPoints; //! CbmTrdPoint array
   TClonesArray* fDigis; //! CbmTrdDigi array
   TClonesArray* fDigiMatches; //! CbmMatch array
   TClonesArray* fMCTracks;  //! CbmMCTrack array

   CbmTrdDigiPar* fDigiPar; //!
   CbmTrdModule* fModuleInfo; //!
   CbmTrdRadiator* fRadiators; //!
   CbmTrdGeoHandler* fGeoHandler; //!

   // Temporary storage for digis.
   std::map<Int_t, pair<CbmTrdDigi*, CbmMatch*> > fDigiMap;

   ClassDef(CbmTrdDigitizerPRF,2)
};
#endif // CBMTRDDIGITIZERPRF_H
