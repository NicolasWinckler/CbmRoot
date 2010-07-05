/** CbmPrimaryGenerator.h
 *@author V.Friese  <v.friese@gsi.de>
 *@since 11.05.2009
 *
Handling of MC input for CBM. Derived from FairPrimaryGenerator.
Additional functionality: Setting of event plane angle and impact parameter.
To be registered to FairRunSim.
**/
 

#ifndef CBMPRIMARYGENERATOR_H
#define CBMPRIMARYGENERATOR_H

#include "FairPrimaryGenerator.h"

class CbmMCEvent;
class TF1;

class CbmPrimaryGenerator : public FairPrimaryGenerator
{

 public:

  /** Default constructor. **/
  CbmPrimaryGenerator();


  /** Constructor with name and title **/
  CbmPrimaryGenerator(const char* name, const char *title="CBM Generator");


  /** Destructor. **/
  virtual ~CbmPrimaryGenerator();


  /** Intialisation
   ** Creates and registeres MC event class
   **/
  virtual void Init();


  /** Public method GenerateEvent
      Called at the beginning of each event from FairMCApplication.
      Generates an event vertex and calls the ReadEvent methods from the 
      registered generators.
      *@param pStack The particle stack
      *@return kTRUE if successful, kFALSE if not
  **/
  virtual Bool_t GenerateEvent(FairGenericStack* pStack);


  /** Set the pointer to the MCEvent **/
  void SetEvent(CbmMCEvent* event) { fEvent = event; };


  /** Accessor to the MCEvent **/
  CbmMCEvent* GetEvent() { return fEvent; };

  

 private:

  CbmMCEvent* fEvent;         //! Pointer to MCEvent
  Bool_t      fdoTracking;    //! go to tracking  
  Double_t    fEventTimeMin;  //! min time for one event (ns)
  Double_t    fEventTimeMax;  //! max time for one Event (ns)
  Double_t    fEventTime;     //! Time of event since th start (ns)
  Double_t    fEventMeanTime; //! EventMean time used (P(t)=1/fEventMeanTime*Exp(-t/fEventMeanTime)
  TF1*        fTimeProb;      //! used to generate random numbers for event time;


  ClassDef(CbmPrimaryGenerator,3);

};

#endif


