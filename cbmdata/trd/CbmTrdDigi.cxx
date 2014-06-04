#include "CbmTrdDigi.h"

#include <sstream>
using std::endl;
using std::stringstream;

CbmTrdDigi::CbmTrdDigi() 
  : CbmDigi(),
    fAddress(-1),
    fCharge(-1.),
    fTime(-1.),
    fPrimeTriggered(false),
    fFNR_Triggered(false)
{
}

CbmTrdDigi::CbmTrdDigi(
      Int_t address,
      Double_t charge,
      Double_t time)
  : CbmDigi(),
    fAddress(address),
    fCharge(charge),
    fTime(time),
    fPrimeTriggered(false),
    fFNR_Triggered(false)
{
}

CbmTrdDigi::CbmTrdDigi(
		       Int_t address,
		       Double_t charge,
		       Double_t time, 
		       Bool_t primeTriggered, 
		       Bool_t fnr_Triggered)
  : CbmDigi(),
    fAddress(address),
    fCharge(charge),
    fTime(time),
    fPrimeTriggered(fPrimeTriggered),
    fFNR_Triggered(fFNR_Triggered)
{
}

CbmTrdDigi::~CbmTrdDigi()
{
}

string CbmTrdDigi::ToString() const {
   stringstream ss;
   ss << "CbmTrdDigi: address=" << fAddress << " charge=" << fCharge
         << " time=" << fTime << endl;
   return ss.str();
}

ClassImp(CbmTrdDigi)
