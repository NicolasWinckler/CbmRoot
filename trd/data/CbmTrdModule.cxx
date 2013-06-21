#include "CbmTrdModule.h"

#include "CbmTrdPoint.h"
#include "CbmTrdAddress.h"
#include "FairLogger.h"

#include "TGeoManager.h"
#include "TMath.h"

CbmTrdModule::CbmTrdModule() 
  : TNamed(),
  fModuleAddress(0),
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
      Int_t address, Double_t x, Double_t y, Double_t z,
      Double_t sizex, Double_t sizey, Double_t sizez, Int_t nofSectors,
      const TArrayD& sectorSizeX, const TArrayD& sectorSizeY,
      const TArrayD& padSizeX, const TArrayD& padSizeY)
  : TNamed(),
  fModuleAddress(address),
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

void CbmTrdModule::ProjectPositionToNextAnodeWire(
      Double_t* local_point) const
{
   // Move the local point to the next anode wire position.
   Double_t local_point_temp[2] = {local_point[0], local_point[1]};
   if (fAnodeWireOffset > 0.0 && fAnodeWireSpacing > 0.0) {
      if (fPadSizeX.At(0) < fPadSizeY.At(0)) {
         local_point[1] = Int_t(((local_point_temp[1] - fAnodeWireOffset) / fAnodeWireSpacing) + 0.5) * fAnodeWireSpacing;
      } else {
         local_point[0] = Int_t(((local_point_temp[0] - fAnodeWireOffset) / fAnodeWireSpacing) + 0.5) * fAnodeWireSpacing;
      }
   } else {
      LOG(ERROR) << "CbmTrdModule::ProjectPositionToNextAnodeWire: fAnodeWireOffset and fAnodeWireSpacing not set. ProjectPositionToNextAnodeWire can not be used." << FairLogger::endl;
   }
}

Int_t CbmTrdModule::GetSector(
      const Double_t* local_point) const
{
   // Calculate the position in the chamber with the origin of
   // the local coordinate system in the lower left corner
   // of the chamber. x goes to the left looking in beam direction
   // y goes upward

   Double_t posx = local_point[0] + fSizeX;
   Double_t posy = local_point[1] + fSizeY;

   for (Int_t i = 0; i < fNofSectors; i++) {
      if (posx >= fSectorBeginX.GetAt(i) && posx < fSectorEndX.GetAt(i)
            && posy >= fSectorBeginY.GetAt(i) && posy < fSectorEndY.GetAt(i)) {
         return i;
      } 
   }

   LOG(ERROR) << "CbmTrdModule::GetSector: Could not find local point in any of the sectors" << FairLogger::endl;
   return -1; // This should be never reached
}

Int_t CbmTrdModule::GetNofColumns() const
{
   Int_t nofColumns = 0;
   if (fSectorSizeX.At(0) < fSizeX) {
      for (Int_t i = 0; i < fNofSectors; i++) {
         nofColumns += (Int_t)(fSectorSizeX.At(i) / fPadSizeX.At(i) + 0.5);
	 //         std::cout << "a " << fSectorSizeX.At(i) / fPadSizeX.At(i) << " " << (Int_t)(fSectorSizeX.At(i) / fPadSizeX.At(i) + 0.5) << std::endl;
      }
   } else {
      nofColumns = (Int_t)(fSectorSizeX.At(0) / fPadSizeX.At(0) + 0.5);
      //      std::cout << "b " << fSectorSizeX.At(0) / fPadSizeX.At(0) << " " << (Int_t)(fSectorSizeX.At(0) / fPadSizeX.At(0) + 0.5) << std::endl;
   }
   return nofColumns;
}

Int_t CbmTrdModule::GetNofRows() const
{
   Int_t nofRows = 0;
   if (fSectorSizeY.At(0) < fSizeY) {
      for (Int_t i = 0; i < fNofSectors; i++) {
         nofRows += (Int_t)(fSectorSizeY.At(i) / fPadSizeY.At(i) + 0.5);
	 //         std::cout << "c " << fSectorSizeY.At(i) / fPadSizeY.At(i) << " " << (Int_t)(fSectorSizeY.At(i) / fPadSizeY.At(i) + 0.5) << std::endl;
      }
   } else {
      nofRows = (Int_t)(fSectorSizeY.At(0) / fPadSizeY.At(0) + 0.5);
      //      std::cout << "d " << fSectorSizeY.At(0) / fPadSizeY.At(0) << " " << (Int_t)(fSectorSizeY.At(0) / fPadSizeY.At(0) + 0.5) << std::endl;
   }
   return nofRows;
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

   // Get the local point in local MC coordinates from
   // the geomanager. This coordinate system is rotated
   // if the chamber is rotated. This is corrected in
   // GetModuleInformation to have a
   // the same local coordinate system in all the chambers
   const Double_t* global_point = gGeoManager->GetCurrentPoint();
   Double_t local_point[3];  // global_point[3];
   gGeoManager->MasterToLocal(global_point, local_point);

   Int_t moduleAddress = CbmTrdAddress::GetModuleAddress(trdPoint->GetDetectorID());
   GetModuleInformation(moduleAddress, local_point, sectorId, columnId, rowId);
}

void CbmTrdModule::TransformToLocalCorner(
      const Double_t* local_point,
      Double_t& posX,
		Double_t& posY) const
{
   // Transformation from local coordinate system with origin in
   // the middle of the module into a system with origin in the
   // lower right corner of the module. Since for both coordinate
   // systems the orientation is the same this is only a shift by
   // the half size of the module in x- and y-direction
   posX = local_point[0] + fSizeX;
   posY = local_point[1] + fSizeY;
}

void CbmTrdModule::TransformToLocalSector(
      const Double_t* local_point,
	   Int_t sector,
	   Double_t& posX,
	   Double_t& posY) const
{
   // Transformation of the module coordinate system with origin
   // in the middle of the chamber into a system with
   // the origin in the lower right corner of the sector the point
   // is in. First transform in a system with origin in the lower
   // right corner.
   TransformToLocalCorner(local_point, posX, posY);
   posX -= fSectorBeginX.GetAt(sector);
   posY -= fSectorBeginY.GetAt(sector);
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

   // calculate in which sector the point is
   sectorId = GetSector(local_point);

   Double_t posX, posY;
   Double_t fpadsizex = GetPadSizeX(sectorId);
   Double_t fpadsizey = GetPadSizeY(sectorId);

   TransformToLocalSector(local_point, sectorId, posX, posY);

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
   // calculate position in global coordinates from digi
   // information(sectorId, columnId, rowId).
   // Returns two TVector3. One with the position and one
   // with the padsize of the fired pad

   if (fModuleAddress != moduleAddress) {
      LOG(ERROR) << "CbmTrdModule::GetPosition This is wrong!" << FairLogger::endl;
   }

   Double_t local_point[3];
   Double_t padsizex = fPadSizeX.At(sectorId);
   Double_t padsizey = fPadSizeY.At(sectorId);

   // calculate position in sector coordinate system with the
   // origin in the lower right corner
   local_point[0] = (((Float_t)columnId - 0.5) * padsizex);
   local_point[1] = (((Float_t)rowId    - 0.5) * padsizey);

   // calculate position in module coordinate system
   // with origin in the lower right corner of the module
   local_point[0] += fSectorBeginX.GetAt(sectorId);
   local_point[1] += fSectorBeginY.GetAt(sectorId);

   // calculte position in the module coordinate system
   // with origin in the middle of the module
   local_point[0] -= fSizeX;
   local_point[1] -= fSizeY;
   local_point[2]  = fSizeZ;

   // Navigate to the correct module. (fX,fY,fZ)
   gGeoManager->FindNode(fX, fY, fZ);

   // Get the local point in local MC coordinates from
   // the geomanager. This coordinate system is rotated
   // if the chamber is rotated. This is corrected in
   // GetModuleInformation to have a
   // the same local coordinate system in all the chambers
   Double_t global_point[3];  // global_point[3];
   gGeoManager->LocalToMaster(local_point, global_point);

   // calculate the position in the global coordinate system
   // with the origin in target
   Float_t posX = global_point[0];
   Float_t posY = global_point[1];
   Float_t posZ = global_point[2];

   padPos.SetXYZ(posX, posY, posZ);
   padSize.SetXYZ(padsizex,padsizey, 0.);
}
ClassImp(CbmTrdModule)
