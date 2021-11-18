#include "balcao.h"
#include "cliente.h"
#include "medico.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX 256

void encerra() {

    printf("\nA encerrar o Balcão...\n");

    for (int i = 3; i > 0; i--) {
        printf("%d\n", i);
        sleep(1);
    }
}

void classifica() {

    pid_t pid;
    size_t tam, tam1;

    //Criacao de pipes anonimos
    int b2c[2], c2b[2];
    char sintomas[MAX], especialidade_Prioridade[MAX];
    pipe(b2c);
    pipe(c2b);
        //b2c[0] -> descritor da leitura
        //b2c[1] -> descritor da escrita

    printf("\n---> Teste de classificação <---\n\n");


    //Criacao de processo filho -> Redirecionamento Balcao <-> Classificador
    pid = fork();

    if (pid < 0) {
        perror("Erro ao criar o filho!\n");
        exit(1);
    }

    if (pid > 0) { //Pai
        close(b2c[0]);
        close(c2b[1]);

        while (1) {
            printf("Sintomas: ");
            fflush(stdout);
            fgets(sintomas, sizeof(sintomas) - 1, stdin);// le o \n

            if (strcmp(sintomas, "#fim\n") == 0)
                break;

            tam = write(b2c[1], sintomas, strlen(sintomas));
            tam1 = read(c2b[0], especialidade_Prioridade, sizeof(especialidade_Prioridade) - 1);

            if (tam > 0) {
                printf("\nEscrevi %zu bytes: %s", tam, sintomas);
            }
            if (tam1 > 0) {
                printf("Recebi %zu bytes: %s \n", tam1, especialidade_Prioridade);
            }
        }

        close(b2c[1]);
        close(c2b[0]);
    }

    if (pid == 0) { //Filho

        close(0);
        dup(b2c[0]);
        close(b2c[0]);
        close(b2c[1]);
        close(1);
        dup(c2b[1]);
        close(c2b[0]);
        close(c2b[1]);

        execl("./classificador", "classificador", NULL);
    }

    printf("Especialidade: %s\n", especialidade_Prioridade);
    printf("---> Fim da classificação <---\n\n");
}


void help() {
    printf("\n-------Comandos-------\n\n");
    printf("-> utentes\n");
    printf("-> especilalistas\n");
    printf("-> delut x\n");
    printf("-> delesp x\n");
    printf("-> freq N\n");
    printf("-> encerra\n");
    printf("-> help\n");
    printf("----------------------\n\n");

}


Balcao inicializarStructBalcao(int MAXMEDICOS, int MAXCLIENTES){

    Balcao b;
    b.maxMedicos = MAXMEDICOS;
    b.maxClientes = MAXCLIENTES;
    for(int i = 0; i < 5; i++){
        b.filaEspera[i] = 0;
    }
    b.numClientes = 0;
    b.numMedicos = 0;

    return b;

}


int main(int argc, char *argv[]) {


    char comando[50];
    int MAXCLIENTES, MAXMEDICOS;


    sscanf(getenv("MAXCLIENTES"), "%d", &MAXCLIENTES);
    sscanf(getenv("MAXMEDICOS"), "%d", &MAXMEDICOS);

    Cliente listaUtentes[MAXCLIENTES];
    Medico listaMedicos[MAXMEDICOS];
    Balcao balcao;

    printf("MAXCLIENTES: %d\n", MAXCLIENTES);
    printf("MAXMEDICOS: %d\n", MAXMEDICOS);

    balcao = inicializarStructBalcao(MAXMEDICOS,MAXCLIENTES);

    classifica();

    while (1) {

        fgets(comando, sizeof(comando) - 1, stdin);// le o \n

        if (strcmp(comando, "utentes\n") == 0) {
            printf("Funcionalidade por implementar...\n");
        } else if (strcmp(comando, "especialistas\n") == 0) {
            printf("Funcionalidade por implementar...\n");
        } else if (strcmp(comando, "delut\n") == 0) {
            printf("Funcionalidade por implementar...\n");
        } else if (strcmp(comando, "delesp\n") == 0) {
            printf("Funcionalidade por implementar...\n");
        } else if (strcmp(comando, "freq\n") == 0) {
            printf("Funcionalidade por implementar...\n");
        } else if (strcmp(comando, "help\n") == 0) {
            help();
        } else if (strcmp(comando, "encerra\n") == 0) {
            encerra();
            return 0;
        }

    }


}
