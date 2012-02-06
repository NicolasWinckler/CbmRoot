/**
* \file CbmRichFitRings.h
*
* \brief FairTask for ring fitting in RICH. Input is TClonesArray of CbmRichRing.
*  Uses as ring fitting algorithm classes derived from CbmRichRingFitter.
*
* \author Supria Das
* \date 2006
**/

#ifndef CBMRICHFITRINGS
#define CBMRICHFITRINGS


#include "FairTask.h"

class CbmRichRingFitter;
class TClonesArray;

/**
 * \class CbmRichFitRings
 *
 * \brief FairTask for ring fitting in RICH.
 *
 * \author Supria Das
 * \date 2006
 **/
class CbmRichFitRings : public FairTask
{

public:

   /**
    * \brief Standard constructor.
    * \param[in] fitter Pointer to RICH ring fitter concrete class.
    */
   CbmRichFitRings(
        CbmRichRingFitter* fitter);

   /**
    * \brief Destructor.
    */
   virtual ~CbmRichFitRings();


   /**
    * \brief Initialisation at beginning of run.
    */
   virtual InitStatus Init();

   /**
    * \brief Task execution for each event.
    * \param[in] opt Options.
    */
   virtual void Exec(
         Option_t* opt);

   /**
    * \brief Finish at the end of run.
    */
   virtual void Finish();

   /**
    * \brief Return pointer to the current fitter algorithm.
    */
   CbmRichRingFitter* GetFitter(){
      return fFitter;
   };

   /**
    * \brief Set concrete ring fitter algorithm.
    * \param[in] fitter Pointer to concrete fitter algorithm.
    */
   void UseFitter(
         CbmRichRingFitter* fitter){
      fFitter = fitter;
   };

private:

   CbmRichRingFitter* fFitter; // Pointer to TrackFinder concrete class
   TClonesArray* fRingArray; // Input array of RICH Rings

   ClassDef(CbmRichFitRings,1);
};

#endif
