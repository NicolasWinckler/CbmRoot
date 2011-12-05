/**
 * \file CbmLitTrackingQaHistNames.h
 * \brief Helper class for histogram names.
 * \author Semen Lebedev <s.lebedev@gsi.de>
 * \date 2011
 */

#ifndef CBMLITTRACKINGQAHISTNAMES_H_
#define CBMLITTRACKINGQAHISTNAMES_H_

#include <vector>
#include <string>
using std::vector;
using std::string;

enum LitQaNameOption { kTracking, kRich, kElid, kDetAcc };

/**
 * \class CbmLitTrackingQaHistNames
 * \brief Helper class for histogram names.
 * \author Semen Lebedev <s.lebedev@gsi.de>
 * \date 2011
 */
class CbmLitTrackingQaHistNames
{
public:
   /**
    *
    */
   static const vector<string>& GetTypes(
        LitQaNameOption opt) {
      return (opt == kTracking || opt == kRich || opt == kElid) ? fTypes : fTypeDetAcc;
   }

   /**
    *
    */
   static const vector<string>& GetCategories(
         LitQaNameOption opt) {
      if (opt == kTracking) {
         return fCatTracking;
      } else if (opt == kRich) {
         return fCatRich;
      } else if (opt == kElid) {
         return fCatElid;
      } else {
         return fCatDetAcc;
      }
   }

   /**
    *
    */
   static int GetNofTypes(
         LitQaNameOption opt) {
      return (opt == kTracking || opt == kRich || opt == kElid) ? fTypes.size() : fTypeDetAcc.size();
   }

   /**
    *
    */
   static int GetNofCategories(
         LitQaNameOption opt) {
      if (opt == kTracking){
         return fCatTracking.size();
      } else if (opt == kRich){
         return fCatRich.size();
      } else if (opt == kElid){
         return fCatElid.size();
      } else {
         return fCatDetAcc.size();
      }
   }

private:
   const static vector<string> fTypes;
   const static vector<string> fTypeDetAcc;

   const static vector<string> fCatTracking;
   const static vector<string> fCatRich;
   const static vector<string> fCatElid;
   const static vector<string> fCatDetAcc;
};


#endif /* CBMLITTRACKINGQAHISTNAMES_H_ */
