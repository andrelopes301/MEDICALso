#ifndef TP_CLIENTE_H
#define TP_CLIENTE_H


typedef struct cliente Cliente, *pCliente;
struct cliente{

    char nome[30];
    char sintomas[100];
    int id;
    char areaEspecialidade[100];
    int prioridade;
    int numUtentesFrente;
    int numEspecialistasArea;

    pCliente prox;
};





#endif //TP_CLIENTE_H
