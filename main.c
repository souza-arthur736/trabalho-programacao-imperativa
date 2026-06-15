#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "mcnamara.h"

#define LINHA  printf("\n--------------------------------------------------------------\n\n")
#define SPACE  printf("\n")
#define ERROR  printf("\nOPCAO INVALIDA\n")

/* ============================================================
 * Helpers de entrada
 * ============================================================ */
int lerIntMin(const char *prompt, int minVal)
{
    int v;
    printf("%s", prompt);
    while (scanf("%d", &v) != 1 || v < minVal)
    {
        printf("Valor invalido. Digite novamente: ");
        while (getchar() != '\n');   /* limpa buffer */
    }
    return v;
}

int lerIntRange(const char *prompt, int minVal, int maxVal)
{
    int v;
    printf("%s", prompt);
    while (scanf("%d", &v) != 1 || v < minVal || v > maxVal)
    {
        printf("Valor invalido (%d a %d). Digite novamente: ", minVal, maxVal);
        while (getchar() != '\n');
    }
    return v;
}

/* ============================================================
 * main
 * ============================================================ */
int main(void)
{

    /* ---- Variaveis ---- */
    int tipoMaxila;         /* 1=normal 2=protruida 3=retruida */
    int mmMaxila = 0;       /* magnitude mm (0 se normal) */
    int CoGnReal, AFAIReal;
    int CoAOriginal, CoACorrigido;
    double ANB, SNGoGn;
    double dist1NA, ang1NA;   /* incisivos superiores */
    double dist1NB, ang1NB;   /* incisivos inferiores */
    int perfilTegumentar;     /* 1=convexo suave 2=reto 3=convexo 4=concavo */

    /* ---- Strings para armazenar os resultados temporariamente ---- */
    char strMaxila[50] = "";
    char strMandibula[50] = "";
    char strCrescimento[50] = "";
    char strIncisivoSupPos[80] = "";
    char strIncisivoSupAng[80] = "";
    char strIncisivoInfPos[80] = "";
    char strIncisivoInfAng[80] = "";
    char strPerfil[50] = "";
    char strClasse[50] = "incomum";

    /* ================================================================
     * PASSO 1 – Maxila e perfil tegumentar
     * ================================================================ */
    printf("=== DIAGNOSTICO ORTODONTICO ESQUELETICO ===\n");
    tipoMaxila = lerIntRange("Posicionamento da Maxila (1=Normal, 2=Protruida, 3=Retruida): ", 1, 3);

    if (tipoMaxila == 2) {
        mmMaxila = lerIntMin("Grau de protrusao (mm): ", 1);
        sprintf(strMaxila, "protruída");
    } else if (tipoMaxila == 3) {
        mmMaxila = lerIntMin("Grau de retrusao (mm): ", 1);
        sprintf(strMaxila, "retruída");
    } else {
        sprintf(strMaxila, "bem posicionada");
    }

    perfilTegumentar = lerIntRange("Perfil Tegumentar (1=Convexo Suave, 2=Reto, 3=Convexo, 4=Concavo): ", 1, 4);
    switch (perfilTegumentar) {
        case 1: sprintf(strPerfil, "normal (convexo suave)"); break;
        case 2: sprintf(strPerfil, "reto"); break;
        case 3: sprintf(strPerfil, "convexo"); break;
        case 4: sprintf(strPerfil, "côncavo"); break;
        default: sprintf(strPerfil, "desconhecido"); break;
    }

    /* ================================================================
     * Entradas das Variaveis Cefalometricas
     * ================================================================ */
    CoAOriginal = lerIntMin("Comprimento da Maxila (CoA em mm): ", 1);
    CoGnReal = lerIntMin("Comprimento Mandibular (CoGn real em mm): ", 1);
    AFAIReal = lerIntMin("Altura Facial Inferior (AFAI real em mm): ", 1);

    printf("Informe os valores de Steiner e incisivos:\n");
    printf("ANB (graus): "); scanf("%lf", &ANB);
    printf("SN.GoGn (graus): "); scanf("%lf", &SNGoGn);

    printf("Distancia 1-NA (mm): "); scanf("%lf", &dist1NA);
    printf("Angulo 1.NA (graus): "); scanf("%lf", &ang1NA);

    printf("Distancia 1-NB (mm): "); scanf("%lf", &dist1NB);
    printf("Angulo 1.NB (graus): "); scanf("%lf", &ang1NB);

    /* ================================================================
     * Processamento e Correcao da Maxila
     * ================================================================ */
    CoACorrigido = CoAOriginal;
    if (tipoMaxila == 2) {
        CoACorrigido = CoAOriginal - mmMaxila;
    } else if (tipoMaxila == 3) {
        CoACorrigido = CoAOriginal + mmMaxila;
    }

    const TabelaMcNamara* ref = buscarTabela(CoACorrigido);

    if (ref == NULL) {
        LINHA;
        printf("Nao foi possivel encontrar a tabela de McNamara para CoA = %d\n", CoACorrigido);
        LINHA;
        return 1;
    }

    /* ================================================================
     * IMPRESSÃO DETALHADA INTERMEDIÁRIA
     * ================================================================ */
    LINHA;
    printf("=== ANALISE DETALHADA DOS DADOS ===\n\n");

    /* 1. Tamanho Mandibular (CoGn) */
    printf("TAMANHO MANDIBULAR:\n");
    if (CoGnReal < ref->coGnMin) {
        printf("  DIMINUIDO (Paciente: %d mm, Ideal: %d a %d mm)\n", CoGnReal, ref->coGnMin, ref->coGnMax);
        sprintf(strMandibula, "diminuído");
    } else if (CoGnReal > ref->coGnMax) {
        printf("  AUMENTADO (Paciente: %d mm, Ideal: %d a %d mm)\n", CoGnReal, ref->coGnMin, ref->coGnMax);
        sprintf(strMandibula, "aumentado");
    } else {
        printf("  NORMAL (Paciente: %d mm, Ideal: %d a %d mm)\n", CoGnReal, ref->coGnMin, ref->coGnMax);
        sprintf(strMandibula, "normal");
    }

    /* 2. AFAI e Giro Mandibular (SN.GoGn) */
    SPACE;
    printf("PADRAO VERTICAL E GIRO MANDIBULAR:\n");
    printf("  Por AFAI: ");
    if (AFAIReal < ref->afaiMin)
        printf("Diminuida -> Padrao Horizontal / Giro Anti-horario\n");
    else if (AFAIReal > ref->afaiMax)
        printf("Aumentada -> Padrao Vertical / Giro Horario\n");
    else
        printf("Normal -> Padrao Equilibrado\n");

    printf("  Por SN.GoGn: ");
    if (SNGoGn < 29.0)
        printf("Diminuido (%.1f graus) -> Braquifacial / Horizontal\n", SNGoGn);
    else if (SNGoGn > 35.0)
        printf("Aumentado (%.1f graus) -> Dolicofacial / Vertical\n", SNGoGn);
    else
        printf("Normal (%.1f graus) -> Mesofacial / Equilibrado\n", SNGoGn);

    if (AFAIReal < ref->afaiMin && SNGoGn < 29.0)
        sprintf(strCrescimento, "horizontal");
    else if (AFAIReal > ref->afaiMax && SNGoGn > 35.0)
        sprintf(strCrescimento, "vertical");
    else if ((AFAIReal >= ref->afaiMin && AFAIReal <= ref->afaiMax) && (SNGoGn >= 29.0 && SNGoGn <= 35.0))
        sprintf(strCrescimento, "equilibrado");
    else
        sprintf(strCrescimento, "misto/indefinido");

    /* 3. Relacao Esqueletica (ANB) */
    SPACE;
    printf("RELACAO ESQUELETICA (ANB):\n");
    if (ANB >= 1.0 && ANB <= 4.0)
        printf("  Classe I esqueletica (%.1f graus)\n", ANB);
    else if (ANB > 4.0)
        printf("  Classe II esqueletica (%.1f graus)\n", ANB);
    else
        printf("  Classe III esqueletica (%.1f graus)\n", ANB);

    /* 4. Diagnostico dos Incisivos Superiores (1-NA) */
    SPACE;
    printf("INCISIVOS SUPERIORES (1.NA):\n");
    double mediaDistNA = 4.0;
    double mediaAngNA  = 24.0;

    // Posição 1-NA
    if (dist1NA < 3.0) {
        int desvio = (int)floor(mediaDistNA - dist1NA);
        char sufixo[10] = ""; for(int i=0; i<desvio && i<9; i++) sufixo[i] = '-';
        sufixo[desvio > 9 ? 9 : desvio] = '\0';
        printf("  Posicao: RETRUIDO (%.1f mm - ideal 3-5 mm) %s\n", dist1NA, sufixo);
        sprintf(strIncisivoSupPos, "retruídos %s", sufixo);
    } else if (dist1NA > 5.0) {
        int desvio = (int)floor(dist1NA - mediaDistNA);
        char sufixo[10] = ""; for(int i=0; i<desvio && i<9; i++) sufixo[i] = '+';
        sufixo[desvio > 9 ? 9 : desvio] = '\0';
        printf("  Posicao: PROTRUIDO (%.1f mm - ideal 3-5 mm) %s\n", dist1NA, sufixo);
        sprintf(strIncisivoSupPos, "protruídos %s", sufixo);
    } else {
        printf("  Posicao: BEM POSICIONADO (%.1f mm - ideal 3-5 mm)\n", dist1NA);
        sprintf(strIncisivoSupPos, "bem posicionados");
    }

    // Inclinação 1.NA
    if (ang1NA < 23.0) {
        int desvio = (int)floor(mediaAngNA - ang1NA);
        char sufixo[10] = ""; for(int i=0; i<desvio && i<9; i++) sufixo[i] = '-';
        sufixo[desvio > 9 ? 9 : desvio] = '\0';
        printf("  Inclinacao: VERTICALIZADO (%.1f graus - ideal 24-25 graus) %s\n", ang1NA, sufixo);
        sprintf(strIncisivoSupAng, "verticalizados %s", sufixo);
    } else if (ang1NA > 25.0) {
        int desvio = (int)floor(ang1NA - mediaAngNA);
        char sufixo[10] = ""; for(int i=0; i<desvio && i<9; i++) sufixo[i] = '+';
        sufixo[desvio > 9 ? 9 : desvio] = '\0';
        printf("  Inclinacao: INCLINADO PARA VESTIBULAR (%.1f graus - ideal 24-25 graus) %s\n", ang1NA, sufixo);
        sprintf(strIncisivoSupAng, "inclinados %s", sufixo);
    } else {
        printf("  Inclinacao: BOA INCLINACAO (%.1f graus - ideal 24-25 graus)\n", ang1NA);
        sprintf(strIncisivoSupAng, "com inclinação normal");
    }

    /* 5. Diagnostico dos Incisivos Inferiores (1-NB) */
    SPACE;
    printf("INCISIVOS INFERIORES (1.NB):\n");
    double mediaDistNB = 4.0;
    double mediaAngNB  = 25.0;

    // Posição 1-NB
    if (dist1NB < 3.0) {
        int desvio = (int)floor(mediaDistNB - dist1NB);
        char sufixo[10] = ""; for(int i=0; i<desvio && i<9; i++) sufixo[i] = '-';
        sufixo[desvio > 9 ? 9 : desvio] = '\0';
        printf("  Posicao: RETRUIDO (%.1f mm - ideal 3-5 mm) %s\n", dist1NB, sufixo);
        sprintf(strIncisivoInfPos, "retruídos %s", sufixo);
    } else if (dist1NB > 5.0) {
        int desvio = (int)floor(dist1NB - mediaDistNB);
        char sufixo[10] = ""; for(int i=0; i<desvio && i<9; i++) sufixo[i] = '+';
        sufixo[desvio > 9 ? 9 : desvio] = '\0';
        printf("  Posicao: PROTRUIDO (%.1f mm - ideal 3-5 mm) %s\n", dist1NB, sufixo);
        sprintf(strIncisivoInfPos, "protruídos %s", sufixo);
    } else {
        printf("  Posicao: BEM POSICIONADO (%.1f mm - ideal 3-5 mm)\n", dist1NB);
        sprintf(strIncisivoInfPos, "bem posicionados");
    }

    // Inclinação 1.NB
    if (ang1NB < 24.0) {
        int desvio = (int)floor(mediaAngNB - ang1NB);
        char sufixo[10] = ""; for(int i=0; i<desvio && i<9; i++) sufixo[i] = '-';
        sufixo[desvio > 9 ? 9 : desvio] = '\0';
        printf("  Inclinacao: VERTICALIZADO (%.1f graus - ideal 24-26 graus) %s\n", ang1NB, sufixo);
        sprintf(strIncisivoInfAng, "verticalizados %s", sufixo);
    } else if (ang1NB > 26.0) {
        int desvio = (int)floor(ang1NB - mediaAngNB);
        char sufixo[10] = ""; for(int i=0; i<desvio && i<9; i++) sufixo[i] = '+';
        sufixo[desvio > 9 ? 9 : desvio] = '\0';
        printf("  Inclinacao: INCLINADO PARA VESTIBULAR (%.1f graus - ideal 24-26 graus) %s\n", ang1NB, sufixo);
        sprintf(strIncisivoInfAng, "inclinados %s", sufixo);
    } else {
        printf("  Inclinacao: BOA INCLINACAO (%.1f graus - ideal 24-26 graus)\n", ang1NB);
        sprintf(strIncisivoInfAng, "com inclinação normal");
    }

    /* 6. Perfil Facial Tegumentar */
    SPACE;
    printf("PERFIL TEGUMENTAR:\n");
    printf("  Resultado: %s\n", strPerfil);

    /* ================================================================
     * Lógica do Cálculo da Ficha de Classe Esqueletica
     * ================================================================ */
    int anbNormal   = (ANB >= 1.0 && ANB <= 4.0);
    int anbAumentado = (ANB > 4.0);
    int anbDiminuido = (ANB < 1.0);

    int afaiNormal    = (AFAIReal >= ref->afaiMin && AFAIReal <= ref->afaiMax);
    int afaiDiminuida = (AFAIReal < ref->afaiMin);
    int afaiAumentada = (AFAIReal > ref->afaiMax);

    int sngognNormal    = (SNGoGn >= 29.0 && SNGoGn <= 35.0);
    int sngognDiminuido = (SNGoGn < 29.0);
    int sngognAumentado = (SNGoGn > 35.0);

    int maxilaNormal   = (tipoMaxila == 1);
    int maxilaProtruida = (tipoMaxila == 2);
    int maxilaRetruida  = (tipoMaxila == 3);

    int mandNormal    = (CoGnReal >= ref->coGnMin && CoGnReal <= ref->coGnMax);
    int mandReduzida  = (CoGnReal < ref->coGnMin);
    int mandAumentada = (CoGnReal > ref->coGnMax);

    int mandMuitoReduzida  = (CoGnReal < (ref->coGnMin - 6));
    int mandMuitoAumentada = (CoGnReal > (ref->coGnMax + 6));

    int padraoIdentificado = 0;

    if (afaiNormal && sngognNormal) {
        if (anbNormal) { sprintf(strClasse, "I"); padraoIdentificado = 1; }
        else if (anbAumentado) {
            if ((maxilaNormal && mandReduzida) || (maxilaProtruida && mandNormal) ||
                (maxilaProtruida && mandReduzida) || (maxilaRetruida && mandMuitoReduzida)) {
                sprintf(strClasse, "II"); padraoIdentificado = 1;
            }
        }
        else if (anbDiminuido) {
            if ((maxilaRetruida && mandNormal) || (maxilaNormal && mandAumentada) ||
                (maxilaRetruida && mandAumentada) || (maxilaProtruida && mandMuitoAumentada)) {
                sprintf(strClasse, "III"); padraoIdentificado = 1;
            }
        }
    }
    else if (afaiDiminuida && sngognDiminuido) {
        if (anbNormal) { sprintf(strClasse, "I"); padraoIdentificado = 1; }
        else if (anbAumentado) { sprintf(strClasse, "II"); padraoIdentificado = 1; }
        else if (anbDiminuido) { sprintf(strClasse, "III"); padraoIdentificado = 1; }
    }
    else if (afaiAumentada && sngognAumentado) {
        if (anbNormal) { sprintf(strClasse, "I"); padraoIdentificado = 1; }
        else if (anbAumentado) { sprintf(strClasse, "II"); padraoIdentificado = 1; }
        else if (anbDiminuido) { sprintf(strClasse, "III"); padraoIdentificado = 1; }
    }

    /* ================================================================
     * DIAGNÓSTICO FINAL (FLUIDO E NATURAL)
     * ================================================================ */
    LINHA;
    printf("=== CONCLUSAO DO LAUDO ORTODONTICO ===\n\n");

    if (!padraoIdentificado) {
        printf("O paciente apresenta um padrão esquelético incomum, cujos dados clínicos e cefalométricos não correspondem aos perfis convencionais armazenados.\n");
    } else {
        printf("Paciente caracterizado como Padrão de Classe %s, apresentando maxila %s associada a um tamanho mandibular %s.\n",
               strClasse, strMaxila, strMandibula);
    }

    printf("No aspecto vertical, observa-se um padrão de crescimento %s.\n", strCrescimento);

    printf("Quanto ao componente dentário, os incisivos superiores encontram-se %s e %s em sua respectiva base apical.\n",
           strIncisivoSupPos, strIncisivoSupAng);

    printf("Da mesma forma, os incisivos inferiores estão %s e %s em relação à sua base apical.\n",
           strIncisivoInfPos, strIncisivoInfAng);

    printf("O perfil facial tegumentar mostra-se %s.\n", strPerfil);

    LINHA;
    return 0;
}
