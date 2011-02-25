#include "qa/CbmLitMTQa.h"

#include "utils/CbmLitConverter.h"
#include "base/CbmLitEnvironment.h"
#include "parallel/LitTrackFinderNNParallel.h"
#include "parallel/LitConverter.h"

#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "CbmMuchPixelHit.h"
#include "CbmStsTrack.h"

#include "TClonesArray.h"
#include "TStopwatch.h"

#include "tbb/tick_count.h"
#include "tbb/task_scheduler_init.h"
#include "tbb/task.h"
#include "tbb/parallel_invoke.h"
#include "tbb/task_scheduler_observer.h"
#include "tbb/spin_mutex.h"
#include "tbb/parallel_for.h"

#include <map>
#include <iostream>
#include <fstream>

int threads_counter = -1;
std::map<int, long> threadToCpuMap; // let get cpuId by threadId
std::map<int, int> threadNumberToCpuMap; // let get cpuId by threadNumber (see threads_counter)
tbb::spin_mutex mutex;

class TMyObserver: public tbb::task_scheduler_observer {
public:
	void FInit(); // set cpu - thread correspondence
protected:
	void on_scheduler_entry(bool Is_worker); // run at begin of each thread execution
	void on_scheduler_exit(bool Is_worker); // run at end of each thread execution
};

// set cpu - thread correspondence
void TMyObserver::FInit() {
	for (int i = 0; i < 8; i++) {
		threadNumberToCpuMap[2 * i + 0] = i;
		threadNumberToCpuMap[2 * i + 1] = i + 8;
		//threadNumberToCpuMap[i] = i;
		//threadNumberToCpuMap[8 + i] = i + 8;
	};
	observe(true);
}

/// redefine function, which will be run at begining of execution of each thread
#define handle_error_en(en, msg) do { errno = en; perror(msg); exit(EXIT_FAILURE); } while (0)
void TMyObserver::on_scheduler_entry(bool Is_worker) {
	pthread_t I = pthread_self();
	tbb::spin_mutex::scoped_lock lock;
	lock.acquire(mutex);
	++threads_counter;
	int cpuId = threadNumberToCpuMap[threads_counter % 16];

      //  std::cout << "ThrId=" << I << " thread have been created " << threads_counter << "-th.";
      //  std::cout << " And was run on cpu " << cpuId << std::endl;

	lock.release();
	threadToCpuMap[I] = cpuId;

	int s, j;
	cpu_set_t cpuset;
	pthread_t thread = I;
	CPU_ZERO(&cpuset);
	CPU_SET(cpuId, &cpuset);
	// cout << "before" << endl; //FIXME: segmentation fault somethere.
	s = pthread_setaffinity_np(thread, sizeof(cpu_set_t), &cpuset);
	if (s != 0) {
		std::cout << " pthread_setaffinity_np " << std::endl;
//		handle_error_en(s, "pthread_setaffinity_np");
	}
};

// run at end of each thread execution
void TMyObserver::on_scheduler_exit(bool Is_worker) //FIXME: don't run
{
	pthread_t I = pthread_self();
	std::cout << "Thread with number " << I << " was ended " << --threads_counter;
};


class FinderTaskQa : public tbb::task {
	LitTrackFinderNNParallel* fFinder;
	std::vector<std::vector<LitScalPixelHit*> >& fHits;
	std::vector<std::vector<LitScalTrack*> >& fTrackSeeds;
public:
	FinderTaskQa(
			LitTrackFinderNNParallel* finder,
			std::vector<std::vector<LitScalPixelHit*> >& hits,
			std::vector<std::vector<LitScalTrack*> >& trackSeeds):
		fFinder(finder),
		fHits(hits),
		fTrackSeeds(trackSeeds){
	}

	task* execute(){
	    unsigned int nofEvents = fHits.size();
//	    for (unsigned int iTimes = 0; iTimes < 5000; iTimes++) {
	    for (unsigned int iEvent = 0; iEvent < nofEvents; iEvent++) {
	    	unsigned int nofHits = fHits[iEvent].size();
	    	LitScalPixelHit** lhits = &fHits[iEvent][0];
	    	unsigned int nofTrackSeeds = fTrackSeeds[iEvent].size();
	    	LitScalTrack** lseeds = &fTrackSeeds[iEvent][0];
	    	LitScalTrack* ltracks[nofTrackSeeds];

	    	unsigned int ntracks = 0;
	    	fFinder->DoFind(lhits, nofHits, lseeds, nofTrackSeeds, ltracks, ntracks);
	    }
//	    }
		return NULL;
	}
};


CbmLitMTQa::CbmLitMTQa()
{
//	for (int i = 0; i < kMAX_NOF_THREADS;i++){
//		fHT[i] = new CbmRichRingFinderHough(0, "compact");
//		//fHT[i] = new CbmL1RichENNRingFinder(0);
//	}
//
//	fEventNumber = 0;
//	fExecTime = 0.;
}

CbmLitMTQa::~CbmLitMTQa()
{
//	delete fHT;
}

InitStatus CbmLitMTQa::Init()
{
	FairRootManager* ioman = FairRootManager::Instance();

	fStsTracks = (TClonesArray*) ioman->GetObject("StsTrack");
	if (NULL == fStsTracks) Fatal("CbmLitMTQa::Init", "No StsTrack array!");

	fMuchPixelHits = (TClonesArray*) ioman->GetObject("MuchPixelHit");
	if (NULL == fMuchPixelHits) Fatal("CbmLitMTQa::Init", "No MuchPixelHit array!");

       // tbb::task_scheduler_init init();
       // TMyObserver obs;
       // obs.FInit();    // set cpu-threads correspondence

    return kSUCCESS;
}

void CbmLitMTQa::Exec(Option_t* option)
{
	const Int_t nofHits = fMuchPixelHits->GetEntriesFast();
	std::vector<LitScalPixelHit*> hits;
	for(Int_t iHit = 0; iHit < nofHits; iHit++) {
		CbmMuchPixelHit* hit = (CbmMuchPixelHit*) fMuchPixelHits->At(iHit);
		LitScalPixelHit* lhit = new LitScalPixelHit();
		CbmPixelHitToLitScalPixelHit(hit, lhit);
		hits.push_back(lhit);
	}
	fHits.push_back(hits);

	const Int_t nofTrackSeeds = fStsTracks->GetEntriesFast();
	std::vector<LitScalTrack*> trackSeeds;
	for(Int_t iTrack = 0; iTrack < nofTrackSeeds; iTrack++) {
		CbmStsTrack* track = (CbmStsTrack*) fStsTracks->At(iTrack);
		LitScalTrack* ltrack = new LitScalTrack();

		FairTrackParam* par = track->GetParamLast();

		LitTrackParam<fscal> lpar;

		lpar.X = par->GetX();
		lpar.Y = par->GetY();
		lpar.Z = par->GetZ();
		lpar.Tx = par->GetTx();
		lpar.Ty = par->GetTy();
		lpar.Qp = par->GetQp();
		double cov[15];
		par->CovMatrix(cov);
		lpar.C0 = cov[0];
		lpar.C1 = cov[1];
		lpar.C2 = cov[2];
		lpar.C3 = cov[3];
		lpar.C4 = cov[4];
		lpar.C5 = cov[5];
		lpar.C6 = cov[6];
		lpar.C7 = cov[7];
		lpar.C8 = cov[8];
		lpar.C9 = cov[9];
		lpar.C10 = cov[10];
		lpar.C11 = cov[11];
		lpar.C12 = cov[12];
		lpar.C13 = cov[13];
		lpar.C14 = cov[14];

		ltrack->chiSq = 0;
		ltrack->NDF = 0;
		ltrack->nofHits = 0;
		ltrack->nofMissingHits = 0;
		ltrack->paramFirst = lpar;
		ltrack->paramLast = lpar;

		trackSeeds.push_back(ltrack);
	}
	fTrackSeeds.push_back(trackSeeds);

//
//	if (fEventNumber == fNofEvents){
//		cout << "-I- NofTasks = " << fNofTasks << endl;
//		TMyObserver obs;
//		obs.FInit();    // set cpu-threads correspondence
//		obs.observe(true);
//		tbb::task_scheduler_init init(fNofTasks);
//		DoTestWithTask();
//	}

	std::cout << "Event: " << fEventNo++ << std::endl;

}

void CbmLitMTQa::Finish()
{
    DoTestWithTasks(1);
    DoTestWithTasks(2);
    DoTestWithTasks(3);
    DoTestWithTasks(4);
    DoTestWithTasks(5);
    DoTestWithTasks(6);
    DoTestWithTasks(7);
    DoTestWithTasks(8);
    DoTestWithTasks(9);
    DoTestWithTasks(10);
    DoTestWithTasks(11);
    DoTestWithTasks(12);
    DoTestWithTasks(13);
    DoTestWithTasks(14);
    DoTestWithTasks(15);
    DoTestWithTasks(16);


}

void CbmLitMTQa::SetParContainers()
{
    FairRunAna* ana = FairRunAna::Instance();
    FairRuntimeDb* rtdb = ana->GetRuntimeDb();

    rtdb->getContainer("FairBaseParSet");
    rtdb->getContainer("CbmGeoMuchPar");
}

void CbmLitMTQa::DoTestWithTasks(
       Int_t nofTasks)
{
//    std::cout << "Number of tasks: " << nofTasks << std::endl;

    tbb::task_scheduler_init init();
    TMyObserver obs;
    obs.FInit();    // set cpu-threads correspondence


    // Create and initialize seperate track finder
    // for each thread
//    std::cout << "Creating tyracking..." << std::endl;
    LitTrackFinderNNParallel* finder[nofTasks];
//    std::cout << "Creating tracking finished..." << std::endl;
    LitDetectorLayout<fvec> layout;
//    std::cout << "Getting layout..." << std::endl;
    CbmLitEnvironment* env = CbmLitEnvironment::Instance();
    env->GetMuchLayoutVec(layout);
//    std::cout << "Getting layout finished..." << std::endl;
    for(Int_t i = 0; i < nofTasks; i++) {
//	std::cout << "i=" << i << std::endl;
        finder[i] = new LitTrackFinderNNParallel;
	finder[i]->SetDetectorLayout(layout);
    }

//    std::cout << "Track finders created" << std::endl;
//    Int_t nofEvents = fHits.size();
//    std::cout << "Number of events: " << nofEvents << std::endl;
//    TStopwatch timer;
//	timer.Start();
//	int trackcnt = 0;
//    for (unsigned int iEvent = 0; iEvent < nofEvents; iEvent++) {
//    	unsigned int nofHits = fHits[iEvent].size();
//    	LitScalPixelHit** lhits = &fHits[iEvent][0];
//    	unsigned int nofTrackSeeds = fTrackSeeds[iEvent].size();
//    	LitScalTrack** lseeds = &fTrackSeeds[iEvent][0];
//    	LitScalTrack* ltracks[nofTrackSeeds];
//
//    	unsigned int ntracks = 0;
//    	finder.DoFind(lhits, nofHits, lseeds, nofTrackSeeds, ltracks, ntracks);
//    	trackcnt += ntracks;
//   }
//   timer.Stop();
//    std::cout << "trackcnt=" << trackcnt << std::endl;
//	std::cout << "Time (total / per event): " << timer.RealTime() << " / "
//		<< timer.RealTime() / nofEvents << std::endl;
    //


    TStopwatch timer;
    timer.Start();
//    tbb::tick_count t0 = tbb::tick_count::now();
    tbb::task* root_task = new (tbb::task::allocate_root()) tbb::empty_task;
    root_task->set_ref_count(nofTasks+1);
    tbb::task_list list;

    for (int iT = 0; iT < nofTasks; iT++){
       list.push_back(*new(root_task->allocate_child()) FinderTaskQa (finder[iT], fHits, fTrackSeeds));
    }

    root_task->spawn_and_wait_for_all(list);
//  tbb::tick_count t1 = tbb::tick_count::now();
    timer.Stop();

    root_task->destroy(*root_task);
    //Double_t execTime = (t1-t0).seconds();
    Double_t execTime = timer.RealTime();

    static std::ofstream out("mtqa.txt");
    out << "Number of tasks: " << nofTasks << ", number of events:" << fHits.size() << std::endl;
    out << "Total time: " << 1000. * execTime << " ms" << std::endl;
    out << "Time per event: " << 1000. * execTime/ (fHits.size()*nofTasks)<< " ms"<< std::endl;
    out << "Events per sec: " << nofTasks * fHits.size() / execTime << std::endl;


    for (int i = 0; i < nofTasks; i++){
        delete finder[i];
    }

//	std::ofstream fout;
//	fout.open("parallel.txt",std::ios_base::app);
//	fout << (int)(fNofTasks*fData.size()/fExecTime) << ",";
}

ClassImp(CbmLitMTQa);

