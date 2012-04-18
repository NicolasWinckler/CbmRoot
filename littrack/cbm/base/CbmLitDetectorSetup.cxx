/**
 * \file CbmLitDetectorSetup.xx
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2012
 */
#include "CbmLitDetectorSetup.h"

#include "TGeoManager.h"
#include "TObjArray.h"
#include "TGeoNode.h"

#include <cassert>
#include <iostream>

using std::cout;

CbmLitDetectorSetup::CbmLitDetectorSetup():
      fIsElectronSetup(false),
      fIsMuonSetup(false),
      fDet()
{
}

CbmLitDetectorSetup::~CbmLitDetectorSetup()
{
}

void CbmLitDetectorSetup::SetDet(
         DetectorId detId,
         bool isDet)
{
   fDet[detId] = isDet;
}

bool CbmLitDetectorSetup::GetDet(
         DetectorId detId) const
{
   assert(fDet.count(detId) != 0);
   return fDet.find(detId)->second;
}

bool CbmLitDetectorSetup::CheckDetectorPresence(
   const std::string& name) const
{
   assert(gGeoManager != NULL);

   TObjArray* nodes = gGeoManager->GetTopNode()->GetNodes();
   for (Int_t iNode = 0; iNode < nodes->GetEntriesFast(); iNode++) {
      TGeoNode* node = (TGeoNode*) nodes->At(iNode);
      if (TString(node->GetName()).Contains(name.c_str())) { return true; }
   }

   if (name == "mvd") {
      TGeoNode* node1 = gGeoManager->GetTopVolume()->FindNode("pipevac1_0");
      if (node1 && node1->GetVolume()->FindNode("mvdstation01_0")) { return true; }
   }

   return false;
}

void CbmLitDetectorSetup::DetermineSetup()
{
   fIsElectronSetup = !CheckDetectorPresence("much");
   fIsMuonSetup = CheckDetectorPresence("much");
   fDet[kMVD] = CheckDetectorPresence("mvd");
   fDet[kSTS] = CheckDetectorPresence("sts");
   fDet[kRICH] = CheckDetectorPresence("rich");
   fDet[kTRD] = CheckDetectorPresence("trd");
   fDet[kMUCH] = CheckDetectorPresence("much");
   fDet[kTOF] = CheckDetectorPresence("tof");
}

string CbmLitDetectorSetup::ToString() const
{
   string str = "-I- CBM detector setup information: \n";
   if (fIsMuonSetup) str += "   Muon setup detected \n";
   if (fIsElectronSetup) str += "   Electron setup detected \n";
   str += "   Detectors found in setup: ";
   if (fDet.find(kMVD)->second) str += "MVD ";
   if (fDet.find(kSTS)->second) str += "STS ";
   if (fDet.find(kRICH)->second) str += "RICH ";
   if (fDet.find(kMUCH)->second) str += "MUCH ";
   if (fDet.find(kTRD)->second) str += "TRD ";
   if (fDet.find(kTOF)->second) str += "TOF ";
   str += "\n";
   return str;
}
