//! ld() fast csv tokenizer, copyright (c) 2021 regents of kparc, bsd-2-clause
#include"l.h"
#define SHR(m) m^=*in_q,*in_q=((J)(m))>>63,m //assumes shr(J) well-defined
#ifdef __ARM_NEON
Zin U cmi(span z,G m){vec vm=vset1(m);R mmask(cmpeq(z.i0,vm),cmpeq(z.i1,vm),cmpeq(z.i2,vm),cmpeq(z.i3,vm));}
Zin U fqm(span x,U*in_q){U qbts=cmi(x,'"');U m=vmull(-1ULL,qbts);R SHR(m);} //no vmull on rpi4
#else
Zin U cmi(span z,G m){vec b=vset1(m);U x=(UI)mmask(cmpeq(b,z.l)),y=mmask(cmpeq(b,z.h));R(y<<32)|x;}
Zin U fqm(span x,U*in_q){U qbts=cmi(x,'"');U m=vmull(aV(qbts,0),aV(-1ULL),0)[0];R SHR(m);}
#endif
#define z(i) p[ba+i]=idx+_(ctzll)(b),b=b&(b-1);
Zin UI zip(UI*p,UI ba,UI idx,U b){P(!b,ba)UI n=_(popcountll)(b),nxb=ba+n;N(8,z(i))Z(n>8,N(8,z(i+8)));Z(n>16,ba+=16;do{z(0)ba++;}W(b));R nxb;}
#define mask(dest) qt_mask=r->quo?fqm(in,&in_qt):0ULL,sep=cmi(in,r->sep),trm=cmi(in,'\n');dest=(trm|sep)&~qt_mask;
#define BN 8 //optional, extra code for better pipelining
U ld(const S s,U len,CSV*r){U intl_idx,sep,trm,qt_mask,f_sep,idx=0,base=0,in_qt=0ULL;UI*base_ptr=r->i;len-=len<64?0:64;
 if(BN&&len>64*BN){U fields[BN];for(;idx<len-64*BN+1;idx+=64*BN){ //eliminated if BN is zero
  N(BN,intl_idx=64*i+idx;_(prefetch)(s+intl_idx+128);span in=vld(s+intl_idx);mask(fields[i]))
  N(BN,intl_idx=64*i+idx;base=zip(base_ptr,base,intl_idx,fields[i]))}}
 for(;idx<len;idx+=64){_(prefetch)(s+idx+128);span in=vld(s+idx);mask(f_sep)base=zip(base_ptr,base,idx,f_sep);}R r->n=base;}

U ld_naive(const S s,U n,CSV*r){r->n=0;G q=0;N(n,S(s[i],C('"',Z(q&&s[i+1]=='"',i++)q=!q)case'|':C('\n',Z(!q,r->i[r->n++]=i))))R r->n;}

//:~
