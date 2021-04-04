//! fast csv parser avx/neon, copyright (c) 2020 regents of kparc, bsd-2-clause
#include"k.h"

typedef struct CSV{U n;I*i;G sep;G quo;}CSV;U ld(const S buf,U len,CSV*pcsv);U ld_naive(const S s,U n,CSV*r);

#define _(x) __builtin_##x
#define aV(a...) ((v128){a})
#define Zin static inline __attribute__((always_inline, unused))

#ifndef __ARM_NEON
#include<x86intrin.h>
typedef __m256i vec;typedef __m128i v128;typedef struct span{vec l;vec h;}span;
#define vld(p)(span){loadu(p),loadu(p+32)}
#define mmask _mm256_movemask_epi8
#define loadu _mm256_loadu_si256
#define cmpeq _mm256_cmpeq_epi8
#define vset1 _mm256_set1_epi8
#define vmull _mm_clmulepi64_si128
#else
#include<arm_neon.h>
typedef uint8x16_t vec;typedef struct span{vec i0;vec i1;vec i2;vec i3;}span;
#define vld(p)(span){loadu(p),loadu(p+16),loadu(p+32),loadu(p+48)}
#define loadu vld1q_u8
#define cmpeq vceqq_u8
#define vset1 vmovq_n_u8
#define vmull vmull_p64
Zin U mmask(vec p0,vec p1,vec p2,vec p3){
 const vec bitmask = { 0x01,0x02,0x4,0x8,0x10,0x20,0x40,0x80,0x01,0x02,0x4,0x8,0x10,0x20,0x40,0x80 };
 vec t0=vandq_u8(p0,bitmask),t1=vandq_u8(p1,bitmask),t2=vandq_u8(p2,bitmask),t3=vandq_u8(p3,bitmask),
 sum0=vpaddq_u8(t0,t1),sum1=vpaddq_u8(t2,t3);sum0=vpaddq_u8(sum0,sum1);sum0=vpaddq_u8(sum0,sum0);
 R vgetq_lane_u64(vreinterpretq_u64_u8(sum0),0);}
#endif

// notes:

// compare mask vs input, 5 uops, should be cheaper with avx512
Zin U cmi(span in,G m);

// find quote mask (which is a half-open mask that covers the first quote in a quote pair and everything in the quote pair)
// we also update the prev_iter_inside_quote value to tell the next iteration whether we finished the final iteration inside
// a quote pair; if so, this inverts our behavior of whether we're inside quotes for the next iteration.

Zin U fqm(span x,U*in_q);
// flatten values in 'b' assuming that they have values of idx plus their position in the bitvector, and store these indexes at p[ba]
// incrementing ba as we go. we potentially store extra values beyond end of valid bits, so p needs to be large enough to handle this.
Zin I zip(I*p,I ba,I idx,U b);

//:~
