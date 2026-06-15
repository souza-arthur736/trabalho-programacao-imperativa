#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "mcnamara.h"

static const TabelaMcNamara tabelaMcNamara[] = {
    {80, 97,100,57,58},
    {81, 99,102,57,58},
    {82,101,104,58,59},
    {83,103,106,58,59},
    {84,104,107,59,60},
    {85,105,108,60,62},
    {86,107,110,60,62},
    {87,109,112,61,63},
    {88,111,114,61,63},
    {89,112,115,62,64},
    {90,113,116,63,64},
    {91,115,118,63,64},
    {92,117,120,64,65},
    {93,119,122,65,66},
    {94,121,124,66,67},
    {95,122,125,67,69},
    {96,124,127,67,69},
    {97,126,129,68,70},
    {98,128,131,68,70},
    {99,129,132,69,71},
    {100,130,133,70,74},
    {101,132,135,71,75},
    {102,134,137,72,76},
    {103,136,139,73,77},
    {104,137,140,74,78},
    {105,138,141,75,79},
    {106,139,142,76,80},
    {107,140,143,77,81},
    {108,141,144,78,82}
};

const TabelaMcNamara* buscarTabela(int coa) {
    if (coa < 80 || coa > 108) {
        return NULL;
    }

    return &tabelaMcNamara[coa - 80];
}

static void acrescentarTexto(char destino[], const char texto[]) {
    if (strlen(destino) + strlen(texto) < TAM_DIAGNOSTICO - 1) {
        strcat(destino, texto);
    }
}

int lerOpcao(int min, int max) {
    int opcao;
    int resultado;

    while (1) {
        resultado = scanf("%d", &opcao);

        if (resultado == 1 && opcao >= min && opcao <= max) {
            while (getchar() != '\n');
            return opcao;
        }

        printf("\nOPCAO INVALIDA | TENTE NOVAMENTE\n\n");
        printf("Escolha uma opcao: ");

        while (getchar() != '\n');
    }
}

int lerIntMin(const char *prompt, int minVal) {
    int valor;
    int resultado;

    printf("%s", prompt);

    while (1) {
        resultado = scanf("%d", &valor);

        if (resultado == 1 && valor >= minVal) {
            while (getchar() != '\n');
            return valor;
        }

        printf("Valor invalido. Digite novamente: ");
        while (getchar() != '\n');
    }
}

int lerIntRange(const char *prompt, int minVal, int maxVal) {
    int valor;
    int resultado;

    printf("%s", prompt);

    while (1) {
        resultado = scanf("%d", &valor);

        if (resultado == 1 && valor >= minVal && valor <= maxVal) {
            while (getchar() != '\n');
            return valor;
        }

        printf("Valor invalido (%d a %d). Digite novamente: ", minVal, maxVal);
        while (getchar() != '\n');
    }
}

double lerDouble(const char *prompt) {
    double valor;
    int resultado;

    printf("%s", prompt);

    while (1) {
        resultado = scanf("%lf", &valor);

        if (resultado == 1) {
            while (getchar() != '\n');
            return valor;
        }

        printf("Valor invalido. Digite novamente: ");
        while (getchar() != '\n');
    }
}

int senhaValida(char senha[]) {
    int temMaiuscula = 0;
    int temMinuscula = 0;
    int temNumero = 0;
    int temEspecial = 0;
    int i;

    if (strlen(senha) < 8) {
        return 0;
    }

    for (i = 0; senha[i] != '\0'; i++) {
        if (isupper((unsigned char)senha[i])) {
            temMaiuscula = 1;
        } else if (islower((unsigned char)senha[i])) {
            temMinuscula = 1;
        } else if (isdigit((unsigned char)senha[i])) {
            temNumero = 1;
        } else {
            temEspecial = 1;
        }
    }

    return temMaiuscula && temMinuscula && temNumero && temEspecial;
}

static void cadastrarSenha(char senha[]) {
    char confirmarSenha[50];

    printf("\n========================================");
    printf("\nDEFINA SUA SENHA");
    printf("\n========================================");

    printf("\nSua senha deve conter:");
    printf("\n- No minimo 8 caracteres");
    printf("\n- Ao menos 1 letra maiuscula");
    printf("\n- Ao menos 1 letra minuscula");
    printf("\n- Ao menos 1 numero");
    printf("\n- Ao menos 1 caractere especial");
    printf("\n  (Ex: #, $, *, @, !, %% ...)\n");

    do {
        printf("\nSenha: ");
        scanf(" %[^\n]", senha);

        if (!senhaValida(senha)) {
            printf("\nESSA SENHA NAO SEGUE AOS CRITERIOS REQUISITADOS | TENTE NOVAMENTE\n");
        }

    } while (!senhaValida(senha));

    do {
        printf("\nConfirmar senha: ");
        scanf(" %[^\n]", confirmarSenha);

        if (strcmp(senha, confirmarSenha) != 0) {
            printf("\nAS SENHAS NAO COINCIDEM | TENTE NOVAMENTE\n");
        }

    } while (strcmp(senha, confirmarSenha) != 0);
}

void cadastrarDentista(Dentista dentistas[], int *qtdDentistas) {
    int i;

    if (*qtdDentistas >= MAX_DENTISTAS) {
        printf("\nLimite de dentistas atingido!\n");
        return;
    }

    i = *qtdDentistas;
    dentistas[i].id = i + 1;

    printf("\n=== CADASTRO DO DENTISTA ===\n");

    printf("Nome: ");
    scanf(" %[^\n]", dentistas[i].nome);

    printf("Email: ");
    scanf(" %[^\n]", dentistas[i].email);

    printf("CRO: ");
    scanf(" %[^\n]", dentistas[i].cro);

    printf("Telefone: ");
    scanf(" %[^\n]", dentistas[i].telefone);

    cadastrarSenha(dentistas[i].senha);

    (*qtdDentistas)++;

    printf("\nDentista cadastrado com sucesso!\n");
}

int loginDentista(Dentista dentistas[], int qtdDentistas) {
    char email[100];
    char senha[50];
    int i;

    if (qtdDentistas == 0) {
        printf("\nNenhum dentista cadastrado.\n");
        return -1;
    }

    printf("\n=== LOGIN DO DENTISTA ===\n");

    printf("Email: ");
    scanf(" %[^\n]", email);

    printf("Senha: ");
    scanf(" %[^\n]", senha);

    for (i = 0; i < qtdDentistas; i++) {
        if (strcmp(email, dentistas[i].email) == 0 &&
            strcmp(senha, dentistas[i].senha) == 0) {

            printf("\nLogin realizado com sucesso!\n");
            printf("Bem-vindo(a), Dr(a). %s!\n", dentistas[i].nome);

            return dentistas[i].id;
        }
    }

    printf("\nEmail ou senha incorretos!\n");
    return -1;
}

void editarPerfilDentista(Dentista dentistas[], int qtdDentistas, int idDentistaLogado) {
    int i;

    for (i = 0; i < qtdDentistas; i++) {
        if (dentistas[i].id == idDentistaLogado) {
            printf("\n=== EDITAR PERFIL DO DENTISTA ===\n");

            printf("Nome: ");
            scanf(" %[^\n]", dentistas[i].nome);

            printf("Email: ");
            scanf(" %[^\n]", dentistas[i].email);

            printf("CRO: ");
            scanf(" %[^\n]", dentistas[i].cro);

            printf("Telefone: ");
            scanf(" %[^\n]", dentistas[i].telefone);

            cadastrarSenha(dentistas[i].senha);

            printf("\nPerfil atualizado com sucesso!\n");
            return;
        }
    }

    printf("\nDentista nao encontrado!\n");
}

void cadastrarPaciente(Paciente pacientes[], int *qtdPacientes, int idDentistaLogado) {
    int i;

    if (*qtdPacientes >= MAX_PACIENTES) {
        printf("\nLimite de pacientes atingido!\n");
        return;
    }

    i = *qtdPacientes;

    pacientes[i].id = i + 1;
    pacientes[i].idDentista = idDentistaLogado;
    pacientes[i].temPerfilOrtodontico = 0;
    pacientes[i].perfilOrtodontico[0] = '\0';

    printf("\n=== CADASTRO DE PACIENTE ===\n");

    printf("Nome: ");
    scanf(" %[^\n]", pacientes[i].nome);

    printf("CPF: ");
    scanf(" %[^\n]", pacientes[i].cpf);

    printf("Telefone: ");
    scanf(" %[^\n]", pacientes[i].telefone);

    printf("Email: ");
    scanf(" %[^\n]", pacientes[i].email);

    printf("Endereco: ");
    scanf(" %[^\n]", pacientes[i].endereco);

    printf("Data de nascimento: ");
    scanf(" %[^\n]", pacientes[i].dataNascimento);

    printf("Observacoes: ");
    scanf(" %[^\n]", pacientes[i].observacoes);

    (*qtdPacientes)++;

    printf("\nPaciente cadastrado com sucesso!\n");
}

void editarPaciente(Paciente pacientes[], int qtdPacientes, int idPaciente, int idDentistaLogado) {
    int i;

    for (i = 0; i < qtdPacientes; i++) {
        if (pacientes[i].id == idPaciente &&
            pacientes[i].idDentista == idDentistaLogado) {

            printf("\n=== EDITAR PACIENTE ===\n");

            printf("Nome: ");
            scanf(" %[^\n]", pacientes[i].nome);

            printf("CPF: ");
            scanf(" %[^\n]", pacientes[i].cpf);

            printf("Telefone: ");
            scanf(" %[^\n]", pacientes[i].telefone);

            printf("Email: ");
            scanf(" %[^\n]", pacientes[i].email);

            printf("Endereco: ");
            scanf(" %[^\n]", pacientes[i].endereco);

            printf("Data de nascimento: ");
            scanf(" %[^\n]", pacientes[i].dataNascimento);

            printf("Observacoes: ");
            scanf(" %[^\n]", pacientes[i].observacoes);

            printf("\nPaciente atualizado com sucesso!\n");
            return;
        }
    }

    printf("\nPaciente nao encontrado!\n");
}

void mostrarPerfilPaciente(Paciente pacientes[], int qtdPacientes, int idPaciente, int idDentistaLogado) {
    int i;
    int opcao;

    for (i = 0; i < qtdPacientes; i++) {
        if (pacientes[i].id == idPaciente &&
            pacientes[i].idDentista == idDentistaLogado) {

            do {
                printf("\n=== DADOS DE PERFIL DO PACIENTE ===\n");
                printf("ID: %d\n", pacientes[i].id);
                printf("Nome: %s\n", pacientes[i].nome);
                printf("CPF: %s\n", pacientes[i].cpf);
                printf("Telefone: %s\n", pacientes[i].telefone);
                printf("Email: %s\n", pacientes[i].email);
                printf("Endereco: %s\n", pacientes[i].endereco);
                printf("Data de nascimento: %s\n", pacientes[i].dataNascimento);
                printf("Observacoes: %s\n", pacientes[i].observacoes);

                printf("\n1 - Editar paciente");
                printf("\n0 - Voltar");
                printf("\nEscolha uma opcao: ");

                opcao = lerOpcao(0, 1);

                if (opcao == 1) {
                    editarPaciente(pacientes, qtdPacientes, idPaciente, idDentistaLogado);
                }

            } while (opcao != 0);

            return;
        }
    }

    printf("\nPaciente nao encontrado!\n");
}

void cadastrarPerfilOrtodontico(Paciente pacientes[], int qtdPacientes, int idPaciente, int idDentistaLogado) {
    int i;
    int tipoMaxila;
    int mmMaxila = 0;
    int CoGnReal;
    int AFAIReal;
    int CoAOriginal;
    int CoACorrigido;
    double ANB;
    double SNGoGn;
    double dist1NA;
    double ang1NA;
    double dist1NB;
    double ang1NB;
    int perfilTegumentar;

    char strMaxila[50] = "";
    char strMandibula[50] = "";
    char strCrescimento[50] = "";
    char strIncisivoSupPos[80] = "";
    char strIncisivoSupAng[80] = "";
    char strIncisivoInfPos[80] = "";
    char strIncisivoInfAng[80] = "";
    char strPerfil[50] = "";
    char strClasse[50] = "incomum";
    char linha[500];

    const TabelaMcNamara *ref;

    for (i = 0; i < qtdPacientes; i++) {
        if (pacientes[i].id == idPaciente &&
            pacientes[i].idDentista == idDentistaLogado) {

            printf("\n=== CADASTRO DO PERFIL ORTODONTICO ===\n");
            printf("Paciente: %s\n\n", pacientes[i].nome);

            tipoMaxila = lerIntRange("Posicionamento da Maxila (1=Normal, 2=Protruida, 3=Retruida): ", 1, 3);

            if (tipoMaxila == 2) {
                mmMaxila = lerIntMin("Grau de protrusao (mm): ", 1);
                strcpy(strMaxila, "protruida");
            } else if (tipoMaxila == 3) {
                mmMaxila = lerIntMin("Grau de retrusao (mm): ", 1);
                strcpy(strMaxila, "retruida");
            } else {
                strcpy(strMaxila, "bem posicionada");
            }

            perfilTegumentar = lerIntRange("Perfil Tegumentar (1=Convexo Suave, 2=Reto, 3=Convexo, 4=Concavo): ", 1, 4);

            switch (perfilTegumentar) {
                case 1: strcpy(strPerfil, "normal (convexo suave)"); break;
                case 2: strcpy(strPerfil, "reto"); break;
                case 3: strcpy(strPerfil, "convexo"); break;
                case 4: strcpy(strPerfil, "concavo"); break;
            }

            CoAOriginal = lerIntMin("Comprimento da Maxila (CoA em mm): ", 1);
            CoGnReal = lerIntMin("Comprimento Mandibular (CoGn real em mm): ", 1);
            AFAIReal = lerIntMin("Altura Facial Inferior (AFAI real em mm): ", 1);

            printf("\nInforme os valores de Steiner e incisivos:\n");
            ANB = lerDouble("ANB (graus): ");
            SNGoGn = lerDouble("SN.GoGn (graus): ");
            dist1NA = lerDouble("Distancia 1-NA (mm): ");
            ang1NA = lerDouble("Angulo 1.NA (graus): ");
            dist1NB = lerDouble("Distancia 1-NB (mm): ");
            ang1NB = lerDouble("Angulo 1.NB (graus): ");

            CoACorrigido = CoAOriginal;

            if (tipoMaxila == 2) {
                CoACorrigido = CoAOriginal - mmMaxila;
            } else if (tipoMaxila == 3) {
                CoACorrigido = CoAOriginal + mmMaxila;
            }

            ref = buscarTabela(CoACorrigido);

            if (ref == NULL) {
                printf("\nNao foi possivel encontrar a tabela de McNamara para CoA corrigido = %d.\n", CoACorrigido);
                printf("O CoA corrigido precisa estar entre 80 e 108.\n");
                return;
            }

            if (CoGnReal < ref->coGnMin) {
                strcpy(strMandibula, "diminuido");
            } else if (CoGnReal > ref->coGnMax) {
                strcpy(strMandibula, "aumentado");
            } else {
                strcpy(strMandibula, "normal");
            }

            if (AFAIReal < ref->afaiMin && SNGoGn < 29.0) {
                strcpy(strCrescimento, "horizontal");
            } else if (AFAIReal > ref->afaiMax && SNGoGn > 35.0) {
                strcpy(strCrescimento, "vertical");
            } else if ((AFAIReal >= ref->afaiMin && AFAIReal <= ref->afaiMax) &&
                       (SNGoGn >= 29.0 && SNGoGn <= 35.0)) {
                strcpy(strCrescimento, "equilibrado");
            } else {
                strcpy(strCrescimento, "misto/indefinido");
            }

            if (dist1NA < 3.0) {
                strcpy(strIncisivoSupPos, "retruidos");
            } else if (dist1NA > 5.0) {
                strcpy(strIncisivoSupPos, "protruidos");
            } else {
                strcpy(strIncisivoSupPos, "bem posicionados");
            }

            if (ang1NA < 23.0) {
                strcpy(strIncisivoSupAng, "verticalizados");
            } else if (ang1NA > 25.0) {
                strcpy(strIncisivoSupAng, "inclinados para vestibular");
            } else {
                strcpy(strIncisivoSupAng, "com inclinacao normal");
            }

            if (dist1NB < 3.0) {
                strcpy(strIncisivoInfPos, "retruidos");
            } else if (dist1NB > 5.0) {
                strcpy(strIncisivoInfPos, "protruidos");
            } else {
                strcpy(strIncisivoInfPos, "bem posicionados");
            }

            if (ang1NB < 24.0) {
                strcpy(strIncisivoInfAng, "verticalizados");
            } else if (ang1NB > 26.0) {
                strcpy(strIncisivoInfAng, "inclinados para vestibular");
            } else {
                strcpy(strIncisivoInfAng, "com inclinacao normal");
            }

            int anbNormal = (ANB >= 1.0 && ANB <= 4.0);
            int anbAumentado = (ANB > 4.0);
            int anbDiminuido = (ANB < 1.0);

            int afaiNormal = (AFAIReal >= ref->afaiMin && AFAIReal <= ref->afaiMax);
            int afaiDiminuida = (AFAIReal < ref->afaiMin);
            int afaiAumentada = (AFAIReal > ref->afaiMax);

            int sngognNormal = (SNGoGn >= 29.0 && SNGoGn <= 35.0);
            int sngognDiminuido = (SNGoGn < 29.0);
            int sngognAumentado = (SNGoGn > 35.0);

            int maxilaNormal = (tipoMaxila == 1);
            int maxilaProtruida = (tipoMaxila == 2);
            int maxilaRetruida = (tipoMaxila == 3);

            int mandNormal = (CoGnReal >= ref->coGnMin && CoGnReal <= ref->coGnMax);
            int mandReduzida = (CoGnReal < ref->coGnMin);
            int mandAumentada = (CoGnReal > ref->coGnMax);

            int mandMuitoReduzida = (CoGnReal < (ref->coGnMin - 6));
            int mandMuitoAumentada = (CoGnReal > (ref->coGnMax + 6));

            int padraoIdentificado = 0;

            if (afaiNormal && sngognNormal) {
                if (anbNormal) {
                    strcpy(strClasse, "I");
                    padraoIdentificado = 1;
                } else if (anbAumentado) {
                    if ((maxilaNormal && mandReduzida) || (maxilaProtruida && mandNormal) ||
                        (maxilaProtruida && mandReduzida) || (maxilaRetruida && mandMuitoReduzida)) {
                        strcpy(strClasse, "II");
                        padraoIdentificado = 1;
                    }
                } else if (anbDiminuido) {
                    if ((maxilaRetruida && mandNormal) || (maxilaNormal && mandAumentada) ||
                        (maxilaRetruida && mandAumentada) || (maxilaProtruida && mandMuitoAumentada)) {
                        strcpy(strClasse, "III");
                        padraoIdentificado = 1;
                    }
                }
            } else if (afaiDiminuida && sngognDiminuido) {
                if (anbNormal) { strcpy(strClasse, "I"); padraoIdentificado = 1; }
                else if (anbAumentado) { strcpy(strClasse, "II"); padraoIdentificado = 1; }
                else if (anbDiminuido) { strcpy(strClasse, "III"); padraoIdentificado = 1; }
            } else if (afaiAumentada && sngognAumentado) {
                if (anbNormal) { strcpy(strClasse, "I"); padraoIdentificado = 1; }
                else if (anbAumentado) { strcpy(strClasse, "II"); padraoIdentificado = 1; }
                else if (anbDiminuido) { strcpy(strClasse, "III"); padraoIdentificado = 1; }
            }

            pacientes[i].perfilOrtodontico[0] = '\0';

            acrescentarTexto(pacientes[i].perfilOrtodontico, "=== PERFIL ORTODONTICO ===\n\n");

            sprintf(linha, "Paciente: %s\nID: %d\n\n", pacientes[i].nome, pacientes[i].id);
            acrescentarTexto(pacientes[i].perfilOrtodontico, linha);

            sprintf(linha, "CoA original: %d mm\nCoA corrigido: %d mm\nCoGn real: %d mm\nAFAI real: %d mm\nANB: %.1f graus\nSN.GoGn: %.1f graus\n\n",
                    CoAOriginal, CoACorrigido, CoGnReal, AFAIReal, ANB, SNGoGn);
            acrescentarTexto(pacientes[i].perfilOrtodontico, linha);

            acrescentarTexto(pacientes[i].perfilOrtodontico, "=== ANALISE DETALHADA ===\n");

            sprintf(linha, "Maxila: %s\n", strMaxila);
            acrescentarTexto(pacientes[i].perfilOrtodontico, linha);

            sprintf(linha, "Mandibula: %s (Paciente: %d mm | Ideal: %d a %d mm)\n",
                    strMandibula, CoGnReal, ref->coGnMin, ref->coGnMax);
            acrescentarTexto(pacientes[i].perfilOrtodontico, linha);

            sprintf(linha, "Padrao de crescimento: %s\n", strCrescimento);
            acrescentarTexto(pacientes[i].perfilOrtodontico, linha);

            if (ANB >= 1.0 && ANB <= 4.0) {
                sprintf(linha, "Relacao esqueletica ANB: Classe I esqueletica (%.1f graus)\n", ANB);
            } else if (ANB > 4.0) {
                sprintf(linha, "Relacao esqueletica ANB: Classe II esqueletica (%.1f graus)\n", ANB);
            } else {
                sprintf(linha, "Relacao esqueletica ANB: Classe III esqueletica (%.1f graus)\n", ANB);
            }
            acrescentarTexto(pacientes[i].perfilOrtodontico, linha);

            sprintf(linha, "Incisivos superiores: %s e %s\n", strIncisivoSupPos, strIncisivoSupAng);
            acrescentarTexto(pacientes[i].perfilOrtodontico, linha);

            sprintf(linha, "Incisivos inferiores: %s e %s\n", strIncisivoInfPos, strIncisivoInfAng);
            acrescentarTexto(pacientes[i].perfilOrtodontico, linha);

            sprintf(linha, "Perfil tegumentar: %s\n\n", strPerfil);
            acrescentarTexto(pacientes[i].perfilOrtodontico, linha);

            acrescentarTexto(pacientes[i].perfilOrtodontico, "=== CONCLUSAO DO LAUDO ORTODONTICO ===\n\n");

            if (!padraoIdentificado) {
                acrescentarTexto(pacientes[i].perfilOrtodontico,
                    "O paciente apresenta um padrao esqueletico incomum, cujos dados clinicos e cefalometricos nao correspondem aos perfis convencionais armazenados.\n");
            } else {
                sprintf(linha,
                    "Paciente caracterizado como Padrao de Classe %s, apresentando maxila %s associada a um tamanho mandibular %s.\n",
                    strClasse, strMaxila, strMandibula);
                acrescentarTexto(pacientes[i].perfilOrtodontico, linha);
            }

            sprintf(linha, "No aspecto vertical, observa-se um padrao de crescimento %s.\n", strCrescimento);
            acrescentarTexto(pacientes[i].perfilOrtodontico, linha);

            sprintf(linha,
                    "Quanto ao componente dentario, os incisivos superiores encontram-se %s e %s em sua respectiva base apical.\n",
                    strIncisivoSupPos, strIncisivoSupAng);
            acrescentarTexto(pacientes[i].perfilOrtodontico, linha);

            sprintf(linha,
                    "Da mesma forma, os incisivos inferiores estao %s e %s em relacao a sua base apical.\n",
                    strIncisivoInfPos, strIncisivoInfAng);
            acrescentarTexto(pacientes[i].perfilOrtodontico, linha);

            sprintf(linha, "O perfil facial tegumentar mostra-se %s.\n", strPerfil);
            acrescentarTexto(pacientes[i].perfilOrtodontico, linha);

            pacientes[i].temPerfilOrtodontico = 1;

            printf("\nPerfil ortodontico cadastrado com sucesso!\n");
            return;
        }
    }

    printf("\nPaciente nao encontrado!\n");
}

void visualizarPerfilOrtodontico(Paciente pacientes[], int qtdPacientes, int idPaciente, int idDentistaLogado) {
    int i;
    int opcao;

    for (i = 0; i < qtdPacientes; i++) {
        if (pacientes[i].id == idPaciente &&
            pacientes[i].idDentista == idDentistaLogado) {

            do {
                printf("\n=== PERFIL ORTODONTICO ===\n");

                if (pacientes[i].temPerfilOrtodontico == 0) {
                    printf("\nNada cadastrado.\n");
                } else {
                    printf("\n%s", pacientes[i].perfilOrtodontico);
                }

                printf("\n0 - Voltar");
                printf("\nEscolha uma opcao: ");
                opcao = lerOpcao(0, 0);

            } while (opcao != 0);

            return;
        }
    }

    printf("\nPaciente nao encontrado!\n");
}

void menuPerfilOrtodontico(Paciente pacientes[], int qtdPacientes, int idPaciente, int idDentistaLogado) {
    int opcao;

    do {
        printf("\n=== PERFIL ORTODONTICO ===\n");
        printf("1 - Cadastrar/Atualizar perfil ortodontico");
        printf("\n2 - Visualizar perfil ortodontico");
        printf("\n0 - Voltar");
        printf("\nEscolha uma opcao: ");

        opcao = lerOpcao(0, 2);

        switch (opcao) {
            case 1:
                cadastrarPerfilOrtodontico(pacientes, qtdPacientes, idPaciente, idDentistaLogado);
                break;

            case 2:
                visualizarPerfilOrtodontico(pacientes, qtdPacientes, idPaciente, idDentistaLogado);
                break;

            case 0:
                break;
        }

    } while (opcao != 0);
}

void menuPacienteSelecionado(Paciente pacientes[], int qtdPacientes, int idPaciente, int idDentistaLogado) {
    int opcao;
    int encontrou = 0;
    int i;

    for (i = 0; i < qtdPacientes; i++) {
        if (pacientes[i].id == idPaciente &&
            pacientes[i].idDentista == idDentistaLogado) {
            encontrou = 1;
            break;
        }
    }

    if (encontrou == 0) {
        printf("\nPaciente nao encontrado!\n");
        return;
    }

    do {
        printf("\n=== PACIENTE SELECIONADO ===\n");
        printf("1 - Ver dados de perfil");
        printf("\n2 - Ver perfil ortodontico");
        printf("\n0 - Voltar");
        printf("\nEscolha uma opcao: ");

        opcao = lerOpcao(0, 2);

        switch (opcao) {
            case 1:
                mostrarPerfilPaciente(pacientes, qtdPacientes, idPaciente, idDentistaLogado);
                break;

            case 2:
                menuPerfilOrtodontico(pacientes, qtdPacientes, idPaciente, idDentistaLogado);
                break;

            case 0:
                break;
        }

    } while (opcao != 0);
}

void visualizarPacientes(Paciente pacientes[], int qtdPacientes, int idDentistaLogado) {
    int idPaciente;
    int encontrou;
    int i;

    do {
        encontrou = 0;

        printf("\n=== PACIENTES CADASTRADOS ===\n");

        for (i = 0; i < qtdPacientes; i++) {
            if (pacientes[i].idDentista == idDentistaLogado) {
                printf("\nNome: %s\n", pacientes[i].nome);
                printf("ID: %d\n", pacientes[i].id);
                encontrou = 1;
            }
        }

        if (encontrou == 0) {
            printf("\nNenhum paciente cadastrado.\n");
            return;
        }

        printf("\nDigite o ID do paciente para abrir.");
        printf("\nDigite 0 para voltar ao painel do dentista.");
        printf("\nEscolha uma opcao: ");

        idPaciente = lerOpcao(0, MAX_PACIENTES);

        if (idPaciente != 0) {
            menuPacienteSelecionado(pacientes, qtdPacientes, idPaciente, idDentistaLogado);
        }

    } while (idPaciente != 0);
}

void menuDentista(Dentista dentistas[], int qtdDentistas,
                  Paciente pacientes[], int *qtdPacientes,
                  int idDentistaLogado) {
    int opcao;

    do {
        printf("\n=== PAINEL DO DENTISTA ===\n");
        printf("1 - Cadastrar paciente\n");
        printf("2 - Visualizar pacientes\n");
        printf("3 - Editar meu perfil\n");
        printf("0 - Sair da conta\n");

        printf("\nEscolha uma opcao: ");
        opcao = lerOpcao(0, 3);

        switch (opcao) {
            case 1:
                cadastrarPaciente(pacientes, qtdPacientes, idDentistaLogado);
                break;

            case 2:
                visualizarPacientes(pacientes, *qtdPacientes, idDentistaLogado);
                break;

            case 3:
                editarPerfilDentista(dentistas, qtdDentistas, idDentistaLogado);
                break;

            case 0:
                printf("\nSaindo da conta...\n");
                break;
        }

    } while (opcao != 0);
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
        strcpy(resultado->maxila, "protruida");
    } else if (medidas->tipoMaxila == 3) {
        CoACorrigido = medidas->CoAOriginal + medidas->mmMaxila;
        strcpy(resultado->maxila, "retruida");
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

    mandMuitoReduzida = (medidas->CoGnReal < (ref->coGnMin - 6));
    mandMuitoAumentada = (medidas->CoGnReal > (ref->coGnMax + 6));

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
