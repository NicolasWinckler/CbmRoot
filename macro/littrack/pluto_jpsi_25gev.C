void pluto_jpsi_25gev(Int_t index)
{
	gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
	basiclibs();
	
	gRandom->SetSeed(index);
	//gRandom->SetSeed((unsigned)time(0));

	Float_t Eb   =25;
	Float_t T1    = 0.250;   // temperature in GeV
	//  Float_t T1    = 0.320;   // temperature in GeV --------> Goran
	Float_t T2    = 0.;      // temperature in GeV
	Float_t blast = 0.;      // radial expansion velocity
	Int_t const num_of_react=10000;

	PFireball *source_JPsi=new PFireball("J/Psi",Eb,T1,T2,1.,blast,0.,0.,0.,0.);
	//  source_JPsi->setSigma(0.8);
	//  source_JPsi->setSigma(0.23); //  --------> Goran
	source_JPsi->setSigma(0.4);

	source_JPsi->Print();

	PParticle *JPsi = new PParticle("J/Psi");
	//  JPsi->SetM(3.686); //Psi'
	PParticle *mumJPsi = new PParticle("mu-");
	PParticle *mupJPsi = new PParticle("mu+");
	/*
	PParticle* s_JPsi[]    = {source_JPsi,JPsi};
	PChannel* c_sJPsi   = new PChannel(s_JPsi,1,1);
	PChannel  *cc_JPsi[]   = {c_sJPsi};
	*/
	PParticle* s_JPsi[]    = {source_JPsi,JPsi};
	PChannel* c_sJPsi   = new PChannel(s_JPsi,1,1);

	PParticle *s_JPsidimu[]  ={JPsi,mumJPsi,mupJPsi};
	PChannel  *c_JPsidimu  = new PChannel(s_JPsidimu,2,1);
	PChannel  *cc_JPsi[]   = {c_sJPsi,c_JPsidimu};


	char name[8];
	if(index<10)sprintf(name, "Jpsi.000%d", index);
	else if(index<100)sprintf(name, "Jpsi.00%d", index);
	else sprintf(name, "Jpsi.0%d", index);

	PReaction *r_JPsi = new PReaction(cc_JPsi,name,sizeof(cc_JPsi)/sizeof(cc_JPsi[0]),0,0,0,0);

	r_JPsi->Print();
	r_JPsi->setHGeant(0);
	r_JPsi->loop(num_of_react);
}


