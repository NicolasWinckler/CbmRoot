#ifndef L1Algo_h
#define L1Algo_h 1

// #define TBB // TODO: Doesn't work now. Renew

  /// Debug features
// #define PULLS            // triplets pulls
// #define TRIP_PERFORMANCE // triplets efficiencies
// #define DOUB_PERFORMANCE // doublets efficiencies
// #define DRAW             // event display
// #define XXX              // time debug

#include "L1Field.h"
#include "L1Station.h"
#include "L1StsHit.h"
#include "L1Triplet.h"
#include "L1Branch.h"
#include "L1Track.h"
#include "L1TrackPar.h"

#include "L1Portion.h"
#include "L1HitPoint.h"
#include "L1Strip.h"

#include <iostream>
#include <vector>
#include <map>

using std::vector;
using std::map;

#ifdef PULLS
#define TRIP_PERFORMANCE
class L1AlgoPulls;
#endif
#ifdef TRIP_PERFORMANCE
template<int NHits> class L1AlgoEfficiencyPerformance;
#endif
#ifdef DOUB_PERFORMANCE
template<int NHits> class L1AlgoEfficiencyPerformance;
#endif

class L1Algo{
 public:

   void Init( fscal geo[] );
   
    /// The main procedure - find tracks.
  void CATrackFinder();

    /// Track fitting procedures
  void KFTrackFitter_simple(); // version, which use procedured used during the reconstruction
  void KFTrackFitter();        // version from SIMD-KF benchmark

    /// ----- Input data ----- 
      // filled in CbmL1::ReadEvent();

  enum{ MaxNStations = 10 };
  int NStations,    // number of all detector stations
      NMvdStations; // number of mvd stations
  L1Station vStations[MaxNStations] _fvecalignment; // station info
  
  vector< L1Strip > vStsStrips,  // strips positions created from hits. Front strips
                    vStsStripsB; // back strips
  vector< fscal >   vStsZPos;    // all possible z-positions of hits
  vector< L1StsHit > vStsHits;     // hits as a combination of front-, backstrips and z-position
  vector< unsigned char > vSFlag,  // information of hits station & using hits in tracks;
                          vSFlagB;
  int StsHitsStartIndex[MaxNStations+1], StsHitsStopIndex[MaxNStations+1]; // station-bounders in vStsHits array

    /// ----- Output data ----- 
  vector< L1Track > vTracks; // reconstructed tracks
  vector< THitI > vRecoHits; // packed hits of reconstructed tracks

  double CATime; // time of trackfinding

  friend class CbmL1;
  private:

        /// =================================  FUNCTIONAL PART  =================================
    
    /// ----- Hit-point-strips conversion routines ------

  void GetHitCoor(const L1StsHit& _h, fscal &_x, fscal &_y, fscal &_z, const L1Station &sta);
  void StripsToCoor(const fscal &u, const fscal &v, fscal &x, fscal &y, const L1Station &sta); // convert strip positions to coordinates
  void StripsToCoor(const fvec &u, const fvec &v, fvec &x, fvec &y, const L1Station &sta);
  L1HitPoint CreateHitPoint(const L1StsHit &hit, char ista); // full the hit point by hit information.

  
    /// ----- Subroutines used by L1Algo::CATrackFinder() ------
  
  void CAFindTrack(vector< L1StsHit > &svStsHits, unsigned int *RealIHit, int ista, const L1Triplet* ptrip,
                   L1Branch& newtrack, unsigned char &new_L, fscal &new_chi2,
                   L1Branch &currenttrack, unsigned char &curr_L, fscal &curr_chi2,
                   int &NCalls);

    /// -- Flags routines --
  unsigned char GetFStation( unsigned char flag ){ return flag/4; }
  bool GetFUsed   ( unsigned char flag ){ return (flag&0x02)!=0; }
//   bool GetFUsedD  ( unsigned char flag ){ return (flag&0x01)!=0; }

  void SetFStation ( unsigned char &flag, unsigned int iStation ){ flag = iStation*4 + (flag%4); }
  void SetFUsed    ( unsigned char &flag ){ flag |= 0x02; }
//   void SetFUsedD   ( unsigned char &flag ){ flag |= 0x01; }
  void SetFUnUsed  ( unsigned char &flag ){ flag &= 0xFC; }
//   void SetFUnUsedD ( unsigned char &flag ){ flag &= 0xFE; }

					/// Prepare the portion of left hits data
  void f10(	// input
                int start_lh, int n1,  L1HitPoint *vStsHits_l, 
									// output
                fvec *u_front, fvec *u_back,  fvec *zPos,
                vector<THitI> &hitsl_1
								);

					/// Get the field approximation. Add the target to parameters estimation. Propagate to middle station.
	void f11(	// input
								int isec, int istal,
								int n1_V, 

                fvec *u_front, fvec *u_back,  fvec *zPos,
									// output
                nsL1::vector<L1TrackPar>::TSimd &T_1, nsL1::vector<L1FieldRegion>::TSimd &fld_1,
								fvec* x_minusV, fvec* x_plusV, fvec* y_minusV, fvec* y_plusV
							 );
	
					/// Find the doublets. Reformat data in the portion of doublets.
	void f20(	// input
								int n1, int istar, L1Station &stal, L1Station &stam,
                L1HitPoint *vStsHits_l, L1HitPoint *vStsHits_m, int NHits_m,
								fscal *y_minus, fscal *x_minus, fscal *y_plus, fscal *x_plus,								
                nsL1::vector<L1TrackPar>::TSimd &T_1, nsL1::vector<L1FieldRegion>::TSimd &fld_1,
                vector<THitI> &hitsl_1,
                map<THitI, THitI> &mrDuplets_start,
									// output
								int &n2,
                vector<THitI> &i1_2,
								int &start_mhit,
#ifdef DOUB_PERFORMANCE
                vector<THitI> &hitsl_2,
#endif // DOUB_PERFORMANCE
                vector<THitI> &hitsm_2,
                map<THitI, THitI> &lmDuplets_start, vector<THitI> &lmDuplets_hits, unsigned int &nDuplets_lm
								);
					
					/// Add the middle hits to parameters estimation. Propagate to right station.
					/// Find the triplets (right hit). Reformat data in the portion of triplets.
	void f30(	// input
                L1HitPoint *vStsHits_r, L1Station &stam, L1Station &star,
								
								int istar, int n1,
								L1HitPoint *vStsHits_m,
								nsL1::vector<L1TrackPar>::TSimd &T_1,nsL1::vector<L1FieldRegion>::TSimd &fld_1,
                vector<THitI> &hitsl_1,
                map<THitI, THitI> &lmDuplets_start, vector<THitI> &lmDuplets_hits,

								int n2,
								vector<THitI> &hitsm_2,
                vector<THitI> &i1_2,
																
                map<THitI, THitI> &mrDuplets_start, vector<THitI> &mrDuplets_hits,
									// output
								int &n3,
                nsL1::vector<L1TrackPar>::TSimd &T_3,
                vector<THitI> &hitsl_3,  vector<THitI> &hitsm_3,  vector<THitI> &hitsr_3,
                nsL1::vector<fvec>::TSimd &u_front_3, nsL1::vector<fvec>::TSimd &u_back_3
								);
					
					/// Add the right hits to parameters estimation.
	void f31(	// input
								int n3_V,  
								L1Station &star, 
                nsL1::vector<fvec>::TSimd &u_front_3, nsL1::vector<fvec>::TSimd &u_back_3,
									// output
                nsL1::vector<L1TrackPar>::TSimd &T_3
							 );

          /// Refit Triplets.
  void f32( // input
                int n3, int istal, THitI* _RealIHit,
                nsL1::vector<L1TrackPar>::TSimd &T_3,
                vector<THitI> &hitsl_3,  vector<THitI> &hitsm_3,  vector<THitI> &hitsr_3,
                int nIterations = 0
                         );
  
					/// Select triplets. Save them into vTriplets.
	void f4(	// input
								int n3, int istal,
                nsL1::vector<L1TrackPar>::TSimd &T_3,
                vector<THitI> &hitsl_3,  vector<THitI> &hitsm_3,  vector<THitI> &hitsr_3,
								// output
								unsigned int &nstaltriplets,
								vector<L1Triplet> &vTriplets_part,
								unsigned int *TripStartIndexH, unsigned int *TripStopIndexH
// #ifdef XXX								
// 								,unsigned int &stat_n_trip			
// #endif
							 );

					/// Find neighbours of triplets. Calculate level of triplets.
	void f5(	// input
							 	// output
							 unsigned int *TripStartIndexH, unsigned int *TripStopIndexH,
							 int *nlevel
							 );

				 /// Find doublets on station
	void DupletsStaPort(	// input
											int isec,
											int istal,
											vector<L1HitPoint> &vStsHits,
#ifdef DOUB_PERFORMANCE
                      THitI* _RealIHit,
#endif // DOUB_PERFORMANCE

                      vector<int> &n_g1, unsigned int *portionStopIndex,
                      L1Portion<L1TrackPar> &T_g1,
                      L1Portion<L1FieldRegion> &fld_g1,
                      L1Portion<THitI> &hitsl_g1,
						
												// output
                      map<THitI,THitI> *Duplets_start, vector<THitI> *Duplets_hits,

                      vector<int> &n_g2,
                      L1Portion<THitI> &i1_g2,
                      L1Portion<THitI> &hitsm_g2
											);
	
						/// Find triplets on station
	void TripletsStaPort(	// input
														int isec,
														int istal,
                            vector<L1HitPoint> &svStsHits,
                            THitI* _RealIHit,

                            vector<int> &n_g1,
                            L1Portion<L1TrackPar> &T_g1,
                            L1Portion<L1FieldRegion> &fld_g1,
                            L1Portion<THitI> &hitsl_g1,

                            vector<int> &n_g2, unsigned int *portionStopIndex,
                            L1Portion<THitI> &i1_g2,
                            L1Portion<THitI> &hitsm_g2,
															
															// output
                            map<THitI,THitI> *Duplets_start, vector<THitI>  *Duplets_hits,
														vector<L1Triplet> *vTriplets_part,
														unsigned int *TripStartIndexH, unsigned int *TripStopIndexH
														);

  
    ///  ------ Subroutines used by L1Algo::KFTrackFitter()  ------
  
  void GuessVec( L1TrackPar &t, fvec *xV, fvec *yV, fvec *zV, fvec *wV, int NHits );
  void FilterFirst( L1TrackPar &track,fvec &x, fvec &y, fvec &w, L1Station &st );
  void FilterLast( L1TrackPar &track,fvec &x, fvec &y, fvec &w, L1Station &st );
  void Filter( L1TrackPar &T, L1UMeasurementInfo &info, fvec &u , fvec &w);
  
#ifdef TBB
	enum { 
		nthreads = 3, // number of threads
		nblocks = 1 // number of stations on one thread
	}; 	

	friend class ParalleledDup;
	friend class ParalleledTrip;
#endif // TBB

        /// =================================  DATA PART  =================================
  
    /// ----- Different parameters of CATrackFinder -----

  enum { FIRSTCASTATION = 0 };  //first station used in CA

    // fNFindIterations - set number of interation for trackfinding ( 0 < fNFindIterations <= 3 )
    // itetation of finding:
    // isec == 0 - primary fast track
    // isec == 1 - primary all track  
    // isec == 2 - secondary all track 
  enum { fNFindIterations = 3 };

  static const float TRACK_CHI2_CUT = 10.0;  // cut for tracks candidates.
  static const float TRIPLET_CHI2_CUT = 5.0; // cut for selecting triplets before collecting tracks.

    /// parameters which are different for different iterations. Set in the begin of CAL1TrackFinder
  float Pick_m, // coefficient for size of region on middle station for add middle hits in triplets: Dx = Pick*sigma_x Dy = Pick*sigma_y
  Pick_r; // same for right hits
  float PickNeighbour; // (PickNeighbour < dp/dp_error)  =>  triplets are neighbours
  fscal MaxInvMom;     // max considered q/p for tracks
  fvec targX, targY, targZ;                        // target coor
  L1FieldValue targB _fvecalignment;               // field in the target point
  L1XYMeasurementInfo TargetXYInfo _fvecalignment; // target constraint  [cm]

    /// standard sizes of the arrays
  enum { 
    multiCoeff = 1, // central - 1, mbias -

    coeff = 64,

    Portion = 1024/coeff, // portion of left hits

    MaxPortionDoublets = 10000/5 * 64/2 /coeff /*/ multiCoeff*/*1,
    MaxPortionTriplets = 10000*5 * 64/2 /coeff /*/ multiCoeff*/*1,
    MaxNPortion = 40 * coeff / multiCoeff,

    
    MaxArrSize = MaxNPortion*MaxPortionDoublets/MaxNStations    //200000,  // standart size of big arrays  // mas be 40000 for normal work in cbmroot!
  };

  L1FieldRegion vtxFieldRegion _fvecalignment;// really doesn't used
  L1FieldValue  vtxFieldValue _fvecalignment; // field at teh vertex position.

  vector <L1Triplet> vTriplets; // container for triplets got in finding
  int TripStartIndex[MaxNStations+1], TripStopIndex[MaxNStations+1]; // containers for stations bounders in vTriplets

  int fTrackingLevel, fGhostSuppression; // really doesn't used
  float fMomentumCutOff;// really doesn't used

  
    /// ----- Debug features -----
#ifdef PULLS
  L1AlgoPulls* fL1Pulls;
#endif
#ifdef TRIP_PERFORMANCE
  L1AlgoEfficiencyPerformance<3>* fL1Eff_triplets;
  L1AlgoEfficiencyPerformance<3>* fL1Eff_triplets2;
#endif
#ifdef DOUB_PERFORMANCE
  L1AlgoEfficiencyPerformance<2>* fL1Eff_doublets;
#endif
#ifdef DRAW
  friend class L1AlgoDraw;
#endif
  
} _fvecalignment;




// #include "L1Algo.cxx"  // uncomment if don't use make
// #include "L1CATrackFinder.cxx"  // uncomment if don't use make


#endif
