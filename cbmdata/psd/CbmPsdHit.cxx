
/** CbmPsdHit.cxx
 **@author Alla Maevskaya <alla@inr.ru>
 **@since 3.08.20212
  **/


#include "CbmPsdHit.h"
#include <iostream>
using std::cout;
using std::endl;


// -----   Default constructor   -------------------------------------------
CbmPsdHit::CbmPsdHit() 
  : TObject(),
    fModuleID(-1) 
{
  
    for (Int_t j=0; j<44; j++)
      fEdep[j]=0;
  
}
CbmPsdHit::CbmPsdHit(Int_t module, Float_t edep) 
  : TObject(),
    fModuleID(module)
{ 
    for (Int_t j=0; j<44; j++)
      fEdep[j] = edep;
  
}



// -----   Destructor   ----------------------------------------------------
CbmPsdHit::~CbmPsdHit() { }
// -------------------------------------------------------------------------

void CbmPsdHit::Print() {
  cout<<"module : "<<fModuleID <<" ELoss "<<fEdep[fModuleID]  <<  endl;

}

ClassImp(CbmPsdHit)
