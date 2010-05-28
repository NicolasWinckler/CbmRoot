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
    if (posx > fSectorBeginX.GetAt(i) &&
        posx < fSectorEndX.GetAt(i) &&
        posy > fSectorBeginY.GetAt(i) &&
        posy < fSectorEndY.GetAt(i) ) {
      return i;
    } 
  }

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
    Sector = 1;

    GetModuleInformation(ModuleID, local_point, Col, Row);

}

// --------------------------------------------------------------------


void CbmTrdModule::GetModuleInformation(Int_t VolumeID, Double_t *local_point, Int_t &Col, Int_t &Row)
{


  // safety check. Up to now allways correct, so could be removed.
  if (fDetectorId != VolumeID ){
    cout<<" -E- This is wrong!!!!!!!!!!!!!!!!!!!!!"<<endl;
  }

  // calculate in which sector the point is
  Int_t sector = GetSector(local_point);

  Double_t fpadsizex = GetPadSizex(sector);
  Double_t fpadsizey = GetPadSizey(sector);
  Double_t fsizex    = GetSizex();
  Double_t fsizey    = GetSizey();

  // Calculate the position in the chamber with the origin of
  // the local coordinate system in the lower left corner
  // of the chamber. x goes to the left looking in beam direction
  // y goes upward

  if (fIsRotated){
    Double_t tempx = local_point[0];
    Double_t tempy = local_point[1];
    local_point[1] = -tempx;
    local_point[0] =  tempy;

    tempx = fsizex;
    tempy = fsizey;
    fsizey = tempx;
    fsizex = tempy;
  }

  Double_t fPosX=local_point[0]+fsizex;
  Double_t fPosY=local_point[1]+fsizey;
 
  /*
  if ( fVerbose > 2 ) {
    cout<<setprecision(5)<<"*** CbmTrdDigitizer::GetModuleInformationFromDigiPar ***"<<endl;
    cout<<setprecision(5)<<"sizex: "<< fsizex <<endl;
    cout<<setprecision(5)<<"sizey: "<< fsizey <<endl;
    cout<<setprecision(5)<<"padsizex: "<< fpadsizex <<endl;
    cout<<setprecision(5)<<"padsizey: "<< fpadsizey <<endl;
  
    cout<<setprecision(5)<<"X: "<< local_point[0] <<endl;
    cout<<setprecision(5)<<"Y: "<< local_point[1] <<endl;
    cout<<setprecision(5)<<"fPosX: "<< fPosX <<endl;
    cout<<setprecision(5)<<"fPosY: "<< fPosY <<endl;
    cout<<"DetID: "<< detID <<endl;
  
    cout<<setprecision(5)<<"CPosX: "<< fModuleInfo->GetX() <<endl;
    cout<<setprecision(5)<<"CPosY: "<< fModuleInfo->GetY() <<endl;
    cout<<setprecision(5)<<"CPosZ: "<< fModuleInfo->GetZ() <<endl;
  }
  */  

  Col = 1+(Int_t)(fPosX/fpadsizex);
  Row = 1+(Int_t)(fPosY/fpadsizey);

  /*
  if ( fVerbose > 2 ){
    cout<<"*** CbmTrdDigitizer::CalculatePixel ***"<<endl;
    cout<<setprecision(5)<<"fPosX: "<< fPosX <<endl;
    cout<<setprecision(5)<<"fPosY: "<< fPosY <<endl;
    cout<<setprecision(5)<<"fPadX: "<< fpadsizex <<endl;
    cout<<setprecision(5)<<"fPadY: "<< fpadsizey <<endl;
    cout<<"Col: "<< fCol <<endl;
    cout<<"Row: "<< fRow <<endl;
  } 
  */

  /*
  cout<<"#######################################"<<endl;
  cout<<VolumeID<<","<<Col<<","<<Row<<endl;
  cout<<VolumeID<<","<<setprecision(5)<<fPosX<<","<<fPosY<<endl;
  */

}


// ---- CalculateHitPosition ------------------------------------------
void CbmTrdModule::GetPosition(const Int_t Col, const Int_t Row, 
				const Int_t VolumeId, const Int_t Sector,
                                TVector3 &posHit, TVector3 &posHitErr) {



  if (fDetectorId != VolumeId ){
    cout<<" -E- This is wrong!!!!!!!!!!!!!!!!!!!!!"<<endl;
  }
    
  Double_t fpadsizex = GetPadSizex(Sector-1);
  Double_t fpadsizey = GetPadSizey(Sector-1);
  Double_t fsizex    = GetSizex();
  Double_t fsizey    = GetSizey();

  if (fIsRotated){
    Double_t tempx = fsizex;
    Double_t tempy = fsizey;
    fsizey = tempx;
    fsizex = tempy;
  }


  Float_t local_point[2];

  local_point[0] = ( ((Col-0.5) * fpadsizex) - fsizex);
  local_point[1] = ( ((Row-0.5) * fpadsizey) - fsizey);

  Float_t PosX=local_point[0]+GetX();
  Float_t PosY=local_point[1]+GetY();
  Float_t PosZ=GetZ();
  Float_t xHitErr = fpadsizex / TMath::Sqrt(12.);
  Float_t yHitErr = fpadsizey / TMath::Sqrt(12.);

  posHit.SetXYZ(PosX, PosY, PosZ);
  posHitErr.SetXYZ(xHitErr,yHitErr, 0.);

  

//  if ( fVerbose > 2 ){ 
//    cout<<"*** CbmTrdHitProducerDigi::CalculateHitPosition ***"<<endl;
//    cout<<"Col: "<< fCol <<endl;
//    cout<<"Row: "<< fRow <<endl;
//    cout<<setprecision(5)<<"fPadX: "<< fpadsizex <<endl;
//    cout<<setprecision(5)<<"fPadY: "<< fpadsizey <<endl;
//    cout<<setprecision(5)<<"fsizex: "<< fsizex <<endl;
//    cout<<setprecision(5)<<"fsizey: "<< fsizey <<endl;
//    cout<<setprecision(5)<<"localx: "<<  local_point[0] <<endl;
//    cout<<setprecision(5)<<"localy: "<<  local_point[1] <<endl;
//  
//    cout<<setprecision(5)<<"fPosX: "<<  fPosX <<endl;
//    cout<<setprecision(5)<<"fPosY: "<<  fPosY <<endl;
//    cout<<setprecision(5)<<"fPosZ: "<<  fPosZ <<endl;
//  }
  
}


// -------------------------------------------------------------------------
ClassImp(CbmTrdModule)
