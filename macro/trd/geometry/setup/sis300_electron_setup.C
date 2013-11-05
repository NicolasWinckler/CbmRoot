//
//   comments
//
// 2013-11-05 - DE - switch to trd_v13p geometry
// 2013-10-11 - DE - add empty string defining the platform                                                                        
// 2013-10-07 - DE - pipe_v13d.geo fixes overlap with TOF
// 2013-10-07 - DE - rich_v13a is not at nominal position 1800mm, but at 1600mm
//

static TString caveGeom;
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

void sis300_electron_setup()
{
  FairLogger* logger = FairLogger::GetLogger();

  // -----   Magnetic field   -----------------------------------------------
  fieldMap     = "field_v12b";   // name of field map
  fieldZ       = 40.;            // field centre z position
  fieldScale   =  1.;            // field scaling factor
  fieldSymType =  3;

  // -----  Geometries  -----------------------------------------------------
  caveGeom     = "cave.geo";
  pipeGeom     = "pipe/pipe_v13b.geo.root";
  magnetGeom   = "magnet/magnet_v12b.geo.root";
  mvdGeom      = "";
  stsGeom      = "sts/sts_v13c.geo.root";
  richGeom     = "rich/rich_v08a.geo";
  muchGeom     = "";
  platformGeom = ""; // "passive/platform_v13b.geo";
  trdGeom      = "trd/trd_v13p_3e.geo.root";
  tofGeom      = "tof/tof_v13-5d.geo.root";
  ecalGeom     = "";
  cout<<"Reading geometry setup and magnetic field settings from "<< gSystem->UnixPathName(gInterpreter->GetCurrentMacroName())<<endl;
}
