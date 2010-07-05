/* Generated by Together */
#include "CbmPipe.h"

#include "CbmGeoPipe.h"
#include "CbmGeoPassivePar.h"

#include "FairGeoLoader.h"
#include "FairGeoInterface.h"
#include "FairGeoNode.h"
#include "FairRuntimeDb.h"
#include "FairRun.h"

#include "TList.h"
#include "TObjArray.h"

CbmPipe::~CbmPipe()
{
}
CbmPipe::CbmPipe()
{
}

CbmPipe::CbmPipe(const char * name, const char * title)
  : FairModule(name ,title)
{
}
void CbmPipe::ConstructGeometry(){

    FairGeoLoader *loader=FairGeoLoader::Instance();
	FairGeoInterface *GeoInterface =loader->getGeoInterface();
	CbmGeoPipe *MGeo=new CbmGeoPipe();
	MGeo->setGeomFile(GetGeometryFileName());
	GeoInterface->addGeoModule(MGeo);
	Bool_t rc = GeoInterface->readSet(MGeo);
	if ( rc ) MGeo->create(loader->getGeoBuilder());


        FairGeoNode *volu=MGeo->getVolume("pipevac1");
	if (volu) {
		TList *masterNodes=GeoInterface->getMasterNodes();
      		masterNodes->Add(new FairGeoNode(*volu));
    	}

        TList* volList = MGeo->getListOfVolumes();
        // store geo parameter
        FairRun *fRun = FairRun::Instance();
        FairRuntimeDb *rtdb= FairRun::Instance()->GetRuntimeDb();
        CbmGeoPassivePar* par=(CbmGeoPassivePar*)(rtdb->getContainer("CbmGeoPassivePar"));
        TObjArray *fSensNodes = par->GetGeoSensitiveNodes();
        TObjArray *fPassNodes = par->GetGeoPassiveNodes();

        TListIter iter(volList);
        FairGeoNode* node   = NULL;
        FairGeoVolume *aVol=NULL;

        while( (node = (FairGeoNode*)iter.Next()) ) {
            aVol = dynamic_cast<FairGeoVolume*> ( node );
            if ( node->isSensitive()  ) {
                fSensNodes->AddLast( aVol );
            }else{
                fPassNodes->AddLast( aVol );
            }
        }
	ProcessNodes( volList );
        par->setChanged();
        par->setInputVersion(fRun->GetRunId(),1);
}
	
ClassImp(CbmPipe)

