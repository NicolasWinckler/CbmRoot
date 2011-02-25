#include "utils/CbmLitUtils.h"

#include "TCanvas.h"

void SaveCanvasAsImage(
		TCanvas* c,
		const std::string& dir)
{
	c->SaveAs(std::string(dir + std::string(c->GetTitle()) + ".eps").c_str());
	c->SaveAs(std::string(dir + std::string(c->GetTitle()) + ".png").c_str());
}
