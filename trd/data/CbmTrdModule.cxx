#include "CbmTrdModule.h"

#include "CbmTrdPoint.h"
#include "CbmTrdAddress.h"
#include "FairLogger.h"

#include "TGeoManager.h"
#include "TMath.h"

#include <iomanip>

CbmTrdModule::CbmTrdModule() 
  : TNamed(),
  fModuleAddress(0),
  fOrientation(0),
  fX(0.),
  fY(0.),
  fZ(0.),
  fSizeX(0.),
  fSizeY(0.),
  fSizeZ(0.),
  fAnodeWireOffset(0),
  fAnodeWireSpacing(0),
  fAnodeWireToPadPlaneDistance(0),
  fNofSectors(0),  
  fSectorX(0),    
  fSectorY(0),    
  fSectorZ(0),    
  fSectorBeginX(0),
  fSectorBeginY(0),
  fSectorEndX(0),
  fSectorEndY(0),
  fSectorSizeX(0),
  fSectorSizeY(0),
  fPadSizeX(0),
  fPadSizeY(0)   
{
}

CbmTrdModule::CbmTrdModule(
      Int_t address, Int_t orientation, Double_t x, Double_t y, Double_t z,
      Double_t sizex, Double_t sizey, Double_t sizez, Int_t nofSectors,
      const TArrayD& sectorSizeX, const TArrayD& sectorSizeY,
      const TArrayD& padSizeX, const TArrayD& padSizeY)
  : TNamed(),
  fModuleAddress(address),
  fOrientation(orientation),
  fX(x),
  fY(y),
  fZ(z),
  fSizeX(sizex),
  fSizeY(sizey),
  fSizeZ(sizez),
  fAnodeWireOffset(0.375),
  fAnodeWireSpacing(0.25),
  fAnodeWireToPadPlaneDistance(0.35),
  fNofSectors(nofSectors),
  fSectorX(nofSectors),
  fSectorY(nofSectors),
  fSectorZ(nofSectors),
  fSectorBeginX(nofSectors),
  fSectorBeginY(nofSectors),
  fSectorEndX(nofSectors),
  fSectorEndY(nofSectors),
  fSectorSizeX(sectorSizeX),
  fSectorSizeY(sectorSizeY),
  fPadSizeX(padSizeX),
  fPadSizeY(padSizeY)   
{
   // Calculate the coordinates of the begin and the end of each sector
   // as well as the coordinates of the center of the sector
   // In the moment it is assumed that there are sectors which have either
   // in x- or y-direction the size of the chamber.

   if (nofSectors > 1) {
      Double_t beginx, beginy, endx, endy;
      Double_t summed_sectsize;
      if (sectorSizeX.GetAt(0) == 2 * sizex) { //substructure only in y-direction
         beginx = 0;
         endx = 2 * sizex;
         summed_sectsize = 0;
         for (Int_t i = 0; i < fNofSectors; i++) {
            if (0 == i) {
               beginy = 0.;
               endy = sectorSizeY.GetAt(i);
               summed_sectsize += sectorSizeY.GetAt(i);
            } else {
               beginy = summed_sectsize;
               endy = summed_sectsize += sectorSizeY.GetAt(i);
            }
            fSectorBeginX.AddAt(beginx, i);
            fSectorBeginY.AddAt(beginy, i);
            fSectorEndX.AddAt(endx, i);
            fSectorEndY.AddAt(endy, i);
            fSectorX.AddAt(beginx + (sectorSizeX.GetAt(i) / 2.), i);
            fSectorY.AddAt(beginy + (sectorSizeY.GetAt(i) / 2.), i);
            fSectorZ.AddAt(fZ, i);
         }
      } else {
         beginy = 0;
         endy = 2 * sizey;
         summed_sectsize = 0;
         for (Int_t i = 0; i < fNofSectors; i++) {
            if (0 == i) {
               beginx = 0.;
               endx = sectorSizeX.GetAt(i);
               summed_sectsize += sectorSizeX.GetAt(i);
            } else {
               beginx = summed_sectsize;
               endx = summed_sectsize += sectorSizeX.GetAt(i);
            }
            fSectorBeginX.AddAt(beginx, i);
            fSectorBeginY.AddAt(beginy, i);
            fSectorEndX.AddAt(endx, i);
            fSectorEndY.AddAt(endy, i);
            fSectorX.AddAt(beginx + (sectorSizeX.GetAt(i) / 2.), i);
            fSectorY.AddAt(beginy + (sectorSizeY.GetAt(i) / 2.), i);
            fSectorZ.AddAt(fZ, i);
         }
      }
   } else {
      fSectorBeginX.AddAt(0., 0);
      fSectorBeginY.AddAt(0., 0);
      fSectorEndX.AddAt(sectorSizeX.GetAt(0), 0);
      fSectorEndY.AddAt(sectorSizeY.GetAt(0), 0);
      fSectorX.AddAt(x, 0);
      fSectorY.AddAt(y, 0);
      fSectorZ.AddAt(z, 0);
   }
}

CbmTrdModule::~CbmTrdModule() 
{
}


// 20131009 - DE - checked OK
void CbmTrdModule::ProjectPositionToNextAnodeWire(
      Double_t* local_point) const
{

   // Move the local point along y to the next anode wire position.
   if (fAnodeWireOffset > 0.0 && fAnodeWireSpacing > 0.0) {  // wires must be defined defined

      // check, if the input is within the allowed range
      if ( fabs(local_point[1]) > fSizeY )
        LOG(ERROR) << "CbmTrdModule::ProjectPositionToNextAnodeWire - local point must be within gas volume y=" << std::setprecision(5) << local_point[1] << FairLogger::endl;

      Double_t ypos = local_point[1];

      LOG(DEBUG2) << "local y before: " << std::setprecision(5) << local_point[1] << " mm" << FairLogger::endl;

      // make sure, local_point[1] is not negative (due to module center coordinate)
      // therefore transform to local corner first and then back at the end of operation
      local_point[1] += fSizeY;   // transform to module corner coordinates

      local_point[1] = Int_t(((local_point[1] - fAnodeWireOffset) / fAnodeWireSpacing) + 0.5) * fAnodeWireSpacing + fAnodeWireOffset;   // find closest anode wire

      if ( local_point[1] > 2 * fSizeY - fAnodeWireOffset )
	local_point[1] = 2 * fSizeY - fAnodeWireOffset;   // move inwards to the last anode wire

      local_point[1] -= fSizeY;   // transform back to module center coordinates

      LOG(DEBUG2) << "local y after : " << std::setprecision(5) << local_point[1] << " mm" << FairLogger::endl;

      // check, if we have left the anode wire grid
      if ( fabs(local_point[1]) > fSizeY - fAnodeWireOffset )
        LOG(ERROR) << "CbmTrdModule::ProjectPositionToNextAnodeWire - local point projected outside anode wire plane, from " 
                   << std::setprecision(5) << ypos << " to "
                   << std::setprecision(5) << local_point[1] << " - last anode wire at "
                   << std::setprecision(5) << fSizeY - fAnodeWireOffset << FairLogger::endl;

   } else {
      LOG(ERROR) << "CbmTrdModule::ProjectPositionToNextAnodeWire: fAnodeWireOffset and fAnodeWireSpacing not set. ProjectPositionToNextAnodeWire can not be used." << FairLogger::endl;
   }
}


Int_t CbmTrdModule::GetSector(
      const Double_t* local_point) const
{
   // Calculate the position in the chamber 
   // with the origin of the local coordinate system 
   // in the lower left corner of the chamber (looking upstream)
   // x goes to the left, looking in beam direction, y goes upward

   Double_t posx, posy;

   TransformToLocalCorner(local_point, posx, posy);

   for (Int_t i = 0; i < fNofSectors; i++) {
      if (posx >= fSectorBeginX.GetAt(i) && posx < fSectorEndX.GetAt(i)
       && posy >= fSectorBeginY.GetAt(i) && posy < fSectorEndY.GetAt(i)) {
         return i;
      } 
   }

   LOG(ERROR) << "CbmTrdModule::GetSector: Could not find local point in any of the sectors" << FairLogger::endl;
   return -1; // This should be never reached
}


Int_t CbmTrdModule::GetNofColumns() const  // get total number of pad columns in module
{
   Int_t nofColumns = 0;
   if (fSectorSizeX.At(0) < fSizeX) {
      for (Int_t i = 0; i < fNofSectors; i++) {
         nofColumns += (Int_t)(fSectorSizeX.At(i) / fPadSizeX.At(i) + 0.5);  // need to round for correct result
      }
   } else {
     nofColumns = (Int_t)(fSectorSizeX.At(0) / fPadSizeX.At(0) + 0.5);       // need to round for correct result
   }
   return nofColumns;
}


Int_t CbmTrdModule::GetNofRows() const  // get total number of pad rows in module
{
   Int_t nofRows = 0;
   if (fSectorSizeY.At(0) < fSizeY) {
      for (Int_t i = 0; i < fNofSectors; i++) {
         nofRows += (Int_t)(fSectorSizeY.At(i) / fPadSizeY.At(i) + 0.5);     // need to round for correct result
      }
   } else {
      nofRows = (Int_t)(fSectorSizeY.At(0) / fPadSizeY.At(0) + 0.5);         // need to round for correct result
   }
   return nofRows;
}


Int_t CbmTrdModule::GetNofColumnsInSector(Int_t i) const
{
   return (Int_t)(fSectorSizeX.At(i) / fPadSizeX.At(i) + 0.5);  // need to round for correct result
}


Int_t CbmTrdModule::GetNofRowsInSector(Int_t i) const
{
   return (Int_t)(fSectorSizeY.At(i) / fPadSizeY.At(i) + 0.5);  // need to round for correct result
}


void CbmTrdModule::GetPadInfo(
      const Double_t *local_point,
      Int_t& sectorId,
      Int_t& columnId,
      Int_t& rowId) const
{
   Double_t posX, posY;
   TransformToLocalSector(local_point, posX, posY);

   // calculate in which sector the point is
   sectorId = GetSector(local_point);

   columnId = (Int_t)(posX / fPadSizeX.At(sectorId));
   rowId    = (Int_t)(posY / fPadSizeY.At(sectorId));
}


void CbmTrdModule::GetPadInfo(
      const CbmTrdPoint* trdPoint,
      Int_t& sectorId,
      Int_t& columnId,
      Int_t& rowId) const
{
   // Calculate point in the middle of the detector. This is
   // for safety reasons, because the point at exit is slightly
   // outside of the active volume. If one does not use a point
   // in the detector one will get a wrong volume from the
   // geomanager. Navigate to the correct volume to get all
   // necessary information about this volume

   Double_t x_mean = 0.5 * (trdPoint->GetXIn() + trdPoint->GetXOut());
   Double_t y_mean = 0.5 * (trdPoint->GetYIn() + trdPoint->GetYOut());
   Double_t z_mean = 0.5 * (trdPoint->GetZIn() + trdPoint->GetZOut());
   gGeoManager->FindNode(x_mean, y_mean, z_mean);

   TString curNode = gGeoManager->GetPath();
   if (! curNode.Contains("gas")) {
     // if the point is not in the gas volume print warning
     LOG(WARNING) << "This point is not in the trd gas" << FairLogger::endl;
     /*
     LOG(WARNING) << "x: " << std::setprecision(7) << trdPoint->GetXIn() << ", " << trdPoint->GetXOut()<< FairLogger::endl;
     LOG(WARNING) << "y: " << std::setprecision(7) << trdPoint->GetYIn() << ", " << trdPoint->GetYOut()<< FairLogger::endl;
     LOG(WARNING) << "z: " << std::setprecision(7) << trdPoint->GetZIn() << ", " << trdPoint->GetZOut()<< FairLogger::endl;
     */
     sectorId=-1;
     return;
   }

   // Get the local point in local MC coordinates from
   // the geomanager. This coordinate system is rotated
   // if the chamber is rotated. This is corrected in
   // GetModuleInformation to have a
   // the same local coordinate system in all the chambers
   const Double_t* global_point = gGeoManager->GetCurrentPoint();
   Double_t local_point[3];

   gGeoManager->MasterToLocal(global_point, local_point);

   /* 
   // 20131009 - DE - debuging output to check module orientation 0,1,2,3 with box generator

   // print module orientation
   LOG(INFO) << "module orientation: " << std::setprecision(5) << fOrientation << FairLogger::endl;

   // print global coordinate
   LOG(INFO) << "global x: " << std::setprecision(5) << global_point[0] 
                   << " y: " << std::setprecision(5) << global_point[1] 
                   << " z: " << std::setprecision(5) << global_point[2] << FairLogger::endl;

   // print local coordinate - relative to module center
   LOG(INFO) << "local  x: " << std::setprecision(5) << local_point[0] 
                   << " y: " << std::setprecision(5) << local_point[1] 
                   << " z: " << std::setprecision(5) << local_point[2] << FairLogger::endl;

   Double_t proj_point[3];
   proj_point[0] = local_point[0];
   proj_point[1] = local_point[1];
   proj_point[2] = local_point[2];

   ProjectPositionToNextAnodeWire(proj_point);

   // print local coordinate - relative to module center
   LOG(INFO) << "proj   x: " << std::setprecision(5) << proj_point[0] 
                   << " y: " << std::setprecision(5) << proj_point[1] 
                   << " z: " << std::setprecision(5) << proj_point[2] << FairLogger::endl;

   Double_t corner_point[3];
   corner_point[2] = local_point[2];
   TransformToLocalCorner(local_point, corner_point[0], corner_point[1]);

   // print local coordinate - relative to module corner
   LOG(INFO) << "corner x: " << std::setprecision(5) << corner_point[0] 
                   << " y: " << std::setprecision(5) << corner_point[1] 
                   << " z: " << std::setprecision(5) << corner_point[2] << FairLogger::endl;

   LOG(INFO) << "pos    x: " << std::setprecision(5) << fX 
                   << " y: " << std::setprecision(5) << fY 
                   << " z: " << std::setprecision(5) << fZ 
                 << " ori: " << std::setprecision(5) << fOrientation << FairLogger::endl;

   LOG(INFO) << "size/2 x: " << std::setprecision(5) << fSizeX 
                   << " y: " << std::setprecision(5) << fSizeY 
                   << " z: " << std::setprecision(5) << fSizeZ << FairLogger::endl;

   Double_t sector_point[3];
   sector_point[2] = local_point[2];
   TransformToLocalSector(local_point, sector_point[0], sector_point[1]);

   // print local coordinate - relative to module sector
   LOG(INFO) << "sector x: " << std::setprecision(5) << sector_point[0] 
                   << " y: " << std::setprecision(5) << sector_point[1] 
                   << " z: " << std::setprecision(5) << sector_point[2] << FairLogger::endl;

   // calculate in which sector the point is
   sectorId = GetSector(local_point);
   LOG(INFO) << "sectornr: " << std::setprecision(5) << sectorId << FairLogger::endl;

   LOG(INFO) << "ncol    : " << std::setprecision(5) << GetNofColumns() << FairLogger::endl;
   LOG(INFO) << "nrow    : " << std::setprecision(5) << GetNofRows()    << FairLogger::endl;

   // print local coordinate - relative to module sector
   LOG(INFO) << "sec2   x: " << std::setprecision(5) << fSectorBeginX.GetAt(2)
                   << " y: " << std::setprecision(5) << fSectorBeginY.GetAt(2) << FairLogger::endl;
   LOG(INFO) << "sec1   x: " << std::setprecision(5) << fSectorBeginX.GetAt(1)
	           << " y: " << std::setprecision(5) << fSectorBeginY.GetAt(1) << FairLogger::endl;
   LOG(INFO) << "sec0   x: " << std::setprecision(5) << fSectorBeginX.GetAt(0)
	           << " y: " << std::setprecision(5) << fSectorBeginY.GetAt(0) << FairLogger::endl
	     << FairLogger::endl;
   */

   Int_t moduleAddress = CbmTrdAddress::GetModuleAddress(trdPoint->GetDetectorID());
   GetModuleInformation(moduleAddress, local_point, sectorId, columnId, rowId);
}


// 20131009 - DE - checked OK for module orientations 0,1,2,3
void CbmTrdModule::TransformToLocalCorner(
      const Double_t* local_point,
      Double_t& posX,
      Double_t& posY) const
{
   // Transformation from local coordinate system 
   // with origin in the middle of the module 
   // into a system with the origin in the lower left corner (looking upstream).
   // Since for both coordinate systems the orientation is the same 
   // this is only a shift by the half size of the module in x- and y-direction
   posX = local_point[0] + fSizeX;
   posY = local_point[1] + fSizeY;
}


// 20131009 - DE - checked OK for module orientations 0,1,2,3
void CbmTrdModule::TransformToLocalSector(
      const Double_t* local_point,
      Double_t& posX,
      Double_t& posY) const
{
   // Transformation of the module coordinate system 
   // with origin in the middle of the module 
   // into a system with the origin in the lower left corner (looking upstream)
   // of the sector the point is in. 
   // First, transform in a system with origin in the lower left corner.
   TransformToLocalCorner(local_point, posX, posY);
   Int_t sector = GetSector(local_point);
   posX -= fSectorBeginX.GetAt(sector);
   posY -= fSectorBeginY.GetAt(sector);
}


void CbmTrdModule::TransformToLocalPad(
      const Double_t* local_point,
      Double_t& posX,
      Double_t& posY) const
{
   // Transformation of the local module coordinate system 
   // with origin in the middle of the module  
   // into a system with the origin in the local pad center
   // of the pad below the point.
   // First, transform in a system with origin in the lower left sector.

   Double_t sector_point[2];
   TransformToLocalSector(local_point, sector_point[0], sector_point[1]);

   //   TransformToLocalCorner(local_point, posX, posY);
   Int_t sectorId = GetSector(local_point);

   Double_t padx = (Int_t(sector_point[0] / fPadSizeX.At(sectorId)) + 0.5) * fPadSizeX.At(sectorId);  // x position of pad center 
   Double_t pady = (Int_t(sector_point[1] / fPadSizeY.At(sectorId)) + 0.5) * fPadSizeY.At(sectorId);  // y position of pad center 

   posX = sector_point[0] - padx;
   posY = sector_point[1] - pady;

   /*
   // print debug info
   LOG(INFO) << "sector x: " << std::setprecision(5) << sector_point[0] 
                   << " y: " << std::setprecision(5) << sector_point[1] << FairLogger::endl;

   LOG(INFO) << "pad    x: " << std::setprecision(5) << padx 
                   << " y: " << std::setprecision(5) << pady << FairLogger::endl;

   LOG(INFO) << "diff   x: " << std::setprecision(5) << posX 
	     << " y: " << std::setprecision(5) << posY << FairLogger::endl;

   LOG(INFO) << "npad   x: " << std::setprecision(5) << padx / fPadSizeX.At(sectorId)
                   << " y: " << std::setprecision(5) << pady / fPadSizeY.At(sectorId) << FairLogger::endl << FairLogger::endl;
   */
}


void CbmTrdModule::GetModuleInformation(
      Int_t moduleAddress,
      const Double_t* local_point,
      Int_t& sectorId,
      Int_t& columnId,
      Int_t& rowId) const
{
   // safety check. Up to now always correct, so could be removed.
   if (fModuleAddress != moduleAddress) {
      LOG(ERROR) << "CbmTrdModule::GetModuleInformation: This is wrong!" << FairLogger::endl;
   }

   Double_t posX, posY;
   TransformToLocalSector(local_point, posX, posY);

   // calculate in which sector the point is
   sectorId = GetSector(local_point);

   columnId = (Int_t)(posX / fPadSizeX.At(sectorId));
   rowId    = (Int_t)(posY / fPadSizeY.At(sectorId));
}


void CbmTrdModule::GetPosition(
      Int_t moduleAddress,
      Int_t sectorId,
      Int_t columnId,
      Int_t rowId,
      TVector3& padPos,
      TVector3& padSize) const
{
   // calculate position in global coordinates 
   // from digi information (sectorId, columnId, rowId).
   // Returns two TVector3. One with the position
   // and one with the padsize of the fired pad

   if (fModuleAddress != moduleAddress) {
      LOG(ERROR) << "CbmTrdModule::GetPosition This is wrong!" << FairLogger::endl;
   }

   // check limits
   if ( (sectorId < 0) || (sectorId > GetNofSectors()-1) )
     LOG(FATAL) << "CbmTrdModule::GetPosition sectorId " << sectorId << " of " << GetNofSectors()-1                 << " in moduleID " << moduleAddress << " is out of bounds!" << FairLogger::endl;

   // check limits
   if ( (columnId < 0) || (columnId > GetNofColumnsInSector(sectorId)-1) )
     LOG(FATAL) << "CbmTrdModule::GetPosition columnId " << columnId << " of " << GetNofColumnsInSector(sectorId)-1 << " in moduleID " << moduleAddress << " is out of bounds!" << FairLogger::endl;

   // check limits
   if ( (rowId < 0) || (rowId > GetNofRowsInSector(sectorId)-1) )
     LOG(FATAL) << "CbmTrdModule::GetPosition rowId "   << rowId     << " of " << GetNofRowsInSector(sectorId)-1    << " in moduleID " << moduleAddress << " is out of bounds!" << FairLogger::endl;

   Double_t local_point[3];
   Double_t padsizex = fPadSizeX.At(sectorId);
   Double_t padsizey = fPadSizeY.At(sectorId);

   // calculate position in sector coordinate system 
   // with the origin in the lower left corner (looking upstream)
   local_point[0] = (((Double_t)columnId + 0.5) * padsizex);
   local_point[1] = (((Double_t)rowId    + 0.5) * padsizey);

   // calculate position in module coordinate system
   // with the origin in the lower left corner (looking upstream)
   local_point[0] += fSectorBeginX.GetAt(sectorId);
   local_point[1] += fSectorBeginY.GetAt(sectorId);
   // local_point[i] must be >= 0 at this point 

   //   std::cout << "lp " << local_point[0] << " " <<  local_point[1] << std::endl;

   // check limits
   if ( (local_point[0]< 0) || (local_point[0] > 2 * fSizeX) )
     LOG(FATAL) << "CbmTrdModule::GetPosition local_point[0]=" << local_point[0] << " in moduleID " << moduleAddress << " is out of bounds!" << FairLogger::endl;
   // check limits
   if ( (local_point[1]< 0) || (local_point[1] > 2 * fSizeY) )
     LOG(FATAL) << "CbmTrdModule::GetPosition local_point[1]=" << local_point[1] << " in moduleID " << moduleAddress << " is out of bounds!" << FairLogger::endl;

   // calculate position in the module coordinate system
   // with origin in the middle of the module
   local_point[0] -= fSizeX;
   local_point[1] -= fSizeY;
   local_point[2]  = fSizeZ;

   // navigate to the correct module. (fX,fY,fZ)
   gGeoManager->FindNode(fX, fY, fZ);

   // get the local point in local MC coordinates from
   // the geomanager. This coordinate system is rotated,
   // if the chamber is rotated. This is corrected in
   // GetModuleInformation to have the same 
   // local coordinate system in all the chambers
   Double_t global_point[3];  // global_point[3];
   gGeoManager->LocalToMaster(local_point, global_point);

   // calculate the position in the global coordinate system
   // with the origin in target
   Double_t posX = global_point[0];
   Double_t posY = global_point[1];
   Double_t posZ = global_point[2];

   padPos.SetXYZ(posX, posY, posZ);
   padSize.SetXYZ(padsizex,padsizey, 0.);
}
ClassImp(CbmTrdModule)
