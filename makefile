ifneq ($(shell uname -m),aarch64)
 A=-march=native
endif

CC=clang
#CC=gcc

o=l
T=taq100m.csv
O=-O3 $A
F=-fno-common -fno-stack-protector -fno-asynchronous-unwind-tables -fdeclspec
N=-Wno-unused-function -Wno-unused-value -Wno-unused-variable \
	-Wno-unused-command-line-argument -Wno-discarded-qualifiers -Wno-unknown-warning-option
W=-Wall -Werror -Wno-incompatible-pointer-types -Wno-pointer-sign $N
C=$(CC) $O $W $F

#U=`uname -rsm`
#-DUNAME="\"$U\""

all: $T *.c *.h
	$C *.c -o$o
	#./$o -vi50 -m2 $T  # naive scalar loop
	#./$o -vi50 -m1 $T  # vec parse quotes
	./$o -vi10 -m0 $T  # vec skip quotes
	@#./$o -d -i1 -m1 q.csv  # validate quotes (vect)
	@#./$o -d -i1 -m2 q.csv  # validate quotes (naive)

test: *.c *.h
	@./$o -d -i1 -m1 q.csv  # validate quotes (vect)
	@./$o -d -i1 -m2 q.csv  # validate quotes (naive)

unroll:
	$C -g *.c -o$o -DUNROLL -DNOINLINE
	objdump --disassemble-functions=_zip $o > zip.UNROLL
	./$o -vi10 -m0 $T

nounroll:
	$C  -g *.c -o$o -UUNROLL -DNOINLINE
	objdump --disassemble-functions=_zip $o > zip.NOUNROLL
	./$o -vi10 -m0 $T

$T:
	gunzip -k $T.gz
