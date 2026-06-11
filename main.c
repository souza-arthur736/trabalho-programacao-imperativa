#include <stdio.h>
#include "mcnamara.h"

#define LINHA printf("\n--------------------------------------------------------------\n\n")
#define SPACE printf("\n")
#define ERROR printf("\nOPCAO INVALIDA\n")

int main()
{
    int tipoMaxila;
    int valorMaxila;
    int CoA;
    int CoGnReal;
    int AFAIReal;

    do
    {
        printf("TIPOS DE MAXILA\n\n");
        printf("1 - MAXILA NORMAL\n");
        printf("2 - MAXILA PROTRUIDA\n");
        printf("3 - MAXILA RETRUIDA\n\n");

        printf("Selecione o tipo de maxila do paciente: ");
        scanf("%d", &tipoMaxila);

        if (tipoMaxila < 1 || tipoMaxila > 3)
        {
            ERROR;
            LINHA;
        }

    } while (tipoMaxila < 1 || tipoMaxila > 3);

    SPACE;

    switch (tipoMaxila)
    {
        case 1:

            valorMaxila = 0;

            break;

        case 2:

            printf("CLASSIFICACAO DA PROTRUSAO\n");
            printf("LEVEMENTE PROTRUIDA (1 ou 2 mm)\n");
            printf("PROTRUIDA (3 ou 4 mm)\n");
            printf("SEVERAMENTE PROTRUIDA (5 mm ou mais)\n\n");

            printf("Digite a quantidade de mm da protrusao: ");

            do
            {
                scanf("%d", &valorMaxila);

                if (valorMaxila < 0)
                {
                    printf("Valor invalido. Digite novamente: ");
                }

            } while (valorMaxila < 0);

            break;

        case 3:

            printf("CLASSIFICACAO DA RETRUSAO\n");
            printf("LEVEMENTE RETRUIDA (1 ou 2 mm)\n");
            printf("RETRUIDA (3 ou 4 mm)\n");
            printf("SEVERAMENTE RETRUIDA (5 mm ou mais)\n\n");

            printf("Digite a quantidade de mm da retrusao: ");

            do
            {
                scanf("%d", &valorMaxila);

                if (valorMaxila < 0)
                {
                    printf("Valor invalido. Digite novamente: ");
                }

            } while (valorMaxila < 0);

            break;
    }

    LINHA;

    printf("Digite o valor de CoA do paciente: ");
    scanf("%d", &CoA);

    printf("Digite o valor de CoGn do paciente: ");
    scanf("%d", &CoGnReal);

    printf("Digite o valor de AFAI do paciente: ");
    scanf("%d", &AFAIReal);

    int CoAOriginal = CoA;

    if (tipoMaxila == 2)
    {
        CoA -= valorMaxila;
    }
    else if (tipoMaxila == 3)
    {
        CoA += valorMaxila;
    }

    LINHA;

    printf("RESUMO DA ANALISE\n\n");

    switch (tipoMaxila)
    {
        case 1:

            printf("Maxila: NORMAL\n");
            printf("CoA utilizado na tabela: %d\n", CoA);

            break;

        case 2:

            printf("Maxila: PROTRUIDA %d mm\n", valorMaxila);
            printf("Correcao aplicada: %d - %d = %d\n",
                   CoAOriginal,
                   valorMaxila,
                   CoA);

            break;

        case 3:

            printf("Maxila: RETRUIDA %d mm\n", valorMaxila);
            printf("Correcao aplicada: %d + %d = %d\n",
                   CoAOriginal,
                   valorMaxila,
                   CoA);

            break;
    }

    LINHA;

    const TabelaMcNamara* dados = buscarTabela(CoA);

    if (dados == NULL)
    {
        printf("ERRO: O CoA corrigido (%d) esta fora dos limites da tabela de McNamara.\n", CoA);
        return 0;
    }

    printf("VALORES IDEAIS SEGUNDO A TABELA DE MCNAMARA\n\n");

    printf("CoGn ideal : %d-%d mm\n",
           dados->coGnMin,
           dados->coGnMax);

    printf("AFAI ideal : %d-%d mm\n",
           dados->afaiMin,
           dados->afaiMax);

    LINHA;

    printf("VALORES DO PACIENTE\n\n");

    printf("CoGn : %d mm\n", CoGnReal);
    printf("AFAI : %d mm\n", AFAIReal);

    LINHA;

    printf("DIAGNOSTICO MANDIBULAR (CoGn)\n");

    if (CoGnReal < dados->coGnMin)
    {
        printf("TAMANHO MANDIBULAR REDUZIDO\n");
    }
    else if (CoGnReal > dados->coGnMax)
    {
        printf("TAMANHO MANDIBULAR AUMENTADO\n");
    }
    else
    {
        printf("TAMANHO MANDIBULAR NORMAL\n");
    }

    LINHA;

    printf("DIAGNOSTICO DA ALTURA FACIAL (AFAI)\n");

    if (AFAIReal < dados->afaiMin)
    {
        printf("ALTURA FACIAL REDUZIDA\n");
        printf("PADRAO DE CRESCIMENTO HORIZONTAL\n");
    }
    else if (AFAIReal > dados->afaiMax)
    {
        printf("ALTURA FACIAL AUMENTADA\n");
        printf("PADRAO DE CRESCIMENTO VERTICAL\n");
    }
    else
    {
        printf("ALTURA FACIAL NORMAL\n");
    }

    LINHA;

    return 0;
}