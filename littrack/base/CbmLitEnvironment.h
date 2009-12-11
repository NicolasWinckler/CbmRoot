/** CbmLitEnvironment.h
 *@author A.Lebedev <alebedev@jinr.ru>
 *@since 2007
 **
 ** The class is an interface to the CBM detector classes and field.
 ** The detector geometries in the form of CbmLitDetectorLayout class
 ** can be got via this class.
 **/

#ifndef CBMLITENVIRONMENT_H_
#define CBMLITENVIRONMENT_H_

#include "CbmLitDetectorLayout.h"
#include "parallel/LitDetectorGeometry.h"

#include <vector>
#include <string>

class FairField;
class TGeoNode;
class TGeoMaterial;

class CbmLitEnvironment
{
public:
	virtual ~CbmLitEnvironment();

	static CbmLitEnvironment* Instance();

	FairField* GetField();
	CbmLitDetectorLayout GetLayout();

	CbmLitDetectorLayout GetMuchLayout() {return fMuchLayout;}

	void GetMuchLayoutVec(LitDetectorLayoutVec& layout);

	int MaterialId(
			int stationGroup,
			int station,
			int substation,
			CbmLitDetectorLayout& layout) const;

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
	void DetermineLayout(
		const std::vector<CbmLitStation>& stations,
		CbmLitDetectorLayout& layout);

	std::vector<CbmLitStation> DivideToSubstations(
			const std::vector<CbmLitStation>& stations);

	bool IsStraw() const;
	bool IsTrdSegmented() const;

	bool CheckDetectorPresence(
			const std::string& name) const;

	static CbmLitEnvironment* fInstance;

	CbmLitDetectorLayout fLayout;
	CbmLitDetectorLayout fMuchTrdLayout;
	CbmLitDetectorLayout fMuchLayout;
	CbmLitDetectorLayout fTrdLayout;
	CbmLitDetectorLayout fTofLayout;
	CbmLitStation fTofStation;

	FairField *fField;
};

#endif // CBMLITENVIRONMENT_H_
