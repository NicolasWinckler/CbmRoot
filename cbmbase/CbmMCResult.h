/*
 * CbmMCResult.h
 *
 *  Created on: Dec 1, 2009
 *      Author: stockman
 */

#ifndef CBMMCRESULT_H_
#define CBMMCRESULT_H_

#include "CbmDetectorList.h"
#include "CbmMCObject.h"

#include <TObject.h>
#include <vector>
#include <string>

#include<iostream>

class CbmMCResult: public CbmMCObject {
public:
	CbmMCResult();
	CbmMCResult(fDetectorType start, fDetectorType stop);
	CbmMCResult(const CbmMCResult& result):
		CbmMCObject(result.GetStartType(), result.GetEntryVector()),
		fStartType(result.GetStartType()),
		fStopType(result.GetStopType())
		{}

	virtual ~CbmMCResult();

	void SetStartType(fDetectorType start){fStartType = start;}
	void SetStopType(fDetectorType stop){fStopType = stop;}

	fDetectorType GetStartType(void) const {return fStartType;}
	fDetectorType GetStopType(void) const {return fStopType;}

	void operator=(const CbmMCResult& result){
		fStartType = result.GetStartType();
		fStopType = result.GetStopType();
		SetStage(result.GetEntryVector());
	}

	virtual void Print(std::ostream& out = std::cout){ out << *this;}

	friend std::ostream& operator<< (std::ostream& out, const CbmMCResult& result){
		out << "MC Link from: " << result.GetStartType() << " to "  << result.GetStopType() << ":" << std::endl;
		((CbmMCObject)result).Print(out);
		return out;
	}


private:
	fDetectorType fStartType;
	fDetectorType fStopType;

	ClassDef(CbmMCResult, 2);
};

#endif /* CbmMCResult_H_ */
