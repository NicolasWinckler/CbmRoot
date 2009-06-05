#include "CbmTrdStringToken.h"
#include "TArrayI.h"

ClassImp(CbmTrdStringToken)

//_____________________________________________________________________________
CbmTrdStringToken::CbmTrdStringToken(const TString& fullStr, const TString& splitRe, Bool_t retVoid) :
   fFullStr    (fullStr),
   fSplitRe    (splitRe),
   fReturnVoid (retVoid),
   fPos        (0)
{
   // Constructor.
}

//_____________________________________________________________________________
Bool_t CbmTrdStringToken::NextToken()
{
   // Get the next token, it is stored in this TString.
   // Returns true if new token is available, false otherwise.

   TArrayI x;
   while (fPos < fFullStr.Length()) {
      if (fSplitRe.Match(fFullStr, "", fPos, 2, &x)) {
         TString::operator=(fFullStr(fPos, x[0] - fPos));
         fPos = x[1];
      } else {
         TString::operator=(fFullStr(fPos, fFullStr.Length() - fPos));
         fPos = fFullStr.Length() + 1;
      }
      if (Length() || fReturnVoid)
         return kTRUE;
   }

   // Special case: void-strings are requested and the full-string
   // ends with the separator. Thus we return another empty string.
   if (fPos == fFullStr.Length() && fReturnVoid) {
      TString::operator=("");
      fPos = fFullStr.Length() + 1;
      return kTRUE;
   }

   return kFALSE;
}
           
