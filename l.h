//! fast csv parser avx/neon, copyright (c) 2020 regents of kparc, bsd-2-clause
#include"k.h"

typedef struct CSV{I n;I*i;G sep;G quo;}CSV;I csv(const S buf,U len,CSV*pcsv);

#define QT '"'
#define LF '\n'
#define CBF 8      // csv buffering rounds for better pipelining

#define Zin static inline __attribute__((always_inline, unused))

#ifndef __ARM_NEON
#include<x86intrin.h>
typedef struct span{__m256i lo;__m256i hi;}span;
#define mvmask _mm256_movemask_epi8
#define loadu _mm256_loadu_si256
#define cmpeq _mm256_cmpeq_epi8
#define vset1 _mm256_set1_epi8
#define vld(p)(span){loadu(p),loadu(p+32)}            //vector load
#else
#include<arm_neon.h>
typedef struct span{uint8x16_t i0;uint8x16_t i1;uint8x16_t i2;uint8x16_t i3;}span;
#define loadu vld1q_u8
#define cmpeq vceqq_u8
#define vset1 vmovq_n_u8
#define vld(p)(span){loadu(p),loadu(p+16),loadu(p+32),loadu(p+48)}
Zin U mvmask(uint8x16_t p0,uint8x16_t p1,uint8x16_t p2,uint8x16_t p3){
 const uint8x16_t bitmask = { 0x01,0x02,0x4,0x8,0x10,0x20,0x40,0x80,0x01,0x02,0x4,0x8,0x10,0x20,0x40,0x80 };
 uint8x16_t t0=vandq_u8(p0,bitmask),t1=vandq_u8(p1,bitmask),t2=vandq_u8(p2,bitmask),t3=vandq_u8(p3,bitmask),
 sum0=vpaddq_u8(t0,t1),sum1=vpaddq_u8(t2,t3);sum0=vpaddq_u8(sum0,sum1);sum0=vpaddq_u8(sum0,sum0);
 R vgetq_lane_u64(vreinterpretq_u64_u8(sum0),0);}
#endif

// compare mask vs input, 5 uops, should be cheaper with avx512
Zin U cmi(span in,G m);
// find quote mask (which is a half-open mask that covers the first quote in a quote pair and everything in the quote pair)
// we also update the prev_iter_inside_quote value to tell the next iteration whether we finished the final iteration inside
// a quote pair; if so, this inverts our behavior of whether we're inside quotes for the next iteration.
Zin U fqm(span x,U*in_q);
// flatten values in 'b' assuming that they have values of idx plus their position in the bitvector, and store these indexes at p[ba]
// incrementing ba as we go will potentially store extra values beyond end of valid bits, so p needs to be large enough to handle this.
Zin I zip(I*p,I ba,I idx,U b);

#define fetch  __builtin_prefetch
#define ctz    __builtin_ctzll
#define popcnt __builtin_popcountll

//:~
