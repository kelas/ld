//! csv(), copyright (c) 2020 regents of kparc, bsd-2-clause
#include"l.h"
#define SHR(m) m^=*in_q,*in_q=((J)(m))>>63,m // assume shr(J) well-defined
#ifdef __ARM_NEON
Zin U cmi(span z,G m){uint8x16_t vm=vset1(m);uint8x16_t r0=cmpeq(z.i0,vm),r1=cmpeq(z.i1,vm),r2=cmpeq(z.i2,vm),r3=cmpeq(z.i3,vm);R mvmask(r0,r1,r2,r3);}
Zin U fqm(span x,U*in_q){U qbts=cmi(x,QT);U m=vmull_p64(-1ULL,qbts);R SHR(m);}
#else
Zin U cmi(span z,G m){__m256i b=vset1(m);U x=(I)mvmask(cmpeq(z.lo,b)),y=mvmask(cmpeq(z.hi,b));R(y<<32)|x;}
Zin U fqm(span x,U*in_q){U qbts=cmi(x,QT);U m=_mm_cvtsi128_si64(_mm_clmulepi64_si128(_mm_set_epi64x(0ULL,qbts),_mm_set1_epi8(0xFF),0));R SHR(m);}
#endif
#define z(i) p[ba+i]=idx+ctz(b),b=b&(b-1);
Zin I zip(I*p,I ba,I idx,U b){P(!b,ba)I n=popcnt(b),nxb=ba+n;N(8,z(i))Z(n>8,N(8,z(i+8)));Z(n>16,ba+=16;do{z(0)ba++;}W(b));R nxb;}
#define mask(dest) qt_mask=r->quo?fqm(in,&in_qt):0ULL,sep=cmi(in,r->sep),end=cmi(in,LF);dest=(end|sep)&~qt_mask;
I csv(const S s,U len,CSV*r){U intl_idx,sep,end,qt_mask,f_sep,idx=0,in_qt=0ULL;I*base_ptr=r->i,base=0;len-=len<64?0:64;
#if 1
 if(CBF&&len>64*CBF){U fields[CBF];for(;idx<len-64*CBF+1;idx+=64*CBF){                           // bulk idx decode for
  N(CBF,intl_idx=64*i+idx;fetch(s+intl_idx+128);span in=vld(s+intl_idx);mask(fields[i]))         // more inst per cycle
  N(CBF,intl_idx=64*i+idx;base=zip(base_ptr,base,intl_idx,fields[i]))}}                          // end is not buffered
#endif
 for(;idx<len;idx+=64){fetch(s+idx+128);span in=vld(s+idx);mask(f_sep)base=zip(base_ptr,base,idx,f_sep);}R  r->n=base;}

//:~
