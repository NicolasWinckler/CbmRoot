// -------------------------------------------------------------------------
// -----                  CbmLitEnvironment header file               -----
// -----                  Created 16/07/07  by A. Lebedev               -----
// -------------------------------------------------------------------------


/** CbmLitEnvironment.h
 *@author A.Lebedev <alebedev@jinr.ru>
 **
 **
 **/


#ifndef CBMLITENVIRONMENT_H_
#define CBMLITENVIRONMENT_H_

#include "CbmLitDetectorLayout.h"
#include "TObject.h"

#include <vector>
#include <string>

class FairField;
class TGeoNode;
class TGeoMaterial;

class CbmLitEnvironment: public TObject {

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

   ClassDef(CbmLitEnvironment,1)
};

#endif // CBMLITENVIRONMENT_H_
