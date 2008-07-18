#ifndef CBMECALTRACKEXTRAPOLATION_H
#define CBMECALTRACKEXTRAPOLATION_H

#include "TObject.h"

class TClonesArray;
class CbmGlobalTrack;
class CbmTrackParam;
class CbmEcalStructure;

class CbmEcalTrackExtrapolation : public TObject
{
public:
  CbmEcalTrackExtrapolation(Int_t verb=0) : fVerbose(verb), fStr(NULL) {};
  virtual ~CbmEcalTrackExtrapolation() {};
  virtual void Init() {}; 
  virtual void Finish() {};
  virtual Int_t DoExtrapolate(TClonesArray* gTrackArr, TClonesArray* fTrackParam)=0;
  /** Set verbosity level **/
  inline void SetVerbose(Int_t verbose) {fVerbose=verbose;}
  inline void SetStructure(CbmEcalStructure* str) {fStr=str;}
  inline CbmEcalStructure* GetStructure() const {return fStr;}
protected:
  inline Int_t Verbose() const {return fVerbose;}
  inline CbmEcalStructure* Str() const {return fStr;}
private:
  Int_t fVerbose;
  CbmEcalStructure* fStr;

  ClassDef(CbmEcalTrackExtrapolation, 1)
};

#endif
