/**
 * \file CbmTrdDigitizer.h
 * \author Florian Uhlig <f.uhlig@gsi.de>
 * \date 2008
 * \brief Simple TRD digitizer.
 * Updated 24/04/2013 by Andrey Lebedev <andrey.lebedev@gsi.de>
 **/

#ifndef CBMTRDDIGITIZER_H_
#define CBMTRDDIGITIZER_H_

#include "FairTask.h"

#include <map>

using std::map;
using std::pair;

class TClonesArray;

class CbmTrdDigiPar;
class CbmTrdDigi;
class CbmTrdDigiMatch;
class CbmTrdModule;
class CbmTrdRadiator;

class CbmTrdDigitizer : public FairTask
{
public:

   /**
    * \brief Constructor.
    * \param[in] raiator TRD radiator to be used in digitization.
    */
   CbmTrdDigitizer(
       CbmTrdRadiator* radiator = NULL);

   /**
    * \brief Destructor.
    **/
   virtual ~CbmTrdDigitizer();

   /**
    * \brief Inherited from FairTask.
    */
   virtual InitStatus Init();

   /**
    * \brief Inherited from FairTask.
    */
   virtual void SetParContainers();

   /**
    * \brief Inherited from FairTask.
    */
   virtual void Exec(Option_t * option);

   /**
    * \brief Inherited from FairTask.
    */
   virtual void Finish();

private:

    /**
     * \brief Create digi and digi match and add to temporary map.
     * \param[in] pointId MC point index.
     */
    void AddDigi(
          Int_t pointId);

    CbmTrdDigitizer(const CbmTrdDigitizer&);
    CbmTrdDigitizer& operator=(const CbmTrdDigitizer&);

    Float_t fEfficiency; // Digi production efficiency [0..1]

    TClonesArray* fTrdPoints; //! Trd MC points
    TClonesArray* fTrdDigis; //! TRD digis
    TClonesArray* fTrdDigiMatches; //! Corresponding MCPoints to TRD digis
    TClonesArray* fMCTracks;  //! MC Track information

    CbmTrdDigiPar* fDigiPar;
    CbmTrdModule* fModuleInfo;
    CbmTrdRadiator* fRadiator;
    
    // Temporary storage for digis.
    // map<address, pair<CbmTrdDigi*, CbmTrdDigiMatch*>
    map<Int_t, pair<CbmTrdDigi*, CbmTrdDigiMatch*> > fDigiMap;

    ClassDef(CbmTrdDigitizer, 4);
};
#endif // CBMTRDDIGITIZER_H_
