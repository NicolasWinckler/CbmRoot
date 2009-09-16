/** determine setup.C
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2008
 * @version 1.0
 * Macro determines detector presence in the CBM setup.
 **/

/* Determines detector presence in the TGeo by its name.
 * @param mcFile Name of the MC file with TGeo.
 * @param name Detector name.
 * @return True if detector presents in the TGeo.
 */
Bool_t CheckDetectorPresence(
		const TString& mcFile,
		const char* name)
{
	TFile* f = new TFile(mcFile);
	TGeoManager *geoMan = (TGeoManager*) f->Get("FAIRGeom");

	TObjArray* nodes = geoMan->GetTopNode()->GetNodes();
	for (Int_t iNode = 0; iNode < nodes->GetEntriesFast(); iNode++) {
		TGeoNode* node = (TGeoNode*) nodes->At(iNode);
		if (TString(node->GetName()).Contains(name)) return true;
	}
	return false;
}

/* Determines TRD detector presence in the TGeo.
 * @param mcFile Name of the MC file with TGeo.
 * @return True if TRD presents in the TGeo.
 */
Bool_t IsTrd(
		const TString& mcFile)
{
	return CheckDetectorPresence(mcFile, "trd");
}

/* Determines MUCH detector presence in the TGeo.
 * @param mcFile Name of the MC file with TGeo.
 * @return True if MUCH presents in the TGeo.
 */
Bool_t IsMuch(
		const TString& mcFile)
{
	return CheckDetectorPresence(mcFile, "much");
}

/* Determines TOFdetector presence in the TGeo.
 * @param mcFile Name of the MC file with TGeo.
 * @return True if TOF presents in the TGeo.
 */
Bool_t IsTof(
		const TString& mcFile)
{
	return CheckDetectorPresence(mcFile, "tof");
}
