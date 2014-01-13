
//----------------------------------------------------------
// RICH barrel
// Names hard coded in: cbmroot/rich/prototype/CbmRichProt.cxx
// and cbmroot/rich/prototype/CbmRichProtHitProducer.cxx
//
// RICHglassJLO: reflectivity according to lab measurements at Wuppertal
// RICHgas_CO2_dis_Ogawa_Mehu: absorption form M. Ogawa, J. Chem. Phys. 54 2550 (1971)
//			       refractive index from A. Bideau-Mehu PhD thesis (1982)
//
//-----------------------------------------------------------

//-----------------------------------------
rich1first
cave
BOX
aluminium
700.0 -540.0 0.0 
700.0 190.0 0.0 
-700.0 190.0 0.0 
-700.0 -540.0 0.0 
700.0 -540.0 160.0 
700.0 190.0 160.0 
-700.0 190.0 160.0 
-700.0 -540.0 160.0 
0. 0. 0.
1. 0. 0. 0. 1. 0. 0. 0. 1.

//-----------------------------------------
rich1gas1
rich1first
BOX
RICHgas_CO2_dis_Ogawa_Mehu
695.0 -535.0 5.0 
695.0 185.0 5.0 
-695.0 185.0 5.0 
-695.0 -535.0 5.0 
695.0 -535.0 160.0 
695.0 185.0 160.0 
-695.0 185.0 160.0 
-695.0 -535.0 160.0 
0. 0. 0.
1. 0. 0. 0. 1. 0. 0. 0. 1.

//-----------------------------------------
rich1trap
cave
TRAP
aluminium
700.0 -540.0 160.0 
700.0 190.0 160.0 
-700.0 190.0 160.0 
-700.0 -540.0 160.0 
700.0 -540.0 272.0 
700.0 860.0 272.0 
-700.0 860.0 272.0 
-700.0 -540.0 272.0 
0. 0. 0.
1. 0. 0. 0. 1. 0. 0. 0. 1.

//-----------------------------------------
rich1gas2
rich1trap
TRAP
RICHgas_CO2_dis_Ogawa_Mehu
695.0 -535.0 160.0 
695.0 185.0 160.0 
-695.0 185.0 160.0 
-695.0 -535.0 160.0 
695.0 -535.0 272.0 
695.0 855.0 272.0 
-695.0 855.0 272.0 
-695.0 -535.0 272.0 
0. 0. 0.
1. 0. 0. 0. 1. 0. 0. 0. 1.

//-----------------------------------------
rich1end
cave
BOX
aluminium
700.0 -540.0 272.0 
700.0 860.0 272.0 
-700.0 860.0 272.0 
-700.0 -540.0 272.0 
700.0 -540.0 2400.0 
700.0 860.0 2400.0 
-700.0 860.0 2400.0 
-700.0 -540.0 2400.0 
0. 0. 0.
1. 0. 0. 0. 1. 0. 0. 0. 1.

//-----------------------------------------
rich1gas3
rich1end
BOX
RICHgas_CO2_dis_Ogawa_Mehu
695.0 -535.0 272.0 
695.0 855.0 272.0 
-695.0 855.0 272.0 
-695.0 -535.0 272.0 
695.0 -535.0 2395.0 
695.0 855.0 2395.0 
-695.0 855.0 2395.0 
-695.0 -535.0 2395.0 
0. 0. 0.
1. 0. 0. 0. 1. 0. 0. 0. 1.

//-------------------------------------
//mirrors
//-----------------------------------------
rich1mgl1
rich1gas3
SPHE
RICHglassJLO
3000.0 3006.0
78.6533 86.1479
86.1479 93.7760
0.0 407.1 -1288.2435
-1.0 0. 0. 0. -0.135716 0.990748 0.0 0.990748 0.135716 
//-----------------------------------------
rich1mgl2
rich1gas3
SPHE
RICHglassJLO
3000.0 3006.0
78.6533 86.1479
93.8521 101.3467
0.0 407.1 -1288.2435
-1.0 0. 0. 0. -0.135716 0.990748 0.0 0.990748 0.135716 
//-----------------------------------------
rich1mgl3
rich1gas3
SPHE
RICHglassJLO
3000.0 3006.0
86.2240 93.8521
86.1479 93.7760
0.0 407.1 -1288.2435
-1.0 0. 0. 0. -0.135716 0.990748 0.0 0.990748 0.135716 
//-----------------------------------------
rich1mgl4
rich1gas3
SPHE
RICHglassJLO
3000.0 3006.0
86.2240 93.8521
93.8521 101.3467
0.0 407.1 -1288.2435
-1.0 0. 0. 0. -0.135716 0.990748 0.0 0.990748 0.135716 


//-------------------------------------
//camera
//-----------------------------------------
rich1detector
rich1gas2
BOX
CsI
107.0 -107.0 0.0
107.0 107.0 0.0
-107.0 107.0 0.0
-107.0 -107.0 0.0
107.0 -107.0 6.0
107.0 107.0 6.0
-107.0 107.0 6.0
-107.0 -107.0 6.0
0.0 424.6 220.8
1.0 0. 0. 0. 0.9728 -0.2318 0.0 0.2318 0.9728 

//-------------------------------------
//Entrance Window
//-----------------------------------------
rich1window
rich1first
BOX
kapton
175.0 -175.0 0.0
175.0 175.0 0.0
-175.0 175.0 0.0
-175.0 -175.0 0.0
175.0 -175.0 2.0
175.0 175.0 2.0
-175.0 175.0 2.0
-175.0 -175.0 2.0
0.0 0.0 0.0
1.0 0.0 0.0 0.0 1.0 0.0 0.0 0.0 1.0
//-----------------------------------------

//-------------------------------------
//Entrance Window
//-----------------------------------------
rich1wingas
rich1first
BOX
RICHgas_CO2_dis_Ogawa_Mehu
175.0 -175.0 2.0
175.0 175.0 2.0
-175.0 175.0 2.0
-175.0 -175.0 2.0
175.0 -175.0 5.0
175.0 175.0 5.0
-175.0 175.0 5.0
-175.0 -175.0 5.0
0.0 0.0 0.0
1.0 0.0 0.0 0.0 1.0 0.0 0.0 0.0 1.0
//-----------------------------------------
