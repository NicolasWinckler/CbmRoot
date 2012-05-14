/**
 * \file CbmLitSubstation.h
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2008
 * \brief Detector substation.
 **/

#ifndef CBMLITSUBSTATION_H_
#define CBMLITSUBSTATION_H_

#include "base/CbmLitFloat.h"
#include "utils/CbmLitUtils.h"
#include "TGeoMatrix.h"
#include <boost/assign/list_of.hpp>
#include <string>
#include <set>
using boost::assign::list_of;

/**
 * \class CbmLitSubstation
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2008
 * \brief Detector substation.
 **/
class CbmLitSubstation
{
public:
   /**
    * \brief Constructor.
    */
   CbmLitSubstation(): fZ(0.) {}

   /**
    * \brief Destructor.
    */
   virtual ~CbmLitSubstation() {}

   /**
    * \brief Set Z position of substation.
    * \param[in] z Z position.
    */
   void SetZ(litfloat z) { fZ = z; }

   /**
    * \brief Return Z position of substation.
    * \return Z position of station.
    */
   litfloat GetZ() const { return fZ; }

   void AddModule(Int_t moduleId, TGeoMatrix* matrix) {
	   TGeoMatrix* rotationMatrix = NULL;
	   Int_t rotationIndex = -1;
	   for (Int_t iRot = 0; iRot < fRotationMatrices.size(); iRot++) {
		   const Double_t* rotRotation = fRotationMatrices[iRot]->GetRotationMatrix();
		   const Double_t* rotation = matrix->GetRotationMatrix();
		   Bool_t isEqual = true;
		   for (Int_t i = 0; i < 9; i++) if (rotation[i] != rotRotation[i]) { isEqual = false; break; }
		   if (!isEqual) { continue; } else { rotationMatrix = matrix; rotationIndex = iRot; break; }
	   }
	   if (rotationMatrix != NULL) { // Add to existing
		   fModules[rotationIndex].insert(moduleId);
	   } else { // Add new
		   fModules.push_back(list_of(moduleId));
		   fRotationMatrices.push_back(matrix);
	   }
   }

   Int_t GetNofModuleRotations() const {
	   return fModules.size();
   }

   Int_t GetModuleRotationId(Int_t moduleId) const {
	   Int_t nofModuleRotations = fModules.size();
	   for (Int_t i = 0; i < nofModuleRotations; i++) {
		   const std::set<Int_t>& moduleIds = fModules[i];
		   if (moduleIds.find(moduleId) != moduleIds.end()) return i;
	   }
	   return -1;
   }

   /**
    * \brief Return string representation of class.
    * \return String representation of class.
    */
   virtual std::string ToString() const {
      std::string str = std::string("Substation: z=") + lit::ToString<litfloat>(GetZ()) + "\n";
      Int_t nofRotationMatrices = fRotationMatrices.size();
      for (Int_t i = 0; i < nofRotationMatrices; i++) {
    	  const std::set<Int_t>& modules = fModules[i];
    	  Int_t nofModules = modules.size();
    	  str += " rotation: nofModules=" + lit::ToString<Int_t>(nofModules) + " (";
    	  for (std::set<Int_t>::const_iterator j = modules.begin(); j != modules.end(); j++) {
    		  str += lit::ToString<Int_t>(*j) + " ";
    	  }
    	  str += ")\n";
      }
      return str;
   }

private:
   litfloat fZ; // Z position of substation

   // [module rotation id][module id]
   std::vector<std::set<Int_t> > fModules;
   // Temporary storage for rotation matrix from tracking geometry
   std::vector<TGeoMatrix*> fRotationMatrices;
};

#endif /*CBMLITSUBSTATION_H_*/
