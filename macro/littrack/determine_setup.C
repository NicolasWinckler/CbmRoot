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


Bool_t IsTrd(
		const TString& mcFile)
{
	return CheckDetectorPresence(mcFile, "trd");
}

Bool_t IsMuch(
		const TString& mcFile)
{
	return CheckDetectorPresence(mcFile, "much");
}

Bool_t IsTof(
		const TString& mcFile)
{
	return CheckDetectorPresence(mcFile, "tof");
}
