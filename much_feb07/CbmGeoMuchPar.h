/** CbmGeoMuchPar class
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
#include "TH1F.h"

class CbmGeoMuchPar : public CbmParGenericSet {
public:
  TObjArray            *fGeoSensNodes; // List of CbmGeoNodes for sensitive volumes
  TObjArray            *fGeoPassNodes; // List of CbmGeoNodes for passive volumes

  CbmGeoMuchPar(const char* name="CbmGeoMuchPar",
		const char* title="Much Geometry Parameters",
		const char* context="TestDefaultContext");
  ~CbmGeoMuchPar(void);
  void clear(void);
  void putParams(CbmParamList*);
  Bool_t getParams(CbmParamList*);
  TObjArray             *GetGeoSensitiveNodes(){return fGeoSensNodes;}
  TObjArray             *GetGeoPassiveNodes(){return fGeoPassNodes;}

  ClassDef(CbmGeoMuchPar,1)
};

#endif /* !CBMGEOMUCHPAR_H */
