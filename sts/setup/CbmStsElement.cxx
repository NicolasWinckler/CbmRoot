/** @file CbmStsElement.cxx
 ** @author Volker Friese <v.friese@gsi.de>
 ** @date 27.05.2013
 **/


#include "TGeoManager.h"
#include "TGeoPhysicalNode.h"

#include "CbmStsElement.h"
#include "CbmStsModule.h"
#include "CbmStsSenzor.h"
#include "CbmStsSetup.h"

using namespace std;


// -----   Default constructor   -------------------------------------------
CbmStsElement::CbmStsElement() : TNamed(),
                                 fAddress(0),
                                 fLevel(kStsNofLevels),
                                 fNode(NULL),
                                 fDaughters()
{
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
CbmStsElement::CbmStsElement(const char* name, const char* title,
                             Int_t level,
                             TGeoPhysicalNode* node) :
                             TNamed(name, title),
                             fAddress(0),
                             fLevel(kStsSystem),
                             fNode(node),
                             fDaughters()
{
  SetLevel(level);
}
// -------------------------------------------------------------------------



// -----   Add a daughter element   ----------------------------------------
void CbmStsElement::AddDaughter(CbmStsElement* element) {
  if ( ! element ) return;
  if ( element->GetLevel() != fLevel + 1) {
    LOG(ERROR) << fName << ": Trying to add an element of level "
               << element->GetLevel() << " as daughter of level "
               << fLevel << "! Command will be ignored."
               << FairLogger::endl;
    return;

  }
  element->fAddress = CbmStsAddress::SetElementId(fAddress,
                                                  element->GetLevel(),
                                                  GetNofDaughters() );
  fDaughters.push_back(element);
}
// -------------------------------------------------------------------------



// -----   Get a daughter element   ----------------------------------------
CbmStsElement* CbmStsElement::GetDaughter(Int_t index) const {
  if ( index < 0 || index >=GetNofDaughters() ) return NULL;
  return fDaughters[index];
}
// -------------------------------------------------------------------------



// -----   Get number of elements at lower hierarchy levels   --------------
Int_t CbmStsElement::GetNofElements(Int_t level) const {

	Int_t nElements = 0;
	if ( level <= fLevel ) nElements = 0;
	else if ( level == fLevel + 1) nElements = GetNofDaughters();
	else
		for (Int_t iDaughter = 0; iDaughter < GetNofDaughters(); iDaughter++)
			nElements += GetDaughter(iDaughter)->GetNofElements(level);

	return nElements;
}
// -------------------------------------------------------------------------



// -----   Recursively read daughters from geometry   ----------------------
void CbmStsElement::InitDaughters() {

  // --- Catch absence of TGeoManager
  if ( ! gGeoManager ) {
    LOG(ERROR) << fName << ": cannot initialise without TGeoManager!"
               << FairLogger::endl;
    return;
  }

  // --- No daughter elements below sensor level
  if ( fLevel > kStsSensor ) return;

  // --- Catch physical node not being set
  if ( ! fNode ) {
    LOG(ERROR) << fName << ": physical node is not set!"
               << FairLogger::endl;
    return;
  }

  TGeoNode* mNode = fNode->GetNode();   // This node
  TString   mPath = fNode->GetName();   // Full path to this node
  Int_t nDaughters = 0;
  for (Int_t iNode = 0; iNode < mNode->GetNdaughters(); iNode++) {

    // Check name of daughter node for level name
    TString dName = mNode->GetDaughter(iNode)->GetName();
    if ( dName.Contains(CbmStsSetup::GetLevelName(fLevel + 1),
                        TString::kIgnoreCase ) ) {

      // Create physical node
      TString dPath = mPath + "/" + dName;
      TGeoPhysicalNode* pNode = new TGeoPhysicalNode(dPath.Data());

      // Create element and add it as daughter
      TString name = CbmStsSetup::GetLevelName(fLevel+1);
      name += Form("%02i", nDaughters++);
      const char* title = mNode->GetDaughter(iNode)->GetVolume()->GetName();
      CbmStsElement* dElement = NULL;
      switch ( fLevel) {
      	case kStsHalfLadder:
      		dElement = new CbmStsModule(name, title, pNode); break;
      	case kStsModule:
      		dElement = new CbmStsSenzor(name, title, pNode); break;
      	default:
      		dElement = new CbmStsElement(name, title, fLevel+1, pNode); break;
      }
      AddDaughter(dElement);

      // Call method recursively for the daughter elements
      dElement->InitDaughters();

    } // name of daughter node

  } // daughter node loop

}
// -------------------------------------------------------------------------



// -----   Print   ---------------------------------------------------------
void CbmStsElement::Print(Option_t* opt) const {
  LOG(INFO) << setw(10) << right << fAddress << "  "
		    << setw(12) << left << fName
		    << "  type " << setw(22) << fTitle << "  path "
		    << fNode->GetName() << "  " << fNode->GetTitle()
		    << FairLogger::endl;
  if ( opt[0] == 'R' ) {
	  for (Int_t iDaughter = 0; iDaughter < GetNofDaughters(); iDaughter++)
		  GetDaughter(iDaughter)->Print("R");
  }
}
// -------------------------------------------------------------------------



// -----   Set element level   ---------------------------------------------
void CbmStsElement::SetLevel(Int_t level) {
  switch (level) {
    case kStsSystem:     fLevel = kStsSystem;     break;
    case kStsStation:    fLevel = kStsStation;    break;
    case kStsLadder:     fLevel = kStsLadder;     break;
    case kStsHalfLadder: fLevel = kStsHalfLadder; break;
    case kStsModule:     fLevel = kStsModule;     break;
    case kStsSensor:     fLevel = kStsSensor;     break;
    default: LOG(ERROR) << fName << ": trying to set the level to "
                        << level << " while max. level is "
                        << kStsNofLevels - 1 << FairLogger::endl; break;
  }
}
// -------------------------------------------------------------------------


ClassImp(CbmStsElement)

