/** determine setup.C
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2008
 * @version 1.0
 * Macro determines detector presence in the CBM setup.
 **/



/* Determines detector presence in the TGeo by its name.
 * @param parFile Name of the parameter file with TGeo.
 * @param name Detector name.
 * @return True if detector presents in the TGeo.
 */
Bool_t CheckDetectorPresence(
		const TString& parFile,
		const char* name)
{
	TFile *currentfile = gFile;
	TFile* f = new TFile(parFile);
	f->Get("FairBaseParSet");
       
	TObjArray* nodes = gGeoManager->GetTopNode()->GetNodes();
	for (Int_t iNode = 0; iNode < nodes->GetEntriesFast(); iNode++) {
		TGeoNode* node = (TGeoNode*) nodes->At(iNode);
		if (TString(node->GetName()).Contains(name)) {
			f->Close();
			delete f;
            gFile=currentfile;
			return true;
		}
	}
	if (std::string(name) == "mvd") {
		TGeoNode* node1 = gGeoManager->GetTopVolume()->FindNode("pipevac1_0");
		TGeoNode* node2 = NULL;
		if (node1) node2 = node1->GetVolume()->FindNode("mvdstation01_0");
		if (node2) {
			f->Close();
			delete f;
			gFile=currentfile;
			return true;
		}
	}
	f->Close();
    gFile=currentfile;
	delete f;
	return false;
}

/* Determines TRD detector presence in the TGeo.
 * @param parFile Name of the parameter file with TGeo.
 * @return True if TRD presents in the TGeo.
 */
Bool_t IsTrd(
		const TString& parFile)
{
	return CheckDetectorPresence(parFile, "trd");
}

/* Determines MUCH detector presence in the TGeo.
 * @param parFile Name of the parameter file with TGeo.
 * @return True if MUCH presents in the TGeo.
 */
Bool_t IsMuch(
		const TString& parFile)
{
	return CheckDetectorPresence(parFile, "much");
}

/* Determines TOFdetector presence in the TGeo.
 * @param parFile Name of the parameter file with TGeo.
 * @return True if TOF presents in the TGeo.
 */
Bool_t IsTof(
		const TString& parFile)
{
	return CheckDetectorPresence(parFile, "tof");
}

/* Determines RICH detector presence in the TGeo.
 * @param parFile Name of the parameter file with TGeo.
 * @return True if RICH presents in the TGeo.
 */
Bool_t IsRich(
		const TString& parFile)
{
	return CheckDetectorPresence(parFile, "rich");
}

/* Determines MVD detector presence in the TGeo.
 * @param parFile Name of the parameter file with TGeo.
 * @return True if RICH presents in the TGeo.
 */
Bool_t IsMvd(
		const TString& parFile)
{
	return CheckDetectorPresence(parFile, "mvd");
}
