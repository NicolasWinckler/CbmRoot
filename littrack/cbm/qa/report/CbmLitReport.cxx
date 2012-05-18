/**
 * \file CbmLitReport.cxx
 * \brief Base class for reports.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2011
 */
#include "CbmLitReport.h"
#include "CbmLitLatexReportElement.h"
#include "CbmLitHtmlReportElement.h"
#include "CbmLitTextReportElement.h"
#include "std/utils/CbmLitUtils.h"

#include "TSystem.h"
#include <fstream>
using std::ifstream;

//#include <boost/filesystem.hpp>
//#include <boost/regex.hpp>
//using boost::filesystem::exists;
//using boost::filesystem::path;
//using boost::filesystem::filesystem_error;
//using boost::filesystem::directory_iterator;

CbmLitReport::CbmLitReport():
   fTitle(""),
   fAuthor(""),
   fErrorColor(""),
   fWarningColor(""),
   fNormalColor(""),
   fIsUseChecking(true),
   fR(NULL)
{
}

CbmLitReport::~CbmLitReport()
{
}

void CbmLitReport::CreateReportElement(
      LitReportType reportType)
{
   if (reportType == kLitLatex) fR = new CbmLitLatexReportElement();
   else if (reportType == kLitHtml) fR = new CbmLitHtmlReportElement();
   else if (reportType == kLitText) fR = new CbmLitTextReportElement();
}

void CbmLitReport::DeleteReportElement()
{
   if (NULL != fR) delete fR;
}

vector<string> CbmLitReport::GetImages(
		   const string& dir,
		   const string& pattern) const
{
	//TODO: When boost::filesystem and boost::system libraries will be available in FAIR externals this can be switched on!
//	const boost::regex e(pattern);
//    path p(dir);
//	try {
//		if (exists(p)) {
//			if (is_regular_file(p)) {
//				cout << "CbmLitReport::GetImageList: " << p << " is a regular file not a directory!" << '\n';
//			} else if (is_directory(p)) {
//				vector<path> v;
//		        copy(directory_iterator(p), directory_iterator(), back_inserter(v));
//
//				vector<string> images;
//				vector<path>::const_iterator it;
//				for (it = v.begin(); it != v.end(); it++) {
//					if (boost::regex_match(it->native(), e)) images.push_back(it->native());
//				}
//				return images;
//			} else {
//				cout << "CbmLitReport::GetImageList: " << p << " exists, but is neither a regular file nor a directory!\n";
//			}
//		} else {
//			cout << "CbmLitReport::GetImageList: " << p << " does not exist\n";
//		}
//	} catch (const filesystem_error& ex) {
//		cout << "CbmLitReport::GetImageList: " << ex.what() << '\n';
//	} catch (const exception& ex) {
//		cout << "CbmLitReport::GetImageList: " << ex.what() << '\n';
//	}
//	return vector<string>();

	// Implementation base on C++ system call.
	string tmpFileName(string("qa_tmp_file_") + lit::ToString<Int_t>(gSystem->GetPid()) + ".tmp");
	string findCommand("find " + dir + " -type f -regex \"" + pattern + "\" -print  | awk -F/ '{print $NF}' > " + tmpFileName);
	system(findCommand.c_str());
	ifstream tmpFile(tmpFileName.c_str(), ifstream::in);
	vector<string> images;
	string image;
	while (tmpFile.good())  {
	    char c = tmpFile.get();
	    if (tmpFile.good()) {
	    	if (c != '\n') { image += c; }
	    	else {
	    	    images.push_back(image);
	    		image = "";
	    	}
	    }
	 }
	tmpFile.close();
	system(string("rm -rf " + tmpFileName).c_str());

	for (Int_t i = 0; i < images.size(); i++) {
		images[i].erase(images[i].size() - 4, 4);
	}

	return images;
	// END Implementation base on C++ system call.
}
