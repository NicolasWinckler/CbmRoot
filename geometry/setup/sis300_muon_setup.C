//
//   comments
//
// 2013-11-05 - DE - switch to trd_v13p geometry
// 2013-10-11 - DE - add empty string defining the platform                                                                        
// 2013-10-07 - DE - pipe_v13e.geo fixes overlap with TOF, also avoids TRD 
//

static TString caveGeom;
static TString pipeGeom;
static TString magnetGeom;
static TString mvdGeom;
static TString stsGeom;
static TString richGeom;
static TString muchGeom;
static TString shieldGeom;
static TString trdGeom;
static TString tofGeom;
static TString ecalGeom;
static TString platformGeom;
static TString psdGeom;
static Double_t psdZpos;
static Double_t psdXpos;

static TString  fieldMap;
static Double_t fieldZ;
static Double_t fieldScale;
static Int_t    fieldSymType;

void sis300_muon_setup()
{
  FairLogger* logger = FairLogger::GetLogger();

  // -----   Magnetic field   -----------------------------------------------
  fieldMap     = "field_v12b";   // name of field map
  fieldZ       = 40.;            // field centre z position
  fieldScale   =  1.;            // field scaling factor
  fieldSymType =  3;

  // -----  Geometries  -----------------------------------------------------
  caveGeom     = "cave.geo";
  pipeGeom     = "pipe/pipe_v13c.geo.root";
  magnetGeom   = "magnet/magnet_v12b.geo.root";
  mvdGeom      = "";
  stsGeom      = "sts/sts_v13d.geo.root";
  richGeom     = "";
  muchGeom     = "much/much_v13f.geo";
  shieldGeom   = "much/shield_v13f.geo";
  platformGeom = ""; // "passive/platform_v13b.geo";
  trdGeom      = "";
  tofGeom      = "tof/tof_v13-5e.geo.root";
  ecalGeom     = "";
  psdGeom      = "psd_geo_xy.txt";
  psdZpos = 1500.;
  psdXpos = 10.;
  cout<<"Reading geometry setup and magnetic field settings from "<< gSystem->UnixPathName(gInterpreter->GetCurrentMacroName())<<endl;
}
