CFLAGS=-Wall -g -DNDEBUG

all: clean ex22_main
# //ex1 ex3 ex7 ex8 ex9 ex10 ex11 ex12 ex13 ex14 ex15 ex15_functions ex16_no_ptr ex17 ex18 ex19 ex22

ex22_main: ex22.o
ex22.o: ex22.c

clean:
# 		rm -f ex1
# 		rm -f ex3
# 		rm -f ex7
# 		rm -f ex8
# 		rm -f ex9
# 		rm -f ex10
# 		rm -f ex11
# 		rm -f ex12
# 		rm -f ex13
# 		rm -f ex14
# 		rm -f ex15
# 		rm -f ex15_functions
# 		rm -f ex16
# 		rm -f ex16_no_ptr
# 		rm -f ex17
# 		rm -f ex18
# 		rm -f ex19
		rm -f ex22_main
		rm -f ex22.o