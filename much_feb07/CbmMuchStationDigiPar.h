/** CbmMuchStationDigiPar.h
 *@author  M.Ryzhinskiy <m.ryzhinskiy@gsi.de>
 *@version 1.0
 *@since   14.03.07
 **
 ** Parameter container for the digitization of a MuCh station.
 ** Holds and manages an array of CbmMuchSectorDigiPar objects.
 **/


#ifndef CBMMUCHSTATIONDIGIPAR_H
#define CBMMUCHSTATIONDIGIPAR_H 1


#include "TObjArray.h"
#include "CbmMuchSectorDigiPar.h"



class CbmMuchStationDigiPar : public TObject
{

 public:

  /** Default constructor **/
  CbmMuchStationDigiPar();


  /** Standard constructor
   *@param  iStation    number of station
   *@param  rotation    Rotation w.r.t. global system [degrees]
   **/
  CbmMuchStationDigiPar(Int_t iStation, Double_t rotation);


  /** Destructor */
  virtual ~CbmMuchStationDigiPar();


  /** Accessors **/
  Int_t      GetStationNr()   { return fStationNr; }
  Double_t   GetRotation()    { return fRotation; }
  Int_t      GetNSectors()    { return fSectors->GetEntries(); }
  TObjArray* GetSectorArray() { return fSectors; }
  CbmMuchSectorDigiPar* GetSector(Int_t iSector) {
    return (CbmMuchSectorDigiPar*) fSectors->At(iSector); 
  }


  /** Add parameters of one sector **/
  void AddSector(CbmMuchSectorDigiPar* sec) { fSectors->Add(sec); }



 private:

  Int_t      fStationNr;     //  Station identifier
  Double_t   fRotation;      //  Rotation angle in global c.s. [rad]
  TObjArray* fSectors;       //  Array of sector parameters


  ClassDef(CbmMuchStationDigiPar,1);

};


#endif
