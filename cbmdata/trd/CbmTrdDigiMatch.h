//* $Id: *//

// -------------------------------------------------------------------------
// -----                    CbmTrdDigiMatch header file                -----
// -----                  Created 27/02/09  by F. Uhlig                -----
// -------------------------------------------------------------------------


/** CbmTrdDigiMatch.h
 **@author F. Uhlig <f.uhlig@gsi.de>
 **@since 27.02.09
 **@version 1.0
 **
 ** Data class for matching CbmTrdDigi with CbmTrdPoint. Up to ten
 ** matches are possible.
 ** The indizes of CbmTrdDigi and CbmTrdDigiMatch correspond in their
 ** respective arrays.
 ** The class holds the indizes of all TrdPoints corresponding to
 ** the TrdDigi.
 **/


#ifndef CBMTRDDIGIMATCH_H
#define CBMTRDDIGIMATCH_H 1


#include "TObject.h"
#include "TArrayI.h"

class CbmTrdDigiMatch : public TObject
{

 public:

  /** Default constructor **/
  CbmTrdDigiMatch();

  /** Standard constructor **/
  CbmTrdDigiMatch(Int_t iPoint);

  /** Destructor **/
  virtual ~CbmTrdDigiMatch();

  /** Add a TrdPoint index
   *@value Number of indizes present **/
  Int_t AddPoint(Int_t iPoint);

  /** Accessor to TrdPoint indizes **/
  Int_t GetRefIndex(Int_t i = 0) const;

  /** Return number of references **/
  Int_t GetNofRefs() const {return fRefIndex.GetSize();}

 private:

  //  Int_t fNrRefIndex;
  TArrayI fRefIndex;

  ClassDef(CbmTrdDigiMatch,1);

};

#endif


  
