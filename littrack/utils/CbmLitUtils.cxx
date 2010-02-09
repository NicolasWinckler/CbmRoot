#include "CbmLitUtils.h"


void SaveCanvasAsImage(
		TCanvas* c,
		const std::string& dir)
{
	c->SaveAs(std::string(dir + std::string(c->GetTitle()) + ".eps").c_str());
	c->SaveAs(std::string(dir + std::string(c->GetTitle()) + ".gif").c_str());
}
