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

    int estado;
    pid_t pid;
    size_t tam, tam1;

    //Criacao dos pipes anonimos
    int b2c[2], c2b[2];
    char sintomas[MAX], sintomasTemp[MAX],  especialidade_Prioridade[MAX], especialidade[MAX];
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
            fgets(sintomas, sizeof(sintomas) - 1, stdin); // le o \n

            if (strcmp(sintomas, "#fim\n") == 0){
                strcpy(sintomas,sintomasTemp); //para evitar ficar com "#fim" nos sintomas
                break;
            }else
                strcpy(sintomasTemp,sintomas);


            tam = write(b2c[1], sintomas, strlen(sintomas));
            tam1 = read(c2b[0], especialidade_Prioridade, sizeof(especialidade_Prioridade) - 1);

            if (tam > 0)
                printf("\nEscrevi %zu bytes: %s", tam, sintomas);

            if (tam1 > 0)
                printf("Recebi %zu bytes: %s \n", tam1, especialidade_Prioridade);

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
    waitpid(pid, &estado, 0); // para evitar que o processo do classificador fique no estado "zombie"


    //Remover o inteiro da string
    for(int i=0; i < strlen(especialidade_Prioridade) - 2; i++)
        especialidade[i] = especialidade_Prioridade[i];

    //Converter a prioridade para um inteiro
    char priorid = especialidade_Prioridade[strlen(especialidade_Prioridade)-2];
    int prioridade = priorid - '0';

    printf("\nClassificação Obtida: \n\n");
    printf("  Sintomas: %s", sintomas);
    printf("  Especialidade: %s\n", especialidade);
    printf("  Prioridade: %d\n\n", prioridade);
    printf("-----> Fim da classificação <-----\n\n");
}


void help() {
    printf("\nComandos disponíveis:\n\n");
    printf("-> utentes\n");
    printf("-> especilalistas\n");
    printf("-> delut x\n");
    printf("-> delesp x\n");
    printf("-> freq N\n");
    printf("-> encerra\n");
    printf("-> help\n\n");
}


Balcao inicializarDadosBalcao(int MAXMEDICOS, int MAXCLIENTES){

    Balcao b;

    b.maxMedicos = MAXMEDICOS;
    b.maxClientes = MAXCLIENTES;
    for(int i = 0; i < MAXESPECIALIDADES; i++){
        b.filaEspera[i] = 0;
        b.numEspecialistas[i] = 0;
    }
    b.numClientes = 0;
    b.numMedicos = 0;


    return b;
}


int main(int argc, char *argv[]) {


    char comando[50];
    int maxCLientes, maxMedicos;

    if(getenv("MAXCLIENTES") != NULL && getenv("MAXMEDICOS") != NULL) {

        //Receber as variáveis de ambiente
        sscanf(getenv("MAXCLIENTES"), "%d", &maxCLientes);
        sscanf(getenv("MAXMEDICOS"), "%d", &maxMedicos);

        printf("Variáveis de ambiente definidas:\n");
        printf("MAXCLIENTES: %d\n", maxCLientes);
        printf("MAXMEDICOS: %d\n", maxMedicos);

    }else {
        fprintf(stderr,"As variáveis de ambiente não foram definidas!\n");
        exit(1);
    }


    Cliente listaUtentes[maxCLientes];
    Medico listaMedicos[maxMedicos];
    Balcao balcao;

    balcao = inicializarDadosBalcao(maxMedicos,maxCLientes);

    classifica();

    while (1) {


        printf("Comando: ");
        fflush(stdout);
        fgets(comando, sizeof(comando) , stdin);// le o \n
        // printf("O administrador introduziu o comando: %s", comando);

        if (strcmp(comando, "utentes\n") == 0) {
            printf("\nFuncionalidade por implementar...\n\n");
        } else if (strcmp(comando, "especialistas\n") == 0) {
            printf("\nFuncionalidade por implementar...\n\n");
        } else if (strcmp(comando, "delut\n") == 0) {
            printf("\nFuncionalidade por implementar...\n\n");
        } else if (strcmp(comando, "delesp\n") == 0) {
            printf("\nFuncionalidade por implementar...\n\n");
        } else if (strcmp(comando, "freq\n") == 0) {
            printf("\nFuncionalidade por implementar...\n\n");
        } else if (strcmp(comando, "help\n") == 0) {
            help();
        } else if (strcmp(comando, "encerra\n") == 0) {
            encerra();
            return 0;
        }  else {
            printf("[INFO] Comando Invalido!\n\n");
        }

    }
}
