

{
    // Open output file
    ofstream *file = new ofstream("/d/cbm02/kresan/rich_prot/may11/epi.CERNPST9.dat");

    // Initialize RN generator
    gRandom->SetSeed(10);

    // Loop over events
    const Int_t nev = 100000;
    Float_t vx, vy, vz = -100.;
    Float_t type;
    Int_t pdg;
    Float_t p, theta, phi, pt, px, py, pz;
    for(Int_t iev = 0; iev < nev; iev++) {
        // Generate vertex coordinates
        vx = gRandom->Gaus(0., 0.9);
	vy = gRandom->Gaus(0., 0.4);

        // Write out event header
	(*file) << 1 << " "
	    << (iev+1) << " "
            << vx << " " << vy << " " << vz << endl;

        // Select electron or pion
	type = gRandom->Uniform(0., 1.);
	if(type < 0.5) {
	    pdg = 11;
	} else {
            pdg = -211;
	}

        // Generate momentum
	p = gRandom->Uniform(1., 10.);
        // Generate polar angle
	theta = TMath::Abs(gRandom->Gaus(0., 5.1e-3));
        // Generate azimuthal angle
	phi = gRandom->Uniform(-1.*TMath::Pi(), TMath::Pi());
        // Calculate momentum components
	pt = p*TMath::Sin(theta);
	px = pt*TMath::Cos(phi);
	py = pt*TMath::Sin(phi);
	pz = p*TMath::Cos(theta);

        // Write out particles information
	(*file) << pdg << " "
	    << px << " " << py << " " << pz << endl;

        // Status output
	if(0 == (iev%1000)) cout << iev << "   " << pdg << endl;
    }// events

    // Close output file
    file->close();
}



