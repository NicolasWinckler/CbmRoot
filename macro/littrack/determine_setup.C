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
       
//	TGeoManager *geoMan = (TGeoManager*) f->Get("FAIRGeom");

	TObjArray* nodes = gGeoManager->GetTopNode()->GetNodes();
//	std::cout << "Number of nodes:" << nodes->GetEntriesFast() << std::endl;
	for (Int_t iNode = 0; iNode < nodes->GetEntriesFast(); iNode++) {
//		std::cout << "  node " << iNode;
		TGeoNode* node = (TGeoNode*) nodes->At(iNode);
//		std::cout << " " << node->GetName() << std::endl;
		if (TString(node->GetName()).Contains(name)) {
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
