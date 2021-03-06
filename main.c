//! avxneon csv parser benchmark, copyright (c) 2020 regents of kparc, bsd-2-clause
#include<unistd.h>  // getopt
#include<stdlib.h>  // atoi
#include<stdio.h>   // printf
#include<time.h>

#define CPD       128 // padding (the input buf should be readable up to buf + CPD)
#define CACHE_LINE 64

#include"f.h"
#include"l.h"
#include"t.h"

Zin _*aligned_malloc(U aln,U n){_*p;P(posix_memalign(&p,aln,n),(_*)0)R p;}
S allocate_padded_buffer(U n,U pad){R(S)aligned_malloc(CACHE_LINE,n+pad);}
U get_corpus(S filename,U pad,S*res){FILE*fp=fopen(filename, "rb");P(!fp,printf("!%s\n",filename),exit(1),0)
  fseek(fp,0,SEEK_END);U len=ftell(fp);S buf=allocate_padded_buffer(len,pad);
  P(!buf,fclose(fp),printf("!oom"),exit(1),0)rewind(fp);U readb=fread(buf,1,len,fp);fclose(fp);
  P(readb!=len,free(buf),printf("!data"),exit(1),0)R*res=buf,len+pad;}

int main(int argc,char*argv[]) {
  int c,verbose=0,dump=0,iterations=50,mode=0;

  W((c=getopt(argc,argv,"vdm:i:s"))!=-1)
   S(c,C('v',verbose=1)C('d',dump=1)C('i',iterations=atoi(optarg))C('m',mode=atoi(optarg)))

  P(optind>=argc,printf("%s <csv>\n",argv[0]),exit(1),1)S filename=(S)argv[optind],p;
  U pn=get_corpus(filename,CPD,&p);

  printf("\n"
   //"host                       : %s\n"
   "file                       : %s\n"
   "size                       : %lld\n"
   "mode                       : %s\n"
   "laps                       : %d\n", filename, pn, mode==2?"naive":mode==1?"quote":"noquote", iterations);

#ifdef __linux__
  int evts[6] = { PERF_COUNT_HW_CPU_CYCLES, PERF_COUNT_HW_INSTRUCTIONS, PERF_COUNT_HW_BRANCH_MISSES,
                  PERF_COUNT_HW_CACHE_REFERENCES, PERF_COUNT_HW_CACHE_MISSES, PERF_COUNT_HW_REF_CPU_CYCLES };
#endif //__linux__

  CSV pcsv;
  pcsv.n   =  0;
  pcsv.sep = '|';
  pcsv.quo = !!(mode==1);
  pcsv.i   = calloc(sizeof(int),pn);

  P(!pcsv.i,printf("!oom\n"), 1)

#ifdef __linux__
  perf_init(2, evts, 6);
#endif // __linux__

  double total = 0;
  double sum,t = 0;

  U i;for(i=0;i<iterations;i++){
      clock_t start = clock();

#ifdef __linux__
      perf_start(0);
#endif // __linux__

      if(2>mode)
        ld((unsigned char *)p, pn, &pcsv);
      else
        ld_naive((unsigned char *)p, pn, &pcsv);
      //pcsv.indexes[pcsv.n++]=pn;  //clamp

#ifdef __linux__
      perf_stop(0);
      //}{TimingPhase p2(ta, 1);}
#endif // __linux__

      total += clock() - start;
  }

  clock_t start = clock();
  sum=0;int ctr=1;
  for(i=0;i<pcsv.n;i++){
    if(dump)printf("idx %d -> %d: ",ctr, pcsv.i[i]);
    ctr++;
    if (i != pcsv.n-1) {
      U j;for (j=pcsv.i[i]; j < pcsv.i[i+1]-1; j++) {
          if(dump)printf("%c",p[j+1]);
      }

      if(!((ctr)%5)){
        if(2>mode){
          if(parse_number((char*)p+pcsv.i[i]+1,&t)){sum+=t;} //lemire's fast_double_parser
        }else
          sum+=strtod(p+pcsv.i[i]+1,0); // stock strtoad
      }

      if(dump)printf("\n");
    }
  }
  F sum_time = clock()-start;
  F time_in_s = sum_time/CLOCKS_PER_SEC;
  printf("control sum                : %f (err %f)\n", sum, sum-350404138.448127);
  printf("parse_double (s)           : %0.2f\n", time_in_s);

  if(verbose) {
    printf("indices                    : %llu\n", pcsv.n);
    printf("bytes / index              : %0.2f\n", pn/(F)pcsv.n);
  }

  F volume = iterations * pn;
  time_in_s = total/CLOCKS_PER_SEC;

  if(verbose) {
    printf("total time (s)             : %0.2f\n", time_in_s);
    //O("number of iterations       : %llu\n", (uint64_t)volume);
  }

#ifdef __linux__
  if(verbose) {
    printf("\n");
    printf("cycles                     : %lu\n",  results[0]);
    printf("cycles/byte                : %.2f\n", results[0] / volume);
    printf("instructions               : %lu\n",  results[1]);
    printf("instructions/byte          : %.2f\n", results[1] / volume);
    printf("instructions/cycle         : %.2f\n", (double)results[1] / results[0]);
    printf("branch misses              : %lu\n",  results[2]);
    printf("branch misses/byte         : %.2f\n", results[2] / volume);
    printf("cache references           : %lu\n",  results[3]);
    printf("cache references/byte      : %.2f\n", results[3] / volume);
    printf("cache misses               : %lu\n",  results[4]);
    printf("cache misses per byte      : %.2f\n", results[4] / volume);
    printf("cpu freq (effective)       : %.2f\n", results[0] / time_in_s / (1000 * 1000 * 1000));
  } else {}

    printf("cycles/byte                : %.2f\n",  (1.0*results[0])/volume);

#endif

    printf("GB/s                       : %.2f\n",  volume / time_in_s / (1024 * 1024 * 1024));
    printf("\n");

  free(pcsv.i);

#ifdef __linux__
  perf_shutdown();
#endif

  R 0;}

//:~
