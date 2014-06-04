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
#include <list>
using std::string;
using std::list;

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
   CbmTrdDigi(Int_t address, Double_t charge, Double_t time, Bool_t primeTriggered, Bool_t fnr_Triggered);
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

   Bool_t GetPrimeTriggerStatus() const { return fPrimeTriggered; }

   Bool_t GetFNR_TriggerStatus() const { return fFNR_Triggered; }

   std::list <Int_t> GetNeighbourTriggerIds() const {return fNeighbourTriggerIds; }

   /** Accessors **/
   void SetAddress(Int_t address) { fAddress = address; }
   void SetCharge(Double_t charge) { fCharge = charge; }
   void SetTime(Double_t time) { fTime = time; }
   void SetPrimeTriggerStatus(Bool_t primeTriggered)  { fPrimeTriggered = primeTriggered; }
   void SetFNR_TriggerStatus(Bool_t fnr_Triggered)  { fFNR_Triggered = fnr_Triggered; }
   void AddNeighbourTriggerId(Int_t digiId) { fNeighbourTriggerIds.push_back(digiId); }

   /** Modifiers **/
   void AddCharge(Double_t charge) { fCharge += charge; }

   string ToString() const;
  
private:
   Int_t fAddress; ///< Unique channel address
   Double_t fCharge; ///< Charge
   Double_t fTime; ///< Absolute time [ns]
   Bool_t fPrimeTriggered;
   Bool_t fFNR_Triggered;
   std::list <Int_t> fNeighbourTriggerIds;
   ClassDef(CbmTrdDigi, 4);
};

#endif
