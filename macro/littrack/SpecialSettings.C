void defineGeometrie(TString setup, TString& caveGeom, TString& targetGeom, 
		     TString& pipeGeom, TString& shieldGeom, TString& mvdGeom, 
		     TString& stsGeom, TString& richGeom, TString& muchGeom, 
		     TString& trdGeom, TString& tofGeom, TString& ecalGeom, 
		     TString& fieldMap, TString& magnetGeom)
{ 
   cout << "Do changes for the normal electron setup."<<endl; 

    caveGeom   = "cave.geo";
    targetGeom = "target_au_250mu.geo";
    pipeGeom   = "pipe_standard.geo";
    mvdGeom    = "mvd/mvd_v07a.geo";
    stsGeom    = "sts/sts_v11a.geo";
    richGeom   = "rich/rich_v08a.geo";
    trdGeom    = "trd/trd_v12a.geo";
    tofGeom    = "tof/tof_v07a.geo";
    ecalGeom   = "";//"ecal_FastMC.geo";
    fieldMap   = "field_v10e";
}

void defineGenerator(TString setup, TString& inputDir, TString& urqmdFile, 
		     TString& urqmd, TString& unigen, Int_t& nofMuonsPlus, 
		     Int_t& nofMuonsMinus, Int_t& nofElectrons, 
		     Int_t& nofPositrons, Int_t& nofPionsPlus, 
		     Int_t& nofPionsMinus, Int_t& nofJPsiToMuons, 
		     Int_t& nofJPsiToElectrons, Int_t& nofAuIons)
{
    cout << "Do changes for the normal generator setup."<<endl; 
}

void defaultOutput(TString setup, TString& mcDir, TString& mcFile, 
		   TString& parFile, TString fileNr)
{
    cout << "Do changes for the normal output."<<endl; 

}
