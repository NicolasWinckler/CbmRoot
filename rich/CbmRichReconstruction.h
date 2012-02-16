/**
* \file CbmRichReconstruction.h
*
* \brief Main class for running event reconstruction in the RICH detector.
*
* \author Semen Lebedev
* \date 2012
**/

#ifndef CBM_RICH_RECONSTRUCTION
#define CBM_RICH_RECONSTRUCTION

#include "FairTask.h"

#include <string>

class TClonesArray;
class CbmRichRingFinder;
class CbmRichRingFitterBase;

using std::string;

/**
* \class CbmRichReconstruction
*
* \brief Main class for running event reconstruction in the RICH detector.
*
* \author Semen Lebedev
* \date 2012
**/
class CbmRichReconstruction : public FairTask
{
public:
   /**
    * \brief Default constructor.
    */
   CbmRichReconstruction();

   /**
    * \brief Destructor.
    */
   virtual ~CbmRichReconstruction();

   /**
    * \brief Inherited from FairTask.
    */
   virtual InitStatus Init();

   /**
    * \brief Inherited from FairTask.
    */
   virtual void Exec(
         Option_t* opt);

   /**
    * \brief Inherited from FairTask.
    */
   virtual void Finish();

   void SetRunExtrapolation(bool b){fRunExtrapolation = b;}
   void SetRunProjection(bool b){fRunProjection = b;}
   void SetRunFinder(bool b){fRunFinder = b;}
   void SetRunFitter(bool b){fRunFitter = b;}
   void SetRunTrackMatch(bool b){fRunTrackMatch = b;}

   void SetExtrapolationName(const string& n){fExtrapolationName = n;}
   void SetProjectionName(const string& n){fProjectionName = n;}
   void SetFinderName(const string& n){fFinderName = n;}
   void SetFitterName(const string& n){fFitterName = n;}
   void SetTrackMatchName(const string& n){fTrackMatchName = n;}

private:
   TClonesArray* fRichHits;
   TClonesArray* fRichRings;
   TClonesArray* fRichProjections;
 //  TClonesArray* fRichRingMatches;

   CbmRichRingFinder* fRingFinder; // pointer to ring finder algorithm
   CbmRichRingFitterBase* fRingFitter; // pointer to ring fitting algorithm

   // What do you wan to run.
   bool fRunExtrapolation;
   bool fRunProjection;
   bool fRunFinder;
   bool fRunFitter;
   bool fRunTrackMatch;

   // Algorithm names for each step of reconstruction.
   string fExtrapolationName; // name of extrapolation algorithm
   string fProjectionName; // name of track projection algorithm
   string fFinderName; // name of ring finder algorithm
   string fFitterName; // name of ring fitter algorithm
   string fTrackMatchName; // name of track-ring matching algorithm

   /**
    * \brief
    */
   void InitExtrapolation();

   /**
    * \brief
    */
   void InitProjection();

   /**
    * \brief
    */
   void InitFinder();

   /**
    * \brief
    */
   void InitFitter();

   /**
    * \brief
    */
   void InitTrackMatch();

   /**
    * \brief
    */
   void RunExtrapolation();

   /**
    * \brief
    */
   void RunProjection();

   /**
    * \brief
    */
   void RunFinder();

   /**
    * \brief
    */
   void RunFitter();

   /**
    * \brief
    */
   void RunTrackMatch();

   /**
    * \brief Copy constructor.
    */
   CbmRichReconstruction(const CbmRichReconstruction&);

   /**
    * \brief Assignment operator.
    */
   CbmRichReconstruction& operator=(const CbmRichReconstruction&);

   ClassDef(CbmRichReconstruction,1);
};

#endif
