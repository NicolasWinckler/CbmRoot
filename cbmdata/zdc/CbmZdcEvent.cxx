#include "CbmZdcEvent.h"

#include "TNtuple.h"

ClassImp(CbmZdcEvent)


//-------------------------------------------------------------------- 
CbmZdcEvent::CbmZdcEvent()
: TObject(),
  fImpact(-1.),
  fNumberOfEvent(-1),
  fChannel() 
{
   //  Reset;
 }
//-------------------------------------------------------------------- 
 
 CbmZdcEvent::~CbmZdcEvent()
 {
   //
 }
//-------------------------------------------------------------------- 
 
void  CbmZdcEvent::Reset()
 {
   //reset information in each channel
   memset(fChannel,0,7200*sizeof(Float_t));
   fImpact=0;
 }
 //-------------------------------------------------------------------- 

Float_t  CbmZdcEvent::GetChannel(Int_t mod)
{
  return fChannel[mod];
}

void  CbmZdcEvent::SetChannel(Int_t mod, Float_t de)
{
  fChannel[mod] =de;
}
/*
  void  CbmZdcEvent::Print( Int_t lay=60)
  {
  printf(" Energy deposition\n");
  for (Int_t l=0; l<lay; l++) {
  printf (" ========= Module %i \n",lay);
    for(Int_t c=0; c<10; c++){
    printf(" +++++++column %i \n",c);
      for(Int_t r=0; r<12; r++){
      printf("%4.1f",fChannel[r][c][l]);
	}
	printf("\n");
    }
  }
}
*/
