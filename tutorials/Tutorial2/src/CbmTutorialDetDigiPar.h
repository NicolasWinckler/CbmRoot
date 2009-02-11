// -------------------------------------------------------------------------
// -----                    CbmTrdDigiPar header file                  -----
// -----                 Created 15/05/08  by F. Uhlig                 -----
// -------------------------------------------------------------------------

#ifndef CBMTUTORIALDETDIGIPAR_H
#define CBMTUTORIALDETDIGIPAR_H


#include "FairParGenericSet.h"

#include "TObject.h"
#include "TObjArray.h"
#include "TArrayF.h"
#include "TArrayI.h"

class FairParIo;
class FairParamList;


class CbmTutorialDetDigiPar : public FairParGenericSet 
{

 public:

  /** Standard constructor **/
  CbmTutorialDetDigiPar(const char* name    = "CbmTutorialDetDigiPar",
			const char* title   = "Tutorial Det digi parameters",
			const char* context = "Default");


  /** Destructor **/
  virtual ~CbmTutorialDetDigiPar();


  /** Initialisation from input device**/
  //virtual Bool_t init(FairParIo* input);


  /** Output to file **/
  //  virtual Int_t write(FairParIo* output);

  //  virtual void print(); 
  virtual void printparams();

  
  /** Reset all parameters **/
  virtual void clear();

  void putParams(FairParamList*);
  Bool_t getParams(FairParamList*);

  // private:

  TArrayF ftutdetdigipar;  //
  TArrayI ftutdetdigiparsector; //
  Int_t ftutdetdigiparstation; //


  ClassDef(CbmTutorialDetDigiPar,1);


};


#endif





