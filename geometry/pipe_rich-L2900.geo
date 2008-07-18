//  $Id: pipe_rich-L2900.geo,v 1.2 2006/01/31 09:14:14 hoehne Exp $

//  History of cvs commits:
//  $Log: pipe_rich-L2900.geo,v $
//  Revision 1.2  2006/01/31 09:14:14  hoehne
//  new pipe geometry from pipe_newvac.geo implemented
//
//  Revision 1.1  2005/09/20 09:27:49  friese
//  Modified beam pipe in sts
//
//  Revision 1.2  2005/07/12 12:54:28  kharlov
//  Cut off a chunk of pipe inside RICH
//
//  - Revision   Johann M. Heuser, 21.7.2005
//  - new geometry for the vacuum section: barrel-like shape
//  - diameter at z=1700: reduced from 

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
 260. 130. 130.5
 270.  13.  13.4
1500.  30.  30.5
1600.  80.  80.5

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
 260.  0. 130.   
 270.  0.  13.
1500.  0.  30.  
1600   0.  80.

0. 0. 0. 
1.  0.  0.  0. 1.  0.  0. 0. 1
//

pipe2
cave
PCON 
carbon
3
0. 360.
4500. 0. 225.5
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
4500. 0. 225.  
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
