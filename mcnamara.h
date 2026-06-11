#ifndef MCNAMARA_H
#define MCNAMARA_H

typedef struct
{
    int coa;

    int coGnMin;
    int coGnMax;

    int afaiMin;
    int afaiMax;

} TabelaMcNamara;

const TabelaMcNamara* buscarTabela(int coa);

#endif
