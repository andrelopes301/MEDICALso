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
    int cliente_fifo_fd; //identificador do FIFO do cliente

    Mensagem_Balcao mens_balc_med;
    Mensagem_utilizador_para_Balcao mens_med_balc;
    Consulta consulta;

    int nfd; //valor retorno do select
    fd_set read_fds;
    struct timeval tv; //timeout para o select

    char medico_FIFO_fnome[30]; //nome do FIFO do cliente
    char cliente_FIFO_fnome[30]; //nome do FIFO do cliente

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



    //Criar FIFO deste medico
    mens_med_balc.pid = pid_Medico;
    mens_med_balc.medico_cliente = 0;
    mens_med_balc.atendido = 0;
    medico.estado = 0;
    strcpy(mens_med_balc.nome,medico.nome);
    strcpy(mens_med_balc.mensagem,medico.especialidade);

    sprintf(medico_FIFO_fnome, MEDICO_FIFO, mens_med_balc.pid);
    if(mkfifo(medico_FIFO_fnome,0777) == -1){
        fprintf(stderr, "\n[INFO] Erro na criação do FIFO do medico!\n");
        exit(EXIT_FAILURE);
    }
    fprintf(stderr, "[INFO] FIFO do medico [%d] criado com sucesso!\n",mens_med_balc.pid);

    printf("\nTAMANHO: %ld\n",sizeof (mens_med_balc));



    //Abrir o FIFO do balcão para escrita

    balcao_fifo_fd = open(BALCAO_FIFO,O_WRONLY); // bloqueante
    if(balcao_fifo_fd == -1){
        fprintf(stderr, "[INFO] Balcão não está em funcionamento!\n");
        unlink(medico_FIFO_fnome);
        exit(EXIT_FAILURE);
    }
    fprintf(stderr, "[INFO] FIFO do Balcão aberto para escrita\n");


    //enviar dados do medico
    write(balcao_fifo_fd, &mens_med_balc, sizeof(mens_med_balc));

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

    printf("Aguarde por um cliente!\n");


    memset(&mens_balc_med, 0, sizeof(mens_balc_med));


    //Receber o cliente atribuido pelo balcao
    read_res = read(medico_fifo_fd, &mens_balc_med, sizeof (mens_balc_med));
    if(read_res == sizeof (mens_balc_med))
        fprintf(stdout, "Cliente atribuído: [%d]\n", mens_balc_med.pid_cliente);
    else
        fprintf(stderr, "[INFO] Mensagem recebida incompleta! [Bytes lidos: %d]\n", read_res);



    sprintf(cliente_FIFO_fnome, CLIENTE_FIFO, mens_balc_med.pid_cliente);


    //Abrir o FIFO do cliente para escrita/leitura
    cliente_fifo_fd = open(cliente_FIFO_fnome,O_RDWR); // bloqueante

    if(cliente_fifo_fd == -1){
        fprintf(stderr, "[INFO]  Erro ao abrir FIFO do cliente!\n");
        //close(balcao_fifo_fd);
        unlink(cliente_FIFO_fnome);
        exit(EXIT_FAILURE);
    }

    fprintf(stderr, "[INFO] FIFO do cliente aberto para leitura e escrita\n");

    printf("Espere pelo seu cliente !\n");


    while(1) {


        tv.tv_sec = 10;
        tv.tv_usec = 0;

        FD_ZERO(&read_fds);
        FD_SET(0, &read_fds);
        FD_SET(medico_fifo_fd, &read_fds);

        nfd = select(medico_fifo_fd + 1, &read_fds, NULL, NULL, &tv);

        if (nfd == 0) {
            //  printf("\n[INFO] À espera...\n");
            fflush(stdout);
            continue;
        }

        if (nfd == -1) {
            perror("\n[INFO] Erro na criação do select!\n");
            close(medico_fifo_fd);
            //close(medico_fifo_fd);
            unlink(BALCAO_FIFO);
            exit(1);
        }


        if (FD_ISSET(0, &read_fds)) {

            //ENVIAR MENSAGENS PARA O MEDICO

            char mensagem[255];
            fgets(mensagem, sizeof(mensagem), stdin);// le o \n
            strcpy(consulta.mensagem, mensagem);

            //Envia mensagem para o cliente
            read_res = write(cliente_fifo_fd, &consulta, sizeof(consulta));

            if (read_res == sizeof(consulta))
                fprintf(stdout, "Mensagem Enviada: %s\n", consulta.mensagem);
            else
                fprintf(stderr, "[INFO] Mensagem enviada incompleta! [Bytes lidos: %d]\n", read_res);
        }


        if (FD_ISSET(medico_fifo_fd, &read_fds)) {

            //RECEBER MENSAGENS DO MEDICO
            read_res = read(medico_fifo_fd, &consulta, sizeof(consulta));

            if (read_res == sizeof(consulta))
                fprintf(stdout, "Mensagem Recebida: %s\n", consulta.mensagem);
            else
                fprintf(stderr, "[INFO] Mensagem enviada incompleta! [Bytes lidos: %d]\n", read_res);
        }

    }




    /*
    while(1){

        char c[30];
        printf("Espere por um cliente!\n");
        scanf("%s", c);

        if(strcmp(c,"fim") == 0)
            break;

        //envia sinal de vida de 20 em 20 segundos
        //while(1) {
            //sleep(4);

            //alarm(5);
            //kill(mens_balc_med.pid_balcao,SIGALRM);

            //sigqueue()
    }*/


    close(medico_fifo_fd);
    close(balcao_fifo_fd);
    unlink(medico_FIFO_fnome);

    return 0;
}


