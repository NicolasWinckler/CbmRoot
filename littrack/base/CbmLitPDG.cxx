#include "CbmLitPDG.h"

#include "TDatabasePDG.h"
#include "TParticlePDG.h"

#include <cstdlib>

CbmLitPDG::CbmLitPDG()
{
}

CbmLitPDG::~CbmLitPDG()
{
}

Double_t CbmLitPDG::GetMass(
		Int_t pdg)
{
	TDatabasePDG* db = TDatabasePDG::Instance();
	return db->GetParticle(pdg)->Mass();
}

Bool_t CbmLitPDG::IsElectron(
		Int_t pdg)
{
	if (std::abs(pdg) == 11) return true;
	else return false;
}

Bool_t CbmLitPDG::IsMuon(
		Int_t pdg)
{
	if (std::abs(pdg) == 13) return true;
	else return false;
}
ClassImp(CbmLitPDG);
