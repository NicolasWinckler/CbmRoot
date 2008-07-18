// -------------------------------------------------------------------------
// -----                   CbmEcalRecParticle source file              -----
// -----                   Created 04/07/05  by Yu.Kharlov             -----
// -------------------------------------------------------------------------

  /* $Id: CbmEcalRecParticle.cxx,v 1.1 2006/06/22 14:02:17 kharlov Exp $ */

  /* History of cvs commits:
   *
   * $Log: CbmEcalRecParticle.cxx,v $
   * Revision 1.1  2006/06/22 14:02:17  kharlov
   * First upload of reconstruction classes for Full MC
   *
   */

/**  CbmEcalRecParticle.cxx
 *@author Yu.Kharlov <Y.Kharlov at gsi.de>
 **
 ** Reconstructed particle in ECAL, characterized by reconstructed 4-momentum
 **/

#include "CbmEcalRecParticle.h"

#include <iostream>

using std::cout;
using std::endl;

// -----   Default constructor   -------------------------------------------
CbmEcalRecParticle::CbmEcalRecParticle() : fPx(0),fPy(0),fPz(0),fE(0), fX(-1111), fY(-1111), fZ(-1111), fId(0), fChi2(-1111), fType(0), fCluster(NULL)
{
  ;
}
// -------------------------------------------------------------------------


// -----   Standard constructor   ------------------------------------------
CbmEcalRecParticle::CbmEcalRecParticle(Double_t px, Double_t py, 
				       Double_t pz, Double_t E,
				       Double_t x, Double_t y, Double_t z, 
				       Int_t id, Double_t ch2, CbmEcalClusterV1* cluster,
				       Int_t type)
  : fPx(px),fPy(py),fPz(pz),fE(E), fX(x), fY(y), fZ(z), fId(id), fChi2(ch2), fType(type), fCluster(cluster)
{
  ;
}
// -------------------------------------------------------------------------


// -----   Destructor   ----------------------------------------------------
CbmEcalRecParticle::~CbmEcalRecParticle()
{
  ;
}
// -------------------------------------------------------------------------


// -----   Public method Print   -------------------------------------------
void CbmEcalRecParticle::Print(const Option_t* opt) const {
  printf("-I- CbmEcalRecParticle: ECAL rec.particle :(px,py,pz,E) = (%f,%f,%f,%f) GeV\n",
	 fPx,fPy,fPz,fE);
}
// -------------------------------------------------------------------------


ClassImp(CbmEcalRecParticle)
