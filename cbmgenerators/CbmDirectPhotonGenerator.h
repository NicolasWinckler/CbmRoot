// CbmDirectPhotonGenerator based on CbmDirectPhotonGenerator
// by Dr.Sys

#ifndef CBMDIRECTPHOTONGENERATOR_H
#define CBMDIRECTPHOTONGENERATOR_H


#include "CbmGenerator.h"

class TChain;
class TVirtualMCStack;
class CbmPrimaryGenerator;



class CbmDirectPhotonGenerator : public CbmGenerator
{
public: 
  /** Default constructor (should not be used) **/
  CbmDirectPhotonGenerator();
  /** Standard constructor with input filename **/
  CbmDirectPhotonGenerator(const Char_t* fileName);
  /** Add file to input chain of the generator **/
  void AddFile(const char* filename);
  /** Destructor **/
  virtual ~CbmDirectPhotonGenerator();
  /** Reads on event from the input file and pushes the tracks onto
   ** the stack. Abstract method in base class.
   ** @param primGen  pointer to the CbmPrimaryGenerator
   **/
  virtual Bool_t ReadEvent(CbmPrimaryGenerator* primGen);
  /** Skip number of events in input **/
  inline virtual Bool_t SkipEvents(Int_t num) {for(Int_t i=0;i<num;i++) if (SkipEvent()==kFALSE) return kFALSE; return kTRUE;}
  virtual Bool_t SkipEvent();
private:
  Int_t fNEvent;	//! Event number
  TChain* fInput;	//! Pointer to input tree
  /** Momentum of the particle **/
  Double_t fPX;		//!
  Double_t fPY;		//!
  Double_t fPZ;		//!
  /** Read event number **/
  Int_t fEv;		//!
  /** Current entry of chain **/
  Int_t fEntry;		//!
  Int_t fEntries;	//!

  ClassDef(CbmDirectPhotonGenerator,1);
};

#endif
