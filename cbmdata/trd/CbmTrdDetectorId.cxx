// -------------------------------------------------------------------------
// -----                   CbmTrdDetectorId source file                -----
// -----                  Created 29/05/09  by F. Uhlig                -----
// -------------------------------------------------------------------------


#include "CbmTrdDetectorId.h"

//#include <iostream>
//using std::cout;
//using std::endl;

const Int_t CbmTrdDetectorId::shiftarray[] = {0,5,9,12,17,25}; 
const Int_t CbmTrdDetectorId::bitarray[] = {5,4,3,5,8,7}; 


/**
 * C++ version 0.4 char* style "itoa":
 * Written by Lukás Chmela
 * Released under GPLv3.
 */
//char* CbmTrdDetectorId::itoa(int value, char* result, int base) {
//  // check that the base if valid
//  if (base < 2 || base > 36) { *result = '\0'; return result; }
//  
//  char* ptr = result, *ptr1 = result, tmp_char;
//  int tmp_value;
//  
//  do {
//    tmp_value = value;
//    value /= base;
//    *ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz" [35 + (tmp_value - value * base)];
//  } while ( value );
//  
//  // Apply negative sign
//  if (tmp_value < 0) *ptr++ = '-';
//  *ptr-- = '\0';
//  while(ptr1 < ptr) {
//    tmp_char = *ptr;
//    *ptr--= *ptr1;
//    *ptr1++ = tmp_char;
//  }
//  return result;
//}


CbmTrdDetectorId::CbmTrdDetectorId()
{
  Int_t sum=0;
  for (Int_t i=0; i < array_length; i++) {
    maskarray[i]=(1 << bitarray[i]) - 1;
    sum+=bitarray[i];
  }
  sum-=bitarray[5];  
  modulemask = (1 << sum) - 1;
}

Int_t* CbmTrdDetectorId::GetDetectorInfo(const Int_t detectorId)
{
  for (Int_t i=0; i < array_length; i++) {
   result_array[i] = (( detectorId >> shiftarray[i] ) & maskarray[i] );
  }
  return result_array;
}

//-----------------------------------------------------------

Int_t  CbmTrdDetectorId::GetSystemId(Int_t detectorId) 
{
  return (detectorId &  maskarray[0]);
}

//-----------------------------------------------------------

Int_t CbmTrdDetectorId::GetModuleId(const Int_t detectorId) 
{
  return (detectorId & modulemask);
}

//-----------------------------------------------------------

Int_t CbmTrdDetectorId::GetSector(const Int_t detectorId)
{
   return (( detectorId >> shiftarray[5] ) & maskarray[5] );
}

//-----------------------------------------------------------

Int_t CbmTrdDetectorId::SetDetectorInfo(const Int_t* detectorInfo)
{
  return ( (detectorInfo[0] << shiftarray[0]) | 
           (detectorInfo[1] << shiftarray[1]) | 
           (detectorInfo[2] << shiftarray[2]) | 
           (detectorInfo[3] << shiftarray[3]) | 
           (detectorInfo[4] << shiftarray[4]) | 
           (detectorInfo[5] << shiftarray[5]) 
         ); 
}

Int_t CbmTrdDetectorId::SetSector(const Int_t moduleInfo, 
                                  const Int_t Sector)
{
  /*
  char buffer [33];
  itoa (moduleInfo,buffer,2);
  printf ("moduleInfo: %s\n",buffer);

  itoa (Sector,buffer,2);
  printf ("Sector(No shift): %s\n",buffer);

  int bla=(Sector << shiftarray[5]);
  itoa (bla,buffer,2);
  printf ("Sector(shift): %s\n",buffer);


  bla=( moduleInfo | (Sector << shiftarray[5]) );
  itoa (bla,buffer,2);
  printf ("Result: %s\n",buffer);

  cout<<"shiftarray[5]: "<<shiftarray[5]<<endl;
  */
  return ( moduleInfo | (Sector << shiftarray[5]) );
}

//ClassImp(CbmTrdDetectorId)
