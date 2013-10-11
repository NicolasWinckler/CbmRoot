static TString caveGeom;
static TString targetGeom; 
static TString pipeGeom;
static TString magnetGeom;
static TString mvdGeom;
static TString stsGeom;
static TString richGeom;
static TString trdGeom;
static TString tofGeom;
static TString ecalGeom;

static TString fieldMap;
static Double_t fieldZ;
static Double_t fieldScale;
static Int_t fieldSymType;

void standard_electron_setup()
{
  FairLogger* logger = FairLogger::GetLogger();

  // -----   Magnetic field   -----------------------------------------------
  fieldMap    = "field_v12b";   // name of field map
  fieldZ     = 40.;             // field centre z position
  fieldScale =  1.;             // field scaling factor
  fieldSymType = 3;

  // -----  Geometries  -----------------------------------------------------
  caveGeom   = "cave.geo";
  pipeGeom   = "pipe/pipe_v13a.geo";
  magnetGeom = "magnet/magnet_v12b.geo.root";
  mvdGeom    = "mvd/mvd_v07a.geo";
  stsGeom    = "sts/sts_v12b.geo.root";
  richGeom   = "rich/rich_v08a.geo";
  trdGeom    = "trd/trd_v13g.geo.root";
  tofGeom    = "tof/tof_v13b.geo.root";
  ecalGeom   = "";
  cout<<"Reading geometry setup and magnetic field settings from "<< gSystem->UnixPathName(gInterpreter->GetCurrentMacroName())<<endl;
}
  
 
