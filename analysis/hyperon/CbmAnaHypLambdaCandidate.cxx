#include "CbmAnaHypPair.h"
#include "CbmAnaHypLambdaCandidate.h"

ClassImp(CbmAnaHypLambdaCandidate)

CbmAnaHypLambdaCandidate::CbmAnaHypLambdaCandidate() {}

CbmAnaHypLambdaCandidate::CbmAnaHypLambdaCandidate(CbmAnaHypPair v0, CbmStsTrack tr)
    : CbmAnaHypPair(v0)
{
   track = CbmStsTrack(tr);
}

CbmAnaHypLambdaCandidate::~CbmAnaHypLambdaCandidate() {}

