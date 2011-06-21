/** LitDetectorGeometryElectron.h
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2010
 * @version 1.0
 *
 * Classes for geometry description of the 'electron' setup of CBM.
 * Detector layout consists of station groups. Each station
 * group consists of stations. In addition virtual planes
 * are introduced in order to store material and field
 * values between STS and TRD detectors.
 * Detector layout also provides access to
 * material of the detector and approximated magnetic field.
 **/

#ifndef LITDETECTORGEOMETRYELECTRON_H_
#define LITDETECTORGEOMETRYELECTRON_H_

#include "../LitTypes.h"
#include "../LitMaterialInfo.h"
#include "../LitField.h"

namespace lit {
namespace parallel {

template<class T>
class LitStationElectron
{
public:
   /* Constructor */
   LitStationElectron():
      fMaterialsBefore(),
      fMaterialsAfter(),
      fZ(0.) {}

   /* Destructor */
   virtual ~LitStationElectron() {};

   void AddMaterialBefore(const LitMaterialInfo<T>& material) {
      fMaterialsBefore.push_back(material);
   }

   void AddMaterialAfter(const LitMaterialInfo<T>& material) {
      fMaterialsAfter.push_back(material);
   }

   const LitMaterialInfo<T>& GetMaterialBefore(unsigned int materialId) const {
      return fMaterialsBefore[materialId];
   }

   const LitMaterialInfo<T>& GetMaterialAfter(unsigned int materialId) const {
      return fMaterialsAfter[materialId];
   }

   unsigned char GetNofMaterialsBefore() const {
      return fMaterialsBefore.size();
   }

   unsigned char GetNofMaterialsAfter() const {
      return fMaterialsAfter.size();
   }

   void SetZ(T z) {
      fZ = z;
   }

   T GetZ() const {
      return fZ;
   }

private:
   // List of material before Z station center
   std::vector<LitMaterialInfo<T> > fMaterialsBefore;
   // List of material after Z station center
   std::vector<LitMaterialInfo<T> > fMaterialsAfter;
   // Z center of the station [cm]
   T fZ;

public:
   friend std::ostream& operator<<(std::ostream& strm, const LitStationElectron& station ) {
      strm << "LitStationElectron: " << "Z=" << station.GetZ() << std::endl;
      strm << "   materials before:" << std::endl;
      for (unsigned char i = 0; i < station.GetNofMaterialsBefore(); i++) {
         strm << "      " << (int)i << " " << station.GetMaterialBefore(i);
      }
      strm << "   materials after:" << std::endl;
      for (unsigned char i = 0; i < station.GetNofMaterialsAfter(); i++) {
         strm << "      " << (int)i << " " << station.GetMaterialAfter(i);
      }
      return strm;
   }

   std::string ToStringShort() const {
      std::string str = ToString<T>(GetZ()) + "\n";
      str += ToString<T>(GetNofMaterialsBefore()) + "\n";
      for (unsigned char i = 0; i < GetNofMaterialsBefore(); i++) {
         str += GetMaterialBefore(i).ToStringShort() + "\n";
      }
      str += ToString<T>(GetNofMaterialsAfter()) + "\n";
      for (unsigned char i = 0; i < GetNofMaterialsAfter(); i++) {
         str += GetMaterialAfter(i).ToStringShort() + "\n";
      }
      return str;
   }
} _fvecalignment;

typedef LitStationElectron<fvec> LitStationElectronVec;
typedef LitStationElectron<fscal> LitStationElectronScal;



template<class T>
class LitVirtualPlaneElectron
{
public:
   /* Constructor */
   LitVirtualPlaneElectron():
      fZ(0.),
      fMaterial(),
//      fFieldSlice(),
//      fFieldSliceMid(),
      fFieldGrid(),
      fFieldGridMid() {}

   /* Destructor */
   virtual ~LitVirtualPlaneElectron() {}

   void SetZ(T z) {
      fZ = z;
   }

   T GetZ() const {
      return fZ;
   }

   void SetMaterial(const LitMaterialInfo<T>& material) {
      fMaterial = material;
   }

   const LitMaterialInfo<T>& GetMaterial() const {
      return fMaterial;
   }

//   void SetFieldSlice(const LitFieldSlice<T>& slice) {
//      fFieldSlice = slice;
//   }
//
//   const LitFieldSlice<T>& GetFieldSlice() const {
//      return fFieldSlice;
//   }
//
//   void SetFieldSliceMid(const LitFieldSlice<T>& slice) {
//      fFieldSliceMid = slice;
//   }
//
//   const LitFieldSlice<T>& GetFieldSliceMid() const {
//      return fFieldSliceMid;
//   }

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

private:
   T fZ; // Z position of center of virtual plane
   LitMaterialInfo<T> fMaterial; // Material of the virtual plane
//   LitFieldSlice<T> fFieldSlice; // Field slice
//   LitFieldSlice<T> fFieldSliceMid;

   LitFieldGrid fFieldGrid;
   LitFieldGrid fFieldGridMid;

   friend std::ostream& operator<<(std::ostream& strm, const LitVirtualPlaneElectron& plane) {
      strm << "LitVirtualPlaneElectron: Z=" << plane.GetZ() << ", material=" << plane.GetMaterial();
      // strm << "fieldSlice=" << plane.GetFieldSlice();
      // strm << "fieldSliceMid=" << plane.GetFieldSliceMid();
//    strm << std::endl;
      return strm;
   }

   std::string ToStringShort() const {
      std::string str = ToString<T>(GetZ()) + "\n" + GetMaterial().ToStringShort();
//      str += GetFieldSlice().ToStringShort();
//      str += GetFieldSliceMid().ToStringShort();
      return str;
   }

} _fvecalignment;

typedef LitVirtualPlaneElectron<fvec> LitVirtualPlaneElectronVec;
typedef LitVirtualPlaneElectron<fscal> LitVirtualPlaneElectronScal;



template<class T>
class LitStationGroupElectron
{
public:
   /* Constructor */
   LitStationGroupElectron():
      fStations() {}

   /* Destructor */
   virtual ~LitStationGroupElectron() {}

   void AddStation(const LitStationElectron<T>& station) {
      fStations.push_back(station);
   }

   const LitStationElectron<T>& GetStation(unsigned int stationId) const {
      return fStations[stationId];
   }

   unsigned char GetNofStations() const {
      return fStations.size();
   }

private:
   // Array with stations in the station group
   std::vector<LitStationElectron<T> > fStations;

public:
   friend std::ostream& operator<<(std::ostream& strm, const LitStationGroupElectron& stationGroup) {
      strm << "LitStationGroupElectron: " << "nofStations=" << (int) stationGroup.GetNofStations() << std::endl;
      for (unsigned char i = 0; i < stationGroup.GetNofStations(); i++) {
         strm << "  " << (int) i << " " << stationGroup.GetStation(i);
      }
      return strm;
   }

   std::string ToStringShort() const {
      std::string str = ToString<int>(GetNofStations()) + "\n";
      for (unsigned char i = 0; i < GetNofStations(); i++) {
//       str += "station\n";
         str += ToString<int>(i) + "\n" + GetStation(i).ToStringShort();
      }
      return str;
   }
} _fvecalignment;

typedef LitStationGroupElectron<fvec> LitStationGroupElectronVec;
typedef LitStationGroupElectron<fscal> LitStationGroupElectronScal;



template<class T>
class LitDetectorLayoutElectron
{
public:
   /* Constructor */
   LitDetectorLayoutElectron():
      fStationGroups(),
      fVirtualPlanes() {}

   /* Destructor */
   virtual ~LitDetectorLayoutElectron() {}

   void AddStationGroup(const LitStationGroupElectron<T>& stationGroup) {
      fStationGroups.push_back(stationGroup);
   }

   void AddVirtualPlane(const LitVirtualPlaneElectron<T>& plane) {
      fVirtualPlanes.push_back(plane);
   }

   unsigned char GetNofStationGroups() const {
      return fStationGroups.size();
   }

   unsigned char GetNofStations(unsigned char stationGroup) const {
      return fStationGroups[stationGroup].GetNofStations();
   }

   unsigned char GetNofVirtualPlanes() const {
      return fVirtualPlanes.size();
   }

   const LitStationGroupElectron<T>& GetStationGroup(unsigned char stationGroup) const {
      return fStationGroups[stationGroup];
   }

   const LitStationElectron<T>& GetStation(unsigned char stationGroup, unsigned char station) const {
      return fStationGroups[stationGroup].GetStation(station);
   }

   const LitVirtualPlaneElectron<T>& GetVirtualPlane(unsigned char planeId) const {
      return fVirtualPlanes[planeId];
   }

private:
   // array with station groups
   std::vector<LitStationGroupElectron<T> > fStationGroups;
   // array with virtual planes
   std::vector<LitVirtualPlaneElectron<T> > fVirtualPlanes;

public:
   friend std::ostream& operator<<(std::ostream& strm, const LitDetectorLayoutElectron& layout) {
      strm << "LitDetectorLayoutElectron: " << std::endl;
      strm << "   virtual planes: nofVirtualPlanes=" << (int)layout.GetNofVirtualPlanes() << std::endl;
      for (unsigned char i = 0; i < layout.GetNofVirtualPlanes(); i++) {
         strm << (int) i << " " << layout.GetVirtualPlane(i);
      }
      strm << "   station groups: nofStationGroups=" << (int)layout.GetNofStationGroups() << std::endl;
      for (unsigned char i = 0; i < layout.GetNofStationGroups(); i++) {
         strm << "      " << (int) i << " " << layout.GetStationGroup(i);
      }
      return strm;
   }

   std::string ToStringShort() const {
      std::string str = ToString<int>(GetNofVirtualPlanes()) + "\n";
      for (unsigned char i = 0; i < GetNofVirtualPlanes(); i++) {
         // str += "virtual planes\n";
         str += ToString<int>(i) + "\n" + GetVirtualPlane(i).ToStringShort();
      }
      str = ToString<int>(GetNofStationGroups()) + "\n";
      for (unsigned char i = 0; i < GetNofStationGroups(); i++) {
//       str += "station group\n";
         str += ToString<int>(i) + "\n" + GetStationGroup(i).ToStringShort();
      }
      return str;
   }
} _fvecalignment;

typedef LitDetectorLayoutElectron<fvec> LitDetectorLayoutElectronVec;
typedef LitDetectorLayoutElectron<fscal> LitDetectorLayoutElectronScal;

} // namespace parallel
} // namespace lit
#endif /*LITDETECTORGEOMETRYELECTRON_H_*/
