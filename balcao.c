#include "balcao.h"


int balcao_fifo_fd; //identificador do FIFO do balcao
int cliente_fifo_fd; //identificador do FIFO do cliente
int medico_fifo_fd; //identificador do FIFO do medico


void encerra() {

    printf("\nA encerrar o Balcão...\n");

    for (int i = 3; i > 0; i--) {
        printf("%d\n", i);
        sleep(1);
    }
}

void trataSig(int i) {
    (void) i;
    fprintf(stderr, "\n [INFO] Balcão interrompido via teclado!\n\n");
    //close(s_fifo_fd);
    unlink(BALCAO_FIFO);
    exit(EXIT_SUCCESS);
}


void funcZombie(int signal) {

    wait(NULL);
}

const char *classifica(char sintomas[]) {

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

        //  signal(SIGCHLD,funcZombie);

        //printf("Sintomas: ");
        //fflush(stdout);

        //fgets(sintomas, sizeof(sintomas) - 1, stdin); // le o \n


        tam = write(b2c[STDOUT_FILENO], sintomas, strlen(sintomas));


        if (strcmp(sintomas, "#fim\n") == 0) {
            //  strcpy(sintomas, sintomasTemp); //para evitar ficar com "#fim" nos sintomas

            read(c2b[STDIN_FILENO], especialidade_Prioridade, sizeof(especialidade_Prioridade) - 1);
            //Fechar canais
            close(b2c[STDOUT_FILENO]);
            close(c2b[STDIN_FILENO]);
            waitpid(pid, &estado, 0); // para evitar que o processo do classificador fique no estado "zombie"


            return "Classificação encerrada!\n";
        } //else
        //strcpy(sintomasTemp, sintomas);

        tam1 = read(c2b[STDIN_FILENO], especialidade_Prioridade, sizeof(especialidade_Prioridade) - 1);
        especialidade_Prioridade[tam1] = '\0';

        if (tam > 0)
            printf("\nEscrevi %zu bytes: %s", tam, sintomas);

        if (tam1 > 0)
            printf("Recebi %zu bytes: %s \n", tam1, especialidade_Prioridade);


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

void comandos(Balcao balcao, pCliente listaUtentes, pMedico listaMedicos) {
    char comando[50];
    fgets(comando, sizeof(comando), stdin);// le o \n
    // printf("O administrador introduziu o comando: %s", comando);

    if (strcmp(comando, "utentes\n") == 0) {

        printf("\n---------- Lista de Utentes ----------\n");
        mostrarTodosClientes(balcao, listaUtentes);
        printf("\n--------------------------------------\n");

    } else if (strcmp(comando, "especialistas\n") == 0) {

        printf("\n---------- Lista de Médicos ----------\n");
        mostrarTodosMedicos(balcao, listaMedicos);
        printf("\n------------------------------------\n");

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

        unlink(BALCAO_FIFO);
        exit(EXIT_SUCCESS);
        exit(1);


    } else {
        printf("[INFO] Comando Invalido!\n\n");
    }

    // signal(SIGINT, trataSig);

}

Balcao inicializarDadosBalcao(int MAXMEDICOS, int MAXCLIENTES) {

    Balcao b;

    b.maxMedicos = MAXMEDICOS;
    b.maxClientes = MAXCLIENTES;
    for (int i = 0; i < MAXESPECIALIDADES; i++) {
        b.filaEspera[i] = 0;
        b.numEspecialistas[i] = 0;
    }
    b.numClientes = 0;
    b.numMedicos = 0;


    return b;
}

void adicionarMedico(Balcao *b, pMedico listaMedicos, int id, Medico medico) {
    if (b->numMedicos == b->maxMedicos) {
        fprintf(stderr, "[INFO] LOTAÇÃO MÁXIMA DE MÉDICOS ATINGIDA\n");
        return;
    }

    listaMedicos[id] = medico;
    b->numMedicos++;
    fprintf(stdout, "[INFO] Medico '%s' adicionado ao sistema!\n", medico.nome);
}

void removerMedico(Balcao *b, pMedico medico, int id) {
    if (b->numMedicos > 0) {
        b->numMedicos--;
        fprintf(stdout, "[INFO] Médico '%s' removido do sistema!\n", medico[id].nome);
    }

    //atribuirDadosMedico( "...", "...", -1);
}

void mostrarTodosMedicos(Balcao b, pMedico medico) {

    if (b.numMedicos > 0) {
        for (int i = 0; i < b.maxMedicos; i++)
            if (medico[i].id != 0)
                mostrarDadosMedico(i, medico[i]);
    } else
        printf("\n Não existem médicos no sistema!\n");


}

void mostrarDadosMedico(int id, Medico medico) {
    printf("\nMédico [%d]:\n\tNome: %s\n\tEspecialidade: %s\n\tEstado: %d\n", id, medico.nome, medico.especialidade,
           medico.estado);
}

void adicionarCliente(Balcao *b, pCliente listaUtentes, int id, Cliente cliente) {

    if (b->numClientes == b->maxClientes) {
        fprintf(stderr, "[INFO] LOTAÇÃO MÁXIMA DE CLIENTES ATINGIDA\n");
        return;
    }

    listaUtentes[id] = cliente;
    b->numClientes++;
    fprintf(stdout, "[INFO] Cliente '%s' adicionado ao sistema!\n", cliente.nome);
}

void removerCLiente(Balcao *b, pCliente utente, int id) {

    if (b->numClientes > 0) {
        b->numClientes--;
        fprintf(stdout, "[INFO] Cliente '%s' removido do sistema!\n", utente[id].nome);
    }

    // utente[id] = atribuirDadosCliente("...","...","...",0,0,-1);


}

int vericaExisteCliente(Balcao b, pCliente listaUtentes, int pid) {


    for (int i = 0; i < b.maxClientes; i++) {
        if (listaUtentes[i].id == pid)
            return 1;
    }

    return 0;
}

int verificaExisteMedico(Balcao b, pMedico listaMedicos, int pid) {

    for (int i = 0; i < b.maxMedicos; i++) {
        if (listaMedicos[i].id == pid)
            return 1;
    }

    return 0;
}

int vericaClienteTemEspecialidade(Balcao b, pCliente listaUtentes, int pid) {

    int i;
    for (i = 0; i < b.maxClientes; i++) {
        if (listaUtentes[i].id == pid)
            if (strcmp("", listaUtentes[i].areaEspecialidade) == 0)
                return 1;
    }

    return 0;
}

Cliente inicializarListaClientes(Balcao b) {

    Cliente c;
    c.id = 0;
    strcpy(c.nome, "");
    strcpy(c.areaEspecialidade, "");
    c.posicaoListaEspera = 0;
    c.prioridade = 0;
    strcpy(c.sintomas, "");

    return c;
}

Medico inicializarListaMedicos(Balcao b) {

    Medico m;
    m.id = 0;
    strcpy(m.nome, "");
    strcpy(m.especialidade, "");
    m.estado = 0;

    return m;
}


void mostrarDadosCliente(int id, Cliente utente) {
    printf("\nCliente [%d]:\n\tNome: %s\n\tSintomas: %s\tEspecialidade: %s\tPrioridade: %d\n\tPosicaoEspera: %d\n",
           id, utente.nome, utente.sintomas, utente.areaEspecialidade, utente.prioridade, utente.posicaoListaEspera);
}

void mostrarTodosClientes(Balcao b, pCliente utente) {

    if (b.numClientes > 0) {
        for (int i = 0; i < b.maxClientes; i++) {
            if (utente[i].id != 0)
                mostrarDadosCliente(i, utente[i]);
        }
    } else
        printf("\n Não existem clientes no sistema!\n");
}

void sinal_vida(int s) {


    alarm(20);
    fflush(stderr);
    fprintf(stderr, "hello there");
}


int main(int argc, char *argv[]) {


    int res;
    int maxCLientes, maxMedicos;
    Balcao balcao;
    Cliente utente;
    Medico medico;
    char cliente_FIFO_nome[30]; //nome do FIFO do cliente
    char medico_FIFO_nome[30]; //nome do FIFO do cliente

    int nfd; //valor retorno do select
    fd_set read_fds;
    struct timeval tv; //timeout para o select



    //Receber as variáveis de ambiente
    if (getenv("MAXCLIENTES") != NULL && getenv("MAXMEDICOS") != NULL) {

        //Receber as variáveis de ambiente
        sscanf(getenv("MAXCLIENTES"), "%d", &maxCLientes);
        sscanf(getenv("MAXMEDICOS"), "%d", &maxMedicos);

        printf("Variáveis de ambiente definidas:\n");
        printf("MAXCLIENTES: %d\n", maxCLientes);
        printf("MAXMEDICOS: %d\n", maxMedicos);

    } else {
        fprintf(stderr, "As variáveis de ambiente não foram definidas!\n");
        exit(1);
    }


    //Criação dos arrays que vão guardar os clientes/médicos
    Cliente listaUtentes[maxCLientes];
    Medico listaMedicos[maxMedicos];

    //Inicializar os dados do balcão/clientes
    balcao = inicializarDadosBalcao(maxMedicos, maxCLientes);

    for (int i = 0; i < balcao.maxClientes; i++)
        listaUtentes[i] = inicializarListaClientes(balcao);

    for (int i = 0; i < balcao.maxMedicos; i++)
        listaMedicos[i] = inicializarListaMedicos(balcao);

    //Estruturas das mensagens
    Mensagem_utilizador_para_Balcao mens_para_balc;
    Mensagem_Balcao mens_Out;


    mens_Out.pid_balcao = getpid();


    //Verificar se ja existe algum balcao em funcionamento
    if (access(BALCAO_FIFO, F_OK) == 0) {
        printf("[INFO] Já existe um Balcão em funcionamento!\n");
        exit(2);
    }

    //Criar pipe do balcao
    mkfifo(BALCAO_FIFO, 0600);
    printf("[INFO] Criei o FIFO do Balcão...\n");

    //Verificar a abertura do fifo
    balcao_fifo_fd = open(BALCAO_FIFO, O_RDWR);
    if (balcao_fifo_fd == -1) {
        perror("\nErro ao abrir o FIFO do servidor (RDWR/blocking)");
        exit(EXIT_FAILURE);
    } else {
        fprintf(stderr, "\nFIFO aberto para READ (+WRITE) bloqueante");
    }


    //Para interromper via CTRL-C
    if (signal(SIGINT, trataSig) == SIG_ERR) {
        perror("\nNão foi possível configurar o sinal SIGINT\n");
        exit(EXIT_FAILURE);
    }


    while (1) {
        tv.tv_sec = 2;
        tv.tv_usec = 0;



        FD_ZERO(&read_fds);
        FD_SET(0, &read_fds);
        FD_SET(balcao_fifo_fd, &read_fds);

        nfd = select(balcao_fifo_fd + 1, &read_fds, NULL, NULL, &tv);

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
            comandos(balcao, listaUtentes, listaMedicos); //Ler comandos do teclado
        }

        if (FD_ISSET(balcao_fifo_fd, &read_fds)) {

            res = read(balcao_fifo_fd, &mens_para_balc, sizeof(mens_para_balc));
            printf("TAMANHO : %ld\n", sizeof(mens_para_balc));
            if (res < (int) sizeof(mens_para_balc)) {
                fprintf(stderr, "[INFO] Mensagem recebida incompleta! [Bytes lidos: %d]\n", res);
                continue;
            }

            printf("asdasdasd");

            //CLIENTE
            if (mens_para_balc.medico_cliente == 1) {

                if (vericaExisteCliente(balcao, listaUtentes, mens_para_balc.pid) == 0) {

                    strcpy(utente.nome, mens_para_balc.nome);
                    utente.id = mens_para_balc.pid;
                    medico.estado = mens_para_balc.atendido;
                    strcpy(utente.sintomas, mens_para_balc.mensagem);
                    strcpy(utente.areaEspecialidade, classifica(utente.sintomas));
                    fprintf(stdout, "Especialidade recebida do classificador: %s\n", utente.areaEspecialidade);
                    utente.prioridade = 0;
                    utente.posicaoListaEspera = 0;


                    adicionarCliente(&balcao, listaUtentes, balcao.numClientes, utente);

                    strcpy(mens_Out.mensagem, utente.areaEspecialidade);

                    //* ---- OBTÉM FILENAME DO FIFO PARA A RESPOSTA ---- *//*
                    sprintf(cliente_FIFO_nome, CLIENTE_FIFO, utente.id);
                    cliente_fifo_fd = open(cliente_FIFO_nome, O_WRONLY);
                    if (cliente_fifo_fd == -1)
                        perror("[INFO] Erro na abertura do FIFO do cliente para escrita\n");
                    else {
                        res = write(cliente_fifo_fd, &mens_Out, sizeof(mens_Out));
                        if (res == sizeof(mens_Out))
                            fprintf(stdout, "[INFO] Mensagem enviada para o cliente com sucesso!\n");
                        else
                            fprintf(stderr, "[INFO] Erro ao enviar a mensagem para o cliente!\n");

                        //close(cliente_fifo_fd); //* FECHA LOGO O FIFO DO CLIENTE! *//
                        fprintf(stdout, "\n[INFO] FIFO do Cliente fechado\n");
                    }
                }

                if (strcmp("#fim\n", mens_para_balc.mensagem) == 0) {

                    strcpy(mens_Out.mensagem, classifica(mens_para_balc.mensagem));

                    //* ---- OBTÉM FILENAME DO FIFO PARA A RESPOSTA ---- *//*
                    sprintf(cliente_FIFO_nome, CLIENTE_FIFO, mens_para_balc.pid);

                    cliente_fifo_fd = open(cliente_FIFO_nome, O_WRONLY);

                    if (cliente_fifo_fd == -1)
                        perror("[INFO] Erro na abertura do FIFO do cliente para escrita\n");
                    else {
                        res = write(cliente_fifo_fd, &mens_Out, sizeof(mens_Out));
                        if (res == sizeof(mens_Out))
                            fprintf(stdout, "[INFO] Mensagem enviada para o cliente com sucesso!\n");
                        else
                            fprintf(stderr, "[INFO] Erro ao enviar a mensagem para o cliente!\n");

                        //close(cliente_fifo_fd); //* FECHA LOGO O FIFO DO CLIENTE! *//*
                        fprintf(stdout, "\n[INFO] FIFO do Cliente fechado\n");
                    }
                }

            }





                //MEDICO
            if (mens_para_balc.medico_cliente == 0) {

                if (verificaExisteMedico(balcao, listaMedicos, mens_para_balc.pid) == 0) {

                    strcpy(medico.nome, mens_para_balc.nome);
                    medico.id = mens_para_balc.pid;
                    medico.estado = mens_para_balc.atendido;
                    strcpy(medico.especialidade, mens_para_balc.mensagem);
                    medico.estado = 0;


                    adicionarMedico(&balcao, listaMedicos, balcao.numMedicos, medico);

                    strcpy(mens_Out.mensagem, "Médico inicializado");

                    // ---- OBTÉM FILENAME DO FIFO PARA A RESPOSTA ----
                    sprintf(medico_FIFO_nome, MEDICO_FIFO, medico.id);

                    medico_fifo_fd = open(medico_FIFO_nome, O_WRONLY);
                    if (medico_fifo_fd == -1)
                        perror("[INFO] Erro na abertura do FIFO do cliente para escrita\n");
                    else {
                        res = write(medico_fifo_fd, &mens_Out, sizeof(mens_Out));

                        if (res < (int) sizeof(mens_Out)) {
                            fprintf(stderr, "[INFO] Mensagem enviada incompleta! [Bytes enviados: %d]\n", res);
                        }
                        fprintf(stdout, "[INFO] Mensagem enviada para o médico com sucesso! [Bytes enviados: %d]\n",
                                res);
                       // close(medico_fifo_fd); // FECHA LOGO O FIFO DO MEDICO!
                        fprintf(stdout, "\n[INFO] FIFO do cliente fechado\n");
                    }

                }













                int atendido = 0;
                int pos;
                for (pos = 0; pos < balcao.numClientes; ++pos) {
                    if (listaUtentes[pos].id == cliente_fifo_fd) {
                        if (listaUtentes[pos].atendido == 1) {
                            atendido = 1;
                            break;
                        }
                    }
                    atendido = 0;
                }

                printf("Mensagem!!!!!!! %s ", mens_para_balc.mensagem);
                if (atendido == 0) {
                    printf("1111");
                    if(balcao.numMedicos > 0) {
                        for (int i = 0; i < balcao.numMedicos; i++) {
                            printf("pid medico ::  %d", listaMedicos[i].id);
                            if (listaMedicos[i].id != 0) {
                                printf("11");
                                printf("id do medico atribuido : %d", listaMedicos[i].id);
                                mens_Out.pid_medico = listaMedicos[i].id;
                                listaUtentes[pos].atendido = 1;
                            } else
                                fprintf(stderr, "[INFO] Ainda nao existem medicos!\n");
                        }

                        cliente_fifo_fd = open(cliente_FIFO_nome, O_WRONLY);
                        if (cliente_fifo_fd == -1) {
                            perror("[INFO] Erro na abertura do FIFO do cliente para escrita\n");
                        }
                        else /*if (listaUtentes[pos].atendido == 1) */{
                            res = write(cliente_fifo_fd, &mens_Out, sizeof(mens_Out));
                            if (res == sizeof(mens_Out))
                                fprintf(stdout, "[INFO] Mensagem enviada para o cliente com sucesso!\n");
                            else
                                fprintf(stderr, "[INFO] Erro ao enviar a mensagem para o cliente!\n");

                            close(cliente_fifo_fd); //* FECHA LOGO O FIFO DO CLIENTE! *//
                        }
                    }
                }












                int posM;
                for (posM = 0; posM < balcao.numMedicos; ++posM) {
                    if (listaMedicos[posM].id == medico_fifo_fd) {
                        printf("medico=MEDICO\n");
                        break;
                    }
                }

                if (listaMedicos[posM].estado == 0 && balcao.numClientes > 0) {
                    printf("estado 0");
                    for (int i = 0; i < balcao.numClientes; i++) {
                        if (listaUtentes[i].id != 0 && listaUtentes[i].atendido == 0) {
                            printf("11");
                            printf("id do cliente atribuido : %d", listaUtentes[i].id);
                            mens_Out.pid_cliente= listaUtentes[i].id;
                            listaMedicos[posM].estado = 1;
                        } else
                            fprintf(stderr, "[INFO] Ainda nao existem medicos!\n");
                    }

                    medico_fifo_fd = open(medico_FIFO_nome, O_WRONLY);
                    if (medico_fifo_fd == -1)
                        perror("[INFO] Erro na abertura do FIFO do medico para escrita\n");
                    else /*if(listaMedicos[posM].estado == 1)*/{
                        res = write(medico_fifo_fd, &mens_Out, sizeof(mens_Out));
                        if (res == sizeof(mens_Out))
                            fprintf(stdout, "[INFO] Mensagem enviada para o medico com sucesso!\n");
                        else
                            fprintf(stderr, "[INFO] Erro ao enviar a mensagem para o medico!\n");

                        close(medico_fifo_fd); //* FECHA LOGO O FIFO DO CLIENTE! *//
                    }
                }
            }
        }
    }

    //signal(SIGALRM, sinal_vida);

    //sigaction(SIGALARM,)  usar em vez do signal

    close(balcao_fifo_fd);
    unlink(BALCAO_FIFO);
    return 0;


}



