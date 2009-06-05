// -------------------------------------------------------------------------
// -----                   CbmTrdDetectorId source file                -----
// -----                  Created 29/05/09  by F. Uhlig                -----
// -------------------------------------------------------------------------


#include "CbmTrdDetectorId.h"

#include <iostream>

using std::cout;
using std::endl;

const Int_t CbmTrdDetectorId::shiftarray[] = {0,5,9,12,17,25}; 
const Int_t CbmTrdDetectorId::bitarray[] = {31,15,7,31,255,127}; 

CbmTrdDetectorId::CbmTrdDetectorId() {

}


Int_t* CbmTrdDetectorId::GetDetectorInfo(const Int_t detectorId)
{
  for (Int_t i=0; i < array_length; i++) {
   result_array[i] = (( detectorId >> shiftarray[i] ) & bitarray[i] );
  }
  return result_array;
}


/*
void SetDetectorInfo(const Int_t (&detInfoArray)[6], Int_t returnval) 
{
  cout<<"Array Size: "<<sizeof(detInfoArray)<<endl;
  returnval=sizeof(detInfoArray);
  return ( (detInfoArray[0] << shiftarray[0]) | 
           (detInfoArray[1] << shiftarray[1]) | 
           (detInfoArray[2] << shiftarray[2]) | 
           (detInfoArray[3] << shiftarray[3]) | 
           (detInfoArray[4] << shiftarray[4]) | 
           (detInfoArray[5] << shiftarray[5]) 
         ); 
}
*/

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

ClassImp(CbmTrdDetectorId)
