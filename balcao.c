#include "balcao.h"


int balcao_fifo_fd; //identificador do FIFO do balcao
int cliente_fifo_fd; //identificador do FIFO do cliente
int medico_FIFO_fd; //identificador do FIFO do medico


void encerra() {

    printf("\nA encerrar o Balcão...\n");

    for (int i = 3; i > 0; i--) {
        printf("%d\n", i);
        sleep(1);
    }
}

void trataSig(int i)
{
    (void) i;
    fprintf(stderr, "\n [INFO] Balcão interrompido via teclado!\n\n");
    //close(s_fifo_fd);
    unlink(BALCAO_FIFO);
    exit(EXIT_SUCCESS);
}

const char* classifica(char sintomas[]) {

    int estado;
    pid_t pid;
    size_t tam, tam1;

    //Criacao dos pipes anonimos
    int b2c[2], c2b[2];
    //char sintomas[MAX], sintomasTemp[MAX],especialidade[MAX];
    static char especialidade_Prioridade[MAX];

    pipe(b2c);
    pipe(c2b);
    //b2c[0] -> descritor da leitura
    //b2c[1] -> descritor da escrita

    //printf("\n---> Teste de classificação <---\n\n");

    //Criacao de processo filho -> Redirecionamento Balcao <-> Classificador
    pid = fork();

    if (pid < 0) {
        perror("Erro ao criar o filho!\n");
        exit(1);
    }


    if (pid > 0) { //Pai

        close(b2c[STDIN_FILENO]); //fecha canal de leitura
        close(c2b[STDOUT_FILENO]); //fecha canal de escrita


        //printf("Sintomas: ");
        //fflush(stdout);

        //fgets(sintomas, sizeof(sintomas) - 1, stdin); // le o \n


        tam = write(b2c[1], sintomas, strlen(sintomas));


        if (strcmp(sintomas, "#fim\n") == 0) {
            //  strcpy(sintomas, sintomasTemp); //para evitar ficar com "#fim" nos sintomas
            waitpid(pid, &estado, 0); // para evitar que o processo do classificador fique no estado "zombie"
            return "Classificação encerrada!\n";
        } //else
        //strcpy(sintomasTemp, sintomas);

        tam1 = read(c2b[0], especialidade_Prioridade, sizeof(especialidade_Prioridade) - 1);
        especialidade_Prioridade[tam1] = '\0';

        if (tam > 0)
            printf("\nEscrevi %zu bytes: %s", tam, sintomas);

        if (tam1 > 0)
            printf("Recebi %zu bytes: %s \n", tam1, especialidade_Prioridade);


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

    /*
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
     */

    return especialidade_Prioridade;
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

int comandos(){
    char comando[50];
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
        return 1;
    }  else {
        printf("[INFO] Comando Invalido!\n\n");
    }

    signal(SIGINT, trataSig);

    return 0;
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



void adicionarMedico(Balcao *b, pMedico listaMedicos, int id, Medico medico){
    if(b->numMedicos == b->maxMedicos){
        fprintf(stderr,"[INFO] LOTAÇÃO MÁXIMA DE MÉDICOS ATINGIDA\n");
        return;
    }

    listaMedicos[id] = medico;
    b->numMedicos++;
    fprintf(stdout,"[INFO] Medico '%s' adicionado ao sistema!\n",medico.nome);
}

void removerMedico(Balcao *b, pMedico medico, int id){
    if(b->numMedicos > 0) {
        b->numMedicos--;
        fprintf(stdout,"[INFO] Médico '%s' removido do sistema!\n",medico[id].nome);
    }

    //atribuirDadosMedico( "...", "...", -1);
}


void mostrarTodosMedicos(pBalcao b, pMedico medico){
    for(int i = 0; i < b->numMedicos;i++)
        mostrarDadosMedico(i,medico[i]);

}
void mostrarDadosMedico(int id, Medico medico){
    printf("\nMédico [%d]:\n\tNome: %s\n\tEspecialidade: %s\n\tEstado: %d\n\n",id,medico.nome,medico.especialidade,medico.estado);
}



void adicionarCliente(Balcao *b, pCliente listaUtentes, int id, Cliente cliente){


    if(b->numClientes == b->maxClientes){
        fprintf(stderr,"[INFO] LOTAÇÃO MÁXIMA DE CLIENTES ATINGIDA\n");
        return;
    }

    listaUtentes[id] = cliente;
    b->numClientes++;
    fprintf(stdout,"[INFO] Cliente '%s' adicionado ao sistema!\n",cliente.nome);
}

void removerCLiente(Balcao *b,pCliente utente, int id){

    if(b->numClientes > 0) {
        b->numClientes--;
        fprintf(stdout,"[INFO] Cliente '%s' removido do sistema!\n",utente[id].nome);
    }

    // utente[id] = atribuirDadosCliente("...","...","...",0,0,-1);


}

void mostrarDadosCliente(int id, Cliente utente){
    printf("\nCliente [%d]:\n\tNome: %s\n\tSintomas: %s\n\tEspecialidade: %s\n\tPrioridade: %d\n\tPosicaoEspera: %d\n\n",id,utente.nome,utente.sintomas,utente.areaEspecialidade,utente.prioridade,utente.posicaoListaEspera);
}

void mostrarTodosClientes(pBalcao b, pCliente utente){

    for(int i = 0; i < b->numClientes;i++)
        mostrarDadosCliente(i,utente[i]);

}

void sinal_vida(int s){


    alarm(20);
    fflush(stderr);
    fprintf(stderr,"hello there");
}


int main(int argc, char *argv[]) {


    int res;
    int maxCLientes, maxMedicos;

    char cliente_FIFO_fnome[30]; //nome do FIFO do cliente
    char medico_FIFO_fnome[30]; //nome do FIFO do cliente

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

    //verifica abertura de fifo
    balcao_fifo_fd = open(BALCAO_FIFO, O_RDWR);
    if (balcao_fifo_fd == -1)
    {
        perror("\nErro ao abrir o FIFO do servidor (RDWR/blocking)");
        exit(EXIT_FAILURE);
    }
    fprintf(stderr, "\nFIFO aberto para READ (+WRITE) bloqueante");


    if (signal(SIGINT, trataSig) == SIG_ERR)
    {
        perror("\nNão foi possível configurar o sinal SIGINT\n");
        exit(EXIT_FAILURE);
    }


    Balcao balcao;
    Cliente utente;
    Cliente listaUtentes[maxCLientes];
    Medico medico;
    Medico listaMedicos[maxMedicos];


    balcao = inicializarDadosBalcao(maxMedicos,maxCLientes);

    //Fifo
    Mensagem_cliente_para_Balcao mens_cli_balc;
    Mensagem_Balcao mens_Out;
    Mensagem_medico_para_Balcao mens_med_balc;

    //
    utente.id = 1;
    medico.id = 0;

    mens_Out.pid_balcao = getpid();


    int incU=0,incM=0;

    while (1) {
///////////////////////////////////Cliente///////////////////////////////////////////////////
       if(utente.id == 0) {
            res = read(balcao_fifo_fd, &utente, sizeof(utente));
            printf("TAMANHO : %ld\n",sizeof (utente));
            if (res < (int) sizeof(utente)) {
                fprintf(stderr, "[INFO] Mensagem recebida incompleta! [Bytes lidos: %d]\n", res);
                continue;
            }
            //printf("\nCliente [%d]:\n\tNome: %s\n\tSintomas: %s\n\tEspecialidade: %s\n\tPrioridade: %d\n\tPosicaoEspera: %d\n\n",0,utente.nome,utente.sintomas,utente.areaEspecialidade,utente.prioridade,utente.posicaoListaEspera);

            adicionarCliente(&balcao,listaUtentes,incU,utente);
            ++incU;

            strcpy(mens_Out.mensagem,"Utente inicializado");


            //* ---- OBTÉM FILENAME DO FIFO PARA A RESPOSTA ---- *//*
            sprintf(cliente_FIFO_fnome, CLIENTE_FIFO, utente.id);

            cliente_fifo_fd = open(cliente_FIFO_fnome, O_WRONLY);
            if (cliente_fifo_fd == -1)
                perror("[INFO] Erro na abertura do FIFO do cliente para escrita\n");
            else {
                res = write(cliente_fifo_fd, &mens_Out, sizeof (mens_Out));

                if (res < (int) sizeof(mens_Out)) {
                    fprintf(stderr, "[INFO] Mensagem enviada incompleta! [Bytes enviados: %d]\n", res);
                }
                fprintf(stdout, "[INFO] Mensagem enviada para o cliente com sucesso! [Bytes enviados: %d]\n",res);
                close(cliente_fifo_fd); //* FECHA LOGO O FIFO DO CLIENTE! *//*
                fprintf(stdout, "\n[INFO] FIFO do Cliente fechado\n");
            }

        }

/*
        // LER E ESCREVER MENSAGENS     CLIENTE <---> BALCAO

        res = read(balcao_fifo_fd, &mens_cli_balc, sizeof(mens_cli_balc));
        if (res < (int) sizeof(mens_cli_balc))
        {
            fprintf(stderr, "[INFO] Mensagem recebida incompleta! [Bytes lidos: %d]\n", res);
        }
        fprintf(stdout, "Mensagem recebida do cliente: [%s]\n", mens_cli_balc.mensagem);
        strcpy(mens_Out.mensagem, classifica(mens_cli_balc.mensagem));

        fprintf(stdout, "Especialidade recebida do classificador: %s\n",  mens_Out.mensagem );

        //* ---- OBTÉM FILENAME DO FIFO PARA A RESPOSTA ---- *//*
        sprintf(cliente_FIFO_fnome, CLIENTE_FIFO, mens_cli_balc.pid_cliente);

        //* ---- ABRE O FIFO do cliente p/ write ---- *//*
        cliente_fifo_fd = open(cliente_FIFO_fnome, O_WRONLY);
        if (cliente_fifo_fd == -1)
            perror("[INFO] Erro na abertura do FIFO do cliente para escrita\n");
        else
        {
            fprintf(stdout, "[INFO] FIFO cliente aberto para escrita\n");

            // ---- ENVIA RESPOSTA ----
            res = write(cliente_fifo_fd, &mens_Out, sizeof(mens_Out));
            if (res == sizeof(mens_Out))
                fprintf(stdout, "[INFO] Mensagem enviada para o cliente com sucesso!\n");
            else
                fprintf(stderr, "[INFO] Erro ao enviar a mensagem para o cliente!\n");

            close(cliente_fifo_fd); //* FECHA LOGO O FIFO DO CLIENTE! *//*
            fprintf(stdout, "\n[INFO] FIFO do Cliente fechado\n");
        }








*/




/////////////////////////////////////////Medico///////////////////////////////////////////////////
        if(medico.id == 0) {
            res = read(balcao_fifo_fd, &medico, sizeof(medico));
            printf("TAMANHO : %ld\n",sizeof (medico));
            if (res < (int) sizeof(medico)) {
                fprintf(stderr, "[INFO] Mensagem recebida incompleta! [Bytes lidos: %d]\n", res);
                continue;
            }
            //printf("\nCliente [%d]:\n\tNome: %s\n\tSintomas: %s\n\tEspecialidade: %s\n\tPrioridade: %d\n\tPosicaoEspera: %d\n\n",0,utente.nome,utente.sintomas,utente.areaEspecialidade,utente.prioridade,utente.posicaoListaEspera);

            adicionarMedico(&balcao,listaMedicos,incM,medico);
            ++incM;

            strcpy(mens_Out.mensagem,"Médico inicializado");


            // ---- OBTÉM FILENAME DO FIFO PARA A RESPOSTA ----
            sprintf(medico_FIFO_fnome, MEDICO_FIFO, medico.id);

            medico_FIFO_fd = open(medico_FIFO_fnome, O_WRONLY);
            if (medico_FIFO_fd == -1)
                perror("[INFO] Erro na abertura do FIFO do cliente para escrita\n");
            else {
                res = write(medico_FIFO_fd, &mens_Out, sizeof (mens_Out));

                if (res < (int) sizeof(mens_Out)) {
                    fprintf(stderr, "[INFO] Mensagem enviada incompleta! [Bytes enviados: %d]\n", res);
                }
                fprintf(stdout, "[INFO] Mensagem enviada para o médico com sucesso! [Bytes enviados: %d]\n",res);
                close(medico_FIFO_fd); // FECHA LOGO O FIFO DO MEDICO!
                fprintf(stdout, "\n[INFO] FIFO do cliente fechado\n");
            }

        }




/***/////////////////////////////////////////Medico//////////////////////////////////////////////////////////***/
/*        // LER E ESCREVER MENSAGENS     MEDICO <---> BALCAO

        res = read(balcao_fifo_fd, &mens_med_balc, sizeof(mens_med_balc));
        if (res < (int) sizeof(mens_med_balc))
        {
            fprintf(stderr, "[INFO] Mensagem recebida incompleta! [Bytes lidos: %d]\n", res);
        }
        fprintf(stdout, "Mensagem recebida do cliente: [%s]\n", mens_med_balc.mensagem);
        strcpy(mens_Out.mensagem, classifica(mens_med_balc.mensagem));

        fprintf(stdout, "Especialidade recebida do classificador: %s\n",  mens_Out.mensagem );


*/
/* ---- OBTÉM FILENAME DO FIFO PARA A RESPOSTA ---- *//*


        sprintf(medico_FIFO_fnome, MEDICO_FIFO, mens_med_balc.pid_medico);


*/
/*---- ABRE O FIFO do cliente p/ write ---- *//*


        cliente_fifo_fd = open(cliente_FIFO_fnome, O_WRONLY);
        if (cliente_fifo_fd == -1)
            perror("[INFO] Erro na abertura do FIFO do cliente para escrita\n");
        else
        {

        }


*/


        while(1){



            //signal(SIGALRM, sinal_vida);

            //sigaction(SIGALARM,)  usar em vez do signal

        }
        //break;


    }


    close(balcao_fifo_fd);
    unlink(BALCAO_FIFO);
    return 0;


}

