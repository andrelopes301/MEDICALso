#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#include "balcao.h"
#include "cliente.h"


Cliente atribuirDadosCliente(char *nome,char *sintomas,char *especialidade,int prioridade,int listaEspera,int pidC){

    Cliente cli;

    strcpy(cli.nome,nome);
    cli.id = pidC;
    strcpy(cli.sintomas,sintomas);
    strcpy(cli.areaEspecialidade,especialidade);
    cli.prioridade = prioridade;
    cli.posicaoListaEspera = listaEspera;

    return cli;
}




int main(int argc, char *argv[]){

    int balcao_fifo_fd; //identificador do FIFO do balcao
    int cliente_fifo_fd; //identificador do FIFO do cliente
    int medico_fifo_fd; //identificador do FIFO do cliente

    char cliente_FIFO_fnome[30]; //nome do FIFO do cliente
    char medico_FIFO_fnome[30]; //nome do FIFO do cliente

    Mensagem_Balcao mens_balc_cli;
    Mensagem_utilizador_para_Balcao mens_cli_balc;
    Consulta consulta;
    int read_res;


    int nfd; //valor retorno do select
    fd_set read_fds;
    struct timeval tv; //timeout para o select


    char sintomas[MAX];
    int pid_Cliente;
    pid_Cliente = getpid();


    //verificar se existe um Balcão em funcionamento
    if(access(BALCAO_FIFO, F_OK) != 0){
        printf("[INFO] Balcão não está em funcionamento!\n");
        exit(1);
    }

    if(argc != 2){
        printf("[INFO] Número de parâmetros incorreto! Sintaxe: ./cliente <nome>\n");
        exit(1);
    }
    printf("Nome: %s",argv[1]);


    //Criar FIFO deste cliente
    mens_cli_balc.pid = pid_Cliente;
    mens_cli_balc.medico_cliente = 1;
    mens_cli_balc.atendido = 0;
    strcpy(mens_cli_balc.nome,argv[1]);

    printf("TAMANHO : %ld\n",sizeof (mens_cli_balc));


    sprintf(cliente_FIFO_fnome, CLIENTE_FIFO, mens_cli_balc.pid);
    if(mkfifo(cliente_FIFO_fnome,0777) == -1){
        fprintf(stderr, "\n[INFO] Erro na criação do FIFO do cliente!\n");
        exit(EXIT_FAILURE);
    }
    fprintf(stderr, "[INFO] FIFO do cliente [%d] criado com sucesso!\n",mens_cli_balc.pid);


    //Abrir o FIFO do balcão para escrita
    balcao_fifo_fd = open(BALCAO_FIFO,O_WRONLY); // bloqueante
    if(balcao_fifo_fd == -1){
        fprintf(stderr, "[INFO] Balcão não está em funcionamento!\n");
        unlink(cliente_FIFO_fnome);
        exit(EXIT_FAILURE);
    }

    fprintf(stderr, "[INFO] FIFO do Balcão aberto para escrita\n");


    //Abrir o FIFO do cliente para escrita/leitura
    cliente_fifo_fd = open(cliente_FIFO_fnome,O_RDWR); // bloqueante
    if(cliente_fifo_fd == -1){
        fprintf(stderr, "[INFO]  Erro ao abrir FIFO do cliente!\n");
       // close(balcao_fifo_fd);
        unlink(cliente_FIFO_fnome);
        exit(EXIT_FAILURE);
    }
    fprintf(stderr, "[INFO] FIFO do cliente aberto para leitura e escrita\n");





    printf("\n----------- MEDICALso -----------\n\n");
    printf("Bem-vindo '%s'\n\n", argv[1]);


    while(1){

        printf("Introduza os seus sintomas: \n");
        fgets(mens_cli_balc.mensagem, sizeof(mens_cli_balc.mensagem) - 1, stdin);

        int tam = strlen(mens_cli_balc.mensagem);
        mens_cli_balc.mensagem[tam] = '\0';


        //Envia dados do cliente com os sintomas
        write(balcao_fifo_fd, &mens_cli_balc, sizeof(mens_cli_balc));

        //Recebe especialidade atribuida pelo Balcao
        read_res = read(cliente_fifo_fd, &mens_balc_cli, sizeof (mens_balc_cli));

        if(read_res == sizeof (mens_balc_cli))
            fprintf(stdout, "Especialidade atribuída: %s\n", mens_balc_cli.mensagem);
        else
            fprintf(stderr, "[INFO] Mensagem recebida incompleta! [Bytes lidos: %d]\n", read_res);

        if (strcmp(mens_balc_cli.mensagem, "Classificação encerrada!\n") == 0) {
            break;
        }
    }

    printf("Espere pelo seu medico !\n");


    //Receber o medico atribuido pelo balcao
    read_res = read(cliente_fifo_fd, &mens_balc_cli, sizeof (mens_balc_cli));
    if(read_res == sizeof (mens_balc_cli))
        fprintf(stdout, "Medico atribuído: [%d]\n", mens_balc_cli.pid_medico);
    else
        fprintf(stderr, "[INFO] Mensagem recebida incompleta! [Bytes lidos: %d]\n", read_res);



    sprintf(medico_FIFO_fnome, MEDICO_FIFO, mens_balc_cli.pid_medico);
    //Abrir o FIFO do medico para escrita/leitura
    medico_fifo_fd = open(medico_FIFO_fnome,O_RDWR); // bloqueante
    if(medico_fifo_fd == -1){
        fprintf(stderr, "[INFO]  Erro ao abrir FIFO do medico!\n");
        //close(balcao_fifo_fd);
        unlink(medico_FIFO_fnome);
        exit(EXIT_FAILURE);
    }
    fprintf(stderr, "[INFO] FIFO do medico aberto para leitura e escrita\n");



    while(1){


        tv.tv_sec = 10;
        tv.tv_usec = 0;

        FD_ZERO(&read_fds);
        FD_SET(0, &read_fds);
        FD_SET(cliente_fifo_fd, &read_fds);

        nfd = select(cliente_fifo_fd + 1, &read_fds, NULL, NULL, &tv);

        if (nfd == 0) {
            //  printf("\n[INFO] À espera...\n");
            fflush(stdout);
            continue;
        }

        if (nfd == -1) {
            perror("\n[INFO] Erro na criação do select!\n");
            close(cliente_fifo_fd);
            //close(medico_fifo_fd);
            unlink(BALCAO_FIFO);
            exit(1);
        }



        if (FD_ISSET(0, &read_fds)) {

            //ENVIAR MENSAGENS PARA O MEDICO

            char mensagem[255];
            fgets(mensagem, sizeof(mensagem), stdin);// le o \n
            strcpy(consulta.mensagem, mensagem);

            //Envia mensagem para o medico
            read_res = write(medico_fifo_fd, &consulta, sizeof(consulta));

            if(read_res == sizeof (consulta))
                fprintf(stdout, "Mensagem Enviada: %s\n", consulta.mensagem);
            else
                fprintf(stderr, "[INFO] Mensagem enviada incompleta! [Bytes lidos: %d]\n", read_res);
        }


        if (FD_ISSET(cliente_fifo_fd, &read_fds)) {

            //RECEBER MENSAGENS DO MEDICO
            read_res = read(cliente_fifo_fd, &consulta, sizeof(consulta));

            if(read_res == sizeof (consulta))
                fprintf(stdout, "Mensagem Recebida: %s\n", consulta.mensagem);
            else
                fprintf(stderr, "[INFO] Mensagem enviada incompleta! [Bytes lidos: %d]\n", read_res);

        }



/*
        char c[30];
        printf("Espere pelo seu medico !\n");
        scanf("%s", c);

        if(strcmp(c,"fim") == 0)
            break;
*/


    }


    close(cliente_fifo_fd);
    close(balcao_fifo_fd);
    unlink(cliente_FIFO_fnome);

    return 0;
}
