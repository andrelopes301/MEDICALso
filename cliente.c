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
    Mensagem_Balcao mens_balc_cli;
    Mensagem_cliente_para_Balcao mens_cli_balc;
    char cliente_FIFO_fnome[30]; //nome do FIFO do cliente
    int read_res;


    Cliente utente;
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


    utente = atribuirDadosCliente(argv[1],"","",0,0, pid_Cliente);
    printf("\nCliente [%d]:\n\tNome: %s\n\tSintomas: %s\n\tEspecialidade: %s\n\tPrioridade: %d\n\tPosicaoEspera: %d\n\n",0,utente.nome,utente.sintomas,utente.areaEspecialidade,utente.prioridade,utente.posicaoListaEspera);

    printf("TAMANHO : %ld\n",sizeof (utente));


    //Criar FIFO deste cliente
    mens_cli_balc.pid_cliente = pid_Cliente;
    sprintf(cliente_FIFO_fnome, CLIENTE_FIFO, mens_cli_balc.pid_cliente);
    if(mkfifo(cliente_FIFO_fnome,0777) == -1){
        fprintf(stderr, "\n[INFO] Erro na criação do FIFO do cliente!\n");
        exit(EXIT_FAILURE);
    }
    fprintf(stderr, "[INFO] FIFO do cliente [%d] criado com sucesso!\n",mens_cli_balc.pid_cliente);

    //Abrir o FIFO do balcão para escrita

    balcao_fifo_fd = open(BALCAO_FIFO,O_WRONLY); // bloqueante
    if(balcao_fifo_fd == -1){
        fprintf(stderr, "[INFO] Balcão não está em funcionamento!\n");
        unlink(cliente_FIFO_fnome);
        exit(EXIT_FAILURE);
    }
    fprintf(stderr, "[INFO] FIFO do Balcão aberto para escrita\n");



    //envia estrutura do cliente
    write(balcao_fifo_fd, &utente, sizeof(utente));


    //Abrir o FIFO do cliente para escrita/leitura

    cliente_fifo_fd = open(cliente_FIFO_fnome,O_RDWR); // bloqueante
    if(cliente_fifo_fd == -1){
        fprintf(stderr, "[INFO]  Erro ao abrir FIFO do cliente!\n");
        close(balcao_fifo_fd);
        unlink(cliente_FIFO_fnome);
        exit(EXIT_FAILURE);
    }
    fprintf(stderr, "[INFO] FIFO do cliente aberto para leitura e escrita\n");



    read_res = read(cliente_fifo_fd, &mens_balc_cli, sizeof (mens_balc_cli));
    if(read_res == sizeof (mens_balc_cli))
        fprintf(stdout, "%s\n", mens_balc_cli.mensagem);
    else
        fprintf(stderr, "[INFO] Mensagem recebida incompleta! [Bytes lidos: %d]\n", read_res);




    printf("\n----------- MEDICALso -----------\n\n");
    printf("Bem-vindo '%s'\n\n",utente.nome);




    while(1){

        printf("Introduza os seus sintomas: \n");
        fgets(mens_cli_balc.mensagem, sizeof(mens_cli_balc.mensagem) - 1, stdin);


        int tam = strlen(mens_cli_balc.mensagem);
        mens_cli_balc.mensagem[tam] = '\0';


        //Envia os sintomas para o Balcao
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



    while(1){

        char *c;
        printf("Espere pelo seu medico !\n");
        scanf("%s", c);

        if(strcmp(c,"fim"))
            break;
    }


    close(cliente_fifo_fd);
    close(balcao_fifo_fd);
    unlink(cliente_FIFO_fnome);

    return 0;
}
