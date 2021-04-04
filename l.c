//! csv(), copyright (c) 2020 regents of kparc, bsd-2-clause
#include"l.h"
#define BN 8                          // bulk rounds for better pipelining
#define SHR(m) m^=*in_q,*in_q=((J)(m))>>63,m // assume shr(J) well-defined
#ifdef __ARM_NEON
Zin U cmi(span z,G m){vec vm=vset1(m);R mvmask(cmpeq(z.i0,vm),cmpeq(z.i1,vm),cmpeq(z.i2,vm),cmpeq(z.i3,vm));}
Zin U fqm(span x,U*in_q){U qbts=cmi(x,'"');U m=vmull(-1ULL,qbts);R SHR(m);}
#else
Zin U cmi(span z,G m){vec b=vset1(m);U x=(I)mvmask(cmpeq(z.l,b)),y=mvmask(cmpeq(z.h,b));R(y<<32)|x;}
Zin U fqm(span x,U*in_q){U m=-1ULL*cmi(x,'"');R SHR(m);}
#endif
#define z(i) p[ba+i]=idx+_(ctzll)(b),b=b&(b-1);
Zin I zip(I*p,I ba,I idx,U b){P(!b,ba)I n=_(popcountll)(b),nxb=ba+n;N(8,z(i))Z(n>8,N(8,z(i+8)));Z(n>16,ba+=16;do{z(0)ba++;}W(b));R nxb;}
#define mask(dest) qt_mask=r->quo?fqm(in,&in_qt):0ULL,sep=cmi(in,r->sep),end=cmi(in,'\n');dest=(end|sep)&~qt_mask;
I csv(const S s,U len,CSV*r){U intl_idx,sep,end,qt_mask,f_sep,idx=0,in_qt=0ULL;I*base_ptr=r->i,base=0;len-=len<64?0:64;
 if(BN&&len>64*BN){U fields[BN];for(;idx<len-64*BN+1;idx+=64*BN){
  N(BN,intl_idx=64*i+idx;_(prefetch)(s+intl_idx+128);span in=vld(s+intl_idx);mask(fields[i]))
  N(BN,intl_idx=64*i+idx;base=zip(base_ptr,base,intl_idx,fields[i]))}}
 for(;idx<len;idx+=64){_(prefetch)(s+idx+128);span in=vld(s+idx);mask(f_sep)base=zip(base_ptr,base,idx,f_sep);}R  r->n=base;}

//:~
