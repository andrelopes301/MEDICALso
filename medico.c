#include "balcao.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>



int main(int argc, char *argv[]){


    Medico medico;

    //verificar se existe um Balcão em funcionamento
    if(access(BALCAO_FIFO, F_OK) != 0){
        printf("[INFO] Balcão não está em funcionamento!\n");
        exit(1);
    }

    if(argc == 3){
        sscanf(argv[1],"%s",medico.nome);
        sscanf(argv[2],"%s",medico.especialidade);
    }else{
        printf("[INFO] Número de parâmetros incorreto! Sintaxe: ./medico <nome> <especialidade>\n");
        exit(1);
    }
    medico.id = getpid();
    medico.estado = 0;

    printf("\n----------- MEDICALso -----------\n\n");
    printf("Bem-vindo '%s'\n\n",medico.nome);


    printf("Médico [%d]:\n\tNome: %s\n\tEspecialidade: %s\n\tEstado: %d\n\tPid: %d\n\n",0,medico.nome,medico.especialidade,medico.estado,medico.id);



    return 0;
}
