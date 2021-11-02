#ifndef TP_MEDICO_H
#define TP_MEDICO_H

#include "balcao.h"

typedef struct medico Medico, *pMedico;
struct medico{

    char nome[30];
    char especialidade[100];
    int id_Medico;
    char estado[100];
    
    pMedico prox;
};

#endif //TP_MEDICO_H

