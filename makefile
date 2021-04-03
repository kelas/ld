ifneq ($(shell uname -m),aarch64)
 A=-march=native
endif

U=`uname -rsm`

o=l
T=taq100m.csv
O=-O3 $A
F=-fno-common -fno-stack-protector -fno-asynchronous-unwind-tables
N=-Wno-unused-function -Wno-unused-value -Wno-unused-variable -Wno-unused-command-line-argument
W=-Wall -Werror -ferror-limit=1 -Wno-incompatible-pointer-types -Wno-pointer-sign $N
C=clang $O $W $F -DUNAME="\"$U\""

all: $T *.c *.h
	@$C *.c -o$o
	./$o -vi50 -m2 $T  # naive loop
	./$o -vi50 -m1 $T  # parse quotes
	./$o -vi50 -m0 $T  # skip quotes

$T:
	gunzip -k $T.gz
