//
//   comments
//
// 2013-10-07 - DE - pipe_v13c.geo fixes overlap with TOF, fills RICH section
//

static TString caveGeom;
static TString targetGeom; 
static TString pipeGeom;
static TString magnetGeom;
static TString mvdGeom;
static TString stsGeom;
static TString richGeom;
static TString muchGeom;
static TString trdGeom;
static TString tofGeom;
static TString ecalGeom;
static TString platformGeom;

static TString  fieldMap;
static Double_t fieldZ;
static Double_t fieldScale;
static Int_t    fieldSymType;

void sis100_hadron_setup()
{
  FairLogger* logger = FairLogger::GetLogger();

  // -----   Magnetic field   -----------------------------------------------
  fieldMap     = "field_v12b";   // name of field map
  fieldZ       = 40.;            // field centre z position
  fieldScale   =  1.;            // field scaling factor
  fieldSymType =  3;

  // -----  Geometries  -----------------------------------------------------
  caveGeom   = "cave.geo";
  targetGeom = "target/target_au_250mu.geo";
  pipeGeom   = "pipe/pipe_v13a.geo";
  magnetGeom = "magnet/magnet_v12b.geo.root";
  mvdGeom    = "mvd/mvd_v07a.geo";
  stsGeom    = "sts/sts_v13c.geo.root";
  richGeom   = "";
  muchGeom   = "";
//  platformGeom = "passive/platform_v13a.geo";
  trdGeom    = "trd/trd_v13h.geo.root";
  tofGeom    = "tof/tof_v13-4a.geo.root";
  ecalGeom   = "";
  cout<<"Reading geometry setup and magnetic field settings from "<< gSystem->UnixPathName(gInterpreter->GetCurrentMacroName())<<endl;
}
