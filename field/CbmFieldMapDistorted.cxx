// -------------------------------------------------------------------------
// -----               CbmFieldMapDistorted source file         -----
// -----                  Created 18/01/08  by E.Litvinenko            -----
// -------------------------------------------------------------------------

#include "CbmFieldMapDistorted.h"

#include "CbmFieldPar.h"
#include "CbmField.h"
#include "CbmFieldConst.h"
#include "CbmFieldMapSym2.h"
#include "CbmFieldMapSym3.h"

#include "TFile.h"
#include "TArrayF.h"

using std::cout;
using std::cerr;
using std::endl;

// -------------   Default constructor  ----------------------------------
CbmFieldMapDistorted::CbmFieldMapDistorted()  
  : CbmFieldMap() {

  fType = kTypeDistorted;  
  fTypeOfParent = 1;   // CbmFieldMap
  fDistortionFilename = "";
  fParentField=0;

  fBxDistortionFormulaMult = fBxDistortionFormulaAdd = 0;    
  fByDistortionFormulaMult = fByDistortionFormulaAdd = 0;    
  fBzDistortionFormulaMult = fBzDistortionFormulaAdd = 0;
}
// ------------------------------------------------------------------------


// -------------   Standard constructor (with FieldMap Parent Field )  ---------------------------------
CbmFieldMapDistorted::CbmFieldMapDistorted(const char* mapName, const char* pfDistortionFilename, const char* parentName, 
					   const char* fileTypeParent, Int_t pfTypeOfParent)
  : CbmFieldMap() {

  fName  = mapName;
  fType = kTypeDistorted;  
  fTypeOfParent = pfTypeOfParent;   
  fDistortionFilename = pfDistortionFilename;

  switch (pfTypeOfParent) {
  case 3: fParentField = new  CbmFieldMapSym3 (parentName, fileTypeParent); break;
  case 2: fParentField = new  CbmFieldMapSym2 (parentName, fileTypeParent); break;
  default: fParentField = new  CbmFieldMap (parentName, fileTypeParent); break;
  }

  fBxDistortionFormulaMult = fBxDistortionFormulaAdd = 0;    
  fByDistortionFormulaMult = fByDistortionFormulaAdd = 0;    
  fBzDistortionFormulaMult = fBzDistortionFormulaAdd = 0;
}
// ------------------------------------------------------------------------

// -------------   Constructor (with Constant Parent Field )   -------------------------
CbmFieldMapDistorted::CbmFieldMapDistorted (Double_t xMin, Double_t xMax,Double_t yMin, Double_t yMax, 
					    Double_t zMin, Double_t zMax, Double_t bX, Double_t bY, Double_t bZ,
					    const char* mapName, const char* pfDistortionFilename, const char* parentname )
  : CbmFieldMap() {
  fName  = mapName;
  fType = kTypeDistorted;  
  fDistortionFilename = pfDistortionFilename;
  fParentField = new  CbmFieldConst(parentname, xMin, xMax, yMin, yMax, zMin, zMax, bX, bY, bZ);

  fBxDistortionFormulaMult = fBxDistortionFormulaAdd = 0;    
  fByDistortionFormulaMult = fByDistortionFormulaAdd = 0;    
  fBzDistortionFormulaMult = fBzDistortionFormulaAdd = 0;
}


// -------------   Constructor from CbmFieldPar   -------------------------
CbmFieldMapDistorted::CbmFieldMapDistorted(CbmFieldPar* fieldPar)
 : CbmFieldMap() {

  fTypeOfParent=0;

  if ( fieldPar ) {
    fieldPar->MapName(fName);
    fType = fieldPar->GetType();

    if (fType==kTypeDistorted) {        // normal case of distorted field map parameters as input

      fTypeOfParent=fieldPar->GetTypeOfParent();

      TString parentName;
      fieldPar->GetParentName(parentName);

      switch (fTypeOfParent) {
      case 3: fParentField = new  CbmFieldMapSym3 (parentName.Data(),"R"); break;
      case 2: fParentField = new  CbmFieldMapSym2 (parentName.Data(),"R"); break;
      case 0: fParentField = new  CbmFieldConst (parentName.Data(), fieldPar->GetXmin(), fieldPar->GetXmax(), fieldPar->GetYmin(), 
						 fieldPar->GetYmax(), fieldPar->GetZmin(), fieldPar->GetZmax(), 
						 fieldPar->GetBx(), fieldPar->GetBy(), fieldPar->GetBz()); 
	break;
      default: fParentField = new  CbmFieldMap (parentName.Data(),"R"); break;
      }

      if ((fParentField)&&(fTypeOfParent))   {   //   for field map parent
	((CbmFieldMap*)fParentField)->SetPosition(fieldPar->GetPositionX(),fieldPar->GetPositionY(),fieldPar->GetPositionZ());
	((CbmFieldMap*)fParentField)->SetScale(fieldPar->GetScale());
      }

      fieldPar->GetDistortionFilename(fDistortionFilename);
    }
    else {                // try to create a distorted field map from the parameters of constant field or normal field map as input
      switch (fType) {
      case 3: fParentField = new  CbmFieldMapSym3 (fieldPar); break;
      case 2: fParentField = new  CbmFieldMapSym2 (fieldPar); break;
      case 0: fParentField = new  CbmFieldConst (fieldPar); break;
      default: fParentField = new  CbmFieldMap (fieldPar); break;
      }
      fType = kTypeDistorted;
      fDistortionFilename = "";
    }}

  fBxDistortionFormulaMult = fBxDistortionFormulaAdd = 0;    
  fByDistortionFormulaMult = fByDistortionFormulaAdd = 0;    
  fBzDistortionFormulaMult = fBzDistortionFormulaAdd = 0;
}
// ------------------------------------------------------------------------



// ------------   Destructor   --------------------------------------------
CbmFieldMapDistorted::~CbmFieldMapDistorted() { }
// ------------------------------------------------------------------------



// -----------   Intialisation   ------------------------------------------
void CbmFieldMapDistorted::Init() {
  fParentField->Init();
  fParentField->Print();
  SetFromParent(fParentField);
  ReadDistortionInformation(fDistortionFilename.Data());

}
// ------------------------------------------------------------------------


// -------------   Set from parent CbmField   -------------------------
void CbmFieldMapDistorted::SetFromParent(CbmField* field)
{
  fTypeOfParent=0;
  fParentField = field;

  if ( field ) {
    fTypeOfParent=field->GetType();

    CbmFieldConst *field0=0;
    CbmFieldMap *field1=0;

    switch (fTypeOfParent) {

    case 0:
      field0=(CbmFieldConst*)field;
      fXmin=field0->GetXmin(); fXmax=field0->GetXmax(); 
      fYmin=field0->GetYmin(); fZmax=field0->GetYmax(); 
      fZmin=field0->GetZmin(); fZmax=field0->GetZmax(); 
      break;

    default:
      field1=(CbmFieldMap*)field;
      fXmin=field1->GetXmin(); fXmax=field1->GetXmax(); 
      fYmin=field1->GetYmin(); fZmax=field1->GetYmax(); 
      fZmin=field1->GetZmin(); fZmax=field1->GetZmax();

      fNx = field1->GetNx(); fNy = field1->GetNy(); fNz = field1->GetNz();

      fXstep = field1->GetXstep();
      fYstep = field1->GetYstep();
      fZstep = field1->GetZstep();

      fScale = field1->GetScale();
      fPosX = field1->GetPositionX();
      fPosY = field1->GetPositionY();
      fPosZ = field1->GetPositionZ();
      break;
    }
  }

}
// ------------------------------------------------------------------------

// -----------   Read Distortion Formulas from Distortion File   ------------------------------------------
void CbmFieldMapDistorted::ReadDistortionInformation(const char *filename) {
  TFile* filesave=gFile;
  gFile=0;
  if (filename) {
    if (strlen(filename)) {
      fDistortionFilename = filename;
    }}
  if (fDistortionFilename.Data()) {
    if (strlen(fDistortionFilename.Data())) {
      TFile* f = new TFile (fDistortionFilename+".root");
      if (f) {
	fBxDistortionFormulaMult = (TFormula*)f->Get("BxDistortionFormulaMult"); 
	fBxDistortionFormulaAdd = (TFormula*)f->Get("BxDistortionFormulaAdd");     
	fByDistortionFormulaMult = (TFormula*)f->Get("ByDistortionFormulaMult");
	fByDistortionFormulaAdd = (TFormula*)f->Get("ByDistortionFormulaAdd");    
	fBzDistortionFormulaMult = (TFormula*)f->Get("BzDistortionFormulaMult");
	fBzDistortionFormulaAdd = (TFormula*)f->Get("BzDistortionFormulaAdd");  
	f->Close(); 
      }
      else {
	cerr << "CbmFieldMapDistorted::ReadDistortionInformation ERROR: file " << (fDistortionFilename.Data()) 
	     << " can not be read !!!" << endl;
      }
    }}
  gFile=filesave;
}

// -----------   Write Distortion Formulas to Distortion File   ------------------------------------------
void CbmFieldMapDistorted::WriteDistortionInformation(char *filename) {
  TFile* filesave=gFile;
  gFile=0;

  if (filename) {
    if (strlen(filename)) {
      fDistortionFilename = filename;
    }}

  if (fDistortionFilename.Data()) {
    if (strlen(fDistortionFilename.Data())) {
      TFile* f = new TFile (fDistortionFilename+".root","RECREATE");
      if (f) {

	if (fBxDistortionFormulaMult)
	  fBxDistortionFormulaMult->Write("BxDistortionFormulaMult"); 
	if (fBxDistortionFormulaAdd)
	  fBxDistortionFormulaAdd->Write("BxDistortionFormulaAdd");  
   
	if (fByDistortionFormulaMult)
	  fByDistortionFormulaMult->Write("ByDistortionFormulaMult");
	if (fByDistortionFormulaAdd)
	  fByDistortionFormulaAdd->Write("ByDistortionFormulaAdd");  
  
	if (fBzDistortionFormulaMult)
	  fBzDistortionFormulaMult->Write("BzDistortionFormulaMult");
	if (fBzDistortionFormulaAdd)
	  fBzDistortionFormulaAdd->Write("BzDistortionFormulaAdd");

	f->Write();  
	f->Close(); 
      }}}
  gFile=filesave;

}

// ---------------Getter and Setter for Distortion Formulas------------------------------
TFormula* CbmFieldMapDistorted::GetDistortionFormula(char *component_option, char *action_option)
{
  // component_option: "x","y","z"; action_option: "m","a"

  TString co= component_option;
  TString ao= action_option;

  if (co.Contains("y")&&ao.Contains("m"))
    return fByDistortionFormulaMult;
  if (co.Contains("y")&&ao.Contains("a"))
    return fByDistortionFormulaAdd;

  if (co.Contains("x")&&ao.Contains("m"))
    return fBxDistortionFormulaMult;
  if (co.Contains("x")&&ao.Contains("a"))
    return fBxDistortionFormulaAdd;

  if (co.Contains("z")&&ao.Contains("m"))
    return fBzDistortionFormulaMult;
  if (co.Contains("z")&&ao.Contains("a"))
    return fBzDistortionFormulaAdd;

  return 0;
}

Bool_t CbmFieldMapDistorted::SetDistortionFormula (TFormula* parDistortionFormula, char *component_option, char *action_option )
{
  TString co= component_option;
  TString ao= action_option;
  Int_t counter=0;

  if (co.Contains("y")&&ao.Contains("m")) {
    fByDistortionFormulaMult = parDistortionFormula;
    counter++;
  }
  if (co.Contains("y")&&ao.Contains("a")) {
    fByDistortionFormulaAdd = parDistortionFormula;
    counter++;
  }

  if (co.Contains("x")&&ao.Contains("m")) {
    fBxDistortionFormulaMult = parDistortionFormula;
    counter++;
  }
  if (co.Contains("x")&&ao.Contains("a")) {
    fBxDistortionFormulaAdd = parDistortionFormula;
    counter++;
  }

  if (co.Contains("z")&&ao.Contains("m")) {
    fBzDistortionFormulaMult = parDistortionFormula;
    counter++;
  }
  if (co.Contains("z")&&ao.Contains("a")) {
    fBzDistortionFormulaAdd = parDistortionFormula;
    counter++;
  }
  return (counter>0);
} 

Bool_t CbmFieldMapDistorted::SetDistortionFormula (char* parDistortionFormulaText, char *component_option, char *action_option )
{
  TString co= component_option;
  TString ao= action_option;
  Int_t counter=0;

  if (co.Contains("y")&&ao.Contains("m")) {
    fByDistortionFormulaMult = new TFormula("ByDistortionFormulaMult",parDistortionFormulaText);
    counter++;
  }
  if (co.Contains("y")&&ao.Contains("a")) {
    fByDistortionFormulaAdd = new TFormula("ByDistortionFormulaAdd",parDistortionFormulaText);
    counter++;
  }

  if (co.Contains("x")&&ao.Contains("m")) {
    fBxDistortionFormulaMult = new TFormula("BxDistortionFormulaMult",parDistortionFormulaText);
    counter++;
  }
  if (co.Contains("x")&&ao.Contains("a")) {
    fBxDistortionFormulaAdd = new TFormula("BxDistortionFormulaAdd",parDistortionFormulaText);
    counter++;
  }

  if (co.Contains("z")&&ao.Contains("m")) {
    fBzDistortionFormulaMult = new TFormula("BzDistortionFormulaMult",parDistortionFormulaText);
    counter++;
  }
  if (co.Contains("z")&&ao.Contains("a")) {
    fBzDistortionFormulaAdd = new TFormula("BzDistortionFormulaAdd",parDistortionFormulaText);
    counter++;
  }
  return (counter>0);
} 
// ------------------------------------------------------------------------

// -----------   Get x component of the field   ---------------------------
Double_t CbmFieldMapDistorted::GetBx(Double_t x, Double_t y, Double_t z) {
  Double_t bx;
  bx = fParentField->GetBx(x,y,z);

  if(fBxDistortionFormulaMult)
    bx *= fBxDistortionFormulaMult->Eval(x,y,z);
  if(fBxDistortionFormulaAdd)
    bx += fBxDistortionFormulaAdd->Eval(x,y,z);
  
    return bx;
}

// -----------   Get y component of the field   ---------------------------
Double_t CbmFieldMapDistorted::GetBy(Double_t x, Double_t y, Double_t z) {
  Double_t by;
  by = fParentField->GetBy(x,y,z);

  if(fByDistortionFormulaMult)
    by *= fByDistortionFormulaMult->Eval(x,y,z);
  if(fByDistortionFormulaAdd)
    by += fByDistortionFormulaAdd->Eval(x,y,z);
  
  return by;
}

// -----------   Get z component of the field   ---------------------------
Double_t CbmFieldMapDistorted::GetBz(Double_t x, Double_t y, Double_t z) {
  Double_t bz;
  bz = fParentField->GetBz(x,y,z);
  
  if(fBzDistortionFormulaMult)
    bz *= fBzDistortionFormulaMult->Eval(x,y,z);
  if(fBzDistortionFormulaAdd)
    bz += fBzDistortionFormulaAdd->Eval(x,y,z);

    return bz;
}

void CbmFieldMapDistorted::Print() {
  cout << "=============================================================" << endl;
  cout << "----  " << fTitle << " : " << fName << endl;

  cout << "==   Parent Field:          ==" << endl;
  fParentField->Print();
  cout << "==============================" << endl;

  cout << "==  Distortion Information File :  ==" << endl;
  cout << fDistortionFilename << endl;
  cout << "==============================" << endl;

  cout << "==  Bx Distortion Formula Mult :  ==" << endl;
  if (fBxDistortionFormulaMult)    cout << fBxDistortionFormulaMult->GetExpFormula("p") << endl;
  cout << "==  Bx Distortion Formula Add :   ==" << endl;
  if (fBxDistortionFormulaAdd)    cout << fBxDistortionFormulaAdd->GetExpFormula("p") << endl;
  cout << "==============================" << endl;

  cout << "==  By Distortion Formula Mult :  ==" << endl;
  if (fByDistortionFormulaMult)    cout << fByDistortionFormulaMult->GetExpFormula("p") << endl;
  cout << "==  By Distortion Formula Add :   ==" << endl;
  if (fByDistortionFormulaAdd)    cout << fByDistortionFormulaAdd->GetExpFormula("p") << endl;
  cout << "==============================" << endl;

  cout << "==  Bz Distortion Formula Mult :  ==" << endl;
  if (fBzDistortionFormulaMult)    cout << fBzDistortionFormulaMult->GetExpFormula("p") << endl;
  cout << "==  Bz Distortion Formula Add :   ==" << endl;
  if (fBzDistortionFormulaAdd)    cout << fBzDistortionFormulaAdd->GetExpFormula("p") << endl;
  cout << "==============================" << endl;

  Double_t po[3],b[3];
  po[0]=0; po[1]=0; po[2]=0;
  GetFieldValue(po,b);
  cout << "----  Distorted field at origin is ( "<< b[0] << ", " << b[1] << ", " << b[2]  << ") kG" << endl;

  cout << "=============================================================" << endl;

}
// ------------------------------------------------------------------------


// -----  Set the position of the field centre in global coordinates  -----
void CbmFieldMapDistorted::SetPosition(Double_t x, Double_t y, Double_t z) {
  fPosX = x;
  fPosY = y;
  fPosZ = z;
  if (fTypeOfParent)
    ((CbmFieldMap*)fParentField)->SetPosition(x,y,z);
}
// ------------------------------------------------------------------------

// ---------------------Set a global field scaling factor------------------
void CbmFieldMapDistorted::SetScale(Double_t factor) { 
  fScale = factor;
  if (fTypeOfParent)
    ((CbmFieldMap*)fParentField)->SetScale(factor);
}
// ------------------------------------------------------------------------

// ---------------------Plot distorted and parend field (By component)-----
void CbmFieldMapDistorted::PlotBy(Int_t n, Double_t zmin, Double_t zmax) { 
  TH1D *h,*hp;
  Int_t n0=n;
  if (!n) n0=1;
  Double_t dz=(zmax-zmin)/n0;

  h=new TH1D ("hField",fName,n,zmin,zmax);
  for (Int_t i=0; i<n0; i++)
    h->SetBinContent(i+1,GetBy(0.,0.,zmin+i*dz));
  h->SetLineWidth(2);
  h->Draw();

  if (fParentField) {
    hp=new TH1D ("hFieldParent",fParentField->GetName(),n,zmin,zmax);
    for (Int_t i=0; i<n0; i++)
      hp->SetBinContent(i+1,fParentField->GetBy(0.,0.,zmin+i*dz));
    hp->SetLineColor(2);
    hp->Draw("sames");
  }
}
// ------------------------------------------------------------------------
