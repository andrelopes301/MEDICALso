#include "balcao.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>


int main(int argc, char *argv[]){


    Cliente utente;
    char sintomas[MAX];


    //verificar se existe um Balcão em funcionamento
    if(access(BALCAO_FIFO, F_OK) != 0){
        printf("[INFO] Balcão não está em funcionamento!\n");
        exit(1);
    }

    if(argc == 2){
           sscanf(argv[1],"%s",utente.nome);
    }else{
        printf("[INFO] Número de parâmetros incorreto! Sintaxe: ./cliente <nome>\n");
        exit(1);
    }
    printf("\n----------- MEDICALso -----------\n\n");

    printf("Bem-vindo '%s'\n\n",utente.nome);


    printf("Introduza os seus sintomas: \n");
    fflush(stdout);
    fgets(sintomas, sizeof(sintomas) - 1, stdin); // le o \n

    printf("\n\nSintomas: %s",sintomas);

    return 0;
}
