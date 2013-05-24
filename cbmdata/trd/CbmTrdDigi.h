/**
 * \file CbmTrdDigi.h
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 03.05.2013
 **/

#ifndef CBMTRDDIGI_H
#define CBMTRDDIGI_H 1

#include "CbmDigi.h"
#include "CbmTrdAddress.h"

#include <string>
using std::string;

class CbmTrdDigi : public CbmDigi
{
public:
   /**
    * \brief Default constructor.
    */
   CbmTrdDigi();

   /**
    * \brief Constructor with assignment.
    * \param[in] address Unique channel address.
    * \param[in] charge Charge.
    * \param[in] time Absolute time [ns].
    */
   CbmTrdDigi(Int_t address, Double_t charge, Double_t time);

   /**
    * \brief Destructor.
    */
   ~CbmTrdDigi();

   /**
    * \brief Inherited from CbmDigi.
    */
   Int_t GetAddress() const { return fAddress; };

   /**
    * \brief Inherited from CbmDigi.
    */
   Double_t GetCharge() const { return fCharge; }

   /**
    * \brief Inherited from CbmDigi.
    */
   Int_t GetSystemId() const { return CbmTrdAddress::GetSystemId(fAddress); }

   /**
    * \brief Inherited from CbmDigi.
    */
   Double_t GetTime() const { return fTime; }

   /** Accessors **/
   void SetAddress(Int_t address) { fAddress = address; }
   void SetCharge(Double_t charge) { fCharge = charge; }
   void SetTime(Double_t time) { fTime = time; }

   /** Modifiers **/
   void AddCharge(Double_t charge) { fCharge += charge; }

   string ToString() const;
  
private:
   Int_t fAddress; ///< Unique channel address
   Double_t fCharge; ///< Charge
   Double_t fTime; ///< Absolute time [ns]

   ClassDef(CbmTrdDigi, 3);
};

#endif
