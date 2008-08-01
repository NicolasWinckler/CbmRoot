#include "CbmLitRobustAna.h"

CbmLitRobustAna* CbmLitRobustAna::fInstance = NULL;

CbmLitRobustAna::CbmLitRobustAna()
{
	CreateHistos();
}

CbmLitRobustAna::~CbmLitRobustAna()
{
	if (fInstance != NULL) delete fInstance;
}

CbmLitRobustAna* CbmLitRobustAna::Instance()
{
	if (fInstance == NULL) {
	   fInstance = new CbmLitRobustAna();
	}
	return fInstance;
}

void CbmLitRobustAna::CreateHistos()
{
	
}

ClassImp(CbmLitRobustAna);
