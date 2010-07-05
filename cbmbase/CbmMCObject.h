/*
 * CbmMCStage.h
 *
 *  Created on: Dec 1, 2009
 *      Author: stockman
 */

#ifndef CBMMCOBJECT_H_
#define CBMMCOBJECT_H_

#include "CbmDetectorList.h"
#include "CbmMCEntry.h"

#include "TObject.h"

#include <vector>
#include <iostream>

class FairLinkedData;

class CbmMCObject: public TObject {
public:
	CbmMCObject();
	CbmMCObject(DataType type):fStageId(type){};
	CbmMCObject(const CbmMCObject& obj):
		fStageId(obj.GetStageId()),fStage(obj.GetEntryVector())
	{}
	CbmMCObject(DataType type, std::vector<CbmMCEntry> stage):
		fStageId(type)
	{
		SetStage(stage);
	};
	virtual ~CbmMCObject();

	void SetStageId(DataType type){fStageId = type;}

	void SetEntry(std::vector<Int_t> type, std::vector<Int_t> link, int index);
	void SetEntry(FairLinkedData* data, int index);
	void SetEntry(CbmMCEntry link);

	void SetStage(std::vector<CbmMCEntry> stage);
	void SetLink(FairLink link, int index);
	void AddLink(FairLink link, int index);

	CbmMCEntry GetEntry(int index) const {return fStage[index];}
	FairLink	GetSingleLink(int entryIndex, int linkIndex) const {
		return fStage.at(entryIndex).GetLink(linkIndex);
	}

	DataType GetStageId(void) const {return fStageId;}

	CbmMCEntry GetMCLink(Int_t index){return fStage.at(index);}

	int GetNEntries() const {return fStage.size();}
	int GetNLinks(int entryIndex)const {return fStage.at(entryIndex).GetNLinks();}
	std::vector<CbmMCEntry> GetEntryVector() const{return fStage;}

	FairMultiLinkedData PosInList(std::pair<int, int> link);
	FairMultiLinkedData PosInList(FairLink link);

	FairMultiLinkedData GetLinksWithType(DataType type){
		FairMultiLinkedData result;
		for (int i = 0; i < GetNEntries(); i++){
			result.AddLinks(GetMCLink(i).GetLinksWithType(type), false);
		}
		return result;
	}

	virtual void ClearEntries(){fStage.clear();}

	virtual void Print(std::ostream& out = std::cout){out << *this;}

	void operator=(const CbmMCObject& obj){
		fStageId = obj.GetStageId();
		fStage = obj.GetEntryVector();
	}

	friend std::ostream& operator<< (std::ostream& out, const CbmMCObject& obj){
		std::vector<CbmMCEntry> stages = obj.GetEntryVector();
		for (int i = 0; i < stages.size(); i++){
			out << i << ": ";
			stages[i].Print(out);
		}
		return out;
	}

private:
	void AdoptSize(int index);
	std::vector<CbmMCEntry> fStage;
	DataType fStageId;



	ClassDef(CbmMCObject, 0);
};

#endif /* PNDMCOBJECT_H_ */
