#include "CbmLitMyField.h"
#include "CbmLitFieldFitter.h"
#include "CbmLitFieldValue.h"
#include "CbmLitDefaultSettings.h"

#include <vector>
#include <algorithm>

class CompareFieldSliceZLess:
	public std::binary_function<
		const CbmLitFieldSlice&,
	    const CbmLitFieldSlice&,
	    bool>
{
public:
	bool operator()(const CbmLitFieldSlice& slice1, const CbmLitFieldSlice& slice2) const {
		return slice1.GetZ() < slice2.GetZ();
	}
};

CbmLitMyField::CbmLitMyField()
{
	CbmLitFieldFitter ff;
	ff.Fit();
	ff.GetFieldSlices(fSlices);
	std::sort(fSlices.begin(), fSlices.end(), CompareFieldSliceZLess());
}

CbmLitMyField::~CbmLitMyField()
{

}

void CbmLitMyField::GetFieldValue(
		myf x,
		myf y,
		myf z,
		CbmLitFieldValue& value) const
{
	if (z > lit::LINE_EXTRAPOLATION_START_Z) value.SetBxByBz(0., 0., 0.);
	CbmLitFieldSlice slice;
	slice.SetZ(z);
	std::vector<CbmLitFieldSlice>::const_iterator it1 = std::lower_bound(fSlices.begin(), fSlices.end(), slice, CompareFieldSliceZLess());
	std::vector<CbmLitFieldSlice>::const_iterator it2 = it1 - 1;
	CbmLitFieldValue value1, value2;
	(*it1).GetFieldValue(x, y, value1);
	(*it2).GetFieldValue(x, y, value2);
	Combine(value2, (*it2).GetZ(), value1, (*it1).GetZ(), z, value);
}

void CbmLitMyField::Combine(
		const CbmLitFieldValue& B0,
		myf B0z,
		const CbmLitFieldValue& B1,
		myf B1z,
		myf z,
		CbmLitFieldValue& value) const
{
	myf cx0, cy0, cz0, cx1, cy1, cz1;
//		fvec dz = rcp(fvec( B1z - B0z));
	myf dz = myf(1. / (B1z - B0z));
	cx0 = B0.GetBx();
	cy0 = B0.GetBy();
	cz0 = B0.GetBz();
	cx1 = (B1.GetBx() - B0.GetBx()) * dz;
	cy1 = (B1.GetBy() - B0.GetBy()) * dz;
	cz1 = (B1.GetBz() - B0.GetBz()) * dz;
	value.SetBx(cx0 + cx1*(z - B0z));
	value.SetBy(cy0 + cy1*(z - B0z));
	value.SetBz(cz0 + cz1*(z - B0z));
}
