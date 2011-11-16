/*
 * CbmLitQaHistNames.h
 *
 *  Created on: 02.11.2011
 *      Author: slebedev
 */

#ifndef CBMLITQAHISTNAMES_H_
#define CBMLITQAHISTNAMES_H_

#include <vector>
#include <string>
#include <boost/assign/list_of.hpp>

enum LitQaNameOption {kTracking, kRich, kElid, kDetAcc};

using namespace boost::assign;
using namespace std;

class CbmLitQaHistNames {
public:
   /**
    *
    */
   static const vector<string>& GetTypes(
        LitQaNameOption opt) {

      if (opt == kTracking || opt == kRich || opt == kElid){
         return fTypes;
      } else {
         return fTypeDetAcc;
      }
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

      if (opt == kTracking || opt == kRich || opt == kElid){
         return fTypes.size();
      } else {
         return fTypeDetAcc.size();
      }
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
   const static vector<string> fTypes;// = list_of("Acc", "Rec", "Eff");
   const static vector<string> fTypeDetAcc;// = list_of("Mc", "Acc", "Eff");

   const static vector<string> fCatTracking;// = list_of("All", "Ref", "Prim", "Sec", "Mu", "El");
   const static vector<string> fCatRich;// = list_of("All", "AllRef", "El", "ElRef", "Pi", "PiRef");
   const static vector<string> fCatElid;// = list_of("ElId", "PiSupp");
   const static vector<string> fCatDetAcc;// = list_of("El");
};


#endif /* CBMLITQAHISTNAMES_H_ */
