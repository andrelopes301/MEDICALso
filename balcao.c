#include "balcao.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>



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
    char sintomas[MAX], sintomasTemp[MAX], especialidade_Prioridade[MAX], especialidade[MAX];
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

        close(b2c[STDIN_FILENO]); //fecha canal de leitura
        close(c2b[STDOUT_FILENO]); //fecha canal de escrita

        while (1) {
            printf("Sintomas: ");
            fflush(stdout);
            fgets(sintomas, sizeof(sintomas) - 1, stdin); // le o \n
            tam = write(b2c[1], sintomas, strlen(sintomas));


            if (strcmp(sintomas, "#fim\n") == 0) {
                strcpy(sintomas, sintomasTemp); //para evitar ficar com "#fim" nos sintomas
                waitpid(pid, &estado, 0); // para evitar que o processo do classificador fique no estado "zombie"
                break;
            } else
                strcpy(sintomasTemp, sintomas);

            tam1 = read(c2b[0], especialidade_Prioridade, sizeof(especialidade_Prioridade) - 1);
            especialidade_Prioridade[tam1] = '\0';

            if (tam > 0)
                printf("\nEscrevi %zu bytes: %s", tam, sintomas);

            if (tam1 > 0)
                printf("Recebi %zu bytes: %s \n", tam1, especialidade_Prioridade);

        }
        //Fechar canais
        close(b2c[STDOUT_FILENO]);
        close(c2b[STDIN_FILENO]);
    }

    if (pid == 0) { //Filho

        close(STDIN_FILENO);   //libertar stdin
        dup(b2c[STDIN_FILENO]); //duplica extremidade de leitura para o lugar libertado
        close(b2c[STDIN_FILENO]); //fecha extremidade de leitura pk ja foi duplicada
        close(b2c[STDOUT_FILENO]); //fecha extremidade de escrita
        close(STDOUT_FILENO);  //libertar stdout
        dup(c2b[STDOUT_FILENO]); //duplica extremidade de escrita para o lugar libertado
        close(c2b[STDOUT_FILENO]); //fecha extremidade de escrita pk ja foi duplicada
        close(c2b[STDIN_FILENO]); //fecha extremidade de leitura


        execl("./classificador", "classificador", NULL);
    }


    //Remover o inteiro da string
    int i = 0;
    while (especialidade_Prioridade[i] != ' ') {
        especialidade[i] = especialidade_Prioridade[i];
        i++;
    }
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

Medico atribuirDadosMedico( char *nome, char *especialidade,int pidM){

    Medico medico;

    strcpy(medico.nome,nome);
    strcpy(medico.especialidade,especialidade);
    medico.id = pidM;
    medico.estado = 0;

    return medico;
}

void adicionarMedico(Balcao *b, pMedico medico, int id, char *nome,char *especialidade,int pidM){
    if(b->numMedicos == b->maxMedicos){
        fprintf(stderr,"[INFO] LOTAÇÃO MÁXIMA DE MÉDICOS ATINGIDA\n");
        return;
    }

    medico[id] = atribuirDadosMedico(nome,especialidade,pidM);
    b->numMedicos++;
    fprintf(stdout,"[INFO] Cliente '%s' adicionado ao sistema!\n",nome);
}

void removerMedico(Balcao *b, pMedico medico, int id){
    if(b->numMedicos > 0) {
        b->numMedicos--;
        fprintf(stdout,"[INFO] Médico '%s' removido do sistema!\n",medico[id].nome);
    }

    atribuirDadosMedico( "...", "...", -1);
}

void mostrarDadosMedico(int id, Medico medico){

    printf("\nMédico [%d]:\n\tNome: %s\n\tEspecialidade: %s\n\tEstado: %d\n\n",id,medico.nome,medico.especialidade,medico.estado);

}


void mostrarTodosMedicos(pBalcao b, pMedico medico){

    for(int i = 0; i < b->numMedicos;i++)
        mostrarDadosMedico(i,medico[i]);

}

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

void adicionarCliente(Balcao *b, pCliente utente, int id, char *nome,char *sintomas,char *especialidade,int prioridade,int listaEspera,int pidC){


    if(b->numClientes == b->maxClientes){
        fprintf(stderr,"[INFO] LOTAÇÃO MÁXIMA DE CLIENTES ATINGIDA\n");
        return;
    }

    utente[id] = atribuirDadosCliente(nome,sintomas,especialidade,prioridade,listaEspera,pidC);
    b->numClientes++;
    fprintf(stdout,"[INFO] Cliente '%s' adicionado ao sistema!\n",nome);
}

void removerCLiente(Balcao *b,pCliente utente, int id){

    if(b->numClientes > 0) {
        b->numClientes--;
        fprintf(stdout,"[INFO] Cliente '%s' removido do sistema!\n",utente[id].nome);
    }

    utente[id] = atribuirDadosCliente("...","...","...",0,0,-1);


}

void mostrarDadosCliente(int id, Cliente utente){
    printf("\nCliente [%d]:\n\tNome: %s\n\tSintomas: %s\n\tEspecialidade: %s\n\tPrioridade: %d\n\tPosicaoEspera: %d\n\n",id,utente.nome,utente.sintomas,utente.areaEspecialidade,utente.prioridade,utente.posicaoListaEspera);
}

void mostrarTodosClientes(pBalcao b, pCliente utente){

    for(int i = 0; i < b->numClientes;i++)
        mostrarDadosCliente(i,utente[i]);

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


    //verifica se ja existe algum balcao em funcionamento
    if(access(BALCAO_FIFO, F_OK) == 0){
        printf("[INFO] Já existe um Balcão em funcionamento!\n");
        exit(2);
    }

    mkfifo(BALCAO_FIFO, 0600);
    printf("[INFO] Criei o FIFO do Balcão...\n");



    Balcao balcao;
    Cliente listaUtentes[maxCLientes];
    Medico listaMedicos[maxMedicos];

    balcao = inicializarDadosBalcao(maxMedicos,maxCLientes);

    adicionarCliente(&balcao,listaUtentes,0,"Andre","dor barriga","estomatologia",1,0,9034);
    mostrarTodosClientes(&balcao,listaUtentes);






    //classifica();

    while (1) {

        printf("Comando: ");
        fflush(stdout);
        fgets(comando, sizeof(comando) , stdin);// le o \n
        // printf("O administrador introduziu o comando: %s", comando);

        if (strcmp(comando, "utentes\n") == 0) {
            printf("[INFO] Funcionalidade por implementar...\n\n");
        } else if (strcmp(comando, "especialistas\n") == 0) {
            printf("[INFO] Funcionalidade por implementar...\n\n");
        } else if (strcmp(comando, "delut\n") == 0) {
            printf("[INFO] Funcionalidade por implementar...\n\n");
        } else if (strcmp(comando, "delesp\n") == 0) {
            printf("[INFO] Funcionalidade por implementar...\n\n");
        } else if (strcmp(comando, "freq\n") == 0) {
            printf("[INFO] Funcionalidade por implementar...\n\n");
        } else if (strcmp(comando, "help\n") == 0) {
            help();
        } else if (strcmp(comando, "encerra\n") == 0) {
            encerra();
            break;
        }  else {
            printf("[INFO] Comando Invalido!\n\n");
        }

    }

    unlink(BALCAO_FIFO);
    return 0;


}
