#define _(x) __builtin_##x
#define VA __attribute__((vector_size(32),aligned(1)))
#define Zin static inline __attribute__((always_inline, unused))
typedef unsigned long long U;typedef unsigned char G,*S,VA V;typedef int I;typedef unsigned int UI;typedef long long J;typedef double F;typedef void _;
#define P(b,a...)   if(b)R({a;});
#define W(b...)     while((b))
#define N(n,a...)   {U _n=(n),i=0;W(i<_n){a;++i;}}
#define Z(b,a...)   if(b){a;}else
#define C(i,a...)   case i:{a;}break;
#define S(i,c,a...) switch(i){c default:a;}
#define R return

#define ZI static I
#define ZC static C
#define ZS static S
#define ZJ static J
#define ZU static U
#define ZF static F
#define ZV static _

#define IN(l,x,r)         ((l)<=(x)&&(x)<=(r))

#ifndef NOINLINE
#define ZZZ Zin
#else
#define ZZZ static inline __attribute__((noinline, unused))
#endif

//:~
