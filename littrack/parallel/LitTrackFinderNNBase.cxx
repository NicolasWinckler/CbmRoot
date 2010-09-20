/** LitTrackFinderNNBase.cxx
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2010
 * @version 1.0
 **/

#include "LitTrackFinderNNBase.h"

#include <algorithm>

//template<class T>
//LitTrackFinderNNBase<T>::LitTrackFinderNNBase():
LitTrackFinderNNBase::LitTrackFinderNNBase():
	fSigmaCoef(5.),
	fMaxCovSq(20*20)
{

}

//template<class T>
//LitTrackFinderNNBase<T>::~LitTrackFinderNNBase()
LitTrackFinderNNBase::~LitTrackFinderNNBase()
{

}

//template<class T>
//void LitTrackFinderNNBase<T>::MinMaxIndex(
void LitTrackFinderNNBase::MinMaxIndex(
		const LitTrackParamScal* par,
		LitScalPixelHit** hits,
		unsigned int nofHits,
		fscal maxErr,
		unsigned int &first,
		unsigned int &last)
{
	LitScalPixelHit hit;
	first = 0;
	last = 0;
	if (par->C0 > fMaxCovSq || par->C0 < 0.) return;
	if (nofHits == 0) return;

	fscal devX = fSigmaCoef * (std::sqrt(par->C0) + maxErr);
//	fscal devX = fSigmaCoef * std::sqrt(par->C0 + maxErr * maxErr);
	LitScalPixelHit** begin = &hits[0];
	LitScalPixelHit** end = &hits[0] + nofHits;
	hit.X = par->X - devX;
	first = std::distance(begin, std::lower_bound(begin, end, &hit, ComparePixelHitXLess()));

	if (first >= nofHits) {
		first = 0;
		last = 0;
		return;
	}

	hit.X = par->X + devX;
	last = std::distance(begin, std::lower_bound(begin, end, &hit, ComparePixelHitXLess()));
	if (last >= nofHits) {
		last = nofHits;
	}
}
