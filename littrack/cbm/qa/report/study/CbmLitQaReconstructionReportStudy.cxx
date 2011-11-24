/**
 * \file CbmLitQaReconstructionReportStudy.cxx
 *
 * \author Semen Lebedev <s.lebedev@gsi.de>
 * \date 2011
 */
#include "CbmLitQaReconstructionReportStudy.h"
#include "CbmLitQaHtmlReportElementStudy.h"
#include "CbmLitQaLatexReportElementStudy.h"

#include "utils/CbmLitUtils.h"

#include "TSystem.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

CbmLitQaReconstructionReportStudy::CbmLitQaReconstructionReportStudy(
      const string& type)
{
   if (type == "latex") fR = new CbmLitQaLatexReportElementStudy();
   else fR = new CbmLitQaHtmlReportElementStudy();
}

CbmLitQaReconstructionReportStudy::~CbmLitQaReconstructionReportStudy()
{
   if (fR != NULL) delete fR;
}

void CbmLitQaReconstructionReportStudy::Create(
   ostream& out)
{
   out.precision(3);
   out << fR->PrintHead();

   out << fR->PrintSubtitle(fTitle);

   out << fR->PrintTableBegin("Number of objects");

   int row = 0;

   out << fR->PrintRow(row++, "hEventNo", "Number of events");

   if (fIsMvd) {
      out << fR->PrintRow(row++, "hNofMvdPoints", "MVD points");
      out << fR->PrintRow(row++, "hNofMvdDigis", "MVD digis");
      out << fR->PrintRow(row++, "hNofMvdClusters", "MVD clusters");
      out << fR->PrintRow(row++, "hNofMvdHits", "MVD hits");
   }

   if (fIsSts) {
      out << fR->PrintRow(row++, "hNofStsPoints", "STS points");
      out << fR->PrintRow(row++, "hNofStsDigis", "STS digis");
      out << fR->PrintRow(row++, "hNofStsClusters", "STS clusters");
      out << fR->PrintRow(row++, "hNofStsHits", "STS hits");
      out << fR->PrintRow(row++, "hNofStsTracks", "STS tracks");
   }

   if (fIsRich) {
      out << fR->PrintRow(row++, "hNofRichPoints", "RICH points");
      out << fR->PrintRow(row++, "hNofRichHits", "RICH hits");
      out << fR->PrintRow(row++, "hNofRichProjections", "RICH projections");
      out << fR->PrintRow(row++, "hNofRichRings", "RICH rings");
   }

   if (fIsTrd) {
      out << fR->PrintRow(row++, "hNofTrdPoints", "TRD points");
      out << fR->PrintRow(row++, "hNofTrdDigis", "TRD digis");
      out << fR->PrintRow(row++, "hNofTrdClusters", "TRD clusters");
      out << fR->PrintRow(row++, "hNofTrdHits", "TRD hits");
      out << fR->PrintRow(row++, "hNofTrdTracks", "TRD tracks");
   }

   if (fIsMuch) {
      out << fR->PrintRow(row++, "hNofMuchPoints", "MUCH points");
      out << fR->PrintRow(row++, "hNofMuchDigis", "MUCH digis");
      out << fR->PrintRow(row++, "hNofMuchClusters", "MUCH clusters");
      out << fR->PrintRow(row++, "hNofMuchPixelHits", "MUCH pixel hits");
      out << fR->PrintRow(row++, "hNofMuchStrawHits", "MUCH straw hits");
      out << fR->PrintRow(row++, "hNofMuchTracks", "MUCH tracks");
   }

   if (fIsTof) {
      out << fR->PrintRow(row++, "hNofTofPoints", "TOF points");
      //out << PrintRow(row++, "hNofTofDigis", "TOF digis");
      //out << PrintRow(row++, "hNofTofClusters", "TOF clusters");
      out << fR->PrintRow(row++, "hNofTofHits", "TOF hits");
   }

   out << fR->PrintRow(row++, "hNofGlobalTracks", "Global tracks");
   out << fR->PrintTableEnd() << endl;


   string signal = "el";
   if (!fIsElectronSetup) signal = "mu";
   string recDet = (fIsTrd) ? "TRD" : (fIsMuch) ? "MUCH" :
         (fIsMuch && fIsTrd) ? "MUCH+TRD" : "";

   out << fR->PrintTableBegin("Reconstruction efficiency");
   row = 0;
   out << fR->PrintRowEff(row++, "hSts3D.all", "STS all");
   out << fR->PrintRowEff(row++, "hSts3D." + signal, "STS " + signal);
   out << fR->PrintRowEff(row++, "hRec3D.all", recDet + " all");
   out << fR->PrintRowEff(row++, "hRec3D." + signal, recDet + " " + signal);
   out << fR->PrintRowEff(row++, "hTof3D.all", "TOF all");
   out << fR->PrintRowEff(row++, "hTof3D." + signal, "TOF " + signal);

   out << fR->PrintEmptyRow(row++, "Normalization STS+" + recDet);
   out << fR->PrintRowEff(row++, "hSts3DNormHalfGlobal.all", "STS all");
   out << fR->PrintRowEff(row++, "hSts3DNormHalfGlobal." + signal, "STS " + signal);
   out << fR->PrintRowEff(row++, "hHalfGlobal3D.all", "STS+" + recDet + " all");
   out << fR->PrintRowEff(row++, "hHalfGlobal3D." + signal, "STS+" + recDet + " " + signal);

   out << fR->PrintEmptyRow(row++, "Normalization STS+" + recDet + "+TOF");
   out << fR->PrintRowEff(row++, "hSts3DNormGlobal.all", "STS all");
   out << fR->PrintRowEff(row++, "hSts3DNormGlobal." + signal, "STS " + signal);
   out << fR->PrintRowEff(row++, "hHalfGlobal3DNormGlobal.all", "STS+" + recDet + " all");
   out << fR->PrintRowEff(row++, "hHalfGlobal3DNormGlobal." + signal, "STS+" + recDet + " " + signal);
   out << fR->PrintRowEff(row++, "hGlobal3D.all", "STS+" + recDet + "+TOF all");
   out << fR->PrintRowEff(row++, "hGlobal3D.all", "STS+" + recDet + "+TOF " + signal);

   if (fIsRich) {
      out << fR->PrintEmptyRow(row++, "Efficiency with RICH");
      out << fR->PrintRowEff(row++, "hRich3D.El", "RICH el");
      out << fR->PrintRowEff(row++, "hRich3D.ElRef", "RICH el ref");

      out << fR->PrintEmptyRow(row++, "Normalization STS+RICH");
      out << fR->PrintRowEff(row++, "hSts3DNormStsRich.El", "STS el");
      out << fR->PrintRowEff(row++, "hStsRich3D.El", "STS+RICH el");

      out << fR->PrintEmptyRow(row++, "Normalization STS+RICH+TRD");
      out << fR->PrintRowEff(row++, "hSts3DNormStsRichTrd.El", "STS el");
      out << fR->PrintRowEff(row++, "hStsRich3DNormStsRichTrd.El", "STS+RICH el");
      out << fR->PrintRowEff(row++, "hStsRichTrd3D.El", "STS+RICH+TRD el");

      out << fR->PrintEmptyRow(row++, "Normalization STS+RICH+TRD+TOF");
      out << fR->PrintRowEff(row++, "hSts3DNormStsRichTrdTof.El", "STS el");
      out << fR->PrintRowEff(row++, "hStsRich3DNormStsRichTrdTof.El", "STS+RICH el");
      out << fR->PrintRowEff(row++, "hStsRichTrd3DNormStsRichTrdTof.El", "STS+RICH+TRD el");
      out << fR->PrintRowEff(row++, "hStsRichTrdTof3D.El", "STS+RICH+TRD+TOF el");
   }
   out << fR->PrintTableEnd() << endl;

   out << fR->PrintTableBegin("Ghost rate");
   row = 0;
   out << fR->PrintRow(row++, "fhStsGhostNh", "STS");
   out << fR->PrintRow(row++, "fhRecGhostNh", recDet);
   if (fIsRich) {
      out << fR->PrintRow(row++, "fhRichGhostNh", "RICH");
      out << fR->PrintEmptyRow(row++, "after STS-RICH matching");
      out << fR->PrintRow(row++, "fhStsGhostRichMatchingNh", "STS");
      out << fR->PrintRow(row++, "fhRichGhostStsMatchingNh", "RICH");
      out << fR->PrintEmptyRow(row++, "after STS-RICH matching and el identification");
      out << fR->PrintRow(row++, "fhRichGhostElIdNh", "RICH");
   }
   out << fR->PrintTableEnd() << endl;
   // For image paths put only file name without type, e.g. ".eps" or ".png".
   // Type will be added automatically.
   if (fIsSts) out << fR->PrintImage("STS reconstruction efficiency", "rec_qa_sts_efficiency");
   if (fIsTrd || fIsMuch) out << fR->PrintImage(recDet + " reconstruction efficiency", "rec_qa_rec_efficiency");
   if (fIsTof) out << fR->PrintImage("TOF hit matching efficiency", "rec_qa_tof_efficiency");
   if (fIsRich) out << fR->PrintImage("RICH efficiency electrons", "rec_qa_rich_efficiency_electrons");
   out << fR->PrintImage("Global reconstruction efficiency", "rec_qa_global_efficiency_all");
   out << fR->PrintImage("Global reconstruction efficiency for signal", "rec_qa_global_efficiency_signal");
   if (fIsRich) out << fR->PrintImage("Global reconstruction efficiency with RICH", "rec_qa_sts_rich_trd_tof_efficiency_electrons");

   out <<  fR->PrintCloseDocument();
}
