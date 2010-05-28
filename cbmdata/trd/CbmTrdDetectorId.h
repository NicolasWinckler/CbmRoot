// -------------------------------------------------------------------------
// -----                   CbmTrdDetectorId header file                -----
// -----                 Created 29/05/09  by F. Uhlig                 -----
// -------------------------------------------------------------------------


/** CbmTrdDetectorId.h
 ** Defines unique detector identifier for all TRD modules.
 ** All classes which uses this scheme should have a data member
 ** of this class 
 ** @author F. Uhlig <f.uhlig@gsi.de>
 **/
 
//                                  3         2         1          shift length
/** Current definition:            10987654321098765432109876543210
 ** System ID (kTRD=5) on bits 0-4 00000000000000000000000000011111      31
 ** Station NR on bits 5-8         00000000000000000000000111100000 <<5  15
 ** Layer NR on bits 9-11          00000000000000000000111000000000 <<9  7
 ** Module Type on bits 12-16      00000000000000011111000000000000 <<12 31
 ** Copy NR on bits 17-24          00000001111111100000000000000000 <<17 255
 ** Sector Nr on bits 25-31        11111110000000000000000000000000 <<25 127
 **/
 

#ifndef CBMTRDDETECTORID_H
#define CBMTRDDETECTORID_H 1

#include"Rtypes.h"

class CbmTrdDetectorId
{

  public:
  
  
  /** Constructor **/
  CbmTrdDetectorId();
  
  
  /** Destructor **/
  ~CbmTrdDetectorId()  {}
  
  
  /** Get System identifier from detector ID **/
  Int_t GetSystemId(const Int_t detectorId);

   
  /** Get complete system info from detector ID
   ** This will return a pointer to an integer
   ** array of length array_length
   **/
  Int_t* GetDetectorInfo(const Int_t detectorId);

  /** Get sector number from detector ID **/
  Int_t GetSector(const Int_t detectorId);

  /** Get module number from detector ID. 
   ** This is masking the the sector number
   ** if this is set.
   **/
  Int_t GetModuleId(const Int_t detectorId);


  /** Calculate the unique detector ID
   ** This will return a pointer to an integer
   ** array of length array_length
   **/
  Int_t SetDetectorInfo(const Int_t* detectorInfo);

  /** Add the sector info to the detoctor ID.
   ** This is needed for the digitization and
   ** hit producing step.
   **/
  Int_t SetSector(const Int_t moduleInfo, const Int_t Sector);

  //  char* itoa(int value, char* result, int base);

 private:
 
  static const Int_t array_length = 6;
  static const Int_t shiftarray[];
  static const Int_t bitarray[];
  Int_t result_array[array_length];
  Int_t maskarray[array_length];
  Int_t modulemask;

  //  ClassDef(CbmTrdDetectorId,1);
  
};

#endif
