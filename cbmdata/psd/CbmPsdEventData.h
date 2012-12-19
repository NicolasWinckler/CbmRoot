/** CbmPsdHit.h
 **@author Alla Maevskaya <alla@inr.ru>
 **@since 23.10.2012
 **@version 1.0
 **
 ** Data class for PSD reconstruction
 ** Energy deposition per module
 **/


#ifndef CBMPSDEventData_H
#define CBMPSDEventData_H 1


#include "CbmDetectorList.h"
#include "TNamed.h"

#include <vector>

class CbmPsdEventData : public TNamed
{

 public:

  /**   Default constructor   **/
  CbmPsdEventData() ;
  CbmPsdEventData(const char *name) ;
  
  // CbmPsdEventData(Float_t rp, Float_t b) ;
  
  
  /**   Destructor   **/
  virtual ~CbmPsdEventData();
  
 
  
  /**   Setters - Getters   **/
  
  Float_t GetRP() const { return fRP; }
  void SetRP(Float_t rp) {fRP = rp;}
  
  Float_t GetImpact() const { return fB; }
  void SetB(Float_t b) {  fB = b; }
  
   void Print();
  
 private:


  /**   Data members  **/
 
   Float_t fRP;         // reaction plane
   Float_t fB;          // impact parameter

  
  ClassDef(CbmPsdEventData,1);

};


#endif
