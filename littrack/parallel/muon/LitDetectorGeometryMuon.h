/** LitDetectorGeometryMuon.h
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2009
 * @version 1.0
 *
 * Classes for geometry description of the 'muon' setup of CBM.
 * Detector layout consists of station groups. Each station
 * group consists of stations and absorber. Each station consists
 * of substations. Detector layout also provides access to
 * material of the detector and approximated magnetic field.
 **/

#ifndef LITDETECTORGEOMETRYMUON_H_
#define LITDETECTORGEOMETRYMUON_H_

#include "../LitMaterialInfo.h"
#include "../LitField.h"
#include "../LitEnums.h"
#include "../LitUtils.h"

#include <vector>

namespace lit {
namespace parallel{

/* Class describes substation for muon setup. */
template<class T>
class LitSubstationMuon
{
public:
   /* Constructor */
   LitSubstationMuon():
      fZ(0.),
      fMaterial()
//      fFieldSlice()
   {}

   /* Destructor */
   virtual ~LitSubstationMuon() {}

   /* @return Z */
   const T& GetZ() const {
      return fZ;
   }

   /* Sets Z
    * @param Value */
   void SetZ(const T& z) {
      fZ = z;
   }

   /* @return Material */
   const LitMaterialInfo<T>& GetMaterial() const {
      return fMaterial;
   }

   /* Sets material
    * @param Value */
   void SetMaterial(const LitMaterialInfo<T>& material) {
      fMaterial = material;
   }

//   /* @return field slice */
//   const LitFieldSlice<T>& GetFieldSlice() const {
//      return fFieldSlice;
//   }
//
//   /* Sets field slice
//    * @param Value */
//   void SetFieldSlice(const LitFieldSlice<T>& fieldSlice) {
//      fFieldSlice = fieldSlice;
//   }

private:
   T fZ; // Z position of the substation in [cm]
   LitMaterialInfo<T> fMaterial; // Material information of the substation
//   LitFieldSlice<T> fFieldSlice; // Magnetic field approximation

public:
   /* Operator << for convenient output to std::ostream */
   friend std::ostream& operator<<(std::ostream& strm, const LitSubstationMuon& substation ) {
      strm << "LitSubstationMuon: " << "Z=" << substation.GetZ() << ", material=" << substation.GetMaterial();
//    strm << ", fieldSlice=" << substation.GetFieldSlice();
      return strm;
   }

   /* @return std:string representation of the class */
   std::string ToStringShort() const {
      std::string str = ToString<T>(GetZ()) + "\n" + fMaterial.ToStringShort();
//      str += fFieldSlice.ToStringShort();
      return str;
   }
} _fvecalignment;

/* Some typedefs for convenience */
typedef LitSubstationMuon<fvec> LitSubstationMuonVec;
typedef LitSubstationMuon<fscal> LitSubstationMuonScal;



/* Class describes station for muon setup. */
template<class T>
class LitStationMuon
{
public:
   /* Constructor */
   LitStationMuon():
      fType(kLITPIXELHIT),
      fSubstations() {}

   /* Destructor */
   virtual ~LitStationMuon() {}

   /* Adds substation to the station.
    * @param substation Reference to substation to be added */
   void AddSubstation(const LitSubstationMuon<T>& substation) {
      fSubstations.push_back(substation);
   }

   /* @param index Index of the substation
    * @return substation */
   const LitSubstationMuon<T>& GetSubstation(unsigned short index) const {
      return fSubstations[index];
   }

   /* @return Number of substaions for this station */
   unsigned char GetNofSubstations() const {
      return fSubstations.size();
   }

   /* @return Type */
   LitHitType GetType() const {
      return fType;
   }

   /* Sets type
    * @param Value */
   void SetType(LitHitType type) {
      fType = type;
   }

//   /* @return field slice */
//   const LitFieldSlice<T>& GetFieldSlice() const {
//      return fFieldSlice;
//   }
//
//   /* Sets field slice
//    * @param Value */
//   void SetFieldSlice(const LitFieldSlice<T>& fieldSlice) {
//      fFieldSlice = fieldSlice;
//   }

private:
   LitHitType fType; // Type of hits on the station
   std::vector<LitSubstationMuon<T> > fSubstations; // Array with substations in the station
//   LitFieldSlice<T> fFieldSlice; // Magnetic field approximation in the middle of the station

public:
   /* Operator << for convenient output to std::ostream */
   friend std::ostream& operator<<(std::ostream& strm, const LitStationMuon& station) {
      strm << "LitStationMuon: type=" << station.GetType() << ", nofSubstations=" << (int) station.GetNofSubstations() << std::endl;
      for (unsigned char i = 0; i < station.GetNofSubstations(); i++) {
         strm << "    " << (int) i << " " << station.GetSubstation(i);
      }
      return strm;
   }

   /* @return std:string representation of the class */
   std::string ToStringShort() const {
      std::string str = GetType() + " " + ToString<int>(GetNofSubstations()) + "\n";
      for (unsigned char i = 0; i < GetNofSubstations(); i++) {
//       str += "substation\n";
         str += ToString<int>(i) + "\n" + GetSubstation(i).ToStringShort();
      }
      return str;
   }

} _fvecalignment;

/* Some typedefs for convenience */
typedef LitStationMuon<fvec> LitStationMuonVec;
typedef LitStationMuon<fscal> LitStationMuonScal;



/* Class describes muon absorber */
template<class T>
class LitAbsorber
{
public:
   /* Constructor */
   LitAbsorber():
      fZ(0.),
      fMaterial(),
      fFieldSliceFront(),
      fFieldSliceMiddle(),
      fFieldSliceBack() {}

   /* Destructor */
   virtual ~LitAbsorber() {}

   /* @return Z */
   const T& GetZ() const {
      return fZ;
   }

   /* Sets Z
    * @param Value */
   void SetZ(const T& z) {
      fZ = z;
   }

   /* @return Material */
   const LitMaterialInfo<T>& GetMaterial() const {
      return fMaterial;
   }

   /* Sets material
    * @param Value */
   void SetMaterial(const LitMaterialInfo<T>& material) {
      fMaterial = material;
   }

   /* @return field slice in front of the absorber */
   const LitFieldSlice<T>& GetFieldSliceFront() const {
      return fFieldSliceFront;
   }

   /* Sets field slice in front of the absorber
    * @param Value */
   void SetFieldSliceFront(const LitFieldSlice<T>& fieldSlice) {
      fFieldSliceFront = fieldSlice;
   }

   /* @return field slice in middle of the absorber */
   const LitFieldSlice<T>& GetFieldSliceMiddle() const {
      return fFieldSliceMiddle;
   }

   /* Sets field slice in the middle of the absorber
    * @param Value */
   void SetFieldSliceMiddle(const LitFieldSlice<T>& fieldSlice) {
      fFieldSliceMiddle = fieldSlice;
   }

   /* @return field slice in back of the absorber */
   const LitFieldSlice<T>& GetFieldSliceBack() const {
      return fFieldSliceBack;
   }

   /* Sets field slice in front of the absorber
    * @param Value */
   void SetFieldSliceBack(const LitFieldSlice<T>& fieldSlice) {
      fFieldSliceBack = fieldSlice;
   }

private:
   T fZ; // Z position of the absorber in [cm]
   LitMaterialInfo<T> fMaterial; // Absorber material
   LitFieldSlice<T> fFieldSliceFront; // Magnetic field approximation in front of the absorber
   LitFieldSlice<T> fFieldSliceMiddle; // Magnetic field approximation in the middle of the absorber
   LitFieldSlice<T> fFieldSliceBack; // Magnetic field approximation in the back of the absorber

public:
   /* Operator << for convenient output to std::ostream */
   friend std::ostream& operator<<(std::ostream& strm, const LitAbsorber& absorber) {
      strm << "LitAbsorber: Z=" << absorber.GetZ() << ", material=" << absorber.GetMaterial();
//    strm << "fieldSliceFront=" << absorber.GetFieldSliceFront()
//       << " fieladSliceBack=" << absorber.GetFieldSliceBack();
      return strm;
   }

   /* @return std:string representation of the class */
   std::string ToStringShort() const {
      std::string str = ToString<T>(GetZ()) + "\n" + GetMaterial().ToStringShort();
      str += GetFieldSliceFront().ToStringShort();
      str += GetFieldSliceBack().ToStringShort();
      return str;
   }

} _fvecalignment;

/* Some typedefs for convenience */
typedef LitAbsorber<fvec> LitAbsorberVec;
typedef LitAbsorber<fscal> LitAbsorberScal;



/* Class describes station group for muon setup */
template<class T>
class LitStationGroupMuon
{
public:
   /* Constructor */
   LitStationGroupMuon():
      fFieldSliceFront(),
      fFieldSliceMiddle(),
      fFieldSliceBack(),
      fStations(),
      fAbsorber() {}

   /* Destructor */
   virtual ~LitStationGroupMuon() {}

   /* @return field slice in front of the absorber */
   const LitFieldSlice<T>& GetFieldSliceFront() const {
      return fFieldSliceFront;
   }

   /* Sets field slice in front of the absorber
    * @param Value */
   void SetFieldSliceFront(const LitFieldSlice<T>& fieldSlice) {
      fFieldSliceFront = fieldSlice;
   }

   /* @return field slice in middle of the absorber */
   const LitFieldSlice<T>& GetFieldSliceMiddle() const {
      return fFieldSliceMiddle;
   }

   /* Sets field slice in the middle of the absorber
    * @param Value */
   void SetFieldSliceMiddle(const LitFieldSlice<T>& fieldSlice) {
      fFieldSliceMiddle = fieldSlice;
   }

   /* @return field slice in back of the absorber */
   const LitFieldSlice<T>& GetFieldSliceBack() const {
      return fFieldSliceBack;
   }

   /* Sets field slice in front of the absorber
    * @param Value */
   void SetFieldSliceBack(const LitFieldSlice<T>& fieldSlice) {
      fFieldSliceBack = fieldSlice;
   }

   /* Adds station to the station group
    * @param station Station to be added */
   void AddStation(const LitStationMuon<T>& station) {
      fStations.push_back(station);
   }

   /* @param index Index of the station
    * @return station */
   const LitStationMuon<T>& GetStation(unsigned short index) const {
      return fStations[index];
   }

   /* @return Number of stations in this station group */
   unsigned char GetNofStations() const {
      return fStations.size();//nofStations;
   }

   /* @return Absorber */
   const LitAbsorber<T>& GetAbsorber() const {
      return fAbsorber;
   }

   /* Sets Absorber
    * @param absorber Value */
   void SetAbsorber(const LitAbsorber<T>& absorber) {
      fAbsorber = absorber;
   }

   /* Calculates field region for the group of stations
    * @param x X position
    * @param y Y position
    * @param field OUTPUT field region */
   void GetFieldRegion(T x, T y, LitFieldRegion<T>& field) const {
      LitFieldValue<T> v1, v2, v3;
      fFieldSliceFront.GetFieldValue(x, y, v1);
      fFieldSliceMiddle.GetFieldValue(x, y, v2);
      fFieldSliceBack.GetFieldValue(x, y, v3);
      field.Set(v1, fFieldSliceFront.GetZ(), v2, fFieldSliceMiddle.GetZ(), v3, fFieldSliceBack.GetZ());
   }

private:
   // This field slices are used to construct LitFieldRegion
   // in the group of stations. We need 3 values in order to use
   // parabolic approximation. So the values are in front, middle
   // and back of the gap.
   LitFieldSlice<T> fFieldSliceFront; // Magnetic field approximation in front
   LitFieldSlice<T> fFieldSliceMiddle; // Magnetic field approximation in the middle
   LitFieldSlice<T> fFieldSliceBack; // Magnetic field approximation in the back
   std::vector<LitStationMuon<T> > fStations; // Array with stations
   LitAbsorber<T> fAbsorber; // Absorber

public:
   /* Operator << for convenient output to std::ostream */
   friend std::ostream& operator<<(std::ostream& strm, const LitStationGroupMuon& stationGroup) {
      strm << "LitStationGroupMuon: " << "nofStations=" << (int) stationGroup.GetNofStations() << std::endl;
      for (unsigned char i = 0; i < stationGroup.GetNofStations(); i++) {
         strm << "  " << (int) i << " " << stationGroup.GetStation(i);
      }
      strm << "  " << stationGroup.GetAbsorber();
      return strm;
   }

   /* @return std:string representation of the class */
   std::string ToStringShort() const {
      std::string str = ToString<int>(GetNofStations()) + "\n";
      for (unsigned char i = 0; i < GetNofStations(); i++) {
//       str += "station\n";
         str += ToString<int>(i) + "\n" + GetStation(i).ToStringShort();
      }
//    str += "absorber\n";
      str += GetAbsorber().ToStringShort();
      return str;
   }

} _fvecalignment;

/* Some typedefs for convenience */
typedef LitStationGroupMuon<fvec> LitStationGroupMuonVec;
typedef LitStationGroupMuon<fscal> LitStationGroupMuonScal;



/* Class describes detector layout for muon setup */
template<class T>
class LitDetectorLayoutMuon
{
public:
   /* Constructor */
   LitDetectorLayoutMuon():fStationGroups() {};

   /* Destructor */
   virtual ~LitDetectorLayoutMuon() {}

   /* Adds station group to the layout
    * @param stationGroup Station group to be added */
   void AddStationGroup(const LitStationGroupMuon<T>& stationGroup) {
      fStationGroups.push_back(stationGroup);
   }

   /* @return Number of station groups */
   unsigned char GetNofStationGroups() const {
      return fStationGroups.size();
   }

   /* @param stationGroup Station group index
    * @return Number of stations for specified station group */
   unsigned char GetNofStations(unsigned char stationGroup) const {
      return fStationGroups[stationGroup].GetNofStations();
   }

   /* @param stationGroup Station group index
    * @param station Station index
    * @return Number of substations for specified station group and station */
   unsigned char GetNofSubstations(unsigned char stationGroup, unsigned char station) const {
      return fStationGroups[stationGroup].GetStation(station).GetNofSubstations();
   }

   /* @param stationGroup Station group index
    * @return Station group for specified station group index */
   const LitStationGroupMuon<T>& GetStationGroup(unsigned char stationGroup) const {
      return fStationGroups[stationGroup];
   }

   /* @param stationGroup Station group index
    * @param station Station index
    * @return Station for specified station group and station indices */
   const LitStationMuon<T>& GetStation(unsigned char stationGroup, unsigned char station) const {
      return fStationGroups[stationGroup].GetStation(station);
   }

   /* @param stationGroup Station group index
    * @param station Station index
    * @param substation Substation index
    * @return Substation for specified station group, station and substation indices */
   const LitSubstationMuon<T>& GetSubstation(unsigned char stationGroup, unsigned char station, unsigned char substation) const {
      return fStationGroups[stationGroup].GetStation(station).GetSubstation(substation);
   }

private:
   std::vector<LitStationGroupMuon<T> > fStationGroups; // Array with station groups

public:
   /* Operator << for convenient output to std::ostream */
   friend std::ostream& operator<<(std::ostream& strm, const LitDetectorLayoutMuon& layout) {
      strm << "LitDetectorLayoutMuon: " << "nofStationGroups=" << (int)layout.GetNofStationGroups() << std::endl;
      for (unsigned char i = 0; i < layout.GetNofStationGroups(); i++) {
         strm << (int) i << " " << layout.GetStationGroup(i);
      }
      return strm;
   }

   /* @return std:string representation of the class */
   std::string ToStringShort() const {
      std::string str = ToString<int>(GetNofStationGroups()) + "\n";
      for (unsigned char i = 0; i < GetNofStationGroups(); i++) {
//       str += "station group\n";
         str += ToString<int>(i) + "\n" + GetStationGroup(i).ToStringShort();
      }
      return str;
   }
} _fvecalignment;

/* Some typedefs for convenience */
typedef LitDetectorLayoutMuon<fvec> LitDetectorLayoutMuonVec;
typedef LitDetectorLayoutMuon<fscal> LitDetectorLayoutMuonScal;

} // namespace parallel
} // namespace lit
#endif /*LITDETECTORGEOMETRYMUON_H_*/
