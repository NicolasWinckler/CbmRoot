/**
 * \file LitDetectorGeometryMuon.h
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2009
 * \brief Classes for muon geometry description of CBM.
 *
 * Classes for muon geometry description of CBM.
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

/**
 * \class LitSubstationMuon
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2009
 * \brief Substation in muon detector layout.
 **/
template<class T>
class LitSubstationMuon
{
public:
   /**
    * \brief Constructor.
    */
   LitSubstationMuon():
      fZ(0.),
      fMaterial() {}

   /**
    * \brief Destructor.
    */
   virtual ~LitSubstationMuon() {}

   /**
    * \brief Return Z position of substation.
    * \return Z position of substation.
    */
   const T& GetZ() const { return fZ; }

   /**
    * \brief Set Z pposition of substation.
    * \param[in] z Z position value.
    */
   void SetZ(const T& z) { fZ = z; }

   /**
    * \brief Return material of substation.
    * \return Material of substation.
    */
   const LitMaterialInfo<T>& GetMaterial() const { return fMaterial; }

   /**
    * \brief Set material of substation.
    * \param[in] materiel Material value.
    */
   void SetMaterial(const LitMaterialInfo<T>& material) { fMaterial = material; }

   /**
    * \brief Return string representation for the class.
    * \return String representation of class.
    */
   std::string ToString() const {
      return "LitSubstationMuon: Z=" + lit::parallel::ToString<T>(GetZ())
            + ", material=" + GetMaterial().ToString();
   }

   /**
    * \brief Operator << for convenient output to std::ostream.
    * \return std::ostream for continuous output.
    */
   friend std::ostream& operator<<(std::ostream& strm, const LitSubstationMuon& substation ) {
      strm << substation.ToString();
      return strm;
   }

private:
   T fZ; // Z position of substation in [cm]
   LitMaterialInfo<T> fMaterial; // Material of substation
} _fvecalignment;

/* Some typedefs for convenience */
typedef LitSubstationMuon<fvec> LitSubstationMuonVec;
typedef LitSubstationMuon<fscal> LitSubstationMuonScal;



/**
 * \class LitStationMuon
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2009
 * \brief Station in muon detector layout.
 **/
template<class T>
class LitStationMuon
{
public:
   /**
    * \brief Constructor.
    */
   LitStationMuon():
      fType(kLITPIXELHIT),
      fSubstations() {}

   /**
    * \brief Destructor.
    */
   virtual ~LitStationMuon() {}

   /**
    * \brief Add substation to station.
    * \param[in] substation Substation to be added.
    */
   void AddSubstation(const LitSubstationMuon<T>& substation) {
      fSubstations.push_back(substation);
   }

   /**
    * \brief Return substation by index.
    * \param[in] index Index of substation.
    * \return Substation by index.
    */
   const LitSubstationMuon<T>& GetSubstation(unsigned short index) const {
      return fSubstations[index];
   }

   /**
    * \brief Return number of substations in station.
    * \return Number of substaions in station.
    */
   unsigned char GetNofSubstations() const { return fSubstations.size(); }

   /**
    * \brief Return type of hits for station.
    * \return Type of hits for station.
    */
   LitHitType GetType() const { return fType; }

   /**
    * \brief Set type of hits for station.
    * \param[in] type Type value.
    */
   void SetType(LitHitType type) { fType = type; }

   /**
    * \brief Return std::string representation for the class.
    * \return std::string representation for the class.
    */
   std::string ToString() const {
      std::string str = "LitStationMuon: type=" + lit::parallel::ToString<LitHitType>(GetType())
            + ", nofSubstations=" + lit::parallel::ToString<int>((int)GetNofSubstations()) + "\n";
      for (unsigned char i = 0; i < GetNofSubstations(); i++) {
         str += "    " + lit::parallel::ToString<int>((int)i) + " " + GetSubstation(i).ToString();
      }
      return str;
   }

   /**
    * \brief Operator << for convenient output to std::ostream.
    * \return std::ostream for continuous output.
    */
   friend std::ostream& operator<<(std::ostream& strm, const LitStationMuon& station) {
      strm << station.ToString();
      return strm;
   }

private:
   LitHitType fType; // Type of hits on the station
   std::vector<LitSubstationMuon<T> > fSubstations; // Array of substations in the station
} _fvecalignment;

/* Some typedefs for convenience */
typedef LitStationMuon<fvec> LitStationMuonVec;
typedef LitStationMuon<fscal> LitStationMuonScal;



/**
 * \class LitAbsorber
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2009
 * \brief Absorber in muon detector layout.
 **/
template<class T>
class LitAbsorber
{
public:
   /**
    * \brief Constructor.
    */
   LitAbsorber():
      fZ(0.),
      fMaterial(),
      fFieldGridFront(),
      fFieldGridMiddle(),
      fFieldGridBack() {}

   /**
    * \brief Destructor.
    */
   virtual ~LitAbsorber() {}

   /**
    * \brief Return Z position of absorber.
    * \return Z position of absorber.
    */
   const T& GetZ() const { return fZ; }

   /**
    * \brief Set Z position of absorber.
    * \param[in] z Z position value.
    */
   void SetZ(const T& z) { fZ = z; }

   /**
    * \brief Return absorber material.
    * \return Absorber material.
    */
   const LitMaterialInfo<T>& GetMaterial() const { return fMaterial; }

   /**
    * \brief Set absorber material.
    * \param[in] material Absorber material value.
    */
   void SetMaterial(const LitMaterialInfo<T>& material) { fMaterial = material; }

   /**
    * \brief Return magnetic field grid in front of the absorber.
    * \return magnetic field grid in front of the absorber.
    */
   const LitFieldGrid& GetFieldGridFront() const { return fFieldGridFront; }

   /**
    * \brief Set magnetic field grid in front of the absorber.
    * \param[in] fieldGrid Magnetic field value.
    */
   void SetFieldGridFront(const LitFieldGrid& fieldGrid) { fFieldGridFront = fieldGrid; }

   /**
    * \brief Return magnetic field grid in the middle of the absorber.
    * \return magnetic field grid in the middle of the absorber.
    */
   const LitFieldGrid& GetFieldGridMiddle() const { return fFieldGridMiddle; }

   /**
    * \brief Set magnetic field grid in the middle of the absorber.
    * \param[in] fieldGrid Magnetic field value.
    */
   void SetFieldGridMiddle(const LitFieldGrid& fieldGrid) { fFieldGridMiddle = fieldGrid; }

   /**
    * \brief Return magnetic field grid in the back of the absorber.
    * \return Magnetic field grid in the back of the absorber.
    */
   const LitFieldGrid& GetFieldGridBack() const { return fFieldGridBack; }

   /**
    * \brief Set magnetic field grid in the back of the absorber.
    * \param[in] fieldGrid magnetic field value.
    */
   void SetFieldGridBack(const LitFieldGrid& fieldGrid) { fFieldGridBack = fieldGrid; }

   /**
    * \brief Return std::string representation of the class.
    * \return std::string representation of the class.
    */
   std::string ToString() const {
      return "LitAbsorber: Z=" + lit::parallel::ToString<T>(GetZ())
            + ", material=" + GetMaterial().ToString();
   }

   /**
    * \brief Operator << for convenient output to std::ostream.
    * \return std::ostream for continuous output.
    */
   friend std::ostream& operator<<(std::ostream& strm, const LitAbsorber& absorber) {
      strm << absorber.ToString();
      return strm;
   }

private:
   T fZ; // Z position of the absorber in [cm]
   LitMaterialInfo<T> fMaterial; // Absorber material
   LitFieldGrid fFieldGridFront; // Magnetic field approximation in front of absorber
   LitFieldGrid fFieldGridMiddle; // Magnetic field approximation in the middle of absorber
   LitFieldGrid fFieldGridBack; // Magnetic field approximation in the back of absorber
} _fvecalignment;

/* Some typedefs for convenience */
typedef LitAbsorber<fvec> LitAbsorberVec;
typedef LitAbsorber<fscal> LitAbsorberScal;



/**
 * \class LitAbsorber
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2009
 * \brief Station group in muon detector layout.
 **/
template<class T>
class LitStationGroupMuon
{
public:
   /**
    * \brief Constructor.
    */
   LitStationGroupMuon():
		fFieldGridFront(),
		fFieldGridMiddle(),
		fFieldGridBack(),
        fStations(),
        fAbsorber() {}

   /**
    * \brief Destructor.
    */
   virtual ~LitStationGroupMuon() {}

   /**
    * \brief Return magnetic field grid in front of the absorber.
    * \return Magnetic field grid in front of the absorber.
    */
   const LitFieldGrid& GetFieldGridFront() const { return fFieldGridFront; }

   /**
    * \brief Set field grid in front of the absorber.
    * \param[in] fieldGrid Field grid value.
    */
   void SetFieldGridFront(const LitFieldGrid& fieldGrid) { fFieldGridFront = fieldGrid; }

   /**
    * \brief Return magnetic field grid in the middle of the absorber.
    * \return Magnetic field grid in the middle of the absorber.
    */
   const LitFieldGrid& GetFieldGridMiddle() const { return fFieldGridMiddle; }

   /**
    * \brief Set field grid in the middle of the absorber.
    * \param[in] fieldGrid Field grid value.
    */
   void SetFieldGridMiddle(const LitFieldGrid& fieldGrid) { fFieldGridMiddle = fieldGrid; }

   /**
    * \brief Return magnetic field grid in the back of the absorber.
    * \return Magnetic field grid in the back of the absorber.
    */
   const LitFieldGrid& GetFieldGridBack() const { return fFieldGridBack; }

   /**
    * \brief Set magnetic field grid in the back of the absorber.
    * \param[in] fieldGrid Field grid value.
    */
   void SetFieldGridBack(const LitFieldGrid& fieldGrid) { fFieldGridBack = fieldGrid; }

   /**
    * \brief Add station to the station group.
    * \param[in] station Station to be added.
    */
   void AddStation(const LitStationMuon<T>& station) { fStations.push_back(station); }

   /**
    * \brief Return station by index.
    * \param[in] index Index of the station.
    * \return Station by index.
    */
   const LitStationMuon<T>& GetStation(unsigned short index) const { return fStations[index]; }

   /**
    * \brief Return number of stations in this station group.
    * \return Number of stations in this station group.
    */
   unsigned char GetNofStations() const { return fStations.size(); }

   /**
    * \brief Return absorber.
    * \return Absorber.
    */
   const LitAbsorber<T>& GetAbsorber() const { return fAbsorber; }

   /**
    * \brief Set absorber.
    * \param[in] absorber Absorber value.
    */
   void SetAbsorber(const LitAbsorber<T>& absorber) { fAbsorber = absorber; }

   /**
    * \brief Calculate field region for the group of stations.
    * \param[in] x X position.
    * \param[in] y Y position.
    * \param[out] field Output field region.
    */
   void GetFieldRegion(T x, T y, LitFieldRegion<T>& field) const {
      LitFieldValue<T> v1, v2, v3;
      fFieldGridFront.GetFieldValue(x, y, v1);
      fFieldGridMiddle.GetFieldValue(x, y, v2);
      fFieldGridBack.GetFieldValue(x, y, v3);
      field.Set(v1, fFieldGridFront.GetZ(), v2, fFieldGridMiddle.GetZ(), v3, fFieldGridBack.GetZ());
   }

   /**
    * \brief Return std::string representation for the class.
    * \return std::string representation for the class.
    */
   std::string ToString() const {
      std::string str = "LitStationGroupMuon: nofStations="
            + lit::parallel::ToString<int>((int)GetNofStations()) + "\n";
      for (unsigned char i = 0; i < GetNofStations(); i++) {
         str += "  " + lit::parallel::ToString<int>((int)i) + " " + GetStation(i).ToString();
      }
      str += "  " + GetAbsorber().ToString();
      return str;
   }

   /**
    * \brief Operator << for convenient output to std::ostream.
    * \return std::ostream for continuous output.
    */
   friend std::ostream& operator<<(std::ostream& strm, const LitStationGroupMuon& stationGroup) {
      strm << stationGroup.ToString();
      return strm;
   }

private:
   // This field slices are used to construct LitFieldRegion
   // in the group of stations. We need 3 values in order to use
   // parabolic approximation. So the values are in front, middle
   // and back of the gap.
   LitFieldGrid fFieldGridFront; // Magnetic field approximation in front
   LitFieldGrid fFieldGridMiddle; // Magnetic field approximation in the middle
   LitFieldGrid fFieldGridBack; // Magnetic field approximation in the back

   std::vector<LitStationMuon<T> > fStations; // Array with stations
   LitAbsorber<T> fAbsorber; // Absorber
} _fvecalignment;

/* Some typedefs for convenience */
typedef LitStationGroupMuon<fvec> LitStationGroupMuonVec;
typedef LitStationGroupMuon<fscal> LitStationGroupMuonScal;



/**
 * \class LitDetectorLayoutMuon
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2009
 * \brief Muon detector layout.
 **/
template<class T>
class LitDetectorLayoutMuon
{
public:
   /**
    * \brief Constructor.
    */
   LitDetectorLayoutMuon():fStationGroups() {};

   /**
    * \brief Destructor.
    */
   virtual ~LitDetectorLayoutMuon() {}

   /**
    * \brief Add station group to the layout.
    * \param stationGroup Station group to be added.
    */
   void AddStationGroup(const LitStationGroupMuon<T>& stationGroup) { fStationGroups.push_back(stationGroup); }

   /**
    * \brief Return number of station groups.
    * \return Number of station groups.
    */
   unsigned char GetNofStationGroups() const { return fStationGroups.size(); }

   /**
    * \brief Return number of stations for specified station group.
    * \param[in] stationGroup Station group index.
    * \return Number of stations for specified station group.
    */
   unsigned char GetNofStations(unsigned char stationGroup) const {
	   return fStationGroups[stationGroup].GetNofStations();
   }

   /**
    * \brief Return number of substations for specified station group and station.
    * \param[in] stationGroup Station group index.
    * \param[in] station Station index.
    * \return Number of substations for specified station group and station.
    */
   unsigned char GetNofSubstations(unsigned char stationGroup, unsigned char station) const {
      return fStationGroups[stationGroup].GetStation(station).GetNofSubstations();
   }

   /**
    * \brief Return station group for specified station group index.
    * \param[in] stationGroup Station group index.
    * \return Station group for specified station group index.
    */
   const LitStationGroupMuon<T>& GetStationGroup(unsigned char stationGroup) const {
      return fStationGroups[stationGroup];
   }

   /**
    * \brief Return station for specified station group and station indices.
    * \param[in] stationGroup Station group index.
    * \param[in] station Station index.
    * \return Station for specified station group and station indices.
    */
   const LitStationMuon<T>& GetStation(unsigned char stationGroup, unsigned char station) const {
      return fStationGroups[stationGroup].GetStation(station);
   }

   /**
    * \brief Return substation for specified station group, station and substation indices.
    * \param[in] stationGroup Station group index.
    * \param[in] station Station index.
    * \param[in] substation Substation index.
    * \return[in] Substation for specified station group, station and substation indices.
    */
   const LitSubstationMuon<T>& GetSubstation(unsigned char stationGroup, unsigned char station, unsigned char substation) const {
      return fStationGroups[stationGroup].GetStation(station).GetSubstation(substation);
   }

   /**
    * \brief Return std::string representation of the class.
    * \return std::string representation of the class.
    */
   std::string ToString() const {
      std::string str = "LitDetectorLayoutMuon: nofStationGroups="
            + lit::parallel::ToString<int>((int)GetNofStationGroups()) + "\n";
      for (unsigned char i = 0; i < GetNofStationGroups(); i++) {
         str += lit::parallel::ToString<int>((int)i) + " " + GetStationGroup(i).ToString();
      }
      return str;
   }

   /**
    * \brief Operator << for convenient output to std::ostream.
    * \return std::ostream for continuous output.
    */
   friend std::ostream& operator<<(std::ostream& strm, const LitDetectorLayoutMuon& layout) {
      strm << layout.ToString();
      return strm;
   }

private:
   std::vector<LitStationGroupMuon<T> > fStationGroups; // Array of station groups
} _fvecalignment;

/* Some typedefs for convenience */
typedef LitDetectorLayoutMuon<fvec> LitDetectorLayoutMuonVec;
typedef LitDetectorLayoutMuon<fscal> LitDetectorLayoutMuonScal;

} // namespace parallel
} // namespace lit
#endif /*LITDETECTORGEOMETRYMUON_H_*/
