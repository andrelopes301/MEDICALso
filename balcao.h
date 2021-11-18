#ifndef TP_BALCAO_H
#define TP_BALCAO_H

#define MAXESPECIALIDADES 5

typedef struct balcao Balcao, *pBalcao;
struct balcao{
    int maxClientes;
    int maxMedicos;
    int numClientes;
    int numMedicos;
    int filaEspera[MAXESPECIALIDADES];
};





#endif //TP_BALCAO_H
