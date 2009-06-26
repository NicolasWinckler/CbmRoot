/** CbmLitEnvironment.h
 *@author A.Lebedev <alebedev@jinr.ru>
 *@since 2007
 **
 **
 **/

#ifndef CBMLITENVIRONMENT_H_
#define CBMLITENVIRONMENT_H_

#include "CbmLitDetectorLayout.h"

#include <vector>
#include <string>

class FairField;
class TGeoNode;
class TGeoMaterial;

class CbmLitEnvironment
{
public:
   virtual ~CbmLitEnvironment();

   static CbmLitEnvironment* Instance();

   FairField* GetField();
   CbmLitDetectorLayout GetLayout();

   CbmLitDetectorLayout GetMuchLayout() {return fMuchLayout;}

   CbmLitDetectorLayout GetTofLayout();
   CbmLitStation GetTofStation();

   bool IsElectronSetup() const;
   bool IsTrd() const;
   bool IsMuch() const;
   bool IsTof() const;

protected:
   CbmLitEnvironment();

   void CombineMuchAndTrd();
   void MuchLayout();
   void NewMuchLayout();
   void OldMuchLayout();
   void TrdLayout();

private:
   static CbmLitEnvironment* fInstance;

   CbmLitDetectorLayout fLayout;
   CbmLitDetectorLayout fMuchTrdLayout;
   CbmLitDetectorLayout fMuchLayout;
   CbmLitDetectorLayout fTrdLayout;
   CbmLitDetectorLayout fTofLayout;
   CbmLitStation fTofStation;

   FairField *fField;

   void DetermineLayout(
		   const std::vector<CbmLitStation>& stations,
		   CbmLitDetectorLayout& layout);

   std::vector<CbmLitStation> DivideToSubstations(
		   const std::vector<CbmLitStation>& stations);

   bool IsStraw() const;
   bool IsTrdSegmented() const;

   bool CheckDetectorPresence(
		   const std::string& name) const;
};

#endif // CBMLITENVIRONMENT_H_
