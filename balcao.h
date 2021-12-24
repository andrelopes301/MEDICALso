#ifndef TP_BALCAO_H
#define TP_BALCAO_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "medico.h"
#include "cliente.h"


#define MAXESPECIALIDADES 5
#define MAX 256

// nome do FIFO do Balcão
#define BALCAO_FIFO "balcao_fifo"
// nome do FIFO de cada Cliente - %d = pid
#define CLIENTE_FIFO "cliente_%d_fifo"
// nome do FIFO de cada Medico - %d = pid
#define MEDICO_FIFO "medico_%d_fifo"


enum especialidades{
    oftalmologia = 0,
    neurologia = 1,
    estomatologia = 2,
    ortopedia = 3,
    geral = 4
};


typedef struct balcao Balcao, *pBalcao;
struct balcao{
    int maxClientes;
    int maxMedicos;
    int numClientes;
    int numMedicos;
    int numEspecialistas[MAXESPECIALIDADES]; // numero de especialistas por area
    int filaEspera[MAXESPECIALIDADES]; //numero de pessoas em fila de espera por area

    //Exemplo:
    //filaEspera[0] - 1 utente - oftalmologia
    //filaEspera[1] - 3 utentes (na fila de espera) - neurologia
    // ...

};

//Estrutura da mensagem: cliente -> balcao
typedef struct {
    pid_t	pid_cliente;
    char	mensagem[MAX];
} Mensagem_cliente_para_Balcao;

//Estrutura da mensagem de respota do balcao
typedef struct {
    pid_t pid_balcao;
    char	mensagem[MAX];
} Mensagem_Balcao;

typedef struct {
    pid_t	pid_medico;
    char	mensagem[MAX];
} Mensagem_medico_para_Balcao;







void encerra();
void help();
const char* classifica(char *sintomas);
int comandos();

Balcao inicializarDadosBalcao(int MAXMEDICOS, int MAXCLIENTES);


/// MEDICO
void adicionarMedico(Balcao *b, pMedico listaMedicos, int id, Medico medico);
void removerMedico(Balcao *b, pMedico medico, int id);
void mostrarTodosMedicos(pBalcao b, pMedico medico);
void mostrarDadosMedico(int id, Medico medico);
void sinal_vida(int s);

/// CLIENTE
void adicionarCliente(Balcao *b, pCliente utente, int id, Cliente cliente);
void removerCLiente(Balcao *b,pCliente utente, int id);
void mostrarDadosCliente(int id, Cliente utente);
void mostrarTodosClientes(pBalcao b, pCliente utente);





#endif //TP_BALCAO_H
