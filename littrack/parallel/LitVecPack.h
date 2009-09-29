/*
 * LitVecPack.h
 *
 *  Created on: Sep 25, 2009
 *      Author: andrey
 */

#ifndef LITVECPACK_H_
#define LITVECPACK_H_

#include "LitTypes.h"
#include "LitTrackParam.h"
#include "LitHit.h"

inline void PackTrackParam(
		const LitScalTrackParam par[],
		LitTrackParam& lpar)
{
	for(unsigned char i = 0; i < fvecLen; i++) {
		lpar.X[i] = par[i].X;
		lpar.Y[i] = par[i].Y;
		lpar.Tx[i] = par[i].Tx;
		lpar.Ty[i] = par[i].Ty;
		lpar.Qp[i] = par[i].Qp;
		lpar.Z[i] = par[i].Z;
		lpar.C0[i] = par[i].C0;
		lpar.C1[i] = par[i].C1;
		lpar.C2[i] = par[i].C2;
		lpar.C3[i] = par[i].C3;
		lpar.C4[i] = par[i].C4;
		lpar.C5[i] = par[i].C5;
		lpar.C6[i] = par[i].C6;
		lpar.C7[i] = par[i].C7;
		lpar.C8[i] = par[i].C8;
		lpar.C9[i] = par[i].C9;
		lpar.C10[i] = par[i].C10;
		lpar.C11[i] = par[i].C11;
		lpar.C12[i] = par[i].C12;
		lpar.C13[i] = par[i].C13;
		lpar.C14[i] = par[i].C14;
	}
}

inline void UnpackTrackParam(
		const LitTrackParam& lpar,
		LitScalTrackParam par[])
{
	for(unsigned char i = 0; i < fvecLen; i++) {
		par[i].X = lpar.X[i];
		par[i].Y = lpar.Y[i];
		par[i].Tx = lpar.Tx[i];
		par[i].Ty = lpar.Ty[i];
		par[i].Qp = lpar.Qp[i];
		par[i].Z = lpar.Z[i];
		par[i].C0 = lpar.C0[i];
		par[i].C1 = lpar.C1[i];
		par[i].C2 = lpar.C2[i];
		par[i].C3 = lpar.C3[i];
		par[i].C4 = lpar.C4[i];
		par[i].C5 = lpar.C5[i];
		par[i].C6 = lpar.C6[i];
		par[i].C7 = lpar.C7[i];
		par[i].C8 = lpar.C8[i];
		par[i].C9 = lpar.C9[i];
		par[i].C10 = lpar.C10[i];
		par[i].C11 = lpar.C11[i];
		par[i].C12 = lpar.C12[i];
		par[i].C13 = lpar.C13[i];
		par[i].C14 = lpar.C14[i];
	}
}

inline void PackPixelHit(
		const LitScalPixelHit hit[],
		LitPixelHit& lhit)
{
	for(unsigned char i = 0; i < fvecLen; i++) {
		lhit.X[i] = hit[i].X;
		lhit.Y[i] = hit[i].Y;
		lhit.Dx[i] = hit[i].Dx;
		lhit.Dy[i] = hit[i].Dy;
		lhit.Dxy[i] = hit[i].Dxy;
	}
}

inline void UnpackPixelHit(
		const LitPixelHit& lhit,
		LitScalPixelHit hit[])
{
	for(unsigned char i = 0; i < fvecLen; i++) {
		hit[i].X = lhit.X[i];
	    hit[i].Y = lhit.Y[i];
		hit[i].Dx = lhit.Dx[i];
		hit[i].Dy = lhit.Dy[i];
		hit[i].Dxy = lhit.Dxy[i];
	}
}

#endif /* LITVECPACK_H_ */
