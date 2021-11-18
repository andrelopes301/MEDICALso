all: balcao 

balcao: balcao.o 
	gcc balcao.o -o balcao

balcao.o: balcao.c balcao.h medico.h cliente.h
	gcc -c balcao.c

clean: 
	rm *.o balcao
