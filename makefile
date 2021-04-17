ifneq ($(shell uname -m),arm64)
 A=-march=native
else
 A=-march=armv8-a+crc+crypto
endif

CC=clang
#CC=gcc-10

o=l
T=taq100m.csv
O=-O3 $A
F=-fno-common -fno-stack-protector -fno-asynchronous-unwind-tables
W=-Wall -Werror -Wno-incompatible-pointer-types -Wno-pointer-sign -Wno-unused-function
C=$(CC) $O $W $F

#U=`uname -rsm`
#-DUNAME="\"$U\""

all: $T *.c *.h
	$C *.c -o$o
	./$o -vi10 -m2 $T  # naive scalar loop
	./$o -vi10 -m1 $T  # vec parse quotes
	./$o -vi10 -m0 $T  # vec skip quotes
	@#./$o -d -i1 -m1 q.csv  # validate quotes (vect)
	@#./$o -d -i1 -m2 q.csv  # validate quotes (naive)

test: *.c *.h
	@./$o -d -i1 -m1 q.csv  # validate quotes (vect)
	@./$o -d -i1 -m2 q.csv  # validate quotes (naive)
	@#objdump --disassemble-functions=_zip $o > zip.NOUNROLL

$T:
	gunzip -k $T.gz
