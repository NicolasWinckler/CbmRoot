#include "CbmTofCreateDigiPar.h"

#include "CbmTofCell.h"
#include "CbmTofGeoHandler.h"
#include "CbmTofDigiPar.h"

#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairBaseParSet.h"

#include <iostream>
using std::cout;
using std::endl;

#include <algorithm>

// ---- Default constructor -------------------------------------------
CbmTofCreateDigiPar::CbmTofCreateDigiPar()
  :FairTask("TofCreateDigiPar"),
   fSMType(-1),
   fSModule(-1),
   fCounter(-1),
   fGap(-1),
   fCell(-1),
   fRegion(-1),
   fSizex(-1.),
   fSizey(-1.),
   fX(-1.),
   fY(-1.),
   fZ(-1.),
   fCellID(-1),
   fCellMap(),
   fCellMapIt(),
   fDigiPar(NULL),
   fGeoHandler(new CbmTofGeoHandler())
{
}
// --------------------------------------------------------------------

// ---- Constructor ----------------------------------------------------
CbmTofCreateDigiPar::CbmTofCreateDigiPar(const char *name, const char *title)
  :FairTask(name),
   fSMType(-1),
   fSModule(-1),
   fCounter(-1),
   fGap(-1),
   fCell(-1),
   fRegion(-1),
   fSizex(-1.),
   fSizey(-1.),
   fX(-1.),
   fY(-1.),
   fZ(-1.),
   fCellID(-1),
   fCellMap(),
   fCellMapIt(),
   fDigiPar(NULL),
   fGeoHandler(new CbmTofGeoHandler())
{
}
// --------------------------------------------------------------------

// ---- Destructor ----------------------------------------------------
CbmTofCreateDigiPar::~CbmTofCreateDigiPar()
{
}
// --------------------------------------------------------------------

// ----  Initialisation  ----------------------------------------------
void CbmTofCreateDigiPar::SetParContainers(){

  LOG(INFO)<<" * CbmTofCreateDigiPar * :: SetParContainers() "<<FairLogger::endl;

  // Get Base Container
  FairRunAna* ana = FairRunAna::Instance();
  FairRuntimeDb* rtdb=ana->GetRuntimeDb();

  fDigiPar = (CbmTofDigiPar*)
             (rtdb->getContainer("CbmTofDigiPar"));

}
// --------------------------------------------------------------------

// ---- ReInit  -------------------------------------------------------
InitStatus CbmTofCreateDigiPar::ReInit(){

  LOG(INFO)<<" * CbmTofCreateDigiPar * :: ReInit() "<<FairLogger::endl;

  // Get Base Container
  FairRunAna* ana = FairRunAna::Instance();
  FairRuntimeDb* rtdb=ana->GetRuntimeDb();

  fDigiPar = (CbmTofDigiPar*)
             (rtdb->getContainer("CbmTofDigiPar"));
  
  return kSUCCESS;
}
// --------------------------------------------------------------------

// ---- Init ----------------------------------------------------------
InitStatus CbmTofCreateDigiPar::Init(){

  Int_t geoVersion = fGeoHandler->Init();

//  fModInfoMap = GetModuleInfoMap();

  LOG(INFO)<<" * CbmTofCreateDigiPar * :: Init() "<<FairLogger::endl;

  FairRootManager *ioman = FairRootManager::Instance();
  if ( ! ioman ) LOG(FATAL)<<"No FairRootManager found"<<FairLogger::endl;
  

  if (k12b == geoVersion) {
    LOG(INFO)<<"Will now create digitization parameters for root geometry."<<FairLogger::endl;
    FillCellMapRootGeometry();
  }
  if (k07a == geoVersion) {
	LOG(INFO)<<"Will now create digitization parameters for ascii geometry."<<FairLogger::endl;
	FillCellMapAsciiGeometry();
  }

  return kSUCCESS;
}
// --------------------------------------------------------------------
void CbmTofCreateDigiPar::FinishTask(){

  LOG(INFO)<<" * CbmTofCreateDigiPar * :: FinishTask() "<<FairLogger::endl;

  FairRunAna* ana = FairRunAna::Instance();
  FairRuntimeDb* rtdb=ana->GetRuntimeDb();

  fDigiPar = (CbmTofDigiPar*)
             (rtdb->getContainer("CbmTofDigiPar"));

  fDigiPar->print();
}

// ---- Exec ----------------------------------------------------------
void CbmTofCreateDigiPar::Exec(Option_t * option)
{
}

// --------------------------------------------------------------------
void CbmTofCreateDigiPar::FillCellMapAsciiGeometry(){
 
  // The geometry structure is treelike with cave as
  // the top node. For the TRD there are keeping volumes
  // trd1-trd4 for each station which are only containers 
  // for the different layers of one station. The trdlayer
  // is again only a container for all volumes of this layer.   
  // Loop over all nodes below the top node (cave). If one of
  // the nodes containes a string trd it must be one of the
  // stations. Now loop over the layers of this station and 
  // then over all modules of the layer to extract in the end
  // all active regions (gas) of the complete TRD. For each
  // of the gas volumes get the information about size and
  // position from the geomanager and the sizes of the sectors
  // and pads from the definitions in CbmTrdPads. This info
  // is then stored in a TrdModule object for each of the
  // TRD modules.

  Int_t nrCells = 0;
  std::vector<CbmTofCell*> cellVector;
  CbmTofCell *tofCell;


  TString TopNode = gGeoManager->GetTopNode()->GetName();
  LOG(DEBUG2)<<TopNode<<FairLogger::endl;

  // Loop over all detector systems to find tof part
  TObjArray* detSystems = gGeoManager->GetTopNode()->GetNodes();
  for (Int_t iSys = 0; iSys < detSystems->GetEntriesFast(); ++iSys) {
    TGeoNode* node = (TGeoNode*) detSystems->At(iSys);
    LOG(DEBUG2)<<node->GetName()<<FairLogger::endl;

    // Only do something useful for tof part of geometry
    // The node name contains a string "tof"
    // e.g. tof_vXXy or tof_v12b
    if (TString(node->GetName()).Contains("tof")) {
      TString TofNode = node->GetName();
      TGeoNode* keep = node;
      TObjArray* keeping = keep->GetNodes();

      // Loop over tof keeping volume. There should be only one.
      for (Int_t ikeep = 0; ikeep < keeping->GetEntriesFast(); ikeep++) {
        TGeoNode* keepvol = (TGeoNode*) keeping->At(ikeep);
        TString KeepNode = keepvol->GetName();
        TObjArray* modarray = keepvol->GetNodes();

        // Loop over the different found modules
        for (Int_t imodule = 0; imodule < modarray->GetEntriesFast(); imodule++) {
      	  TGeoNode* module = (TGeoNode*) modarray->At(imodule);
      	  TString ModuleNode = module->GetName();
      	  TObjArray* cellarray =module->GetNodes();

      	  // Loop over all cells of each module
      	  for (Int_t icell = 0; icell < cellarray->GetEntriesFast(); icell++) {
      	    TGeoNode* cell = (TGeoNode*) cellarray->At(icell);
      	    TString CellNode = cell->GetName();
      	    TObjArray* gaparray = cell->GetNodes();

      	    // Loop over all elements belonging to one cell
      	    // Relevant are only the gaps which are the active parts of the detector
      	    for (Int_t igap = 0; igap < gaparray->GetEntriesFast(); igap++) {
      	      TGeoNode* gap = (TGeoNode*) gaparray->At(igap);
      	      TString GapNode = gap->GetName();

      	      if (GapNode.Contains("gap")) {

      	        // Construct full path name for the gap
                // Extract the necessary geometrical information and store
      	        // this information in member variables
      	        TString FullPath = "/" + TopNode + "/" + TofNode + "/" +
      	            KeepNode + "/" + ModuleNode + "/" + CellNode + "/" + GapNode;
      	        LOG(DEBUG2) <<"Path: "<<FullPath<<FairLogger::endl;
      	        FillCellInfoFromGeoHandler(FullPath);

                // Since there are 8 gaps per cell, the information for all these
      	        // gaps are stored. After all the information is available the
      	        // position of the cell is calculated as mean position of the
      	        // 8 individual gaps. The size of each cell should be the same for
      	        // all 8 gaps.


      	        fCellMapIt =  fCellMap.find(fCellID);
      	        if ( fCellMapIt == fCellMap.end() ) {
      	          // new tof cell
                  tofCell = new CbmTofCell(fCellID, fX, fY, fZ, fSizex, fSizey);
      	          cellVector.clear();
      	          cellVector.push_back(tofCell);
      	          fCellMap.insert( std::pair<Int_t, std::vector<CbmTofCell*> >(fCellID, cellVector));
      	        }
      	        else {
      	          // already existing cell
                  tofCell = new CbmTofCell(fCellID, fX, fY, fZ, fSizex, fSizey);
                  fCellMap[fCellID].push_back(tofCell);
      	        }

      	      }
      	    }
      	  }
        }
      }
    }
  }
  // Calculate the mean position for each cell and fill the tof digi parameters
  FillDigiPar();
}

// --------------------------------------------------------------------
void CbmTofCreateDigiPar::FillCellMapRootGeometry(){
 
  // The geometry structure is treelike with cave as
  // the top node. For the TRD there are keeping volumes
  // trd1-trd4 for each station which are only containers 
  // for the different layers of one station. The trdlayer
  // is again only a container for all volumes of this layer.   
  // Loop over all nodes below the top node (cave). If one of
  // the nodes containes a string trd it must be one of the
  // stations. Now loop over the layers of this station and 
  // then over all modules of the layer to extract in the end
  // all active regions (gas) of the complete TRD. For each
  // of the gas volumes get the information about size and
  // position from the geomanager and the sizes of the sectors
  // and pads from the definitions in CbmTrdPads. This info
  // is then stored in a TrdModule object for each of the
  // TRD modules.

  Int_t nmodules = 0;
  TString TopNode = gGeoManager->GetTopNode()->GetName();

  TObjArray* nodes = gGeoManager->GetTopNode()->GetNodes();
  for (Int_t iNode = 0; iNode < nodes->GetEntriesFast(); iNode++) {
    TGeoNode* node = (TGeoNode*) nodes->At(iNode);
    if (TString(node->GetName()).Contains("trd")) {
      TString StationNode = node->GetName();
      TGeoNode* station = node;

	TObjArray* modules = station->GetNodes();
	for (Int_t iLayerPart = 0; iLayerPart < modules->GetEntriesFast(); iLayerPart++) {
          TGeoNode* module = (TGeoNode*) modules->At(iLayerPart);
          TString ModuleNode = module->GetName();

          TObjArray* parts = module->GetNodes();
	  for (Int_t iPart = 0; iPart < parts->GetEntriesFast(); iPart++) {
            TGeoNode* part = (TGeoNode*) parts->At(iPart);
            if (TString(part->GetName()).Contains("gas")) {
              TString PartNode = part->GetName();

              // Put together the full path to the interesting volume, which
	      // is needed to navigate with the geomanager to this volume.
              // Extract the geometry information (size, global position)
              // from this volume.;

              TString FullPath = "/" + TopNode + "/" + StationNode + "/" + 
                                 ModuleNode + "/" + PartNode;

              FillCellInfoFromGeoHandler(FullPath);


              nmodules++;

              // Create new CbmTrdModule and add it to the map
/*

 	      fCellMap[fCellID] =
                new CbmTofCell(fCellID, fX, fY, fZ, fSizex, fSizey);
*/	    }
	  }
      }
    }
  }

  FillDigiPar();
}


void CbmTofCreateDigiPar::FillCellInfoFromGeoHandler(TString FullPath)
{
  // Calculate the unique detector ID including the gap information.
  // Since all gaps for a given cell are added up to one channel the
  // id and corresponding information stored in the parameter container
  // should be calculated without the gap information.



  fDetID = fGeoHandler->GetUniqueDetectorId(FullPath);

  fSMType=fGeoHandler->GetSMType(fDetID);
  fSModule=fGeoHandler->GetSModule(fDetID);
  fCounter=fGeoHandler->GetCounter(fDetID);
  fGap=fGeoHandler->GetGap(fDetID);
  fCell=fGeoHandler->GetCell(fDetID);
  fRegion=fGeoHandler->GetRegion(fDetID);

  fSizex = fGeoHandler->GetSizeX(FullPath);
  fSizey = fGeoHandler->GetSizeY(FullPath);

  fX = fGeoHandler->GetX(FullPath);
  fY = fGeoHandler->GetY(FullPath);
  fZ = fGeoHandler->GetZ(FullPath);

  LOG(DEBUG2)<<"X: "<< fX  <<FairLogger::endl;
  LOG(DEBUG2)<<"Y: "<< fY <<FairLogger::endl;
  LOG(DEBUG2)<<"Z: "<< fZ <<FairLogger::endl;
  LOG(DEBUG2)<<"SizeX: "<< fSizex <<FairLogger::endl;
  LOG(DEBUG2)<<"SizeY: "<< fSizey <<FairLogger::endl;
  LOG(DEBUG2)<<"Region: "<< fRegion <<FairLogger::endl;
  LOG(DEBUG2)<<"Module: "<< fCounter <<FairLogger::endl;
  LOG(DEBUG2)<<"Gap: "<< fGap <<FairLogger::endl;
  LOG(DEBUG2)<<"Cell: "<< fCell <<FairLogger::endl;

  fCellID = fGeoHandler->GetCellId(fDetID);

  fSMType=fGeoHandler->GetSMType(fCellID);
  fSModule=fGeoHandler->GetSModule(fCellID);
  fCounter=fGeoHandler->GetCounter(fCellID);
  fGap=fGeoHandler->GetGap(fCellID);
  fCell=fGeoHandler->GetCell(fCellID);
  fRegion=fGeoHandler->GetRegion(fCellID);

  LOG(DEBUG2)<<"Cell ID: "<< fCellID << FairLogger::endl;
  LOG(DEBUG2)<<"Region: "<< fGeoHandler->GetRegion(fCellID) <<FairLogger::endl;
  LOG(DEBUG2)<<"Module: "<< fGeoHandler->GetCounter(fCellID) <<FairLogger::endl;
  LOG(DEBUG2)<<"Gap: "<< fGeoHandler->GetGap(fCellID) <<FairLogger::endl;
  LOG(DEBUG2)<<"Cell: "<< fGeoHandler->GetCell(fCellID) <<FairLogger::endl;
  LOG(DEBUG2)<<"**************************"<<FairLogger::endl;

}


void CbmTofCreateDigiPar::FillDigiPar()
{

  ofstream fout;
  fout.open("output.txt");
  fout << "#####################################################################################"<<"\n";
  fout << "# Geometry for the TOF detector"<<"\n";
  fout << "# Format:"<<"\n";
  fout << "#"<<"\n";
  fout << "# Region Module Cell   type       X[mm]           Y[mm]         Dx[mm]  Dy[mm]"<<"\n";
  fout << "#####################################################################################"<<"\n";
  fout << "[TofGeoPar]"<<"\n";

  Int_t Nrcells = (Int_t)fCellMap.size();
  LOG(INFO) <<"Nr. of modules: "<<Nrcells<<FairLogger::endl;

  fDigiPar->SetNrOfCells(Nrcells);

  TArrayI *ModuleId  = new TArrayI(Nrcells);

  Int_t iDigi=0;

  std::map<Int_t, CbmTofCell* > singleCellMap;

  CbmTofCell* singlecell;

  for ( fCellMapIt=fCellMap.begin(); fCellMapIt!=fCellMap.end(); fCellMapIt++) {

    ModuleId->AddAt(fCellMapIt->first,iDigi);
    iDigi++;

    std::vector<CbmTofCell*> vcell = fCellMapIt->second;
    Int_t cellId = fCellMapIt->first;
    // sanity check
    std::vector<CbmTofCell*>::iterator vcellIt;
    Int_t id;
    Int_t oldid;
    Bool_t first=kTRUE;
    CbmTofCell* tofcell;
    Double_t x=0.;
    Double_t y=0.;
    Double_t z=0.;
    Double_t sizex=0.;
    Double_t sizey=0.;
    for ( vcellIt=vcell.begin(); vcellIt!=vcell.end(); ++vcellIt) {
      tofcell = (*vcellIt);
      id = tofcell->GetDetectorId();
      if ( id != cellId) {
        LOG(INFO)<<"id, cellId: "<<id<<" , "<< cellId<<"\n";
        LOG(INFO)<<"id and CellId differ"<<FairLogger::endl;
      }
      x+=tofcell->GetX();
      y+=tofcell->GetY();
      z+=tofcell->GetZ();
      sizex+=tofcell->GetSizex();
      sizey+=tofcell->GetSizey();
    }

    singlecell = new CbmTofCell(cellId, x/vcell.size(), y/vcell.size(), z/vcell.size(),
        sizex/vcell.size(),sizey/vcell.size());

    singleCellMap.insert( std::pair<Int_t, CbmTofCell*>(cellId, singlecell));

    fRegion=fGeoHandler->GetRegion(cellId);
    fSModule=fGeoHandler->GetSModule(cellId);
    fCounter=fGeoHandler->GetCounter(cellId);
    fCell=fGeoHandler->GetCell(cellId);

    fout << fRegion <<"   "<< fCounter <<"   "<< fSModule <<"   "<< fCell
        <<"   "<< x/vcell.size()*10 <<"   "<< y/vcell.size()*10 <<"   "<< z/vcell.size()*10
        <<"   "<<sizex/vcell.size()*20 <<"   "<< sizey/vcell.size()*20<<"\n";
  }


  fDigiPar->SetNrOfCells(Nrcells);
  fDigiPar->SetCellIdArray(*ModuleId);
  fDigiPar->SetCellMap(singleCellMap);
}


ClassImp(CbmTofCreateDigiPar)
