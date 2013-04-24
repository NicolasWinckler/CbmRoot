/**
 * \file CbmTrdDigitizer.h
 * \author Florian Uhlig <f.uhlig@gsi.de>
 * \date 2008
 * \brief Simple TRD digitizer.
 * Update 24.04.2013 by Andrey Lebedev
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
class CbmTrdModule;
class CbmTrdRadiator;
class CbmTrdGeoHandler;

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
   
    void AddDigi(
          Int_t pointId);

    Float_t fEfficiency; // Digi production efficiency (0-100%)

    TClonesArray* fTrdPoints; //! Trd MC points
    TClonesArray* fTrdDigis; //! TRD digis
    TClonesArray* fTrdDigiMatches; //! Corresponding MCPoints to TRD digis
    TClonesArray* fMCTracks;  //! MC Track information

    CbmTrdDigiPar* fDigiPar;
    CbmTrdModule* fModuleInfo;
    CbmTrdRadiator* fRadiator;

    CbmTrdGeoHandler* fGeoHandler; //!
    
    // map to store digis for pair of x,y position in module
    // map<pair<ModuleID,pair<x,y>>, CbmTrdDigi*>
    map<pair< Int_t, pair< Int_t, Int_t > >, CbmTrdDigi* > fDigiMap;

    CbmTrdDigitizer(const CbmTrdDigitizer&);
    CbmTrdDigitizer& operator=(const CbmTrdDigitizer&);

    ClassDef(CbmTrdDigitizer, 4)
};
#endif // CBMTRDDIGITIZER_H_
