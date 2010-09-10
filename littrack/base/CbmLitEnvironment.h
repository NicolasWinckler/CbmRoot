/** CbmLitEnvironment.h
 *@author A.Lebedev <alebedev@jinr.ru>
 *@since 2007
 **
 ** The class is an interface to the CBM detector classes and field.
 ** The detector geometry which is represented in the form of
 ** CbmLitDetectorLayout class constructed in this class.
 **/

#ifndef CBMLITENVIRONMENT_H_
#define CBMLITENVIRONMENT_H_

#include "CbmLitDetectorLayout.h"
#include "parallel/muon/LitDetectorGeometryMuon.h"
#include "parallel/electron/LitDetectorGeometryElectron.h"

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
	const CbmLitDetectorLayout& GetLayout();
//	void GetLayoutParallelVec(LitDetectorLayoutVec& layout);
//	void GetLayoutParallelScal(LitDetectorLayoutScal& layout);
//	template<class T> void GetLayoutParallel(LitDetectorLayoutMuon<T>& layout);

	const CbmLitDetectorLayout& GetMuchLayout() {return fMuchLayout;}
	const CbmLitDetectorLayout& GetTrdLayout() {return fTrdLayout;}

	void GetMuchLayoutVec(LitDetectorLayoutVec& layout);
	void GetMuchLayoutScal(LitDetectorLayoutScal& layout);
	template<class T> void GetMuchLayout(LitDetectorLayoutMuon<T>& layout);

	void GetTrdLayoutVec(LitDetectorLayoutElectronVec& layout);
	void GetTrdLayoutScal(LitDetectorLayoutElectronScal& layout);
	template<class T> void GetTrdLayout(LitDetectorLayoutElectron<T>& layout);

	int MaterialId(
			int stationGroup,
			int station,
			int substation,
			const CbmLitDetectorLayout& layout) const;

	int TrdMaterialId(
			int stationGroup,
			int station,
			const CbmLitDetectorLayout& layout) const;

	const CbmLitDetectorLayout& GetTofLayout();
	const CbmLitStation& GetTofStation();

	bool IsElectronSetup() const;
	bool IsSts() const;
	bool IsTrd() const;
	bool IsMuch() const;
	bool IsTof() const;

	bool IsTrdSimple() const;

protected:
	CbmLitEnvironment();

	void CombineMuchAndTrd();
	void MuchLayout();
	void TrdLayout();
	void TrdLayoutSimple();

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
