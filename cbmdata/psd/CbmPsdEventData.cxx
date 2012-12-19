
/** CbmPsdEventData.cxx
 **@author Alla Maevskaya <alla@inr.ru>
 **@since 3.08.20212
  **/


#include "CbmPsdEventData.h"
#include <iostream>
using std::cout;
using std::endl;


// -----   Default constructor   -------------------------------------------
CbmPsdEventData::CbmPsdEventData() 
  : TNamed(),
    fRP(-9999),
    fB(-1) 
{
}  
// -----   constructor   -------------------------------------------
CbmPsdEventData::CbmPsdEventData(const char *name) 
  : TNamed(),
    fRP(-9999),
    fB(-1) 
{
  TString namst = "RP_";
  namst += name;
  SetName(namst.Data());
  SetTitle(namst.Data());
  
}
/*
CbmPsdEventData::CbmPsdEventData(Float_t rp, Float_t b) 
  : TNamed(),
    fRP(rp),
    fB(b) 
{ 
 
}

*/

// -----   Destructor   ----------------------------------------------------
CbmPsdEventData::~CbmPsdEventData() { }
// -------------------------------------------------------------------------

void CbmPsdEventData::Print() {
  cout<<"REACTION PLANE : "<<fRP <<" IMPACT PARAMETER "<<fB <<  endl;

}

ClassImp(CbmPsdEventData)
