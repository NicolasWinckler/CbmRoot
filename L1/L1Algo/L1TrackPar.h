#ifndef L1TrackPar_h
#define L1TrackPar_h 1

#include "L1Types.h"

class L1TrackPar{

  public:

    fvec
      x,y,tx,ty,qp,z,
      C00,
      C10, C11,
      C20, C21, C22,
      C30, C31, C32, C33,
      C40, C41, C42, C43, C44,
      chi2, NDF                ;

    L1TrackPar(){};
    L1TrackPar(double *T, double *C){
      x = T[0];
      y = T[1];
      tx = T[2];
      ty = T[3];
      qp = T[4];
      z = T[5];

      C00 = C[0];
      C10 = C[1];
      C11 = C[2];
      C20 = C[3];
      C21 = C[4];
      C22 = C[5];
      C30 = C[6];
      C31 = C[7];
      C32 = C[8];
      C33 = C[9];
      C40 = C[10];
      C41 = C[11];
      C42 = C[12];
      C43 = C[13];
      C44 = C[14];
    };
      
    void SetOneEntry( const int i0, const L1TrackPar &T1, const int i1 );

    void Print(int i = -1);
// void L1Extrapolate
// ( 
// //  L1TrackPar &T, // input track parameters (x,y,tx,ty,Q/p) and cov.matrix
//  fvec        z_out  , // extrapolate to this z position
//  fvec       qp0    , // use Q/p linearisation at this value
//  L1FieldRegion &F
//  );

    
} _fvecalignment;

 // =============================================================================================

inline void L1TrackPar::Print(int i){
  cout.setf(std::ios::scientific,std::ios::floatfield);
  if (i == -1){
    cout << "T = " << endl;
    cout << x  << endl;
    cout << y  << endl;
    cout << tx << endl;
    cout << ty << endl;
    cout << qp << endl;
    cout << z  << endl;
  }
  else{
    cout << "T = ";
    cout << x[i]  << " ";
    cout << y[i]  << " ";
    cout << tx[i] << " ";
    cout << ty[i] << " ";
    cout << qp[i] << " ";
    cout << z[i]  << endl;
    cout << "C = ";
    cout << C00[i]  << " ";
    cout << C11[i]  << " ";
    cout << C22[i]  << " ";
    cout << C33[i]  << " ";
    cout << C44[i]  << endl;
  }
};

inline void L1TrackPar::SetOneEntry( const int i0, const L1TrackPar &T1, const int i1 )
{
  x[i0] = T1.x[i1];
  y[i0] = T1.y[i1];
  tx[i0] = T1.tx[i1];
  ty[i0] = T1.ty[i1];
  qp[i0] = T1.qp[i1];
  z[i0] = T1.z[i1];
  C00[i0] = T1.C00[i1];
  C10[i0] = T1.C10[i1];
  C11[i0] = T1.C11[i1];
  C20[i0] = T1.C20[i1];
  C21[i0] = T1.C21[i1];
  C22[i0] = T1.C22[i1];
  C30[i0] = T1.C30[i1];
  C31[i0] = T1.C31[i1];
  C32[i0] = T1.C32[i1];
  C33[i0] = T1.C33[i1];
  C40[i0] = T1.C40[i1];
  C41[i0] = T1.C41[i1];
  C42[i0] = T1.C42[i1];
  C43[i0] = T1.C43[i1];
  C44[i0] = T1.C44[i1];
   
  chi2[i0] = T1.chi2[i1];
  NDF[i0] = T1.NDF[i1];
} // SetOneEntry

// #define cnst static const fvec 
// inline void L1TrackPar::L1Extrapolate
//     (
// //  L1TrackPar &T, // input track parameters (x_,y_,tx,ty,Q/p) and cov.matrix
//     fvec        z_out  , // extrapolate to this z position
//  fvec       qp0    , // use Q/p linearisation at this value
//  L1FieldRegion &F
//     )
// {
//   //cout<<"Extrapolation..."<<endl;
//   //
//   //  Part of the analytic extrapolation formula with error (c_light*B*dz)^4/4!
//   //
// 
//   cnst ZERO = 0.0, ONE = 1.;
//   cnst c_light = 0.000299792458, c_light_i = 1./c_light;
// 
//   cnst
//       c1 = 1., c2 = 2., c3 = 3., c4 = 4., c6 = 6., c9 = 9., c15 = 15., c18 = 18., c45 = 45.,
//   c2i = 1./2., c3i = 1./3., c6i = 1./6., c12i = 1./12.;
// 
//   fvec dz = (z_out -z);
//   fvec dz2 = dz*dz;
//   fvec dz3 = dz2*dz;
// 
//   // construct coefficients
// 
//   fvec x_   =tx;
//   fvec y_   =ty;
//   fvec xx  = x_*x_;
//   fvec xy = x_*y_;
//   fvec yy = y_*y_;
//   fvec y2 = y_*c2;
//   fvec x2 = x_*c2;
//   fvec x4 = x_*c4;
//   fvec xx31 = xx*c3+c1;
//   fvec xx159 = xx*c15+c9;
// 
//   fvec Ay = -xx-c1;
//   fvec Ayy = x_*(xx*c3+c3);
//   fvec Ayz = -c2*xy;
//   fvec Ayyy = -(c15*xx*xx+c18*xx+c3);
// 
//   fvec Ayy_x = c3*xx31;
//   fvec Ayyy_x = -x4*xx159;
// 
//   fvec Bx = yy+c1;
//   fvec Byy = y_*xx31;
//   fvec Byz = c2*xx+c1;
//   fvec Byyy = -xy*xx159;
// 
//   fvec Byy_x = c6*xy;
//   fvec Byyy_x = -y_*(c45*xx+c9);
//   fvec Byyy_y = -x_*xx159;
// 
//   // end of coefficients calculation
// 
//   fvec t2   = c1 + xx + yy;
//   fvec t    = sqrt( t2 );
//   fvec h    = qp0*c_light;
//   fvec ht   = h*t;
// 
//   // get field integrals
//   fvec ddz =z-F.z0;
//   fvec Fx0 = F.cx0 + F.cx1*ddz + F.cx2*ddz*ddz;
//   fvec Fx1 = (F.cx1 + c2*F.cx2*ddz)*dz;
//   fvec Fx2 = F.cx2*dz2;
//   fvec Fy0 = F.cy0 + F.cy1*ddz + F.cy2*ddz*ddz;
//   fvec Fy1 = (F.cy1 + c2*F.cy2*ddz)*dz;
//   fvec Fy2 = F.cy2*dz2;
//   fvec Fz0 = F.cz0 + F.cz1*ddz + F.cz2*ddz*ddz;
//   fvec Fz1 = (F.cz1 + c2*F.cz2*ddz)*dz;
//   fvec Fz2 = F.cz2*dz2;
// 
//   //
// 
//   fvec sx = ( Fx0 + Fx1*c2i + Fx2*c3i  );
//   fvec sy = ( Fy0 + Fy1*c2i + Fy2*c3i );
//   fvec sz = ( Fz0 + Fz1*c2i + Fz2*c3i );
// 
//   fvec Sx = ( Fx0*c2i + Fx1*c6i + Fx2*c12i );
//   fvec Sy = ( Fy0*c2i + Fy1*c6i + Fy2*c12i );
//   fvec Sz = ( Fz0*c2i + Fz1*c6i + Fz2*c12i );
// 
//   fvec syz;
//   {
//     cnst
//         d = 1./360.,
//     c00 = 30.*6.*d, c01 = 30.*2.*d,   c02 = 30.*d,
//     c10 = 3.*40.*d, c11 = 3.*15.*d,   c12 = 3.*8.*d,
//     c20 = 2.*45.*d, c21 = 2.*2.*9.*d, c22 = 2.*2.*5.*d;
//     syz = Fy0*( c00*Fz0 + c01*Fz1 + c02*Fz2)
//         +   Fy1*( c10*Fz0 + c11*Fz1 + c12*Fz2)
//         +   Fy2*( c20*Fz0 + c21*Fz1 + c22*Fz2) ;
//   }
// 
//   fvec Syz;
//   {
//     cnst
//         d = 1./2520.,
//     c00 = 21.*20.*d, c01 = 21.*5.*d, c02 = 21.*2.*d,
//     c10 =  7.*30.*d, c11 =  7.*9.*d, c12 =  7.*4.*d,
//     c20 =  2.*63.*d, c21 = 2.*21.*d, c22 = 2.*10.*d;
//     Syz = Fy0*( c00*Fz0 + c01*Fz1 + c02*Fz2 )
//         +   Fy1*( c10*Fz0 + c11*Fz1 + c12*Fz2 )
//         +   Fy2*( c20*Fz0 + c21*Fz1 + c22*Fz2 ) ;
//   }
// 
//   fvec syy  = sy*sy*c2i;
//   fvec syyy = syy*sy*c3i;
// 
//   fvec Syy ;
//   {
//     cnst
//         d= 1./2520., c00= 420.*d, c01= 21.*15.*d, c02= 21.*8.*d,
//     c03= 63.*d, c04= 70.*d, c05= 20.*d;
//     Syy =  Fy0*(c00*Fy0+c01*Fy1+c02*Fy2) + Fy1*(c03*Fy1+c04*Fy2) + c05*Fy2*Fy2 ;
//   }
// 
//   fvec Syyy;
//   {
//     cnst
//         d = 1./181440.,
//     c000 =   7560*d, c001 = 9*1008*d, c002 = 5*1008*d,
//     c011 = 21*180*d, c012 = 24*180*d, c022 =  7*180*d,
//     c111 =    540*d, c112 =    945*d, c122 =    560*d, c222 = 112*d;
//     fvec Fy22 = Fy2*Fy2;
//     Syyy = Fy0*( Fy0*(c000*Fy0+c001*Fy1+c002*Fy2)+ Fy1*(c011*Fy1+c012*Fy2)+c022*Fy22 )
//         +    Fy1*( Fy1*(c111*Fy1+c112*Fy2)+c122*Fy22) + c222*Fy22*Fy2                  ;
//   }
// 
// 
//   fvec sA1   = sx*xy   + sy*Ay   + sz*y_ ;
//   fvec sA1_x = sx*y_ - sy*x2 ;
//   fvec sA1_y = sx*x_ + sz ;
// 
//   fvec sB1   = sx*Bx   - sy*xy   - sz*x_ ;
//   fvec sB1_x = -sy*y_ - sz ;
//   fvec sB1_y = sx*y2 - sy*x_ ;
// 
//   fvec SA1   = Sx*xy   + Sy*Ay   + Sz*y_ ;
//   fvec SA1_x = Sx*y_ - Sy*x2 ;
//   fvec SA1_y = Sx*x_ + Sz;
// 
//   fvec SB1   = Sx*Bx   - Sy*xy   - Sz*x_ ;
//   fvec SB1_x = -Sy*y_ - Sz;
//   fvec SB1_y = Sx*y2 - Sy*x_;
// 
// 
//   fvec sA2   = syy*Ayy   + syz*Ayz ;
//   fvec sA2_x = syy*Ayy_x - syz*y2 ;
//   fvec sA2_y = -syz*x2 ;
//   fvec sB2   = syy*Byy   + syz*Byz  ;
//   fvec sB2_x = syy*Byy_x + syz*x4 ;
//   fvec sB2_y = syy*xx31 ;
// 
//   fvec SA2   = Syy*Ayy   + Syz*Ayz ;
//   fvec SA2_x = Syy*Ayy_x - Syz*y2 ;
//   fvec SA2_y = -Syz*x2 ;
//   fvec SB2   = Syy*Byy   + Syz*Byz ;
//   fvec SB2_x = Syy*Byy_x + Syz*x4 ;
//   fvec SB2_y = Syy*xx31 ;
// 
//   fvec sA3   = syyy*Ayyy  ;
//   fvec sA3_x = syyy*Ayyy_x;
//   fvec sB3   = syyy*Byyy  ;
//   fvec sB3_x = syyy*Byyy_x;
//   fvec sB3_y = syyy*Byyy_y;
// 
// 
//   fvec SA3   = Syyy*Ayyy  ;
//   fvec SA3_x = Syyy*Ayyy_x;
//   fvec SB3   = Syyy*Byyy  ;
//   fvec SB3_x = Syyy*Byyy_x;
//   fvec SB3_y = Syyy*Byyy_y;
// 
//   fvec ht1 = ht*dz;
//   fvec ht2 = ht*ht*dz2;
//   fvec ht3 = ht*ht*ht*dz3;
//   fvec ht1sA1 = ht1*sA1;
//   fvec ht1sB1 = ht1*sB1;
//   fvec ht1SA1 = ht1*SA1;
//   fvec ht1SB1 = ht1*SB1;
//   fvec ht2sA2 = ht2*sA2;
//   fvec ht2SA2 = ht2*SA2;
//   fvec ht2sB2 = ht2*sB2;
//   fvec ht2SB2 = ht2*SB2;
//   fvec ht3sA3 = ht3*sA3;
//   fvec ht3sB3 = ht3*sB3;
//   fvec ht3SA3 = ht3*SA3;
//   fvec ht3SB3 = ht3*SB3;
// 
//   x_  += (x_ + ht1SA1 + ht2SA2 + ht3SA3)*dz ;
//   y_  += (y_ + ht1SB1 + ht2SB2 + ht3SB3)*dz ;
//   tx += ht1sA1 + ht2sA2 + ht3sA3;
//   ty += ht1sB1 + ht2sB2 + ht3sB3;
//   z  += dz;
// 
//   fvec ctdz  = c_light*t*dz;
//   fvec ctdz2 = c_light*t*dz2;
// 
//   fvec dqp = qp - qp0;
//   fvec t2i = c1*rcp(t2);// /t2;
//   fvec xt2i = x_*t2i;
//   fvec yt2i = y_*t2i;
//   fvec tmp0 = ht1SA1 + c2*ht2SA2 + c3*ht3SA3;
//   fvec tmp1 = ht1SB1 + c2*ht2SB2 + c3*ht3SB3;
//   fvec tmp2 = ht1sA1 + c2*ht2sA2 + c3*ht3sA3;
//   fvec tmp3 = ht1sB1 + c2*ht2sB2 + c3*ht3sB3;
// 
//   fvec j02 = dz*(c1 + xt2i*tmp0 + ht1*SA1_x + ht2*SA2_x + ht3*SA3_x);
//   fvec j12 = dz*(     xt2i*tmp1 + ht1*SB1_x + ht2*SB2_x + ht3*SB3_x);
//   fvec j22 =     c1 + xt2i*tmp2 + ht1*sA1_x + ht2*sA2_x + ht3*sA3_x ;
//   fvec j32 =          xt2i*tmp3 + ht1*sB1_x + ht2*sB2_x + ht3*sB3_x ;
// 
//   fvec j03 = dz*(     yt2i*tmp0 + ht1*SA1_y + ht2*SA2_y );
//   fvec j13 = dz*(c1 + yt2i*tmp1 + ht1*SB1_y + ht2*SB2_y + ht3*SB3_y );
//   fvec j23 =          yt2i*tmp2 + ht1*sA1_y + ht2*sA2_y  ;
//   fvec j33 =     c1 + yt2i*tmp3 + ht1*sB1_y + ht2*sB2_y + ht3*sB3_y ;
// 
//   fvec j04 = ctdz2*( SA1 + c2*ht1*SA2 + c3*ht2*SA3 );
//   fvec j14 = ctdz2*( SB1 + c2*ht1*SB2 + c3*ht2*SB3 );
//   fvec j24 = ctdz *( sA1 + c2*ht1*sA2 + c3*ht2*sA3 );
//   fvec j34 = ctdz *( sB1 + c2*ht1*sB2 + c3*ht2*sB3 );
// 
//   // extrapolate inverse momentum
// 
//   x_ +=j04*dqp;
//   y_ +=j14*dqp;
//   tx+=j24*dqp;
//   ty+=j34*dqp;
// 
//  //          covariance matrix transport
// 
//   fvec c42 =C42, c43 =C43;
// 
//   fvec cj00 =C00 +C20*j02 +C30*j03 +C40*j04;
//   fvec cj10 =C10 +C21*j02 +C31*j03 +C41*j04;
//   fvec cj20 =C20 +C22*j02 +C32*j03 + c42*j04;
//   fvec cj30 =C30 +C32*j02 +C33*j03 + c43*j04;
// 
//   fvec cj01 =C10 +C20*j12 +C30*j13 +C40*j14;
//   fvec cj11 =C11 +C21*j12 +C31*j13 +C41*j14;
//   fvec cj21 =C21 +C22*j12 +C32*j13 + c42*j14;
//   fvec cj31 =C31 +C32*j12 +C33*j13 + c43*j14;
// 
//   fvec cj02 =C20*j22 +C30*j23 +C40*j24;
//   fvec cj12 =C21*j22 +C31*j23 +C41*j24;
//   fvec cj22 =C22*j22 +C32*j23 + c42*j24;
//   fvec cj32 =C32*j22 +C33*j23 + c43*j24;
// 
//   fvec cj03 =C20*j32 +C30*j33 +C40*j34;
//   fvec cj13 =C21*j32 +C31*j33 +C41*j34;
//   fvec cj23 =C22*j32 +C32*j33 + c42*j34;
//   fvec cj33 =C32*j32 +C33*j33 + c43*j34;
// 
//   C40+= c42*j02 + c43*j03 +C44*j04; // cj40
//   C41+= c42*j12 + c43*j13 +C44*j14; // cj41
//   C42 = c42*j22 + c43*j23 +C44*j24; // cj42
//   C43 = c42*j32 + c43*j33 +C44*j34; // cj43
// 
//   C00 = cj00 + j02*cj20 + j03*cj30 + j04*C40;
//   C10 = cj01 + j02*cj21 + j03*cj31 + j04*C41;
//   C11 = cj11 + j12*cj21 + j13*cj31 + j14*C41;
// 
//   C20 = j22*cj20 + j23*cj30 + j24*C40 ;
//   C30 = j32*cj20 + j33*cj30 + j34*C40 ;
//   C21 = j22*cj21 + j23*cj31 + j24*C41 ;
//   C31 = j32*cj21 + j33*cj31 + j34*C41 ;
//   C22 = j22*cj22 + j23*cj32 + j24*C42 ;
//   C32 = j32*cj22 + j33*cj32 + j34*C42 ;
//   C33 = j32*cj23 + j33*cj33 + j34*C43 ;
//   //cout<<"Extrapolation ok"<<endl;
// 
// }
// 



#endif