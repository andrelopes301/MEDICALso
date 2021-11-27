#ifndef TP_MEDICO_H
#define TP_MEDICO_H


typedef struct medico Medico, *pMedico;
struct medico{

    char nome[30];
    char especialidade[100];
    int id;// 0 - medico nao existe
            // > 0, - medico existe e o id corresponde ao seu pid
    int estado; // 0 - parado
                // 1 - a trabalhar
};


#endif //TP_MEDICO_H

