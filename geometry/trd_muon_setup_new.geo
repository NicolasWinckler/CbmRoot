//*********************************
// This is the geometry file of the following configuration
// Number of stations: 1
// Number of layers per station: 4
//*******************************************
// Inner Radius of station 1: 250.208542
// Outer Radius of station 1: 4370.419922
// z-position at the middle of the station 1: 5120.000000
//*******************************************
//*********************************
trd1
cave
PGON
air
2
45 360 4
-120.000000 249.208542 5380.419922 
120.000000 249.208542 5380.419922 
0. 0. 5120.0
1.  0.  0.  0.  1.  0.  0.  0.  1.
//*********************************
//*********************************
trd1layer#1
trd1
PGON
air
2
45 360 4
-30.000000 249.708542 4870.419922 
30.000000 249.708542 4870.419922 
0. 0. -90.0
1.  0.  0.  0.  1.  0.  0.  0.  1.
//*********************************
//*********************************
trd1mod1#1
trd1layer#1
BOX
air
500.000000 -250.000000 -30.000000
500.000000 250.000000 -30.000000
-500.000000 250.000000 -30.000000
-500.000000 -250.000000 -30.000000
500.000000 -250.000000 30.000000
500.000000 250.000000 30.000000
-500.000000 250.000000 30.000000
-500.000000 -250.000000 30.000000
-250.000000 500.000000 0.000000
1.  0.  0.  0.  1.  0.  0.  0.  1.
//*********************************
//*********************************
trd1mod1radiator
trd1mod1#1
BOX
polypropylene
480.000000 -230.000000 -15.000000
480.000000 230.000000 -15.000000
-480.000000 230.000000 -15.000000
-480.000000 -230.000000 -15.000000
480.000000 -230.000000 15.000000
480.000000 230.000000 15.000000
-480.000000 230.000000 15.000000
-480.000000 -230.000000 15.000000
0.000000 0.000000 -15.000000
1.  0.  0.  0.  1.  0.  0.  0.  1.
//*********************************
//*********************************
trd1mod1gas
trd1mod1#1
BOX
TRDgas
480.000000 -230.000000 -3.000000
480.000000 230.000000 -3.000000
-480.000000 230.000000 -3.000000
-480.000000 -230.000000 -3.000000
480.000000 -230.000000 3.000000
480.000000 230.000000 3.000000
-480.000000 230.000000 3.000000
-480.000000 -230.000000 3.000000
0.000000 0.000000 3.000000
1.  0.  0.  0.  1.  0.  0.  0.  1.
//*********************************
//*********************************
trd1mod1padplane
trd1mod1#1
BOX
goldcoatedcopper
480.000000 -230.000000 -0.025000
480.000000 230.000000 -0.025000
-480.000000 230.000000 -0.025000
-480.000000 -230.000000 -0.025000
480.000000 -230.000000 0.025000
480.000000 230.000000 0.025000
-480.000000 230.000000 0.025000
-480.000000 -230.000000 0.025000
0.000000 0.000000 6.025000
1.  0.  0.  0.  1.  0.  0.  0.  1.
//*********************************
//*********************************
trd1mod1mylar
trd1mod1#1
BOX
mylar
480.000000 -230.000000 -1.000000
480.000000 230.000000 -1.000000
-480.000000 230.000000 -1.000000
-480.000000 -230.000000 -1.000000
480.000000 -230.000000 1.000000
480.000000 230.000000 1.000000
-480.000000 230.000000 1.000000
-480.000000 -230.000000 1.000000
0.000000 0.000000 7.500000
1.  0.  0.  0.  1.  0.  0.  0.  1.
//*********************************
//*********************************
trd1mod1electronics
trd1mod1#1
BOX
goldcoatedcopper
480.000000 -230.000000 -0.050000
480.000000 230.000000 -0.050000
-480.000000 230.000000 -0.050000
-480.000000 -230.000000 -0.050000
480.000000 -230.000000 0.050000
480.000000 230.000000 0.050000
-480.000000 230.000000 0.050000
-480.000000 -230.000000 0.050000
0.000000 0.000000 8.550000
1.  0.  0.  0.  1.  0.  0.  0.  1.
//*********************************
//*********************************
trd1mod1carbon1#1
trd1mod1#1
BOX
carbon
500.000000 -10.000000 -19.299999
500.000000 10.000000 -19.299999
-500.000000 10.000000 -19.299999
-500.000000 -10.000000 -19.299999
500.000000 -10.000000 19.299999
500.000000 10.000000 19.299999
-500.000000 10.000000 19.299999
-500.000000 -10.000000 19.299999
0.000000 240.000000 -10.700000
1.  0.  0.  0.  1.  0.  0.  0.  1.
//*********************************
//*********************************
trd1mod1carbon1#2
trd1mod1#1
0.000000 -240.000000 -10.700000
1.  0.  0.  0.  1.  0.  0.  0.  1.
//*********************************
//*********************************
trd1mod1carbon2#1
trd1mod1#1
BOX
carbon
10.000000 -230.000000 -19.299999
10.000000 230.000000 -19.299999
-10.000000 230.000000 -19.299999
-10.000000 -230.000000 -19.299999
10.000000 -230.000000 19.299999
10.000000 230.000000 19.299999
-10.000000 230.000000 19.299999
-10.000000 -230.000000 19.299999
490.000000 0.000000 -10.700000
1.  0.  0.  0.  1.  0.  0.  0.  1.
//*********************************
//*********************************
trd1mod1carbon2#2
trd1mod1#1
-490.000000 0.000000 -10.700000
1.  0.  0.  0.  1.  0.  0.  0.  1.
//*********************************
//*********************************
trd1mod1#2
trd1layer#1
250.000000 -500.000000 0.000000
1.  0.  0.  0.  1.  0.  0.  0.  1.
//*********************************
//*********************************
trd1mod1#3
trd1layer#1
500.000000 250.000000 0.000000
0.  1.  0.  -1.  0.  0.  0.  0.  1.
//*********************************
//*********************************
trd1mod1#4
trd1layer#1
-500.000000 -250.000000 0.000000
0.  1.  0.  -1.  0.  0.  0.  0.  1.
//*********************************
//*********************************
trd1mod2#1
trd1layer#1
BOX
air
750.000000 -375.000000 -30.000000
750.000000 375.000000 -30.000000
-750.000000 375.000000 -30.000000
-750.000000 -375.000000 -30.000000
750.000000 -375.000000 30.000000
750.000000 375.000000 30.000000
-750.000000 375.000000 30.000000
-750.000000 -375.000000 30.000000
-750.000000 1125.000000 0.000000
1.  0.  0.  0.  1.  0.  0.  0.  1.
//*********************************
//*********************************
trd1mod2radiator
trd1mod2#1
BOX
polypropylene
730.000000 -355.000000 -15.000000
730.000000 355.000000 -15.000000
-730.000000 355.000000 -15.000000
-730.000000 -355.000000 -15.000000
730.000000 -355.000000 15.000000
730.000000 355.000000 15.000000
-730.000000 355.000000 15.000000
-730.000000 -355.000000 15.000000
0.000000 0.000000 -15.000000
1.  0.  0.  0.  1.  0.  0.  0.  1.
//*********************************
//*********************************
trd1mod2gas
trd1mod2#1
BOX
TRDgas
730.000000 -355.000000 -3.000000
730.000000 355.000000 -3.000000
-730.000000 355.000000 -3.000000
-730.000000 -355.000000 -3.000000
730.000000 -355.000000 3.000000
730.000000 355.000000 3.000000
-730.000000 355.000000 3.000000
-730.000000 -355.000000 3.000000
0.000000 0.000000 3.000000
1.  0.  0.  0.  1.  0.  0.  0.  1.
//*********************************
//*********************************
trd1mod2padplane
trd1mod2#1
BOX
goldcoatedcopper
730.000000 -355.000000 -0.025000
730.000000 355.000000 -0.025000
-730.000000 355.000000 -0.025000
-730.000000 -355.000000 -0.025000
730.000000 -355.000000 0.025000
730.000000 355.000000 0.025000
-730.000000 355.000000 0.025000
-730.000000 -355.000000 0.025000
0.000000 0.000000 6.025000
1.  0.  0.  0.  1.  0.  0.  0.  1.
//*********************************
//*********************************
trd1mod2mylar
trd1mod2#1
BOX
mylar
730.000000 -355.000000 -1.000000
730.000000 355.000000 -1.000000
-730.000000 355.000000 -1.000000
-730.000000 -355.000000 -1.000000
730.000000 -355.000000 1.000000
730.000000 355.000000 1.000000
-730.000000 355.000000 1.000000
-730.000000 -355.000000 1.000000
0.000000 0.000000 7.500000
1.  0.  0.  0.  1.  0.  0.  0.  1.
//*********************************
//*********************************
trd1mod2electronics
trd1mod2#1
BOX
goldcoatedcopper
730.000000 -355.000000 -0.050000
730.000000 355.000000 -0.050000
-730.000000 355.000000 -0.050000
-730.000000 -355.000000 -0.050000
730.000000 -355.000000 0.050000
730.000000 355.000000 0.050000
-730.000000 355.000000 0.050000
-730.000000 -355.000000 0.050000
0.000000 0.000000 8.550000
1.  0.  0.  0.  1.  0.  0.  0.  1.
//*********************************
//*********************************
trd1mod2carbon1#1
trd1mod2#1
BOX
carbon
750.000000 -10.000000 -19.299999
750.000000 10.000000 -19.299999
-750.000000 10.000000 -19.299999
-750.000000 -10.000000 -19.299999
750.000000 -10.000000 19.299999
750.000000 10.000000 19.299999
-750.000000 10.000000 19.299999
-750.000000 -10.000000 19.299999
0.000000 365.000000 -10.700000
1.  0.  0.  0.  1.  0.  0.  0.  1.
//*********************************
//*********************************
trd1mod2carbon1#2
trd1mod2#1
0.000000 -365.000000 -10.700000
1.  0.  0.  0.  1.  0.  0.  0.  1.
//*********************************
//*********************************
trd1mod2carbon2#1
trd1mod2#1
BOX
carbon
10.000000 -355.000000 -19.299999
10.000000 355.000000 -19.299999
-10.000000 355.000000 -19.299999
-10.000000 -355.000000 -19.299999
10.000000 -355.000000 19.299999
10.000000 355.000000 19.299999
-10.000000 355.000000 19.299999
-10.000000 -355.000000 19.299999
740.000000 0.000000 -10.700000
1.  0.  0.  0.  1.  0.  0.  0.  1.
//*********************************
//*********************************
trd1mod2carbon2#2
trd1mod2#1
-740.000000 0.000000 -10.700000
1.  0.  0.  0.  1.  0.  0.  0.  1.
//*********************************
//*********************************
trd1mod2#2
trd1layer#1
750.000000 1125.000000 0.000000
1.  0.  0.  0.  1.  0.  0.  0.  1.
//*********************************
//*********************************
trd1mod2#3
trd1layer#1
-750.000000 -1125.000000 0.000000
1.  0.  0.  0.  1.  0.  0.  0.  1.
//*********************************
//*********************************
trd1mod2#4
trd1layer#1
750.000000 -1125.000000 0.000000
1.  0.  0.  0.  1.  0.  0.  0.  1.
//*********************************
//*********************************
trd1mod2#5
trd1layer#1
-1125.000000 0.000000 0.000000
0.  1.  0.  -1.  0.  0.  0.  0.  1.
//*********************************
//*********************************
trd1mod2#6
trd1layer#1
1125.000000 0.000000 0.000000
0.  1.  0.  -1.  0.  0.  0.  0.  1.
//*********************************
//*********************************
trd1mod3#1
trd1layer#1
BOX
air
1000.000000 -500.000000 -30.000000
1000.000000 500.000000 -30.000000
-1000.000000 500.000000 -30.000000
-1000.000000 -500.000000 -30.000000
1000.000000 -500.000000 30.000000
1000.000000 500.000000 30.000000
-1000.000000 500.000000 30.000000
-1000.000000 -500.000000 30.000000
-500.000000 2000.000000 0.000000
1.  0.  0.  0.  1.  0.  0.  0.  1.
//*********************************
//*********************************
trd1mod3radiator
trd1mod3#1
BOX
polypropylene
980.000000 -480.000000 -15.000000
980.000000 480.000000 -15.000000
-980.000000 480.000000 -15.000000
-980.000000 -480.000000 -15.000000
980.000000 -480.000000 15.000000
980.000000 480.000000 15.000000
-980.000000 480.000000 15.000000
-980.000000 -480.000000 15.000000
0.000000 0.000000 -15.000000
1.  0.  0.  0.  1.  0.  0.  0.  1.
//*********************************
//*********************************
trd1mod3gas
trd1mod3#1
BOX
TRDgas
980.000000 -480.000000 -3.000000
980.000000 480.000000 -3.000000
-980.000000 480.000000 -3.000000
-980.000000 -480.000000 -3.000000
980.000000 -480.000000 3.000000
980.000000 480.000000 3.000000
-980.000000 480.000000 3.000000
-980.000000 -480.000000 3.000000
0.000000 0.000000 3.000000
1.  0.  0.  0.  1.  0.  0.  0.  1.
//*********************************
//*********************************
trd1mod3padplane
trd1mod3#1
BOX
goldcoatedcopper
980.000000 -480.000000 -0.025000
980.000000 480.000000 -0.025000
-980.000000 480.000000 -0.025000
-980.000000 -480.000000 -0.025000
980.000000 -480.000000 0.025000
980.000000 480.000000 0.025000
-980.000000 480.000000 0.025000
-980.000000 -480.000000 0.025000
0.000000 0.000000 6.025000
1.  0.  0.  0.  1.  0.  0.  0.  1.
//*********************************
//*********************************
trd1mod3mylar
trd1mod3#1
BOX
mylar
980.000000 -480.000000 -1.000000
980.000000 480.000000 -1.000000
-980.000000 480.000000 -1.000000
-980.000000 -480.000000 -1.000000
980.000000 -480.000000 1.000000
980.000000 480.000000 1.000000
-980.000000 480.000000 1.000000
-980.000000 -480.000000 1.000000
0.000000 0.000000 7.500000
1.  0.  0.  0.  1.  0.  0.  0.  1.
//*********************************
//*********************************
trd1mod3electronics
trd1mod3#1
BOX
goldcoatedcopper
980.000000 -480.000000 -0.050000
980.000000 480.000000 -0.050000
-980.000000 480.000000 -0.050000
-980.000000 -480.000000 -0.050000
980.000000 -480.000000 0.050000
980.000000 480.000000 0.050000
-980.000000 480.000000 0.050000
-980.000000 -480.000000 0.050000
0.000000 0.000000 8.550000
1.  0.  0.  0.  1.  0.  0.  0.  1.
//*********************************
//*********************************
trd1mod3carbon1#1
trd1mod3#1
BOX
carbon
1000.000000 -10.000000 -19.299999
1000.000000 10.000000 -19.299999
-1000.000000 10.000000 -19.299999
-1000.000000 -10.000000 -19.299999
1000.000000 -10.000000 19.299999
1000.000000 10.000000 19.299999
-1000.000000 10.000000 19.299999
-1000.000000 -10.000000 19.299999
0.000000 490.000000 -10.700000
1.  0.  0.  0.  1.  0.  0.  0.  1.
//*********************************
//*********************************
trd1mod3carbon1#2
trd1mod3#1
0.000000 -490.000000 -10.700000
1.  0.  0.  0.  1.  0.  0.  0.  1.
//*********************************
//*********************************
trd1mod3carbon2#1
trd1mod3#1
BOX
carbon
10.000000 -480.000000 -19.299999
10.000000 480.000000 -19.299999
-10.000000 480.000000 -19.299999
-10.000000 -480.000000 -19.299999
10.000000 -480.000000 19.299999
10.000000 480.000000 19.299999
-10.000000 480.000000 19.299999
-10.000000 -480.000000 19.299999
990.000000 0.000000 -10.700000
1.  0.  0.  0.  1.  0.  0.  0.  1.
//*********************************
//*********************************
trd1mod3carbon2#2
trd1mod3#1
-990.000000 0.000000 -10.700000
1.  0.  0.  0.  1.  0.  0.  0.  1.
//*********************************
//*********************************
trd1mod3#2
trd1layer#1
1500.000000 2000.000000 0.000000
1.  0.  0.  0.  1.  0.  0.  0.  1.
//*********************************
//*********************************
trd1mod3#3
trd1layer#1
-1500.000000 -2000.000000 0.000000
1.  0.  0.  0.  1.  0.  0.  0.  1.
//*********************************
//*********************************
trd1mod3#4
trd1layer#1
500.000000 -2000.000000 0.000000
1.  0.  0.  0.  1.  0.  0.  0.  1.
//*********************************
//*********************************
trd1mod3#5
trd1layer#1
-2000.000000 1500.000000 0.000000
0.  1.  0.  -1.  0.  0.  0.  0.  1.
//*********************************
//*********************************
trd1mod3#6
trd1layer#1
-2000.000000 -500.000000 0.000000
0.  1.  0.  -1.  0.  0.  0.  0.  1.
//*********************************
//*********************************
trd1mod3#7
trd1layer#1
2000.000000 500.000000 0.000000
0.  1.  0.  -1.  0.  0.  0.  0.  1.
//*********************************
//*********************************
trd1mod3#8
trd1layer#1
2000.000000 -1500.000000 0.000000
0.  1.  0.  -1.  0.  0.  0.  0.  1.
//*********************************
//*********************************
trd1mod3#9
trd1layer#1
3500.000000 2000.000000 0.000000
1.  0.  0.  0.  1.  0.  0.  0.  1.
//*********************************
//*********************************
trd1mod3#10
trd1layer#1
3500.000000 1000.000000 0.000000
1.  0.  0.  0.  1.  0.  0.  0.  1.
//*********************************
//*********************************
trd1mod3#11
trd1layer#1
3500.000000 0.000000 0.000000
1.  0.  0.  0.  1.  0.  0.  0.  1.
//*********************************
//*********************************
trd1mod3#12
trd1layer#1
3500.000000 -1000.000000 0.000000
1.  0.  0.  0.  1.  0.  0.  0.  1.
//*********************************
//*********************************
trd1mod3#13
trd1layer#1
3500.000000 -2000.000000 0.000000
1.  0.  0.  0.  1.  0.  0.  0.  1.
//*********************************
//*********************************
trd1mod3#14
trd1layer#1
-3500.000000 2000.000000 0.000000
1.  0.  0.  0.  1.  0.  0.  0.  1.
//*********************************
//*********************************
trd1mod3#15
trd1layer#1
-3500.000000 1000.000000 0.000000
1.  0.  0.  0.  1.  0.  0.  0.  1.
//*********************************
//*********************************
trd1mod3#16
trd1layer#1
-3500.000000 0.000000 0.000000
1.  0.  0.  0.  1.  0.  0.  0.  1.
//*********************************
//*********************************
trd1mod3#17
trd1layer#1
-3500.000000 -1000.000000 0.000000
1.  0.  0.  0.  1.  0.  0.  0.  1.
//*********************************
//*********************************
trd1mod3#18
trd1layer#1
-3500.000000 -2000.000000 0.000000
1.  0.  0.  0.  1.  0.  0.  0.  1.
//*********************************
//*********************************
trd1layer#2
trd1
0. 0. -30.0
1.  0.  0.  0.  1.  0.  0.  0.  1.
//*********************************
//*********************************
trd1layer#3
trd1
0. 0. 30.0
1.  0.  0.  0.  1.  0.  0.  0.  1.
//*********************************
//*********************************
trd1layer#4
trd1
0. 0. 90.0
1.  0.  0.  0.  1.  0.  0.  0.  1.
//*********************************
