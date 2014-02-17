/** @file CbmStsSetup.h
 ** @author Volker Friese <v.friese@gsi.de>
 ** @date 27.05.2013
 **/


#ifndef CBMSTSSETUP_H
#define CBMSTSSETUP_H 1

#include <map>
#include "setup/CbmStsElement.h"

class TGeoManager;


/** @class CbmStsSetup
 ** @brief Class representing the top level of the STS setup
 ** @author V.Friese <v.friese@gsi.de>
 ** @version 1.0
 **
 ** The CbmStsSetup is a singleton class and represents the
 ** interface to the CBM geometry and its elements.
 **/
class CbmStsSetup : public CbmStsElement
{

  public:

    /** Destructor **/
    virtual ~CbmStsSetup() { };


    /** Get an STS element by address
     ** @param address  Unique element address (see CbmStsAddress)
     ** @param level    Element level (EStsElementLevel)
     ** @return Pointer to STS element
     **/
    CbmStsElement* GetElement(UInt_t address, Int_t level);


    /** Get the name of an element level
     ** @param level    Element level (EStsElementLevel)
     ** @return  Name of element level (station, ladder, etc.)
     **/
    static const char* GetLevelName(Int_t level) {
      if ( level < 0 || level >= kStsNofLevels ) return "";
      return (fgkLevelName[level]).Data();
    }


    /** Initialise setup from geometry
     ** @param geo  Instance of TGeoManager
     ** @return kTRUE if successfully initialised; kFALSE else
     **/
    Bool_t Init(TGeoManager* geo);


    /** Static instance of CbmStsSetup **/
    static CbmStsSetup* Instance();



  private:

    static CbmStsSetup* fgInstance;    ///< Static instance of this class

    static const TString fgkLevelName[kStsNofLevels];  ///< Level names

    /** Default constructor  **/
    CbmStsSetup();

    ClassDef(CbmStsSetup,1);

};

#endif /* CBMSTSSETUP_H */
