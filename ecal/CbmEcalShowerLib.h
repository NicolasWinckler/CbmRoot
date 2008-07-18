/** Shower library realisation by Mikhail Prokudin **/

#ifndef CBMECALSHOWERLIB_H
#define CBMECALSHOWERLIB_H

#include "TNamed.h"
#include "TString.h"

/** Representation of record in shower library.
 ** Of course, better to have an abstract interface to shower
 ** library, to make implementation and interface independent.
 ** But at the end we will have exactly one implementation for
 ** shower lib. **/
class CbmEcalShowerLibRecord
{
public:
  CbmEcalShowerLibRecord();     
  CbmEcalShowerLibRecord(Int_t size);
  void Clear();
  ~CbmEcalShowerLibRecord();
  Float_t fE;
  Float_t fTheta;
  Float_t fPhi;
  Float_t* fData;
};

class CbmEcalShowerLib : public TNamed
{
public:
  /** Constructor to use **/
  CbmEcalShowerLib(const char* libname, Int_t verbose=0);
  
  /** Get/Set for library name **/
  void SetLibname(const char* libname);
  const char* GetLibName() const;

  /** Get/Set verbosity level **/
  Int_t Verb() const;
  void SetVerbosity(Int_t verbosity);
  
  /** Default constructor **/
  CbmEcalShowerLib();
  
  void Init();
  
  virtual ~CbmEcalShowerLib();

  /** Get response for photon with given energy **/
  Float_t GetResponse(Float_t theta, Float_t phi, Float_t lx, Float_t ly, Float_t energy, Float_t cellsize);

  /** Return sum from (ix, iy) to (ix+cellx, iy+celly) **/
  Float_t GetSum(Int_t ix, Int_t iy, Int_t cellx, Int_t celly, Int_t n);
  /** Return sum for cell with center (x,y) **/
  Float_t GetSum(Float_t x, Float_t y, Float_t cell, Int_t n);
  /** Return sum for cell with center (x,y) **/
  /** phi in grad **/
  Float_t GetSumPhi(Float_t x, Float_t y, Float_t cell, Int_t pren, Float_t phi);
  /** Return sum for cell with center (x,y) **/
  Float_t GetSumThetaPhi(Float_t x, Float_t y, Float_t cell, Int_t pren, Float_t theta, Float_t phi);
  /** Return sum for cell with center (x,y) **/
  /** phi and theta in grad **/
  Float_t GetSumEThetaPhi(Float_t x, Float_t y, Float_t cell, Float_t e, Float_t theta, Float_t phi);
  /** Returns record number in shower library **/
  Int_t GetNum(UInt_t en, UInt_t thetan, UInt_t phin) const;
  /** Returns number of first entry with energy more then given **/
  Int_t GetENum(Float_t e) const;
  /** Returns number of first entry with theta more then given **/
  Int_t GetThetaNum(Float_t theta) const;
  /** Returns number of first entry with phi more then given **/
  Int_t GetPhiNum(Float_t phi) const;

  /** Gradients **/
  Float_t GetGradE() const;
  Float_t GetGradX() const;
  Float_t GetGradY() const;
private:
  /** library name **/
  TString fLibName;
  /** verbosity level **/
  Int_t fVerb;
  
  /** Library size **/
  Int_t fSize;

  /** Size of shower library record **/
  Int_t fRSize;
  /** Size of library record in volumes **/
  Int_t fIX;
  Int_t fIY;
  /** Size of library record volumes in cm **/
  Float_t fCellX;
  Float_t fCellY;
  /** Coordinates of volume[0] in cm **/
  Float_t fStartX;
  Float_t fStartY;
  /** Z coordinate of the calorimeter **/
  Float_t fCaloZ;

  /** Gradients **/
  Float_t fGradE;
  Float_t fGradX;
  Float_t fGradY;

  Float_t fPreGradX;
  Float_t fPreGradY;
  /** Sizes of arrays for energy, theta and phi**/
  UInt_t fES;
  UInt_t fThetaS;
  UInt_t fPhiS;
 
  /** Arrays for energy, theta and phi**/
  Float_t* fEs;			//!
  Float_t* fThetas; 		//!
  Float_t* fPhis;		//!

  CbmEcalShowerLibRecord* fLib;
  ClassDef(CbmEcalShowerLib, 1)
};

/** Get/Set for library name **/
inline void CbmEcalShowerLib::SetLibname(const char* libname)
{
  fLibName=libname;
}

inline const char* CbmEcalShowerLib::GetLibName() const
{
  return fLibName;
}

/** Get/Set verbosity level **/
inline Int_t CbmEcalShowerLib::Verb() const
{
  return fVerb;
}

inline void CbmEcalShowerLib::SetVerbosity(Int_t verbosity)
{
  fVerb=verbosity;
}

/** Returns number of first entry with energy more then given **/
inline Int_t CbmEcalShowerLib::GetENum(Float_t e) const
{
  UInt_t i=0;
  for(;i<fES;i++)
    if (fEs[i]>=e) break;
  return i;
}

/** Returns number of first entry with theta more then given **/
inline Int_t CbmEcalShowerLib::GetThetaNum(Float_t theta) const
{
  UInt_t i=0;
  for(;i<fThetaS;i++)
    if (fThetas[i]>=theta) break;
  return i;
}

/** Returns number of first entry with phi more then given **/
inline Int_t CbmEcalShowerLib::GetPhiNum(Float_t phi) const
{
  UInt_t i=0;
  for(;i<fPhiS;i++)
    if (fPhis[i]>=phi) break;
  return i;
}

/** Returns record number in shower library **/
inline Int_t CbmEcalShowerLib::GetNum(UInt_t en, UInt_t thetan, UInt_t phin) const
{
  if (en>fES) return -1;
  if (thetan>fThetaS) return -1;
  if (phin>fPhiS) return -1;
  return phin+fPhiS*(thetan+fThetaS*en);
}

/** Gradients **/
inline Float_t CbmEcalShowerLib::GetGradE() const
{
  return fGradE;
}

inline Float_t CbmEcalShowerLib::GetGradX() const
{
  return fGradX;
}

inline Float_t CbmEcalShowerLib::GetGradY() const
{
  return fGradY;
}

// ---------------------------------------------------------

inline CbmEcalShowerLibRecord::CbmEcalShowerLibRecord()
  : fE(-1111), fTheta(-1111), fPhi(-1111), fData(NULL)
{
  ;
}

inline CbmEcalShowerLibRecord::CbmEcalShowerLibRecord(Int_t size)
  : fE(-1111), fTheta(-1111), fPhi(-1111), fData(NULL)
{
  fData=new Float_t[size];
}

inline void CbmEcalShowerLibRecord::Clear()
{
  delete fData;
  fData=NULL;
}

inline CbmEcalShowerLibRecord::~CbmEcalShowerLibRecord()
{
  Clear();
}

#endif 

