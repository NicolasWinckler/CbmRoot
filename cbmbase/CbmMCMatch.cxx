/*
 * CbmMCMatch.cxx
 *
 *  Created on: Nov 23, 2009
 *      Author: stockman
 */

#include "CbmMCMatch.h"

#include "TClonesArray.h"
#include <iostream>

ClassImp(CbmMCMatch);

CbmMCMatch::CbmMCMatch():fUltimateStage(kMCTrack) {
	// TODO Auto-generated constructor stub

}

CbmMCMatch::~CbmMCMatch() {
	for (TListIterator iter = fList.begin(); iter != fList.end(); iter++){
		delete(iter->second);
	}
	fList.clear();
}

void CbmMCMatch::AddElement(fDetectorType sourceType, int index, fDetectorType targetType, int link)
{
	FairLink myPair(targetType, link);
	AddElement(sourceType, index, myPair);
}

void CbmMCMatch::AddElement(fDetectorType type, int index, FairLink link){
	fList[type]->AddLink(link, index);
}



void CbmMCMatch::SetElements(fDetectorType sourceType, int index, FairLinkedData* links){
	fList[sourceType]->SetEntry(links, index);
}

void CbmMCMatch::InitStage(fDetectorType type, std::string fileName, std::string branchName)
{
	if (fList[type] == 0){
		CbmMCStage* newStage = new CbmMCStage(type, fileName, branchName);
		fList[type] = newStage;
	}
	else{
		std::cout << "-W- CbmMCMatch::InitStage: Stage " << type << " exists already!" << std::endl;
	}
}

void CbmMCMatch::RemoveStage(fDetectorType type){
	fList.erase(type);
}


void CbmMCMatch::SetCommonWeightStages(Float_t weight)
{
	for (int i = 0; i < GetNMCStages(); i++){
		GetMCStage(i)->SetWeight(weight);
	}
}

CbmMCResult CbmMCMatch::GetMCInfo(fDetectorType start, fDetectorType stop){

	CbmMCResult result(start, stop);
	if(!IsTypeInList(start))
		return result;
	if (start < stop)
		return GetMCInfoForward(start, stop);
	else
		return GetMCInfoBackward(start, stop);
}

CbmMCEntry CbmMCMatch::GetMCInfoSingle(FairLink aLink, fDetectorType stop)
{
	CbmMCEntry result;
	if(!IsTypeInList((fDetectorType)aLink.GetType()))
		return result;
	if(!(fList[(fDetectorType)aLink.GetType()]->GetNEntries() > aLink.GetIndex()))
		return result;

	if (aLink.GetType() < stop)
		return GetMCInfoForwardSingle(aLink, stop);
	else
		return GetMCInfoBackwardSingle(aLink, stop);
}

CbmMCResult CbmMCMatch::GetMCInfoForward(fDetectorType start, fDetectorType stop)
{
	CbmMCResult result(start, stop);
	CbmMCStage startVec = *(fList[start]);
	for (int i = 0; i < startVec.GetNEntries(); i++){
		FairLink tempLink(startVec.GetStageId(), i);

/*		FairMultiLinkedData tempStage;
		tempStage.AddLink(tempLink, true);
		FindStagesPointingToLinks(tempStage, stop);
		//FindStagesPointingToLink(tempLink);
		result.SetEntry(&fFinalStageML, i);
		fFinalStageML.Reset();
		*/
		std::cout << "FairLink: " << tempLink << std::endl;
		CbmMCEntry tempEntry(GetMCInfoForwardSingle(tempLink, stop));
		tempEntry.SetSource(start);
		tempEntry.SetPos(i);
		std::cout << "CbmMCEntry: " << tempEntry << std::endl;
		result.SetEntry(tempEntry);
	}
	return result;

}

CbmMCEntry CbmMCMatch::GetMCInfoForwardSingle(FairLink link, fDetectorType stop)
{
	CbmMCEntry result;
	ClearFinalStage();

	FairMultiLinkedData tempStage;
	tempStage.AddLink(link, true);
	FindStagesPointingToLinks(tempStage, stop);
	result.SetLinks(fFinalStageML);
	return result;

}

CbmMCResult CbmMCMatch::GetMCInfoBackward(fDetectorType start, fDetectorType stop)
{
	CbmMCResult result(start, stop);
	CbmMCStage startVec = *(fList[start]);
	for (int i = 0; i < startVec.GetNEntries(); i++){
//		std::cout << "Stage: " << i << std::endl;
/*		ClearFinalStage();
		FairMultiLinkedData temp = startVec.GetEntry(i);
		temp.MultiplyAllWeights(startVec.GetWeight());
		GetNextStage(temp, stop);
		//if (result.GetNEntries() > 0)
*/
		FairLink tempLink(start, i);
		GetMCInfoBackwardSingle(tempLink, stop, startVec.GetWeight());
		result.SetEntry(&fFinalStageML, result.GetNEntries());
	}
	return result;
}

CbmMCEntry CbmMCMatch::GetMCInfoBackwardSingle(FairLink aLink, fDetectorType stop, Double_t weight)
{
	CbmMCEntry result;
	FairMultiLinkedData multiLink = fList[(fDetectorType)aLink.GetType()]->GetEntry(aLink.GetIndex());

	ClearFinalStage();
	multiLink.MultiplyAllWeights(weight);
	GetNextStage(multiLink, stop);
	//if (result.GetNEntries() > 0)
	result.SetLinks(fFinalStageML);

	return result;
}


void CbmMCMatch::FindStagesPointingToLinks(FairMultiLinkedData links, fDetectorType stop)
{
	FairMultiLinkedData tempLinks;
	for (int i = 0; i < links.GetNLinks(); i++){
		FairLink myLink = links.GetLink(i);
		FairMultiLinkedData myNewLinks = FindStagesPointingToLink(myLink);
		if (myNewLinks.GetNLinks() == 0)
			fFinalStageML.AddLink(myLink, true);
		else{
			for (int j = 0; j < myNewLinks.GetNLinks(); j++){
				if (myNewLinks.GetLink(j).GetType() == (Int_t)stop)
					fFinalStageML.AddLink(myNewLinks.GetLink(j), true);
				else if (myNewLinks.GetLink(j).GetType() > (Int_t)stop)
					fFinalStageML.AddLink(myLink, true);
				else
					tempLinks.AddLink(myNewLinks.GetLink(j), true);
			}
		}
	}
	if (tempLinks.GetNLinks() != 0)
		FindStagesPointingToLinks(tempLinks, stop);
}


FairMultiLinkedData CbmMCMatch::FindStagesPointingToLink(FairLink link)
{
	FairMultiLinkedData result;
	TListIteratorConst iter = fList.find((fDetectorType)link.GetType());
	for(;iter!= fList.end(); iter++){
		if (iter->second->PosInList(link).GetNLinks() > 0){
			result.AddLinks(iter->second->PosInList(link), true);
		}
	}
	return result;
}


FairMultiLinkedData CbmMCMatch::FindLinksToStage(fDetectorType stage)
{
	FairMultiLinkedData result;
	for (int i = 0; i < GetNMCStages(); i++){
		result.AddLinks(GetMCStage(i)->GetLinksWithType(stage), true);
	}
	return result;
}


void CbmMCMatch::CreateArtificialStage(fDetectorType stage, std::string fileName, std::string branchName)
{
	FairMultiLinkedData stageLinks = FindLinksToStage(stage);
	if (stageLinks.GetNLinks() > 0){
		InitStage(stage, fileName, branchName);
		FairMultiLinkedData artData;
		artData.SetLink(FairLink((Int_t)kUnknown, -1));
		for (int i = 0; i < stageLinks.GetNLinks(); i++){
			fList[stage]->SetEntry(&artData, stageLinks.GetLink(i).GetIndex());
		}
	}
}



void CbmMCMatch::GetNextStage(FairMultiLinkedData& startStage, fDetectorType stopStage){

	CbmMCEntry tempStage;

	for (int i = 0; i < startStage.GetNLinks(); i++){
		if (startStage.GetLink(i).GetType() == stopStage){
			AddToFinalStage(startStage.GetLink(i),1);
		}
		else if (startStage.GetLink(i).GetType() == fUltimateStage){
		}
		else{
			tempStage = GetEntry(startStage.GetLink(i));
//			std::cout << "Link ";
			//startStage.GetLink(i).Print();

//			for (int j = 0; j < tempStage.size(); j++){
//				if (tempStage[j].first > stopStage)
//					AddToFinalStage(startStage[i]);
//			}
			if (tempStage.GetNLinks() == 0){
				AddToFinalStage(startStage.GetLink(i),1);
			}
			else{
				double tempStageWeight = GetMCStageType(static_cast<fDetectorType>(tempStage.GetSource()))->GetWeight();
				double startLinkWeight = startStage.GetLink(i).GetWeight()/startStage.GetNLinks();
				//std::cout << " StageWeight: " << tempStageWeight << " startLinkWeight: " << startLinkWeight;
				tempStage.MultiplyAllWeights(tempStageWeight);

				if ((tempStageWeight * startLinkWeight) == 0.){
//					std::cout << " NLinks: " << tempStage.GetNLinks();
					tempStage.AddAllWeights(startLinkWeight/tempStage.GetNLinks());
				}
				else
					tempStage.MultiplyAllWeights(startLinkWeight*startStage.GetNLinks());


			}
//			std::cout << " TempStage: " << tempStage;

			GetNextStage(tempStage, stopStage);
		}
	}
}

CbmMCEntry CbmMCMatch::GetEntry(fDetectorType type, int index){

	CbmMCEntry empty;
	if (index < 0) return empty;
	if (fList[type] == 0)
		return empty;
	if (fList[type]->GetNEntries() > index)
		return fList[type]->GetEntry(index);
	return empty;
}

CbmMCEntry CbmMCMatch::GetEntry(FairLink link){
	return GetEntry(static_cast<fDetectorType>(link.GetType()), link.GetIndex());
}

void CbmMCMatch::AddToFinalStage(FairLink hitPair, Float_t mult){
	fFinalStageML.AddLink(hitPair, true, mult);
}

void CbmMCMatch::ClearFinalStage(){
	fFinalStageML.Reset();
}

void CbmMCMatch::ClearMCList()
{
	for (TListIterator iter = fList.begin(); iter != fList.end(); iter++){
		if (iter->second != 0)
			iter->second->ClearEntries();
		else fList.erase(iter);
	}
	//fList.clear();
}

bool CbmMCMatch::IsTypeInList(fDetectorType type){
	for(TListIterator iter = fList.begin(); iter != fList.end(); iter++){
		if (iter->first == type)
			return true;
	}
	return false;
}

void CbmMCMatch::LoadInMCLists(TClonesArray* myLinkArray){
	for (int i = 0; i < myLinkArray->GetEntriesFast(); i++){
		CbmMCEntry* myLink = (CbmMCEntry*)myLinkArray->At(i);
		//std::cout << "myLink.size(): " << myLink->GetNLinks() << ":";
		if (IsTypeInList((fDetectorType)myLink->GetSource())){
			//fList[(fDetectorType)myLink->GetSource()]->ClearEntries();
			fList[(fDetectorType)myLink->GetSource()]->SetEntry(*myLink);
			fList[(fDetectorType)myLink->GetSource()]->SetLoaded(kTRUE);
		}
	}
}
