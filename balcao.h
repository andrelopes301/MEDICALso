#ifndef TP_BALCAO_H
#define TP_BALCAO_H

#include "cliente.h"
#include "medico.h"

#define MAXESPECIALIDADES 5
#define MAX 256
#define BALCAO_FIFO "balcao_fifo"

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



void encerra();
void help();
void classifica();

Balcao inicializarDadosBalcao(int MAXMEDICOS, int MAXCLIENTES);




/// CLIENTE
Cliente atribuirDadosCliente(char *nome,char *sintomas,char *especialidade,int prioridade,int listaEspera,int pidC);
void adicionarCliente(Balcao *b, pCliente utente, int id, char *nome,char *sintomas,char *especialidade,int prioridade,int listaEspera,int pidC);
void removerCLiente(Balcao *b,pCliente utente, int id);
void mostrarDadosCliente(int id, Cliente utente);
void mostrarTodosClientes(pBalcao b, pCliente utente);

/// MEDICO
Medico atribuirDadosMedico( char *nome, char *especialidade,int pidM);
void adicionarMedico(Balcao *b, pMedico medico, int id, char *nome,char *especialidade,int pidM);
void removerMedico(Balcao *b, pMedico medico, int id);
void mostrarDadosMedico(int id, Medico medico);
void mostrarTodosMedicos(pBalcao b, pMedico medico);



#endif //TP_BALCAO_H
