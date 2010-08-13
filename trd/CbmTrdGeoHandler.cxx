// -------------------------------------------------------------------------
// -----                 CbmTrdGeoHandler source file                  -----
// -----                 Created 13/08/10  by F. Uhlig                 -----
// -------------------------------------------------------------------------


#include "CbmTrdGeoHandler.h"

#include "TGeoVolume.h"
#include "TGeoNode.h"
#include "TGeoManager.h"

#include <iostream>
#include <vector>
using std::cout;
using std::endl;

CbmTrdGeoHandler::CbmTrdGeoHandler() 
{
}

Bool_t CbmTrdGeoHandler::GetLayerInfo(std::vector<Int_t> &layersBeforeStation)
{

  // Check if old geometry version with kepping volumes for each layer
  // or new geometry version with modules put directly in the mother
  // volume for a TRD station
  TGeoVolume *fm=NULL;
  fm = (TGeoVolume *)gGeoManager->GetListOfVolumes()->FindObject("trd1layer");
  if (fm) {
    cout<<"Found old TRD geometry version."<<endl;
    //    return kTRUE;     
    return GetLayerInfoFromOldGeometry(layersBeforeStation);
  } else {
    cout<<"Found new TRD geometry version."<<endl;
    //    return kTRUE;     
    return GetLayerInfoFromNewGeometry(layersBeforeStation);
  }
}

Bool_t CbmTrdGeoHandler::GetLayerInfoFromOldGeometry(std::vector<Int_t> &layersBeforeStation)
{
    // Extract information about the number of TRD stations and
    // the number of layers per TRD station from the geomanager.
    // Store the information about the number of layers at the entrance
    // of subsequent stations in a vector. 
    // This allows to calculate the layer number starting with 1 for the
    // first layer of the first station at a later stage by only adding 
    // the layer number in the station to the number of layers in 
    // previous stations 
    TGeoVolume *fm=NULL;
    Int_t stationNr = 1;
    Int_t totalNrOfLayers = 0;
    layersBeforeStation.push_back(totalNrOfLayers);
    char volumeName[10];
    sprintf(volumeName, "trd%d", stationNr);
    fm = (TGeoVolume *)gGeoManager->GetListOfVolumes()->FindObject(volumeName);  
    if (fm){
      Int_t nrOfLayers = fm->GetNdaughters();
      totalNrOfLayers += nrOfLayers;
      layersBeforeStation.push_back(totalNrOfLayers);
      do {
	stationNr++;
        sprintf(volumeName, "trd%d", stationNr);
	fm = (TGeoVolume *)gGeoManager->GetListOfVolumes()->FindObject(volumeName);  
        if (fm) {
          nrOfLayers = fm->GetNdaughters();
	  totalNrOfLayers += nrOfLayers;
	  layersBeforeStation.push_back(totalNrOfLayers);
	}
      } while (fm);
    } else {
      cout << "***************************************" <<endl;
      cout << "                                       " <<endl;
      cout << " - FATAL ERROR Unknown geometry version" <<endl;
      cout << "   in CbmTrdHitProducerSmearing        " <<endl;
      cout << " No TRD stations found in the geometry " <<endl;
      cout << "                                       " <<endl;
      cout << "***************************************" <<endl;
      return kFALSE;
    }

    return kTRUE;
}

Bool_t CbmTrdGeoHandler::GetLayerInfoFromNewGeometry(std::vector<Int_t> &layersBeforeStation)
{

    TGeoVolume *fm=NULL;
    TGeoNode *node = NULL;
    

    Int_t stationNr = 1;
    Int_t totalNrOfLayers = 0;
    Int_t totalNrOfStations = 0;
    layersBeforeStation.push_back(totalNrOfLayers);
    char volumeName[15];


    // Now extract first the number of stations
    // If a geometry without keeping volumes for the TRD station is
    // found the execution of the program is stopped
    sprintf(volumeName, "trd%d", stationNr);
    fm = (TGeoVolume *)gGeoManager->GetListOfVolumes()->FindObject(volumeName);
    if (fm) {
      do {
	sprintf(volumeName, "trd%d", stationNr);
	fm = (TGeoVolume *)gGeoManager->GetListOfVolumes()->FindObject(volumeName);  
	totalNrOfStations++;
        stationNr++;
      } while (fm);
      totalNrOfStations--;
    } else {
      cout << "***************************************" <<endl;
      cout << "                                       " <<endl;
      cout << " - FATAL ERROR Unknown geometry version" <<endl;
      cout << "   in CbmTrdHitProducerSmearing        " <<endl;
      cout << " No TRD stations found in the geometry " <<endl;
      cout << "                                       " <<endl;
      cout << "***************************************" <<endl;
      return kFALSE;
    }


    Int_t layersPerStation[totalNrOfStations];
    Int_t maxModuleType = 3;
    Int_t maxLayer = 6 ;


    // Now loop over the stations and extract the number of layers
    // for each station. Get a pointer to the station volume. With
    // this get all nodes which belong to this volume. Loop for all
    // possible differnt module types over all theoratically possible 
    // layers. The highest found layer number is the number of layers
    // for a station.     
    


    for ( Int_t iStation=1; iStation<=totalNrOfStations; iStation++) {

      sprintf(volumeName, "trd%d", iStation);
      fm = (TGeoVolume *)gGeoManager->GetListOfVolumes()->FindObject(volumeName);  

      // Check if a volume trdXmodY#1001 exists (first copy of trd 
      // module type Y in trad station X). Start with the highest
      // possible layer number. The layer of the first found module
      // is the number of layers in this station

      Bool_t foundModule=kFALSE;
      

      for ( Int_t iLayer=maxLayer; iLayer>=1; iLayer--) {
	for ( Int_t iModuleType=1; iModuleType<=maxModuleType; iModuleType++) {

	  sprintf(volumeName, "trd%dmod%d_%d001", iStation, iModuleType, iLayer);
          node = (TGeoNode *) fm->GetNodes()->FindObject(volumeName);
          if (node) {
	    foundModule=kTRUE;
	    break;
          }
	}

        if (foundModule){
          totalNrOfLayers += iLayer;
	  layersBeforeStation.push_back(totalNrOfLayers);
	  layersPerStation[iStation-1]=iLayer;
	  break;
	}
      }

    }

    
    for ( Int_t iStation=1; iStation<=totalNrOfStations; iStation++) {

      cout<<"TRD Station "<< iStation <<" has "<< layersPerStation[iStation-1] 
          <<" layers."<<endl;
    }

    return kTRUE;

}

ClassImp(CbmTrdGeoHandler)
