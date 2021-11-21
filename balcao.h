#ifndef TP_BALCAO_H
#define TP_BALCAO_H

#define MAXESPECIALIDADES 5

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




#endif //TP_BALCAO_H
