#pragma once
typedef unsigned long long U;typedef unsigned char G,*S;typedef unsigned int I;typedef long long J;typedef double F;typedef void _;

#define R return
#define P(b,a...)   if(b)R({a;});
#define W(b...)     while((b))                //while
#define N(n,a...)   {U _n=(n),i=0;W(i<_n){a;++i;}}
#define Z(b,a...)   if(b){a;}else             //cond
#define C(i,a...)   case i:{a;}break;         //case
#define S(i,c,a...) switch(i){c default:a;} //switch

#ifdef SIGHANDLER
#include<signal.h>
#define AB(s)       (w2((S)"AB: "),w2((S)s),raise(SIGABRT),(K)0L) //abort string
#else
#define AB(s)       (w2((S)"AB: "),w2((S)s),nl(),exit(1),(K)0L)           //abort string
#endif

//:~
