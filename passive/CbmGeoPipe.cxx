//*-- AUTHOR : Ilse Koenig
//*-- Created : 10/11/2003

/////////////////////////////////////////////////////////////
// CbmGeoPipe
//
// Class for geometry of support structure
//
/////////////////////////////////////////////////////////////

#include "CbmGeoPipe.h"

#include "CbmGeoLoader.h"
#include "CbmGeoInterface.h"
    
ClassImp(CbmGeoPipe)

CbmGeoPipe::CbmGeoPipe() {
  // Constructor
  fName="pipe";
  strcpy(modName,"p");
  strcpy(eleName,"p");
  maxSectors=0;
  maxModules=1;
}
Bool_t  CbmGeoPipe::create ( CbmGeoBuilder * build ) {
    Bool_t rc = CbmGeoSet::create( build );
    if ( rc ) {
	CbmGeoLoader *loader=CbmGeoLoader::Instance();
	CbmGeoInterface *GeoInterface =loader->getGeoInterface();

        GeoInterface->getMasterNodes()->Add( (TObject*)  getVolume("pipevac1") );

    }
 return rc;
}

CbmGeoPipe::~CbmGeoPipe() {

}

