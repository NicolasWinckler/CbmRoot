/** CbmLitEnvironment.h
 *@author A.Lebedev <alebedev@jinr.ru>
 *@since 2007
 **
 ** Class is an interface to the CBM detector classes and field.
 ** The detector geometry which is represented in the form of
 ** CbmLitDetectorLayout class constructed in this class.
 ** This class implements singleton pattern.
 **/

#ifndef CBMLITENVIRONMENT_H_
#define CBMLITENVIRONMENT_H_

#include "base/CbmLitDetectorLayout.h"
#include "parallel/muon/LitDetectorLayoutMuon.h"
#include "parallel/electron/LitDetectorGeometryElectron.h"

#include <vector>
#include <string>

class FairField;
class TGeoNode;
class TGeoMaterial;

class CbmLitEnvironment
{
public:
   /* Returns pointer to the singleton object instance. */
   static CbmLitEnvironment* Instance();

   /* Returns pointer to the magnetic field map. */
   FairField* GetField();

   /* Returns reference to the detector layout. */
   const CbmLitDetectorLayout& GetLayout();

   /* Returns reference to the MUCH detector layout. */
   const CbmLitDetectorLayout& GetMuchLayout() {return fMuchLayout;}

   /* Returns reference to the TRD detector layout. */
   const CbmLitDetectorLayout& GetTrdLayout() {return fTrdLayout;}

   /* Returns reference to the TRD detector layout in SIMD format. */
   void GetTrdLayoutVec(lit::parallel::LitDetectorLayoutElectronVec& layout);

   /* Returns reference to the TRD detector layout in scalar format. */
   void GetTrdLayoutScal(lit::parallel::LitDetectorLayoutElectronScal& layout);

   /* Template function that returns reference to the TRD detector layout. */
   template<class T> void GetTrdLayout(lit::parallel::LitDetectorLayoutElectron<T>& layout);

   /* Returns reference to the MVD detector layout. */
   const CbmLitDetectorLayout& GetMvdLayout();

   /* Returns index of the TRD material needed for the construction of the
    * simplified layout for the parallel tracking. */
   int TrdMaterialId(
      int stationGroup,
      int station,
      const CbmLitDetectorLayout& layout) const;

   /* Returns reference to the TOF detector layout. */
   const CbmLitDetectorLayout& GetTofLayout();

   /* Returns reference to the TOF detector layout. */
   const CbmLitStation& GetTofStation();

private:
   /* Constructor is protected since singleton pattern is used.
    * Pointer to the object is returned by static Instance() method. */
   CbmLitEnvironment();

   /* Destructor */
   virtual ~CbmLitEnvironment();

   /* Copy constructor */
   CbmLitEnvironment(const CbmLitEnvironment&);

   /* Assignment operator */
   const CbmLitEnvironment& operator=(const CbmLitEnvironment&);

   /* Combines MUCH and TRD layout. */
   void CombineMuchAndTrd();

   /* Constructs MUCH layout. */
   void MuchLayout();

   /* Constructs TRD layout. */
   void TrdLayout();

   /* Constructs MVD layout */
   void MvdLayout();

   CbmLitDetectorLayout fLayout; // current detector layout
   CbmLitDetectorLayout fMuchTrdLayout; // MUCH+TRD detector layout
   CbmLitDetectorLayout fMuchLayout; // MUCH detector layout
   CbmLitDetectorLayout fTrdLayout; // TRD detector layout
   CbmLitDetectorLayout fTofLayout; // TOF detector layout
   CbmLitStation fTofStation; // TOF station
   CbmLitDetectorLayout fMvdLayout; // MVD detector layout

   FairField* fField; // pointer to the magnetic field
};

#endif // CBMLITENVIRONMENT_H_
