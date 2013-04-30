#include "CbmEcalNN.h"

#include "TString.h"
#include "TRandom.h"
#include "TMath.h"
#include "TMatrixD.h"

#include <iostream>
#include <fstream>

using namespace std;
  
CbmEcalNN::CbmEcalNN(const char* str)
  : fN(-1111), fStr(NULL), fNeurons(NULL), fWNeurons(NULL), fWSynapses(NULL),
    fNNeurons(-1111), fNSynapses(-1111), fMS(-1111), fCurrent(NULL), fDNeurons(NULL),
    fDeDNeurons(NULL), fDeDSynapses(NULL), fDEDNeurons(NULL), fDEDSynapses(NULL),
    fReset(50), fEta(0.1), fEpsilon(0.0), fDelta(0.0), fEtaDecay(1.0)
{
  Int_t i;
  Int_t n=1;
  TString st="";

  for(i=0;str[i]!=0;i++)
  {
    if (str[i]==':') { n++; continue; }
    if (str[i]<48||str[i]>57)
    {
      cerr << "Wrong symbol in " << str << " at " << i+1 << endl; 
      return;
    }
  }

  fStr=new Int_t[n];

  n=0; 
  for(i=0;str[i]!=0;i++)
  {
    if (str[i]==':')
    {
      fStr[n++]=st.Atoi();
      st=""; continue;
    }
    st+=str[i];
  }
  fStr[n++]=st.Atoi();
  fN=n;

  //Counting neurons and synapses
  n=0;
  for(i=0;i<fN;i++) n+=fStr[i];
  fWNeurons=new Double_t[n];
  fDeDNeurons=new Double_t[n];
  fDEDNeurons=new Double_t[n];
  fNeurons=new Double_t[n];
  fDNeurons=new Double_t[n];
  fNNeurons=n;
  for(i=0;i<fNNeurons;i++) fDEDNeurons[i]=0.0;
  n=0;
  for(i=1;i<fN;i++) n+=fStr[i]*fStr[i-1];
  fWSynapses=new Double_t[n];
  fDeDSynapses=new Double_t[n];
  fDEDSynapses=new Double_t[n];
  fNSynapses=n;
  for(i=0;i<fNSynapses;i++) fDEDSynapses[i]=0.0;

  fMS=fNSynapses+fNNeurons;

  fCurrent=new Double_t[fStr[0]+fStr[fN-1]];
}

void CbmEcalNN::LoadWeights(const char* filename)
{
  char* buff=new char[100];
  ifstream f(filename);
  Int_t i;
  Double_t n1;
  Double_t n2;
  
  if (!f)
  {
    Error("LoadWeights", "Can't open file %s", filename);
    return;
  }
  f.getline(buff, 100); //input normalization header
  for(i=0;i<fStr[0];i++)
    f >> n1 >> n2; //Skip the normalization. We do not use it!
  f.getline(buff, 100);

  f.getline(buff, 100); //output normalization header
  for(i=0;i<fStr[fN-1];i++)
    f >> n1 >> n2; //Skip the normalization. We do not use it!
  f.getline(buff, 100);


  f.getline(buff, 100); //neuron weights header
  for(i=0;i<fNNeurons;i++)
  {
    f >> fWNeurons[i];
  }
  f.getline(buff, 100);

  f.getline(buff, 100); //synapse weights header
  for(i=0;i<fNSynapses;i++)
  {
    f >> fWSynapses[i];
  }
  f.getline(buff, 100);

  f.close();
  delete[] buff;
}


void CbmEcalNN::DumpWeights(const char* filename)	// In ROOT compatible format
{
  ofstream f(filename);
  Int_t i;
  f << "#input normalization" << endl;
  for(i=0;i<fStr[0];i++)
    f << "1 0" << endl; //Write unity normalization.

  f << "#output normalization" << endl;
  for(i=0;i<fStr[fN-1];i++)
    f << "1 0" << endl; //Write unity normalization.

  f << "#neurons weights" << endl;
  for(i=0;i<fNNeurons;i++)
    f << fWNeurons[i] << endl;

  f << "#synapses weights" << endl;
  for(i=0;i<fNSynapses;i++)
    f << fWSynapses[i] << endl;

  f.close();
}

void CbmEcalNN::Randomize()
{
  Int_t i;

  for(i=0;i<fNNeurons;i++)  fWNeurons[i]=gRandom->Uniform(-0.5, 0.5);
  for(i=0;i<fNSynapses;i++) fWSynapses[i]=gRandom->Uniform(-0.5, 0.5);
}

Double_t CbmEcalNN::Eval(Int_t idx, Double_t* input)
{
  Int_t i;
  Int_t j;
  Int_t k;
  Int_t dn=0;
  Int_t dn1;
  Int_t dn2;
  Int_t ds=0;
  Int_t ds1;
  Int_t ds2;
  Int_t tmp;
  Double_t t;

  for(i=0;i<fStr[0];i++)
    fNeurons[i]=input[i];

  dn=i; dn2=0;
  //calculate values in all layers
  for(i=1;i<fN-1;i++)
  {
    dn1=dn+fStr[i];
    ds2=fStr[i-1];
    //For all neurons in layer
    for(j=dn;j<dn1;j++)
    {
      t=fWNeurons[j];
      ds1=ds+ds2;
      tmp=dn2-ds;
      //Loop over synapses
      for(k=ds;k<ds1;k++)
	t+=fNeurons[k+tmp]*fWSynapses[k];
      fNeurons[j]=TMath::Exp(-t*t);
      ds=ds1;
    }
    dn2=dn;
    dn=dn1;
  }

  //Last layer threated separately
  {
    dn1=dn+fStr[i];
    ds2=fStr[i-1];
    //For all neurons in layer
    for(j=dn;j<dn1;j++)
    {
      t=fWNeurons[j];
      ds1=ds+ds2;
      tmp=dn2-ds;
      //Loop over synapses
      for(k=ds;k<ds1;k++)
	t+=fNeurons[k+tmp]*fWSynapses[k];
      fNeurons[j]=t;
      ds=ds1;
    }
  }

  return fNeurons[fNNeurons+idx-fStr[fN-1]];
}


Double_t CbmEcalNN::EvalD(Int_t idx, Double_t* input)
//Compute output and derivaties of neural network
{
  Int_t i;
  Int_t j;
  Int_t k;
  Int_t dn=0;
  Int_t dn1;
  Int_t dn2;
  Int_t ds=0;
  Int_t ds1;
  Int_t ds2;
  Int_t tmp;
  Double_t t;
  Double_t t1;

  for(i=0;i<fStr[0];i++)
    fNeurons[i]=input[i];

  dn=i; dn2=0;
  //calculate values in all layers
  for(i=1;i<fN-1;i++)
  {
    dn1=dn+fStr[i];
    ds2=fStr[i-1];
    //For all neurons in layer
    for(j=dn;j<dn1;j++)
    {
      t=fWNeurons[j];
      ds1=ds+ds2;
      tmp=dn2-ds;
      //Loop over synapses
      for(k=ds;k<ds1;k++)
	t+=fNeurons[k+tmp]*fWSynapses[k];
      t1=TMath::Exp(-t*t);
      fNeurons[j]=t1;
      fDNeurons[j]=(-2.0)*t*t1;
      ds=ds1;
    }
    dn2=dn;
    dn=dn1;
  }

  //Last layer threated separately
  {
    dn1=dn+fStr[i];
    ds2=fStr[i-1];
    //For all neurons in layer
    for(j=dn;j<dn1;j++)
    {
      t=fWNeurons[j];
      ds1=ds+ds2;
      tmp=dn2-ds;
      //Loop over synapses
      for(k=ds;k<ds1;k++)
	t+=fNeurons[k+tmp]*fWSynapses[k];
      fNeurons[j]=t;
      fDNeurons[j]=1.0;
      ds=ds1;
    }
  }

  return fNeurons[fNNeurons+idx-fStr[fN-1]];
}

Double_t CbmEcalNN::GetError(Int_t n, Double_t* dta)
// Get error for given dataset
// n --- is number of entries
// dta --- array of input/output pairs
{
  Int_t ninp=fStr[0];
  Int_t nout=fNNeurons-fStr[fN-1];
  Int_t n1=fStr[fN-1];
  Int_t n2=ninp+n1;
  Int_t i;
  Int_t j;
  Double_t err=0;
  Double_t d;

  for(i=0;i<n;i++)
  {
    Eval(0, &(dta[i*n2])); //Calculate neuron network 
    for(j=0;j<n1;j++)
    {
      //chi^2 used for gaussian activation function
      d=fNeurons[nout+j]-dta[i*n2+ninp+j]; 
      d*=d; err+=d;
    }
  }

  return err/2.0;
}

void CbmEcalNN::ComputeDeDs()
{
  Int_t i=fN-1;
  Int_t j;
  Int_t k;
  Int_t dn=0;
  Int_t dn1;
  Int_t dn2;
  Int_t ss;
  Int_t ds=0;
  Int_t ds1;
  Int_t ds2;
  Int_t tmp;
  Double_t t;
  Double_t d;
  
  EvalD(0, fCurrent);

  dn=fNNeurons-fStr[fN-1];
  dn2=dn-fStr[fN-2];
  ss=ds=fNSynapses-fStr[fN-2]*fStr[fN-1];

  //Last layer threated separately
  {
    dn1=dn+fStr[i];
    ds2=fStr[i-1];
    //For all neurons in layer
    for(j=dn;j<dn1;j++)
    {
      fDeDNeurons[j]=fNeurons[j]-fCurrent[j-dn+fStr[0]];
      ds1=ds+ds2;
      tmp=dn2-ds;
      //Loop over synapses
      for(k=dn2;k<dn;k++)
	fDeDSynapses[k-tmp]=fNeurons[k]*fDeDNeurons[j];
      ds=ds1;
    }
  }

  //Other layers
  for(i=fN-2;i>0;i--)
  {
    dn1=dn; dn-=fStr[i];
    ds=ss;
    ds2=fStr[i+1];
    //For all neurons in layer
    for(j=dn;j<dn1;j++)
    {
      t=0;
      ds1=ds+ds2;
      tmp=dn1+ds-ds1;
      //Loop over synapses
      for(k=0;k<ds1-ds;k++)
      {
	t+=fWSynapses[k*fStr[i]+ds]*fDeDNeurons[k+dn1];
      }

      fDeDNeurons[j]=t*fDNeurons[j];
      ds++;
    }
    ss-=fStr[i]*fStr[i-1];
  }

  dn=fNNeurons-fStr[fN-1];
  ss=ds=fNSynapses-fStr[fN-2]*fStr[fN-1];
  for(i=fN-2;i>0;i--)
  {
    dn1=dn; dn-=fStr[i];
    dn2=dn-fStr[i-1];
    ds=ss=ss-fStr[i]*fStr[i-1];
    ds2=fStr[i-1];
    //For all neurons in layer
    for(j=dn;j<dn1;j++)
    {
      t=0;
      ds1=ds+ds2;
      tmp=dn2-ds;
      //Loop over synapses
      for(k=dn2;k<dn;k++)
	fDeDSynapses[k-tmp]=fNeurons[k]*fDeDNeurons[j];
      ds=ds1;
    }
  }

}

void CbmEcalNN::Train(Int_t epochs, Int_t n, Double_t* dta)
//Train a neural netrork. Use BFGS algo
{
  Int_t ie;
  TMatrixD bfgsh(fMS, fMS);
  TMatrixD gamma(fMS, 1);
  TMatrixD delta(fMS, 1);
  Double_t* buf;
  Double_t* dir;

  buf=new Double_t[fMS];
  dir=new Double_t[fMS];
  for(ie=0;ie<epochs;ie++)
  {
    ;
  }

  delete [] buf;
  delete [] dir;
  //TODO
}

void CbmEcalNN::SetGammaDelta(TMatrixD& gamma, TMatrixD& delta, Double_t* buf)
{
  Int_t i;
  Int_t j;

  for(i=0;i<fMS;i++) delta[i]=buf[i];
  for(i=0;i<fNNeurons;i++)
    gamma[i][0]=-fDEDNeurons[i];
  j=i;
  for(i=0;i<fNSynapses;i++)
    gamma[j++][0]=-fDEDSynapses[i];
  //TODO
}

void CbmEcalNN::TrainStochastic(Int_t epochs, Int_t n, Double_t* dta)
//Train a neural netrork. Use BFGS algo
{
  Double_t* buf;
  Int_t* idx;
  Int_t ie;
  Int_t i;
  Int_t j;
  Int_t k;
  Int_t ninp=fStr[0];
  Int_t nout=fNNeurons-fStr[fN-1];
  Int_t n1=fStr[fN-1];
  Int_t n2=ninp+n1;

  buf=new Double_t[fMS];
  idx=new Int_t[n];
  for(i=0;i<fMS;i++) buf[i]=0.0;
  for(i=0;i<n;i++) idx[i]=i;

  cout << "Train: Starting error is " <<  GetError(n, dta) << endl << flush;
  for(ie=0;ie<epochs;ie++)
  {
    Shuffle(n, idx);
    for(i=0;i<n;i++)
    {
      fCurrent=&(dta[idx[i]*n2]);
      ComputeDeDs();
      for(j=0;j<fNNeurons;j++)
      {
	buf[j]=(-fEta)*(fDelta+fDeDNeurons[j])+fEpsilon*buf[j];
	fWNeurons[j]+=buf[j];
      }
      for(k=0;k<fNSynapses;k++)
      {
	buf[j]=(-fEta)*(fDelta+fDeDSynapses[k])+fEpsilon*buf[j];
	fWSynapses[k]+=buf[j++];
      }
    }
    fEta*=fEtaDecay;

    cout << "Epoch: "<< ie << " (Error: " <<  GetError(n, dta) << ")" << endl << flush;
  }

  delete [] buf;
  delete [] idx;
}

void CbmEcalNN::Shuffle(Int_t n, Int_t* idx)
//Shuffle indexes
{
  Int_t i;
  Int_t j;
  Int_t k;

  for(i=0;i<n;i++)
  {
    j=gRandom->Integer(n);
    k=idx[j];
    idx[j]=idx[i];
    idx[i]=k;
  }
}

CbmEcalNN::~CbmEcalNN()
{
  delete [] fStr;
  delete [] fWNeurons;
  delete [] fNeurons;
  delete [] fWSynapses;

  delete [] fCurrent;
  delete [] fDNeurons;
  delete [] fDeDNeurons;
  delete [] fDeDSynapses;
  delete [] fDEDNeurons;
  delete [] fDEDSynapses;
}

ClassImp(CbmEcalNN)
