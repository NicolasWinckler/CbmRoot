#ifndef CBM_TRD_GAS_H
#define CBM_TRD_GAS_H

#include "TObject.h"
#include "TString.h"

class CbmTrdGas : public TObject {

 public:
    CbmTrdGas();
    //    virtual ~CbmTrdGas();

    void Init();
    TString GetFileName(TString method);

    inline Int_t    GetDetType()      {return fDetType;}
    inline Double_t GetGasThick()     {return fGasThick;}
    inline Double_t GetNobleGas()     {return fPercentNobleGas;}
    inline Double_t GetCO2()          {return fPercentCO2;}
    inline Int_t    GetNobleGasType() {return fNobleGasType;}

    // static access method
    static CbmTrdGas* Instance() {return fgInstance;}


 private:
    Int_t              fDetType;         /** Detector type (GSI=0, MB=1) */
    Double_t           fGasThick;        /** Thickness of the gas layer */
    Double_t           fPercentNobleGas; /** Percentage of noble gas */
    Double_t           fPercentCO2;       /** Percentage of CO2 */
    Int_t              fNobleGasType;     /** Type of noble gas (Xe=1, Ar=2)*/
    static CbmTrdGas*  fgInstance;       /** Singleton Instance */
    TString            fFileNameLike; //!   /** File name of input for likelihood method */
    TString            fFileNameANN;  //!  /** File name of input for ANN method */
    void SetFileName();


    ClassDef(CbmTrdGas,1) // Container for gas properties of TRD
};


#endif //CBM_TRD_GAS_H
