/**
 * \file CbmMuchDigitizeStraws.h
 * \brief Digitizer for MUCH straw detectors.
 **/

#ifndef CBMMUCHDIGITIZESTRAWS_H
#define CBMMUCHDIGITIZESTRAWS_H 1

#include "FairTask.h"
#include "TString.h"

class TClonesArray;
class CbmMuchGeoScheme;

class CbmMuchDigitizeStraws : public FairTask
{
public:

   /**
   * \brief Constructor.
   */
   CbmMuchDigitizeStraws(const char* digiFileName);

   /**
   * \brief Destructor.
   */
   virtual ~CbmMuchDigitizeStraws();

   /**
   * \brief Inherited from FairTask.
   */
   virtual void Exec(Option_t* opt);

   /**
   * \brief Inherited from FairTask.
   **/
   virtual void Finish();

   /**
   * \brief Inherited from FairTask.
   **/
   virtual InitStatus Init();

private:

   CbmMuchGeoScheme* fGeoScheme; // TODO add comments
   TString fDigiFile; // Digitization file

   TClonesArray* fPoints; // Input array of CbmMuchPoint
   TClonesArray* fDigis; // Output array of CbmMuchDigi
   TClonesArray* fDigiMatches; // Output array of CbmMuchDigiMatches

   CbmMuchDigitizeStraws(const CbmMuchDigitizeStraws&);
   CbmMuchDigitizeStraws& operator=(const CbmMuchDigitizeStraws&);

   ClassDef(CbmMuchDigitizeStraws,1)
};
#endif
