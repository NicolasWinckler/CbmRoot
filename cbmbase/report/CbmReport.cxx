/**
 * \file CbmReport.cxx
 * \brief Base class for reports.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2011
 */
#include "CbmReport.h"
#include "CbmLatexReportElement.h"
#include "CbmHtmlReportElement.h"
#include "CbmTextReportElement.h"

#include "TSystem.h"
#include <fstream>
#include <sstream>
using std::ifstream;
using std::stringstream;

//#include <boost/filesystem.hpp>
//#include <boost/regex.hpp>
//using boost::filesystem::exists;
//using boost::filesystem::path;
//using boost::filesystem::filesystem_error;
//using boost::filesystem::directory_iterator;

CbmReport::CbmReport():
   fTitle(""),
   fAuthor(""),
   fErrorColor(""),
   fWarningColor(""),
   fNormalColor(""),
   fIsUseChecking(true),
   fR(NULL)
{
}

CbmReport::~CbmReport()
{
}

void CbmReport::CreateReportElement(
      ReportType reportType)
{
   if (reportType == kLatexReport) fR = new CbmLatexReportElement();
   else if (reportType == kHtmlReport) fR = new CbmHtmlReportElement();
   else if (reportType == kTextReport) fR = new CbmTextReportElement();
}

void CbmReport::DeleteReportElement()
{
   if (NULL != fR) delete fR;
}

vector<string> CbmReport::GetImages(
		   const string& dir,
		   const string& pattern) const
{
	//TODO: When boost::filesystem and boost::system libraries will be available in FAIR externals this can be switched on!
//	const boost::regex e(pattern);
//    path p(dir);
//	try {
//		if (exists(p)) {
//			if (is_regular_file(p)) {
//				cout << "CbmReport::GetImageList: " << p << " is a regular file not a directory!" << '\n';
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
//				cout << "CbmReport::GetImageList: " << p << " exists, but is neither a regular file nor a directory!\n";
//			}
//		} else {
//			cout << "CbmReport::GetImageList: " << p << " does not exist\n";
//		}
//	} catch (const filesystem_error& ex) {
//		cout << "CbmReport::GetImageList: " << ex.what() << '\n';
//	} catch (const exception& ex) {
//		cout << "CbmReport::GetImageList: " << ex.what() << '\n';
//	}
//	return vector<string>();

	// Implementation base on C++ system call.
   stringstream tmpFileName;
   tmpFileName << "qa_tmp_file_" << gSystem->GetPid() << ".tmp";
	string findCommand("find " + dir + " -type f -regex \"" + pattern + "\" -print  | awk -F/ '{print $NF}' > " + tmpFileName.str());
	system(findCommand.c_str());
	ifstream tmpFile(tmpFileName.str().c_str(), ifstream::in);
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
	system(string("rm -rf " + tmpFileName.str()).c_str());

	for (Int_t i = 0; i < images.size(); i++) {
		images[i].erase(images[i].size() - 4, 4);
	}

	return images;
	// END Implementation base on C++ system call.
}
