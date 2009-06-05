#ifndef CbmTrdStringToken_h
#define CbmTrdStringToken_h


// Backport of TStringToken which comes with Root v5.20.0

#include "TString.h"
#include "TPRegexp.h"

class CbmTrdStringToken : public TString {

protected:
   const TString fFullStr;
   TPRegexp      fSplitRe;
   Bool_t        fReturnVoid;
   Int_t         fPos;

public:
   CbmTrdStringToken(const TString& fullStr, const TString& splitRe, Bool_t retVoid=kFALSE);
   virtual ~CbmTrdStringToken() {}

   Bool_t NextToken();
   Bool_t AtEnd() const { return fPos >= fFullStr.Length(); }

   ClassDef(CbmTrdStringToken,0) // String tokenizer using PCRE for finding next token
};

#endif
