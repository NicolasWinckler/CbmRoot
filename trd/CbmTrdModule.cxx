#include "CbmTrdModule.h"

#include "CbmTrdPoint.h"

#include "TGeoManager.h"
#include "TMath.h"

#include <iostream>
using std::cout;
using std::endl;

// -----   Default constructor   -------------------------------------------
CbmTrdModule::CbmTrdModule() 
  :
  fDetectorId(0),
  fX(-666.),
  fY(-666.),
  fZ(-666.),
  fSizex(-666.),
  fSizey(-66.),
  fNoSectors(0),  
  fSectorX(0),    
  fSectorY(0),    
  fSectorZ(0),    
  fSectorBeginX(0),
  fSectorBeginY(0),
  fSectorSizex(0),
  fSectorSizey(0),
  fPadSizex(0),   
  fPadSizey(0),   
  fIsRotated(kFALSE)

{
}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
CbmTrdModule::CbmTrdModule(Int_t detId, Double_t x, Double_t y, Double_t z, 
                           Double_t sizex, Double_t sizey, 
                           Bool_t rotated) 
  :
  fDetectorId(detId),
  fX(x),
  fY(y),
  fZ(z),
  fSizex(sizex),
  fSizey(sizey),
  fNoSectors(0),  
  fSectorX(0),    
  fSectorY(0),    
  fSectorZ(0), 
  fSectorBeginX(0),
  fSectorBeginY(0),
  fSectorSizex(0),
  fSectorSizey(0),
  fPadSizex(0),   
  fPadSizey(0),   
  fIsRotated(rotated)
{
}
// -----   Standard constructor   ------------------------------------------
CbmTrdModule::CbmTrdModule(Int_t detId, Double_t x, Double_t y, Double_t z, 
                           Double_t sizex, Double_t sizey, Int_t nSectors, 
                           TArrayD sectorX, TArrayD sectorY, TArrayD sectorZ,
                           TArrayD sectorSizeX, TArrayD sectorSizeY,
                           TArrayD padSizeX, TArrayD padSizeY,      
                           Bool_t rotated) 
  :
  fDetectorId(detId),
  fX(x),
  fY(y),
  fZ(z),
  fSizex(sizex),
  fSizey(sizey),
  fNoSectors(nSectors),  
  fSectorX(sectorX),    
  fSectorY(sectorY),    
  fSectorZ(sectorZ),    
  fSectorSizex(sectorSizeX),
  fSectorSizey(sectorSizeY),
  fPadSizex(padSizeX),   
  fPadSizey(padSizeY),   
  fIsRotated(rotated)
{
}
// -------------------------------------------------------------------------
CbmTrdModule::CbmTrdModule(Int_t detId, Double_t x, Double_t y, Double_t z, 
                           Double_t sizex, Double_t sizey, Int_t nSectors, 
                           TArrayD sectorSizeX, TArrayD sectorSizeY,
                           TArrayD padSizeX, TArrayD padSizeY,      
                           Bool_t rotated) 
  :
  fDetectorId(detId),
  fX(x),
  fY(y),
  fZ(z),
  fSizex(sizex),
  fSizey(sizey),
  fNoSectors(nSectors),  
  fSectorX(nSectors),    
  fSectorY(nSectors),    
  fSectorZ(nSectors),    
  fSectorBeginX(nSectors),    
  fSectorBeginY(nSectors),    
  fSectorEndX(nSectors),    
  fSectorEndY(nSectors),    
  fSectorSizex(sectorSizeX),
  fSectorSizey(sectorSizeY),
  fPadSizex(padSizeX),   
  fPadSizey(padSizeY),   
  fIsRotated(rotated)
{
  // Calculate the coordinates of the begin and the end of each sector
  // as well as  the cordinates of the center of the sector
  // In the moment it is assumed that there are sectors which have either
  // in x- or y-direction the size of the chamber.

  if(nSectors>1) {

    Double_t beginx, beginy, endx, endy;
    Double_t summed_sectsize;
    if ( sectorSizeX.GetAt(0) == 2*sizex ) { //substructure only in y-direction
      beginx = 0;
      endx = 2*sizex;
      summed_sectsize=0;
      for( Int_t i=0; i<fNoSectors; i++){
	if ( 0==i ){
	  beginy = 0.;
	  endy = sectorSizeY.GetAt(i);
	  summed_sectsize+=sectorSizeY.GetAt(i);
	}
	else {
	  beginy = summed_sectsize;
	  endy = summed_sectsize+=sectorSizeY.GetAt(i);
	}
	fSectorBeginX.AddAt(beginx,i);
	fSectorBeginY.AddAt(beginy,i); 
	fSectorEndX.AddAt(endx,i);
	fSectorEndY.AddAt(endy,i); 
	fSectorX.AddAt(beginx+(sectorSizeX.GetAt(i)/2),i);
	fSectorY.AddAt(beginy+(sectorSizeY.GetAt(i)/2),i);
	fSectorZ.AddAt(fZ,i);
      }
    } else {
      beginy = 0;
      endy = 2*sizey;
      summed_sectsize=0;
      for( Int_t i=0; i<fNoSectors; i++){
	if ( 0==i ){
	  beginx = 0.;
	  endx = sectorSizeX.GetAt(i);
	  summed_sectsize+=sectorSizeX.GetAt(i);
	}
	else {
	  beginx = summed_sectsize;
	  endx = summed_sectsize+=sectorSizeX.GetAt(i);
	}
	fSectorBeginX.AddAt(beginx,i);
	fSectorBeginY.AddAt(beginy,i); 
	fSectorEndX.AddAt(endx,i);
	fSectorEndY.AddAt(endy,i); 
	fSectorX.AddAt(beginx+(sectorSizeX.GetAt(i)/2),i);
	fSectorY.AddAt(beginy+(sectorSizeY.GetAt(i)/2),i);
	fSectorZ.AddAt(fZ,i);
      }
    }
  } else {
    fSectorBeginX.AddAt(0.,0);
    fSectorBeginY.AddAt(0.,0); 
    fSectorEndX.AddAt(sectorSizeX.GetAt(0),0);
    fSectorEndY.AddAt(sectorSizeY.GetAt(0),0); 
    fSectorX.AddAt(x,0);
    fSectorY.AddAt(y,0);
    fSectorZ.AddAt(z,0);
  }
}
// -------------------------------------------------------------------------
CbmTrdModule::~CbmTrdModule() 
{
}

Int_t CbmTrdModule::GetSector(Double_t *local_point)
{
  // Calculate the position in the chamber with the origin of
  // the local coordinate system in the lower left corner
  // of the chamber. x goes to the left looking in beam direction
  // y goes upward

  Double_t sizex=fSizex;
  Double_t sizey=fSizey;

  if (fIsRotated){
    Double_t tempx = local_point[0];
    Double_t tempy = local_point[1];
    local_point[1] = -tempx;
    local_point[0] =  tempy;

    tempx = sizex;
    tempy = sizey;
    sizey = tempx;
    sizex = tempy;
  }

  Double_t posx=local_point[0]+sizex;
  Double_t posy=local_point[1]+sizey;

  for(Int_t i=0; i<fNoSectors; i++){
    if (posx >= fSectorBeginX.GetAt(i) &&
        posx <  fSectorEndX.GetAt(i)   &&
        posy >= fSectorBeginY.GetAt(i) &&
        posy <  fSectorEndY.GetAt(i) ) 
    {
      return i;
    } 
  }

  Error("CbmTrdModule::GetSector","Could not find local point in any of the sectors");
  return -1; //This should be never reached

}



void CbmTrdModule::GetPadInfo(CbmTrdPoint *trdPoint, Int_t &Col, 
                              Int_t &Row, Int_t &Sector)
{

    // Calculate point in the middle of the detector. This is
    // for safety reasons, because the point at exit is slightly
    // outside of the active volume. If one does not use a point
    // in the detector one will get a wrong volume from the
    // geomanager. Navigate to the correct volume to get all
    // necessary information about this volume

    Double_t x_mean = (trdPoint->GetXIn()+trdPoint->GetXOut())/2.;
    Double_t y_mean = (trdPoint->GetYIn()+trdPoint->GetYOut())/2.;
    Double_t z_mean = (trdPoint->GetZIn()+trdPoint->GetZOut())/2.;
    gGeoManager->FindNode(x_mean, y_mean, z_mean);

    // Get the local point in local MC coordinates from
    // the geomanager. This coordinate system is rotated
    // if the chamber is rotated. This is corrected in 
    // GetModuleInformation to have a
    // the same local coordinate system in all the chambers
    const Double_t *global_point = gGeoManager->GetCurrentPoint();
    Double_t local_point[3];  // global_point[3];
    gGeoManager->MasterToLocal(global_point, local_point);

    Int_t ModuleID = trdPoint->GetDetectorID();

    GetModuleInformation(ModuleID, local_point, Col, Row, Sector);

}

// --------------------------------------------------------------------

void CbmTrdModule::TransformToLocalCorner(Double_t *local_point, Double_t &posX,
                                          Double_t &posY)
{
  // Transformation from local coordinate system with origin in
  // the middle of the module into a system with origin in the
  // lower right corner of the module. Since for both coordinate
  // systems the orientation is the same this is only a shift by
  // the half size of the module in x- and y-direction
  posX=local_point[0]+fSizex;
  posY=local_point[1]+fSizey;
}

void CbmTrdModule::TransformToLocalSector(Double_t *local_point, 
                                          const Double_t &sector,
                                          Double_t &posX, Double_t &posY)
{
  // Transformation of the module coordinate system with origin
  // in the middle of the chamber into a system with 
  // the origin in the lower right corner of the sector the point
  // is in. First transform in a system with origin in the lower
  // right corner.
  TransformToLocalCorner(local_point, posX, posY);
  posX-=fSectorBeginX.GetAt(sector);
  posY-=fSectorBeginY.GetAt(sector);
}

void CbmTrdModule::GetModuleInformation(Int_t VolumeID, Double_t *local_point, Int_t &Col, Int_t &Row, Int_t &sector)
{

  // safety check. Up to now allways correct, so could be removed.
  if (fDetectorId != VolumeID ){
    cout<<" -E- This is wrong!!!!!!!!!!!!!!!!!!!!!"<<endl;
  }

  // calculate in which sector the point is
  sector = GetSector(local_point);

  Double_t posX, posY;
  Double_t fpadsizex = GetPadSizex(sector);
  Double_t fpadsizey = GetPadSizey(sector);

  TransformToLocalSector(local_point, sector, posX, posY);

  Col = 1+(Int_t)(posX/fPadSizex.At(sector));
  Row = 1+(Int_t)(posY/fPadSizey.At(sector));

}


// ---- CalculateHitPosition ------------------------------------------
void CbmTrdModule::GetPosition(const Int_t Col, const Int_t Row, 
				const Int_t VolumeId, const Int_t sector,
                                TVector3 &posHit, TVector3 &padSize) {

  // calculate position in global coordinates from digi 
  // information(Sector, Col, Row).  
  // Returns two TVector3. One with the position and one 
  // with the padsize of the fired pad

  if (fDetectorId != VolumeId ){
    cout<<" -E- This is wrong!!!!!!!!!!!!!!!!!!!!!"<<endl;
  }
    
  Double_t local_point[2];
  Double_t padsizex = fPadSizex.At(sector);
  Double_t padsizey = fPadSizey.At(sector);

  // calculate position in sector coordinate system with the
  // origin in the lower right corner
  local_point[0] = (((Float_t)Col-0.5) * padsizex);
  local_point[1] = (((Float_t)Row-0.5) * padsizey); 

  // calculate position in module coordinate system
  // with origin in the lower right corner of the module
  local_point[0]+=fSectorBeginX.GetAt(sector);
  local_point[1]+=fSectorBeginY.GetAt(sector);

  // calculte position in the module coordinate system 
  // with origin in the middle of the module
  local_point[0]-=fSizex;
  local_point[1]-=fSizey;

  // calculate the position in the global coordinate system
  // with the origin in target
  Float_t posX=local_point[0]+fX;
  Float_t posY=local_point[1]+fY;
  Float_t posZ=fZ;

  posHit.SetXYZ(posX, posY, posZ);
  padSize.SetXYZ(padsizex,padsizey, 0.);
  
}


// -------------------------------------------------------------------------
ClassImp(CbmTrdModule)
