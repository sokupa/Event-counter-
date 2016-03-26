all:	bbst
bbst: bbst.o
	g++ -o bbst bbst.cpp
clean :  
	rm -rf *.o bbst 
