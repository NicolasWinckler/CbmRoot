// -------------------------------------------------------------------------
// -----                      CbmMvdHit header file                    -----
// -----                 Created 07/11/06  by V. Friese                -----
// -------------------------------------------------------------------------


/** CbmMvdHit.h
 *@author V.Friese <v.friese@gsi.de>
 **
 ** Data class for hits in the CBM-MVD. 
 ** Data level RAW.
 ** Original source (CbmStsMapsHit) by M. Deveaux.
 ** 
 ** Hit flag not used up to now (will come later with real hit finding).
 **/


#ifndef CBMMVDHIT_H
#define CBMMVDHIT_H 1


#include "CbmTrkHit.h"

#include "TVector3.h"

class CbmMvdHit : public CbmTrkHit 
{

 public:    

  /** Default constructor **/
  CbmMvdHit();


  /** Standard constructor 
  *@param statNr Station number
  *@param pos    Position coordinates [cm]
  *@param dpos   Errors in position coordinates [cm]
  *@param flag   Hit flag
  **/
  CbmMvdHit(Int_t statNr, TVector3& pos, TVector3& dpos, Int_t flag);


  /** Destructor **/
  virtual ~CbmMvdHit();    


  /** Output to screen (not yet implemented) **/
  virtual void Print(const Option_t* opt = 0) const;


  /** Accessors **/
  Int_t GetSystemId()  const { 
    return ( ( fDetectorID & (15<<24) ) >> 24 ); }
  virtual Int_t GetStationNr() const { 
    return ( fDetectorID & (255<<16) ) >> 16; }
  Int_t GetFlag()      const { return fFlag; }; 

  

 protected:

  Int_t fFlag;     // Hit flag; to be used later

    
  ClassDef(CbmMvdHit,1);

};


#endif
