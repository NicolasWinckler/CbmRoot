//  $Id: pipe_newvac.geo,v 1.5 2006/07/19 07:05:59 heuser Exp $
//
//  History of cvs commits:
//  $Log: pipe_newvac.geo,v $
//  Revision 1.5  2006/07/19 07:05:59  heuser
//  back to revision 1.3:
//
//  - pipevac1 is again in volume cave.
//  (volume pipe1 is just a tube with a wall thickness of 500 um.
//   The vacuum pipevac1 must therefore be in the mother volume of pipe1,
//   i.e. in cave).
//
//  Revision 1.4  2006/07/11 13:17:14  heuser
//  correction:    wrong:    pipevac1 was in volume cave.
//                 correct:  pipevac1 is now in volume pipe1.
//
//  Revision 1.3  2006/05/24 15:46:45  heuser
//  // Revision 1.3  Johann M. Heuser, 22.5.2006
//  // new geometry for the vacuum section:
//  //    * barrel-like shape with 13 cm radius between z=3.5cm and z=24cm.
//  //    * inclined downstream window "end plate" between z=24 and z=27 cm,
//  //      thickness 500 micron.
//  //  - diameter at z=1700: 8.5 cm to match the beam pipe in the RICH.
//
//  Revision 1.2  2005/07/12 12:54:28  kharlov
//  Cut off a chunk of pipe inside RICH
//
//  - Revision 1.3  Johann M. Heuser, 22.5.2006
//  - new geometry for the vacuum section: 
//    * barrel-like shape with 13 cm radius between z=3.5cm and z=24cm.
//    * inclined downstream window "end plate" between z=24 and z=27 cm, 
//      thickness 500 micron.
//  - diameter at z=1700: 8.5 cm to match the beam pipe in the RICH.
//
//  description of shape PCON:
//  - material
//  - number of points
//  - azimuthal angle from xxx to yyy   [deg]
//  - point 1: z  inner-radius  outer-radius [mm]
//  - ...
//  - point n 
//  
//----------------------------------------------------------
pipe1
cave
PCON 
carbon
7
0. 360.
 -50.  25.  25.5 
  25.  25.  25.5 
  35. 130. 130.5
 240. 130. 130.5
 270.  10.  10.5
1500.  30.  30.5
1700.  85.  85.5

0. 0. 0. 
1.  0.  0.  0.  1.  0.  0.  0.  1
//
pipevac1
cave
PCON
vacuum
7
0. 360.  
 -50.  0.  25.   
  25.  0.  25.  
  35.  0. 130.
 240.  0. 130.   
 270.  0.  10.
1500.  0.  30.  
1700.  0.  85.

0. 0. 0. 
1.  0.  0.  0. 1.  0.  0. 0. 1
//

pipe2
cave
PCON 
carbon
3
0. 360.
3700.25 0. 185.5 
6000. 0. 300.5 
10999. 0. 300.5 
0. 0. 0. 
1.  0.  0.  0.  1.  0.  0.  0.  1

pipevac2
pipe2
PCON
vacuum
3
0. 360.  
3700.25 0. 185.  
6000. 0. 300.  
10999. 0. 300. 
0. 0. 0. 
1.  0.  0.  0. 1.  0.  0. 0. 1
//

pipe3
cave
PCON 
carbon
2
0. 360.
11001. 0. 300.5 
11400. 0. 300.5
0. 0. 0. 
1.  0.  0.  0.  1.  0.  0.  0.  1

pipevac3
pipe3
PCON
vacuum
2
0. 360.
11001. 0. 300.
11400. 0. 300.
0. 0. 0. 
1.  0.  0.  0. 1.  0.  0. 0. 1
