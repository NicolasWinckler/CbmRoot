/** CbmGeoMuchPar class
 * @author  E.Kryshen <e.kryshen@gsi.de>
 * @version 1.0
 * @since   12.02.08
 *
 * @author  A.Kiseleva
 * @version 0.0
 * @since   13.04.06
 * 
 *  Class for geometry parameters of MUon CHambers
 *
 */

#ifndef CBMGEOMUCHPAR_H
#define CBMGEOMUCHPAR_H

#include "CbmParGenericSet.h"
#include "TObjArray.h"

class CbmGeoMuchPar : public CbmParGenericSet {
public:
  TObjArray            *fGeoSensNodes; // List of CbmGeoNodes for sensitive volumes
  TObjArray            *fGeoPassNodes; // List of CbmGeoNodes for passive volumes
  TObjArray            *fStations;
  
  CbmGeoMuchPar(const char* name="CbmGeoMuchPar",
		const char* title="Much Geometry Parameters",
		const char* context="TestDefaultContext");
  ~CbmGeoMuchPar(void);
  void clear(void);
  void putParams(CbmParamList*);
  Bool_t getParams(CbmParamList*);
  TObjArray             *GetGeoSensitiveNodes(){return fGeoSensNodes;}
  TObjArray             *GetGeoPassiveNodes(){return fGeoPassNodes;}
  TObjArray             *GetStations(){return fStations;}
  Int_t                  GetNStations() const {return fStations->GetEntriesFast();}

  ClassDef(CbmGeoMuchPar,1)
};

#endif /* !CBMGEOMUCHPAR_H */
