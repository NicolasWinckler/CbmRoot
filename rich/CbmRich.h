/**
* \file CbmRich.h
*
* \brief Defines the active detector RICH. Constructs the geometry and creates MCPoints.
*
* \author Volker Friese
* \date 2004
**/

#ifndef CBM_RICH
#define CBM_RICH


#include "FairDetector.h"

#include "TVector3.h"

class TClonesArray;
class CbmRichRefPlanePoint;
class CbmRichPoint;
class CbmRichMirrorPoint;
class FairVolume; 

/**
* \class CbmRich
*
* \brief Defines the active detector RICH. Constructs the geometry and creates MCPoints.
*
* \author Volker Friese
* \date 2004
**/
class CbmRich : public FairDetector 
{

public:

   /**
   * \brief Default constructor.
   */
   CbmRich();


   /**
    * \brief Standard constructor.
    * \param[in] name Detector name.
    * \param[in] active Sensitivity flag.
    */
   CbmRich(
         const char* name,
         Bool_t active);


   /**
    * \brief Destructor.
    */
   virtual ~CbmRich();


   /**
    * \brief Initialize detector. Stores volume IDs for RICH detector and mirror.
    */
   virtual void Initialize();


   /**
    * \brief Defines the action to be taken when a step is inside the
    * active volume. Creates CbmRichPoints and CbmRichMirrorPoints and adds
    * them to the collections.
    * \param[in] vol Pointer to the active volume.
    */
   virtual Bool_t ProcessHits(
         FairVolume* vol = 0);


   /**
    * \brief If verbosity level is set, print hit collection at the
    * end of the event and resets it afterwards.
    */
   virtual void EndOfEvent();


   /**
    * \brief Registers the hit collection in the ROOT manager.
    */
   virtual void Register();


   /**
    * \brief Return hit collection.
    */
   virtual TClonesArray* GetCollection(
         Int_t iColl) const;


   /**
    * \brief Screen output of hit collection.
    */
   virtual void Print() const;


   /**
    * \brief Clears the hit collection.
    */
   virtual void Reset();


   /**
    * \brief Copies the hit collection with a given track index offset.
    * \param[in] cl1 Origin array.
    * \param[out] cl2 Target array.
    * \param[in] offset Index offset.
    */
   virtual void CopyClones(
         TClonesArray* cl1,
         TClonesArray* cl2,
         Int_t offset);


   /**
    * \brief Construct geometry.
    */
   virtual void ConstructGeometry();

   /**
    * \brief Put some optical properties.
    */
   void ConstructOpGeometry();

private:

   Int_t fPosIndex;
   Int_t volDetector; // MC volume ID of RICH photodetector
   Int_t volRefPlane; //  MC volume ID of RICH reference plane
   Int_t volMir; // MC volume ID of RICH Mirror (3 parts)
   Int_t volMir1; // MC volume ID of RICH Mirror
   Int_t volMir2; // MC volume ID of RICH Mirror

   TClonesArray* fRichCollection; // Hit collection (photodetector)
   TClonesArray* fRichRefPlaneCollection; // Hit collection (reference plane)
   TClonesArray* fRichMirrorCollection; // Hit collection (Mirror)

   /**
    * \brief Adds a RichPoint to the HitCollection.
    */
   CbmRichPoint* AddHit(
         Int_t trackID,
         Int_t detID,
         TVector3 pos,
         TVector3 mom,
         Double_t time,
         Double_t length,
         Double_t eLoss);

   /**
    * \brief Adds a RichRefPlanePoint to the HitCollection.
    */
   CbmRichPoint* AddRefPlaneHit(
         Int_t trackID,
         Int_t detID,
         TVector3 pos,
         TVector3 mom,
         Double_t time,
         Double_t length,
         Double_t eLoss);

   /**
    * \brief Adds a RichMirrorPoint to the HitCollection.
    */
   CbmRichPoint* AddMirrorHit(
         Int_t trackID,
         Int_t detID,
         TVector3 pos,
         TVector3 mom,
         Double_t time,
         Double_t length,
         Double_t eLoss);

   /**
    * \brief Copy constructor.
    */
   CbmRich(const CbmRich&);

   /**
    * \brief Assignment operator.
    */
   CbmRich& operator=(const CbmRich&);

   ClassDef(CbmRich,2)
};


#endif
