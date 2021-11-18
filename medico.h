#ifndef TP_MEDICO_H
#define TP_MEDICO_H

#include "balcao.h"

typedef struct medico Medico;
struct medico{

    char nome[30];
    char especialidade[100];
    int id_Medico;
    int estado; // 0 - parado
                //1 - a trabalhar
};

#endif //TP_MEDICO_H

