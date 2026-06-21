#include <stdio.h>
#include <string.h>
#include "mcnamara.h"

static const TabelaMcNamara tabelaMcNamara[] = {
    {80, 97,100,57,58}, {81, 99,102,57,58}, {82,101,104,58,59},
    {83,103,106,58,59}, {84,104,107,59,60}, {85,105,108,60,62},
    {86,107,110,60,62}, {87,109,112,61,63}, {88,111,114,61,63},
    {89,112,115,62,64}, {90,113,116,63,64}, {91,115,118,63,64},
    {92,117,120,64,65}, {93,119,122,65,66}, {94,121,124,66,67},
    {95,122,125,67,69}, {96,124,127,67,69}, {97,126,129,68,70},
    {98,128,131,68,70}, {99,129,132,69,71}, {100,130,133,70,74},
    {101,132,135,71,75}, {102,134,137,72,76}, {103,136,139,73,77},
    {104,137,140,74,78}, {105,138,141,75,79}, {106,139,142,76,80},
    {107,140,143,77,81}, {108,141,144,78,82}
};

const TabelaMcNamara *buscarTabela(int coa) {
    if (coa < 80 || coa > 108) return NULL;
    return &tabelaMcNamara[coa - 80];
}

static void acrescentarTexto(char destino[], const char texto[]) {
    if (strlen(destino) + strlen(texto) < TAM_DIAGNOSTICO - 1) strcat(destino, texto);
}

int calcularAnaliseOrtodontica(const char *nomePaciente,
                               int idPaciente,
                               const MedidasOrtodonticas *medidas,
                               ResultadoOrtodontico *resultado) {
    int CoACorrigido;
    const TabelaMcNamara *ref;
    char linha[700];

    int anbNormal, anbAumentado, anbDiminuido;
    int afaiNormal, afaiDiminuida, afaiAumentada;
    int sngognNormal, sngognDiminuido, sngognAumentado;
    int maxilaNormal, maxilaProtruida, maxilaRetruida;
    int mandNormal, mandReduzida, mandAumentada;
    int mandMuitoReduzida, mandMuitoAumentada;
    int padraoIdentificado = 0;

    if (medidas == NULL || resultado == NULL) {
        return 0;
    }

    memset(resultado, 0, sizeof(ResultadoOrtodontico));

    CoACorrigido = medidas->CoAOriginal;

    if (medidas->tipoMaxila == 2) {
        CoACorrigido = medidas->CoAOriginal - medidas->mmMaxila;
        if (medidas->mmMaxila == 1) {
            strcpy(resultado->maxila, "levemente protruida (1mm)");
        } else if (medidas->mmMaxila == 2) {
            strcpy(resultado->maxila, "protruida (2mm)");
        } else {
            sprintf(resultado->maxila, "muito protruida (%dmm)", medidas->mmMaxila);
        }
    } else if (medidas->tipoMaxila == 3) {
        CoACorrigido = medidas->CoAOriginal + medidas->mmMaxila;
        if (medidas->mmMaxila == 1) {
            strcpy(resultado->maxila, "levemente retruida (1mm)");
        } else if (medidas->mmMaxila == 2) {
            strcpy(resultado->maxila, "retruida (2mm)");
        } else {
            sprintf(resultado->maxila, "muito retruida (%dmm)", medidas->mmMaxila);
        }
    } else {
        strcpy(resultado->maxila, "bem posicionada");
    }

    switch (medidas->perfilTegumentar) {
        case 1: strcpy(resultado->perfil, "normal (convexo suave)"); break;
        case 2: strcpy(resultado->perfil, "reto"); break;
        case 3: strcpy(resultado->perfil, "convexo"); break;
        case 4: strcpy(resultado->perfil, "concavo"); break;
        default: strcpy(resultado->perfil, "nao informado"); break;
    }

    ref = buscarTabela(CoACorrigido);

    if (ref == NULL) {
        sprintf(resultado->laudo,
                "Erro: nao foi possivel encontrar a tabela de McNamara para CoA corrigido = %d.\n"
                "O CoA corrigido precisa estar entre 80 e 108.",
                CoACorrigido);
        resultado->CoACorrigido = CoACorrigido;
        return 0;
    }

    resultado->CoACorrigido = CoACorrigido;
    resultado->coGnMin = ref->coGnMin;
    resultado->coGnMax = ref->coGnMax;
    resultado->afaiMin = ref->afaiMin;
    resultado->afaiMax = ref->afaiMax;

    if (medidas->CoGnReal < ref->coGnMin) {
        strcpy(resultado->mandibula, "diminuido");
    } else if (medidas->CoGnReal > ref->coGnMax) {
        strcpy(resultado->mandibula, "aumentado");
    } else {
        strcpy(resultado->mandibula, "normal");
    }

    if (medidas->AFAIReal < ref->afaiMin && medidas->SNGoGn < 29.0) {
        strcpy(resultado->crescimento, "horizontal");
    } else if (medidas->AFAIReal > ref->afaiMax && medidas->SNGoGn > 35.0) {
        strcpy(resultado->crescimento, "vertical");
    } else if ((medidas->AFAIReal >= ref->afaiMin && medidas->AFAIReal <= ref->afaiMax) &&
               (medidas->SNGoGn >= 29.0 && medidas->SNGoGn <= 35.0)) {
        strcpy(resultado->crescimento, "equilibrado");
    } else {
        strcpy(resultado->crescimento, "misto/indefinido");
    }

    if (medidas->dist1NA < 3.0) {
        strcpy(resultado->incisivoSupPos, "retruidos");
    } else if (medidas->dist1NA > 5.0) {
        strcpy(resultado->incisivoSupPos, "protruidos");
    } else {
        strcpy(resultado->incisivoSupPos, "bem posicionados");
    }

    if (medidas->ang1NA < 23.0) {
        strcpy(resultado->incisivoSupAng, "verticalizados");
    } else if (medidas->ang1NA > 25.0) {
        strcpy(resultado->incisivoSupAng, "inclinados para vestibular");
    } else {
        strcpy(resultado->incisivoSupAng, "com inclinacao normal");
    }

    if (medidas->dist1NB < 3.0) {
        strcpy(resultado->incisivoInfPos, "retruidos");
    } else if (medidas->dist1NB > 5.0) {
        strcpy(resultado->incisivoInfPos, "protruidos");
    } else {
        strcpy(resultado->incisivoInfPos, "bem posicionados");
    }

    if (medidas->ang1NB < 24.0) {
        strcpy(resultado->incisivoInfAng, "verticalizados");
    } else if (medidas->ang1NB > 26.0) {
        strcpy(resultado->incisivoInfAng, "inclinados para vestibular");
    } else {
        strcpy(resultado->incisivoInfAng, "com inclinacao normal");
    }

    anbNormal = (medidas->ANB >= 1.0 && medidas->ANB <= 4.0);
    anbAumentado = (medidas->ANB > 4.0);
    anbDiminuido = (medidas->ANB < 1.0);

    afaiNormal = (medidas->AFAIReal >= ref->afaiMin && medidas->AFAIReal <= ref->afaiMax);
    afaiDiminuida = (medidas->AFAIReal < ref->afaiMin);
    afaiAumentada = (medidas->AFAIReal > ref->afaiMax);

    sngognNormal = (medidas->SNGoGn >= 29.0 && medidas->SNGoGn <= 35.0);
    sngognDiminuido = (medidas->SNGoGn < 29.0);
    sngognAumentado = (medidas->SNGoGn > 35.0);

    maxilaNormal = (medidas->tipoMaxila == 1);
    maxilaProtruida = (medidas->tipoMaxila == 2);
    maxilaRetruida = (medidas->tipoMaxila == 3);

    mandNormal = (medidas->CoGnReal >= ref->coGnMin && medidas->CoGnReal <= ref->coGnMax);
    mandReduzida = (medidas->CoGnReal < ref->coGnMin);
    mandAumentada = (medidas->CoGnReal > ref->coGnMax);

    /* "Muito reduzida/aumentada" usa o mesmo deslocamento aplicado na correcao
       da maxila (mmMaxila), pois e esse excesso que justifica a classificacao.
       Garante simetria com a correcao do CoA feita no passo 2. */
    mandMuitoReduzida = (medidas->CoGnReal < (ref->coGnMin - medidas->mmMaxila));
    mandMuitoAumentada = (medidas->CoGnReal > (ref->coGnMax + medidas->mmMaxila));

    strcpy(resultado->classe, "incomum");

    if (afaiNormal && sngognNormal) {
        if (anbNormal) {
            strcpy(resultado->classe, "I");
            padraoIdentificado = 1;
        } else if (anbAumentado) {
            if ((maxilaNormal && mandReduzida) || (maxilaProtruida && mandNormal) ||
                (maxilaProtruida && mandReduzida) || (maxilaRetruida && mandMuitoReduzida)) {
                strcpy(resultado->classe, "II");
                padraoIdentificado = 1;
            }
        } else if (anbDiminuido) {
            if ((maxilaRetruida && mandNormal) || (maxilaNormal && mandAumentada) ||
                (maxilaRetruida && mandAumentada) || (maxilaProtruida && mandMuitoAumentada)) {
                strcpy(resultado->classe, "III");
                padraoIdentificado = 1;
            }
        }
    } else if (afaiDiminuida && sngognDiminuido) {
        if (anbNormal) { strcpy(resultado->classe, "I"); padraoIdentificado = 1; }
        else if (anbAumentado) { strcpy(resultado->classe, "II"); padraoIdentificado = 1; }
        else if (anbDiminuido) { strcpy(resultado->classe, "III"); padraoIdentificado = 1; }
    } else if (afaiAumentada && sngognAumentado) {
        if (anbNormal) { strcpy(resultado->classe, "I"); padraoIdentificado = 1; }
        else if (anbAumentado) { strcpy(resultado->classe, "II"); padraoIdentificado = 1; }
        else if (anbDiminuido) { strcpy(resultado->classe, "III"); padraoIdentificado = 1; }
    } else {
        /* Casos mistos: AFAI e SN.GoGn divergem entre si (ex: AFAI aumentada
           mas SNGoGn normal, ou vice-versa). O documento instrui que o padrao
           de classe ainda e determinado pelo ANB, independente do componente
           vertical. Classificamos normalmente e registramos o crescimento como
           misto/indefinido (ja definido acima em resultado->crescimento). */
        if (anbNormal) { strcpy(resultado->classe, "I"); padraoIdentificado = 1; }
        else if (anbAumentado) { strcpy(resultado->classe, "II"); padraoIdentificado = 1; }
        else if (anbDiminuido) { strcpy(resultado->classe, "III"); padraoIdentificado = 1; }
    }

    resultado->laudo[0] = '\0';

    acrescentarTexto(resultado->laudo, "=== PERFIL ORTODONTICO ===\n\n");

    sprintf(linha, "Paciente: %s\nID: %d\n\n",
            (nomePaciente != NULL && strlen(nomePaciente) > 0) ? nomePaciente : "Nao informado",
            idPaciente);
    acrescentarTexto(resultado->laudo, linha);

    sprintf(linha,
            "CoA original: %d mm\nCoA corrigido: %d mm\nCoGn real: %d mm\nAFAI real: %d mm\nANB: %.1f graus\nSN.GoGn: %.1f graus\n\n",
            medidas->CoAOriginal, CoACorrigido, medidas->CoGnReal, medidas->AFAIReal,
            medidas->ANB, medidas->SNGoGn);
    acrescentarTexto(resultado->laudo, linha);

    acrescentarTexto(resultado->laudo, "=== ANALISE DETALHADA ===\n");

    sprintf(linha, "Maxila: %s\n", resultado->maxila);
    acrescentarTexto(resultado->laudo, linha);

    sprintf(linha, "Mandibula: %s (Paciente: %d mm | Ideal: %d a %d mm)\n",
            resultado->mandibula, medidas->CoGnReal, ref->coGnMin, ref->coGnMax);
    acrescentarTexto(resultado->laudo, linha);

    sprintf(linha, "Padrao de crescimento: %s\n", resultado->crescimento);
    acrescentarTexto(resultado->laudo, linha);

    if (medidas->ANB >= 1.0 && medidas->ANB <= 4.0) {
        sprintf(linha, "Relacao esqueletica ANB: Classe I esqueletica (%.1f graus)\n", medidas->ANB);
    } else if (medidas->ANB > 4.0) {
        sprintf(linha, "Relacao esqueletica ANB: Classe II esqueletica (%.1f graus)\n", medidas->ANB);
    } else {
        sprintf(linha, "Relacao esqueletica ANB: Classe III esqueletica (%.1f graus)\n", medidas->ANB);
    }
    acrescentarTexto(resultado->laudo, linha);

    sprintf(linha, "Incisivos superiores: %s e %s\n", resultado->incisivoSupPos, resultado->incisivoSupAng);
    acrescentarTexto(resultado->laudo, linha);

    sprintf(linha, "Incisivos inferiores: %s e %s\n", resultado->incisivoInfPos, resultado->incisivoInfAng);
    acrescentarTexto(resultado->laudo, linha);

    sprintf(linha, "Perfil tegumentar: %s\n\n", resultado->perfil);
    acrescentarTexto(resultado->laudo, linha);

    acrescentarTexto(resultado->laudo, "=== CONCLUSAO DO LAUDO ORTODONTICO ===\n\n");

    if (!padraoIdentificado) {
        acrescentarTexto(resultado->laudo,
            "O paciente apresenta um padrao esqueletico incomum, cujos dados clinicos e cefalometricos nao correspondem aos perfis convencionais armazenados.\n");
    } else {
        sprintf(linha,
            "Paciente caracterizado como Padrao de Classe %s, apresentando maxila %s associada a um tamanho mandibular %s.\n",
            resultado->classe, resultado->maxila, resultado->mandibula);
        acrescentarTexto(resultado->laudo, linha);
    }

    sprintf(linha, "No aspecto vertical, observa-se um padrao de crescimento %s.\n", resultado->crescimento);
    acrescentarTexto(resultado->laudo, linha);

    sprintf(linha,
            "Quanto ao componente dentario, os incisivos superiores encontram-se %s e %s em sua respectiva base apical.\n",
            resultado->incisivoSupPos, resultado->incisivoSupAng);
    acrescentarTexto(resultado->laudo, linha);

    sprintf(linha,
            "Da mesma forma, os incisivos inferiores estao %s e %s em relacao a sua base apical.\n",
            resultado->incisivoInfPos, resultado->incisivoInfAng);
    acrescentarTexto(resultado->laudo, linha);

    sprintf(linha, "O perfil facial tegumentar mostra-se %s.\n", resultado->perfil);
    acrescentarTexto(resultado->laudo, linha);

    return 1;
}
