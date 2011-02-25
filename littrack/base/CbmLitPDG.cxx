/** CbmLitPDG.css
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2008
 * @version 1.0
 **/

#include "base/CbmLitPDG.h"

#include "TDatabasePDG.h"
#include "TParticlePDG.h"

#include <cstdlib>
#include <iostream>

CbmLitPDG::CbmLitPDG()
{
}

CbmLitPDG::~CbmLitPDG()
{
}

myf CbmLitPDG::GetMass(
		int pdg)
{
	TDatabasePDG* db = TDatabasePDG::Instance();
	return db->GetParticle(pdg)->Mass();
}

bool CbmLitPDG::IsElectron(
		int pdg)
{
	if (std::abs(pdg) == 11) return true;
	else return false;
}

bool CbmLitPDG::IsMuon(
		int pdg)
{
	if (std::abs(pdg) == 13) return true;
	else return false;
}

bool CbmLitPDG::IsPion(
		int pdg)
{
	if (std::abs(pdg) == 211) return true;
	else return false;
}
