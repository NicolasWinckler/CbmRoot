/** CbmBaseHit.h
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2009
 * @version 1.0
 **
 ** Base class for hits used for tracking in CBM.
 ** Derives from TObject.
 ** Each hit has its unique identifier of type HitType,
 ** which can be later use for safe type casting.
 **/
#ifndef CBMBASEHIT_H_
#define CBMBASEHIT_H_

enum HitType {
	kHIT,
	kPIXELHIT,
	kSTRIPHIT,
	kMUCHPIXELHIT,
	kMUCHSTRAWHIT
};

#include "TObject.h"
#include "TString.h"

class CbmBaseHit : public TObject
{
public:
	/** Default constructor */
	CbmBaseHit();

	/**
     * Standard constructor.
     * @param detectorId detector unique identifier
     * @param z Z position of the hit [cm]
     * @param dz Z position error of the hit
     * @param refId some reference ID
     **/
	CbmBaseHit(
			Int_t detectorId,
			Double_t z,
			Double_t dz,
			Int_t refId);

	/** Destructor */
	virtual ~CbmBaseHit();

	/**
	 * Pure virtual function. Must be implemented in derived class.
	 * Should print the object current state to the standard std::cout output.
	 **/
	virtual void Print() const = 0;

	/** Returns the hit type */
	HitType GetType() const {return fType;}

	/** Returns Z position of the hit */
	Double_t GetZ() const {return fZ;}

	/** Returns Zposition error of the hit */
	Double_t GetDz() const {return fDz;}

	/** Returns reference ID*/
	Int_t GetRefId() const {return fRefId;}

	/** Returns detector unique identifier */
	Int_t GetDetectorId() const {return fDetectorId;}

	/**
	 * Pure virtual function. Must be implemented in derived class.
	 * Should return plane identifier of the hit. Usually this is station or layer
	 * number of the detector. Can be calculated using unique detector identifier
	 * or can use additional class member from the derived class to store the plane identifier.
	 **/
	virtual Int_t GetPlaneId() const = 0;

	/**
     * Sets Z position of the hit.
     * @param z new z position of the hit [cm]
     **/
	void SetZ(Double_t z) {fZ = z;}

	/**
     * Sets Z position error of the hit.
     * @param dz new z position error of the hit [cm]
     **/
	void SetDz(Double_t dz) {fDz = dz;}

	/**
	  * Sets reference identifier for the hit.
	  * @param refId new reference ID
	  **/
	void SetRefId(Int_t refId) {fRefId = refId;}

	/**
	  * Sets unique detector identifier
	  * @param detectorId new detector identifier
	  **/
	void SetDetectorId(Int_t detectorId) {fDetectorId = detectorId;}

protected:
	/**
     * Sets hit type.
     * @param type hit type
     **/
	void SetType(HitType type) {fType = type;}

private:
	HitType fType; // hit type
	Double_t fZ; // Z position of hit [cm]
	Double_t fDz; // Z position error [cm]
	Int_t fRefId; // some reference id (usually to cluster, digi or MC point)
	Int_t fDetectorId; // detector unique identifier

	ClassDef(CbmBaseHit, 1);
};

#endif /* CBMBASEHIT_H_ */
