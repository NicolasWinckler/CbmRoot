/**
 * \file LitDetectorGeometryElectron.h
 *
 * \brief Geometry description classes.
 *
 * Classes for geometry description of the 'electron' setup of CBM.
 * Detector layout consists of station groups. Each station
 * group consists of stations. In addition virtual planes
 * are introduced in order to store material and field
 * values between STS and TRD detectors.
 * Detector layout also provides access to
 * material of the detector and approximated magnetic field.
 *
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2009
 *
 **/

#ifndef LITDETECTORGEOMETRYELECTRON_H_
#define LITDETECTORGEOMETRYELECTRON_H_

#include "../LitTypes.h"
#include "../LitMaterialInfo.h"
#include "../LitField.h"
#include "../LitUtils.h"

namespace lit {
namespace parallel {

/**
 * \class LitStationElectron
 *
 * \brief Represents detector station for the CBM electron setup.
 *
 * Station stores Z position of its center and a list of
 * materials before station center and after station center.
 * This represents a layered structure of the TRD layer.
 *
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2009
 */
template<class T>
class LitStationElectron
{
public:
   /**
    * \brief Constructor.
    */
   LitStationElectron():
      fMaterialsBefore(),
      fMaterialsAfter(),
      fZ(0.) {}

   /**
    * \brief Destructor
    */
   virtual ~LitStationElectron() {};

   /**
    * \brief Adds material to the list of materials before station center.
    * \param[in] material Material to be added.
    */
   void AddMaterialBefore(const LitMaterialInfo<T>& material) {
      fMaterialsBefore.push_back(material);
   }

   /**
    * \brief Adds material to the list of materials after station center.
    * \param[in] material Material to be added.
    */
   void AddMaterialAfter(const LitMaterialInfo<T>& material) {
      fMaterialsAfter.push_back(material);
   }

   /**
    * \brief Return material from the list of materials before station center by material index.
    * \param[in] materialId Index of the material to be returned.
    * \return Material from the list which corresponds to materialId index.
    */
   const LitMaterialInfo<T>& GetMaterialBefore(unsigned int materialId) const {
      return fMaterialsBefore[materialId];
   }

   /**
    * \brief Return material from the list of materials after station center by material index.
    * \param[in] materialId Index of the material to be returned.
    * \return Material from the list which corresponds to materialId index.
    */
   const LitMaterialInfo<T>& GetMaterialAfter(unsigned int materialId) const {
      return fMaterialsAfter[materialId];
   }

   /**
    * \brief Return number of material layers in the list of materials before station center.
    * \return Number of material layers.
    */
   unsigned char GetNofMaterialsBefore() const {
      return fMaterialsBefore.size();
   }

   /**
    * \brief Return number of material layers in the list of materials after station center.
    * \return Number of material layers.
    */
   unsigned char GetNofMaterialsAfter() const {
      return fMaterialsAfter.size();
   }

   /**
    * \brief Set Z center of the station [cm].
    * \param[in] z Value of the Z center [cm].
    */
   void SetZ(T z) {
      fZ = z;
   }

   /**
    * \brief Return Z center of the station [cm].
    */
   T GetZ() const {
      return fZ;
   }

   /**
    * \brief Returns std::string representation of the class.
    * \return Class representation as std::string.
    */
   std::string ToString() const {
      std::string str = "LitStationElectron: Z=" + lit::parallel::ToString<T>(GetZ()) + "\n";
      str += "   materials before:\n";
      for (unsigned char i = 0; i < GetNofMaterialsBefore(); i++) {
         str += "      " + lit::parallel::ToString<int>((int)i)
               + " " + GetMaterialBefore(i).ToString();
      }
      str += "   materials after:\n";
      for (unsigned char i = 0; i < GetNofMaterialsAfter(); i++) {
         str += "      " + lit::parallel::ToString<int>((int)i)
               + " " + GetMaterialAfter(i).ToString();
      }
      return str;
   }

   /**
    * \brief Operator << for convenient output to std::ostream.
    * \return Insertion stream in order to be able to call a succession of insertion operations.
    */
   friend std::ostream& operator<<(std::ostream& strm, const LitStationElectron& station ) {
      strm << station.ToString();
      return strm;
   }

private:
   // List of material before Z station center
   std::vector<LitMaterialInfo<T> > fMaterialsBefore;
   // List of material after Z station center
   std::vector<LitMaterialInfo<T> > fMaterialsAfter;
   // Z center of the station [cm]
   T fZ;
} _fvecalignment;

/**
 * \typedef LitStationElectron<fvec> LitStationElectronVec
 * \brief Vector version of LitStationElectron.
 */
typedef LitStationElectron<fvec> LitStationElectronVec;

/**
 * \typedef LitStationElectron<fscal> LitStationElectronScal
 * \brief Scalar version of LitStationElectron.
 */
typedef LitStationElectron<fscal> LitStationElectronScal;



/**
 * \class LitVirtualPlaneElectron
 *
 * \brief Represents virtual plane which is used for track propagation.
 *
 * Virtual planes store material and field values and
 * are used for STS track propagation to the first TRD
 * station. The track propagation is done between
 * virtual planes.
 *
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2009
 */
template<class T>
class LitVirtualPlaneElectron
{
public:
   /**
    * \brief Constructor.
    */
   LitVirtualPlaneElectron():
      fZ(0.),
      fMaterial(),
      fFieldGrid(),
      fFieldGridMid() {}

   /**
    * \brief Destructor.
    */
   virtual ~LitVirtualPlaneElectron() {}

   /**
    * \brief Set Z center of the virtual plane [cm].
    * \param[in] z Value of the Z center [cm].
    */
   void SetZ(T z) {
      fZ = z;
   }

   /**
    * \brief Return Z center of the virtual plane [cm].
    * \return Value of the Z center [cm].
    */
   T GetZ() const {
      return fZ;
   }

   /**
    * \brief Set material for virtual plane.
    * \param[in] material Material to be set.
    */
   void SetMaterial(const LitMaterialInfo<T>& material) {
      fMaterial = material;
   }

   /**
    * \brief Return material of virtual plane.
    * \return Virtual plane material.
    */
   const LitMaterialInfo<T>& GetMaterial() const {
      return fMaterial;
   }

   void SetFieldGrid(const LitFieldGrid& grid) {
      fFieldGrid = grid;
   }

   const LitFieldGrid& GetFieldGrid() const {
      return fFieldGrid;
   }

   void SetFieldGridMid(const LitFieldGrid& grid) {
      fFieldGridMid = grid;
   }

   const LitFieldGrid& GetFieldGridMid() const {
      return fFieldGridMid;
   }

   /**
    * \brief Returns std::string representation of the class.
    * \return Class representation as std::string.
    */
   std::string ToString() const {
      std::string str = "LitVirtualPlaneElectron: Z=" + lit::parallel::ToString<T>(GetZ());
      str += GetMaterial().ToString();
      // str += GetFieldGrid().ToString();
      // str += GetFieldGridMid().ToString();
      return str;
   }

   /**
    * \brief Operator << for convenient output to std::ostream.
    * \return Insertion stream in order to be able to call a succession of insertion operations.
    */
   friend std::ostream& operator<<(std::ostream& strm, const LitVirtualPlaneElectron& plane) {
      strm << plane.ToString();
      return strm;
   }

private:
   // Z position of center of virtual plane.
   T fZ;
   // Material of the virtual plane.
   LitMaterialInfo<T> fMaterial;
   // TODO: Clearify!!! Field approximation using grid.
   LitFieldGrid fFieldGrid;
   LitFieldGrid fFieldGridMid;

} _fvecalignment;

/**
 * \typedef LitVirtualPlaneElectron<fvec> LitVirtualPlaneElectronVec
 * \brief Vector version of LitVirtualPlaneElectron.
 */
typedef LitVirtualPlaneElectron<fvec> LitVirtualPlaneElectronVec;

/**
 * \typedef LitVirtualPlaneElectron<fscal> LitVirtualPlaneElectronScal
 * \brief Scalar version of LitVirtualPlaneElectron.
 */
typedef LitVirtualPlaneElectron<fscal> LitVirtualPlaneElectronScal;



/**
 * \class LitStationGroupElectron
 *
 * \brief Represents station group for the electron CBM setup.
 *
 * Station group stores a list of stations and
 * provides access to them.
 *
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2009
 */
template<class T>
class LitStationGroupElectron
{
public:
   /**
    * \brief Constructor.
    */
   LitStationGroupElectron():
      fStations() {}

   /**
    * \brief Destructor.
    */
   virtual ~LitStationGroupElectron() {}

   /**
    * \brief Add station to the list of stations.
    * \param[in] station Station to be added.
    */
   void AddStation(const LitStationElectron<T>& station) {
      fStations.push_back(station);
   }

   /**
    * \brief Return station by station index.
    * \param[in] stationId Index of station in station group.
    * \return Station with stationId index.
    */
   const LitStationElectron<T>& GetStation(unsigned int stationId) const {
      return fStations[stationId];
   }

   /**
    * \brief Return number of stations in station group.
    * \return Number of stations in station group.
    */
   unsigned char GetNofStations() const {
      return fStations.size();
   }

   /**
    * \brief Returns std::string representation of the class.
    * \return Class representation as std::string.
    */
   std::string ToString() const {
      std::string str = "LitStationGroupElectron: nofStations="
            + lit::parallel::ToString<int>(GetNofStations()) + "\n";
      for (unsigned char i = 0; i < GetNofStations(); i++) {
         str += lit::parallel::ToString<int>(i) + " " + GetStation(i).ToString();
      }
      return str;
   }

   /**
    * \brief Operator << for convenient output to std::ostream.
    * \return Insertion stream in order to be able to call a succession of insertion operations.
    */
   friend std::ostream& operator<<(std::ostream& strm, const LitStationGroupElectron& stationGroup) {
      strm << stationGroup.ToString();
      return strm;
   }

private:
   // Array with stations in the station group
   std::vector<LitStationElectron<T> > fStations;

} _fvecalignment;

/**
 * \typedef LitStationGroupElectron<fvec> LitStationGroupElectronVec
 * \brief Vector version of LitStationGroupElectron.
 */
typedef LitStationGroupElectron<fvec> LitStationGroupElectronVec;

/**
 * \typedef LitStationGroupElectron<fscal> LitStationGroupElectronScal
 * \brief Scalar version of LitStationGroupElectron.
 */
typedef LitStationGroupElectron<fscal> LitStationGroupElectronScal;



/**
 * \class LitDetectorLayoutElectron
 *
 * \brief Represents detector layout for the electron CBM setup.
 *
 * Detector layout provides access to the detector
 * information. It stores a list of virtual planes
 * and a list of station groups.
 *
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2009
 */
template<class T>
class LitDetectorLayoutElectron
{
public:
   /**
    * \brief Constructor.
    */
   LitDetectorLayoutElectron():
      fStationGroups(),
      fVirtualPlanes() {}

   /**
    * \brief Destructor.
    */
   virtual ~LitDetectorLayoutElectron() {}

   /**
    * \brief Add station group to detector layout.
    * \param[in] stationGroup Station group to be added.
    */
   void AddStationGroup(const LitStationGroupElectron<T>& stationGroup) {
      fStationGroups.push_back(stationGroup);
   }

   /**
    * \brief Add virtual plane to detector layout.
    * \param[in] plane Virtual plane to be added.
    */
   void AddVirtualPlane(const LitVirtualPlaneElectron<T>& plane) {
      fVirtualPlanes.push_back(plane);
   }

   /**
    * \brief Return number of station groups.
    * \return Number of station groups.
    */
   unsigned char GetNofStationGroups() const {
      return fStationGroups.size();
   }

   /**
    * \brief Return number of station for station group with stationGroup index.
    * \param[in] stationGroup Index of station group in detector layout.
    * \return Number of stations.
    */
   unsigned char GetNofStations(unsigned char stationGroup) const {
      return fStationGroups[stationGroup].GetNofStations();
   }

   /**
    * \brief Return number of virtual planes.
    * \return Number of virtual planes.
    */
   unsigned char GetNofVirtualPlanes() const {
      return fVirtualPlanes.size();
   }

   /**
    * \brief Return station group with stationGroup index.
    * \param[in] stationGroup Index of station group in detector layout.
    * \return Station group with stationGroup index.
    */
   const LitStationGroupElectron<T>& GetStationGroup(unsigned char stationGroup) const {
      return fStationGroups[stationGroup];
   }

   /**
    * \brief Return station specified by stationGroup and station indices.
    * \param[in] stationGroup Station group index in detector layout.
    * \param[in] station Station index in station group.
    * \return Station specified by stationGroup and station indices.
    */
   const LitStationElectron<T>& GetStation(unsigned char stationGroup, unsigned char station) const {
      return fStationGroups[stationGroup].GetStation(station);
   }

   /**
    * \brief Return virtual plane with planeId index.
    * \param[in] planeId Virtual plane index.
    * \return Virtual plane with planeId index.
    */
   const LitVirtualPlaneElectron<T>& GetVirtualPlane(unsigned char planeId) const {
      return fVirtualPlanes[planeId];
   }

   /**
    * \brief Returns std::string representation of the class.
    * \return Class representation as std::string.
    */
   std::string ToString() const {
      std::string str =  "LitDetectorLayoutElectron: \n";
      str += "   virtual planes: nofVirtualPlanes="
            + lit::parallel::ToString<int>(GetNofVirtualPlanes()) + "\n";
      for (unsigned char i = 0; i < GetNofVirtualPlanes(); i++) {
         str += lit::parallel::ToString<int>(i) + " " + GetVirtualPlane(i).ToString();
      }
      str += "   station groups: nofStationGroups="
            + lit::parallel::ToString<int>(GetNofStationGroups()) + "\n";
      for (unsigned char i = 0; i < GetNofStationGroups(); i++) {
         str += "      " + lit::parallel::ToString<int>(i) + " " + GetStationGroup(i).ToString();
      }
      return str;
   }

   /**
    * \brief Operator << for convenient output to std::ostream.
    * \return Insertion stream in order to be able to call a succession of insertion operations.
    */
   friend std::ostream& operator<<(std::ostream& strm, const LitDetectorLayoutElectron& layout) {
      strm << layout.ToString();
      return strm;
   }

private:
   // Array with station groups
   std::vector<LitStationGroupElectron<T> > fStationGroups;
   // Array with virtual planes
   std::vector<LitVirtualPlaneElectron<T> > fVirtualPlanes;

} _fvecalignment;

/**
 * \typedef LitDetectorLayoutElectron<fvec> LitDetectorLayoutElectronVec
 * \brief Vector version of LitDetectorLayoutElectron.
 */
typedef LitDetectorLayoutElectron<fvec> LitDetectorLayoutElectronVec;

/**
 * \typedef LitDetectorLayoutElectron<fscal> LitDetectorLayoutElectronScal
 * \brief Scalar version of LitDetectorLayoutElectron.
 */
typedef LitDetectorLayoutElectron<fscal> LitDetectorLayoutElectronScal;

} // namespace parallel
} // namespace lit
#endif /*LITDETECTORGEOMETRYELECTRON_H_*/
