#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include "balcao.h"
#include "medico.h"


Medico atribuirDadosMedico( char *nome, char *especialidade,int pidM){

    Medico medico;

    strcpy(medico.nome,nome);
    strcpy(medico.especialidade,especialidade);
    medico.id = pidM;
    medico.estado = 0;

    return medico;
}






int main(int argc, char *argv[]){

    int balcao_fifo_fd; //identificador do FIFO do balcao
    int medico_fifo_fd; //identificador do FIFO do medico
    Mensagem_Balcao mens_balc_med;
    Mensagem_medico_para_Balcao mens_med_balc;
    char medico_FIFO_fnome[30]; //nome do FIFO do cliente
    int read_res, pid_Medico;

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
    pid_Medico = getpid();



    medico = atribuirDadosMedico(argv[1],argv[2],pid_Medico);



    //Criar FIFO deste medico
    mens_med_balc.pid_medico = pid_Medico;
    sprintf(medico_FIFO_fnome, MEDICO_FIFO, mens_med_balc.pid_medico);
    if(mkfifo(medico_FIFO_fnome,0777) == -1){
        fprintf(stderr, "\n[INFO] Erro na criação do FIFO do medico!\n");
        exit(EXIT_FAILURE);
    }
    fprintf(stderr, "[INFO] FIFO do medico [%d] criado com sucesso!\n",mens_med_balc.pid_medico);

    printf("\nTAMANHO: %ld\n",sizeof (medico));



    //Abrir o FIFO do balcão para escrita

    balcao_fifo_fd = open(BALCAO_FIFO,O_WRONLY); // bloqueante
    if(balcao_fifo_fd == -1){
        fprintf(stderr, "[INFO] Balcão não está em funcionamento!\n");
        unlink(medico_FIFO_fnome);
        exit(EXIT_FAILURE);
    }
    fprintf(stderr, "[INFO] FIFO do Balcão aberto para escrita\n");



    //envia estrutura do medico
    write(balcao_fifo_fd, &medico, sizeof(medico));



    //Abrir o FIFO do medico para escrita/leitura

    medico_fifo_fd = open(medico_FIFO_fnome,O_RDWR); // bloqueante
    if(medico_fifo_fd == -1){
        fprintf(stderr, "[INFO]  Erro ao abrir FIFO do Medico!\n");
        close(balcao_fifo_fd);
        unlink(medico_FIFO_fnome);
        exit(EXIT_FAILURE);
    }
    fprintf(stderr, "[INFO] FIFO do Medico aberto para leitura e escrita\n");

    read_res = read(medico_fifo_fd, &mens_balc_med, sizeof (mens_balc_med));
    if(read_res == sizeof (mens_balc_med))
        fprintf(stdout, "%s\n", mens_balc_med.mensagem);
    else
        fprintf(stderr, "[INFO] Mensagem recebida incompleta! [Bytes lidos: %d]\n", read_res);



    printf("\n----------- MEDICALso -----------\n\n");
    printf("Bem-vindo '%s'\n\n",medico.nome);


    printf("Médico [%d]:\n\tNome: %s\n\tEspecialidade: %s\n\tEstado: %d\n\tPid: %d\n\n",0,medico.nome,medico.especialidade,medico.estado,medico.id);

    while(1){

        //envia sinal de vida de 20 em 20 segundos
        while(1) {
            //sleep(4);

            alarm(5);
            kill(mens_balc_med.pid_balcao,SIGALRM);

            //sigqueue()
        }
        break;
    }

    return 0;
}


