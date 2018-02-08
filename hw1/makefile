#.PHONY: all
#all:
gcc99: as1p2.c
	gcc -Wpedantic -Wall -Wextra -std=gnu99 -Wno-unused $< -o as1p2

.PHONY: clang11
clang11: as1p2.c
	clang -Wpedantic -Wall -Wextra -std=gnu11 $< -o as1p2

#.PHONY: backup
#backup:
#	rm -f ./backup/*;
#	cp ./* ./backup

.PHONY: clean
clean:
	rm -f *.o
