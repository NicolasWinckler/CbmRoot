// --------------------------------------------------------------------------------------
// CbmRichRingFinderHough source file
// Base class for ring finders based on on HT method
// Implementation: Semen Lebedev (s.lebedev@gsi.de)

#include "CbmRichRingFinderHough.h"

#include <iostream>
#include <cmath>
#include <set>
#include <algorithm>
#include <fstream>

using std::cout;
using std::endl;
using std::vector;

// -----   Standard constructor   ------------------------------------------
CbmRichRingFinderHough::CbmRichRingFinderHough  ( )
{
    fRingCount = 0;
    fNEvent = 0;

	fHTImpl = new CbmRichRingFinderHoughImpl();
	fHTImpl->Init();
}

CbmRichRingFinderHough::~CbmRichRingFinderHough()
{
	delete fHTImpl;
}

int CbmRichRingFinderHough::DoFind(const vector<CbmRichHoughHit>& data)
{
	fNEvent++;
	std::vector<CbmRichHoughHit> UpH;
	std::vector<CbmRichHoughHit> DownH;
    fRingCount = 0;

	UpH.reserve(data.size()/2);
	DownH.reserve(data.size()/2);

	for(int iHit = 0; iHit < data.size(); iHit++) {
		CbmRichHoughHit hit = data[iHit];
		if (hit.fHit.fY >= 0)
			UpH.push_back(hit);
		else
			DownH.push_back(hit);
	}

	fHTImpl->SetData(UpH);
	fHTImpl->DoFind();
	int nofFoundRings = fHTImpl->GetFoundRings().size();
	//if (rRingArray!=NULL) AddRingsToOutputArray(rRingArray, fHTImpl->GetFoundRings());
	UpH.clear();

	fHTImpl->SetData(DownH);
	fHTImpl->DoFind();
	nofFoundRings += fHTImpl->GetFoundRings().size();
	//if (rRingArray!=NULL) AddRingsToOutputArray(rRingArray, fHTImpl->GetFoundRings());
	DownH.clear();

}

