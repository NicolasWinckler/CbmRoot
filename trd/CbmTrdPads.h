// -------------------------------------------------------------------------
// -----                     CbmTrdPads header file                    -----
// -----               Created 03/12/2009 by D. Emschermann            -----
// -------------------------------------------------------------------------
//
// v0.2 - 20091207 DE - fixed typo in pad x size in large module
// v0.1 - 20091204 DE - initial version

#ifndef CBMTRDPADS_H
#define CBMTRDPADS_H

  Float_t fst1_pad_type[6][2] = // array of pad sizes in station 1
    { {  .5, 46./23},     // 5 x  20    mm,  92 * 23 = 2116 pads mod1
      {  .5, 46./13},     // 5 x  35.38 mm,  92 * 13 = 1196 pads mod1
      {  .5, 71./20},     // 5 x  35.5  mm, 142 * 20 = 2840 pads mod2
      {  .5, 71./10},     // 5 x  71    mm, 142 * 10 = 1420 pads mod2
      {  .6, 96./12},     // 6 x  80    mm, 160 * 12 = 1920 pads mod3
      {  .8, 96./8 } };   // 8 x 120    mm, 120 *  8 =  960 pads mod3

  Float_t fst2_pad_type[6][2] = // array of pad sizes in station 2
    { {  .5, 46./10},     // 5 x  46    mm,  92 * 10 =  920 pads mod1
      {  .5, 46./10},     // 5 x  46    mm,  92 * 10 =  920 pads mod1
      {  .5, 71./10},     // 5 x  71    mm, 142 * 10 = 1420 pads mod2
      {  .7, 71./10},     // 7.1x 71    mm, 100 * 10 = 1000 pads mod2
      {  .8, 96./8 },     // 8 x 120    mm, 120 *  8 =  960 pads mod3
      {  .8, 96./8 } };   // 8 x 120    mm, 120 *  8 =  960 pads mod3

  Float_t fst3_pad_type[2][2] = // array of pad sizes in station 3
    { {  .6, 96./12},     // 6 x  80    mm, 160 * 12 = 1920 pads mod3
      {  .8, 96./8 } };   // 8 x 120    mm, 120 *  8 =  960 pads mod3

#endif





