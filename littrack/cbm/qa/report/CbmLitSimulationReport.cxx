/**
 * \file CbmLitSimulationReport.cxx
 *
 * \author Semen Lebedev <s.lebedev@gsi.de>
 * \date 2011
 */
#include "CbmLitSimulationReport.h"

CbmLitSimulationReport::CbmLitSimulationReport()
{

}

CbmLitSimulationReport::~CbmLitSimulationReport()
{

}

bool CbmLitSimulationReport::PropertyExists(
      const std::string& name) const
{
   return (fQa.count(name) != 0) ? true : false;
}
