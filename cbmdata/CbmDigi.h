/** @file CbmDigi.h
 * @author Volker Friese <v.friese@gsi.de>
 * @date 26.03.2013
 */


// -------------------------------------------------------------------------
// -----                         CbmDigi header file                   -----
// -----                  Created 09/05/07  by V. Friese               -----
// -------------------------------------------------------------------------

/** @class CbmDigi
 ** @author V.Friese <v.friese@gsi.de>
 ** @since 09.05.07
 ** @version 2.0
 ** @brief Base class for persistent representation of digital information.
 **
 ** CbmDigi is an abstract base class for the ROOT representation of
 ** the smallest information unit delivered by the detector front-ends.
 ** It is equivalent to the message of a single electronics channel.
 ** The information content is the channel address (unique identifier),
 ** the time stamp and (optionally) the charge (ADC).
 **
 ** Unlike the data class used to actually transport the information
 ** through the data acquisition ("message"), the digi contains
 ** context-free information, i.e. absolute time and unique address.
 **
 ** The class contains a pointer to an object of type FairMultiLinkedData,
 ** where the link information to MCPoints can be stored and retrieved.
 ** If there is no such information, the pointer will be NULL.
 **/


#ifndef CBMDIGI_H
#define CBMDIGI_H 1

#include <iostream>

#include "TObject.h"
#include "FairMultiLinkedData.h"



class CbmDigi : public TObject
{

 public:

  //* Default constructor
  CbmDigi();


  //* Copy constructor
  CbmDigi(const CbmDigi& digi);


  //* Destructor
  virtual ~CbmDigi();


  //* Add a link to MCPoint
  void AddLink(Int_t file, Int_t entry, TString branch,
               Int_t index, Float_t weight) {
    if ( ! fLinks ) fLinks = new FairMultiLinkedData();
    fLinks->AddLink(FairLink(file, entry, branch, index, weight));
  }


  //* Unique channel address
  virtual Int_t    GetAddress() const = 0;


  //* Charge (optional)
  virtual Double_t GetCharge()  const { return 0.; }


  /** Get a link to MCPoint
   ** @param iLink  Number of link in list
   ** @value Pointer to FairLink object. If iLink is out of range,
   ** a new, empty FairLink is created.
   **/
  FairLink GetLink(Int_t iLink) {
    if ( ! fLinks ) {
      FairLink link;
      return link;
    }
    return fLinks->GetLink(iLink);
  }


  //* Monte-Carlo link
  FairMultiLinkedData* GetLinkObject() const { return fLinks; }


  //* System (enum DetectorId)
  virtual Int_t    GetSystemId() const = 0;


  //* Absolute time [ns]
  virtual Double_t GetTime()    const = 0;


  //* Assignment operator
  CbmDigi& operator=(const CbmDigi& digi);

  

 protected:

  FairMultiLinkedData* fLinks;



  ClassDef(CbmDigi,2);

};

#endif
