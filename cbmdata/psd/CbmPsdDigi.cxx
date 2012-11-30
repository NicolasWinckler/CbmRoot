/** CbmPsdDigi.cxx
 **@author Alla Maevskaya <alla@inr.ru>
 **@since 3.08.20212
  **/


#include "CbmPsdDigi.h"
#include <iostream>
using std::cout;
using std::endl;


// -----   Default constructor   -------------------------------------------

CbmPsdDigi::CbmPsdDigi() 
  : TObject(),
    fModuleID(-1),
    fSectionID(-1)   
{
  
  for (Int_t i=0; i<10; i++)
    for (Int_t j=0; j<44; j++)
      fEdep[i][j]=-1;
  
}


CbmPsdDigi::CbmPsdDigi(Int_t section, Int_t module, Float_t edep) 
  : TObject(),
    fModuleID(module),
    fSectionID(section)    
{
 
  for (Int_t i=0; i<10; i++)
    for (Int_t j=0; j<44; j++)
      fEdep[i][j] = edep;
   
}



// -----   Destructor   ----------------------------------------------------
CbmPsdDigi::~CbmPsdDigi() { }
// -------------------------------------------------------------------------

void CbmPsdDigi::Print() {
  cout<<"module : "<<fModuleID <<" section    : "<<fSectionID <<
    " ELoss "<<fEdep[fSectionID][fModuleID]  <<  endl;


}


ClassImp(CbmPsdDigi)
