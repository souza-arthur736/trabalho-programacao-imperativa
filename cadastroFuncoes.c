#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "cadastroFuncoes.h"

int senhaValida(char senha[]) {
    int temMaiuscula = 0;
    int temMinuscula = 0;
    int temNumero = 0;
    int temEspecial = 0;

    int i;

    if(strlen(senha) < 8)
        return 0;

    for(i = 0; senha[i] != '\0'; i++) {

        if(isupper(senha[i]))
            temMaiuscula = 1;

        else if(islower(senha[i]))
            temMinuscula = 1;

        else if(isdigit(senha[i]))
            temNumero = 1;

        else
            temEspecial = 1;
    }

    return temMaiuscula &&
           temMinuscula &&
           temNumero &&
           temEspecial;
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

void cadastrarDentista(Dentista dentistas[], int *qtdDentistas) {
    int i;
    char confirmarSenha[50];

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
        scanf(" %[^\n]", dentistas[i].senha);

        if(!senhaValida(dentistas[i].senha)) {
            printf("\nESSA SENHA NAO SEGUE AOS CRITERIOS REQUISITADOS | TENTE NOVAMENTE\n");
        }

    } while(!senhaValida(dentistas[i].senha));

    do {
        printf("\nConfirmar senha: ");
        scanf(" %[^\n]", confirmarSenha);

        if (strcmp(dentistas[i].senha, confirmarSenha) != 0) {
            printf("\nAS SENHAS NAO COINCIDEM | TENTE NOVAMENTE\n");
        }

    } while (strcmp(dentistas[i].senha, confirmarSenha) != 0);

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
    char confirmarSenha[50];

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

            printf("\nCadastre sua nova senha.\n");

            do {
                printf("Senha: ");
                scanf(" %[^\n]", dentistas[i].senha);

                printf("Confirmar senha: ");
                scanf(" %[^\n]", confirmarSenha);

                if (strcmp(dentistas[i].senha, confirmarSenha) != 0) {
                    printf("\nAs senhas nao coincidem. Tente novamente.\n\n");
                }

            } while (strcmp(dentistas[i].senha, confirmarSenha) != 0);

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

void mostrarPerfilOrtodontico(Paciente pacientes[], int qtdPacientes, int idPaciente, int idDentistaLogado) {
    int i;
    int opcao;

    for (i = 0; i < qtdPacientes; i++) {
        if (pacientes[i].id == idPaciente &&
            pacientes[i].idDentista == idDentistaLogado) {

            do {
                printf("\n=== PERFIL ORTODONTICO ===\n");
                printf("Paciente: %s\n", pacientes[i].nome);
                printf("\nNada.\n");

                printf("\n0 - Voltar");
                printf("\nEscolha uma opcao: ");

                opcao = lerOpcao(0, 0);

            } while (opcao != 0);

            return;
        }
    }

    printf("\nPaciente nao encontrado!\n");
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
                mostrarPerfilOrtodontico(pacientes, qtdPacientes, idPaciente, idDentistaLogado);
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
