/*
 * CbmMCMatch.h
 *
 *  Created on: Nov 23, 2009
 *      Author: stockman
 */

#ifndef CBMMCMATCH_H_
#define CBMMCMATCH_H_


#include "CbmDetectorList.h"
#include "CbmMCStage.h"
#include "CbmMCResult.h"

#include <TNamed.h>

#include <map>
#include <vector>
#include <string>

typedef std::map<fDetectorType, CbmMCStage*>::iterator TListIterator;
typedef std::map<fDetectorType, CbmMCStage*>::const_iterator TListIteratorConst;

class CbmMCMatch: public TNamed {
public:
	CbmMCMatch();
	CbmMCMatch(const char* name, const char* title)
		:TNamed(name, title), fUltimateStage(kMCTrack){};
	virtual ~CbmMCMatch();

	void AddElement(fDetectorType type, int index, FairLink link);
	void AddElement(fDetectorType sourceType, int index, fDetectorType targetType, int link);
	void SetElements(fDetectorType sourceType, int index, FairLinkedData* links);
	void InitStage(fDetectorType type, std::string fileName, std::string branchName);
	void RemoveStage(fDetectorType type);
	void LoadInMCLists(TClonesArray* myLinkArray);
	void ClearMCList();

	void SetCommonWeightStages(Float_t weight);

	CbmMCEntry GetEntry(fDetectorType type, int index);
	CbmMCEntry GetEntry(FairLink link);

	CbmMCResult GetMCInfo(fDetectorType start, fDetectorType stop);
	CbmMCEntry  GetMCInfoSingle(FairLink aLink, fDetectorType stop);

	int GetNMCStages() const {return fList.size();}

	CbmMCStage* GetMCStage(int index) const{
		TListIteratorConst iter = fList.begin();
		for (int i = 0; i < index; i++)
			iter++;
		return (iter->second);
	}

	CbmMCStage* GetMCStageType(fDetectorType type){
		return fList[type];
	}

	void CreateArtificialStage(fDetectorType stage, std::string fileName = "", std::string branchName = "");

	FairMultiLinkedData FindLinksToStage(fDetectorType stage);

	bool IsTypeInList(fDetectorType type);

	void Print(std::ostream& out = std::cout){out << *this;}

	friend std::ostream& operator<< (std::ostream& out, const CbmMCMatch& match){
		for (int i = 0; i < match.GetNMCStages(); i++){
			if (match.GetMCStage(i)->GetLoaded() == kTRUE){
				match.GetMCStage(i)->Print(out);
				out << std::endl;
			}
		}
		return out;
	}

private:
	fDetectorType fUltimateStage; ///< last stage in link chain. Here all recursive operations must stop.
	std::map<fDetectorType, CbmMCStage*> fList;
	FairMultiLinkedData fFinalStageML;

	void FindStagesPointingToLinks(FairMultiLinkedData links, fDetectorType stop);
	FairMultiLinkedData FindStagesPointingToLink(FairLink link);

	CbmMCResult GetMCInfoForward(fDetectorType start, fDetectorType stop);
	CbmMCResult GetMCInfoBackward(fDetectorType start, fDetectorType stop);
	CbmMCEntry GetMCInfoForwardSingle(FairLink link, fDetectorType stop);
	CbmMCEntry GetMCInfoBackwardSingle(FairLink link, fDetectorType stop, Double_t weight = 1.);

	void GetNextStage(FairMultiLinkedData& startEntry, fDetectorType stopStage);
	void AddToFinalStage(FairLink link, Float_t mult);
	void ClearFinalStage();
	ClassDef(CbmMCMatch, 1);
};

#endif /* PNDMCMATCH_H_ */
