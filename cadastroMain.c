#include <stdio.h>
#include "cadastroFuncoes.h"

int main() {
    Dentista dentistas[MAX_DENTISTAS];
    Paciente pacientes[MAX_PACIENTES];

    int qtdDentistas = 0;
    int qtdPacientes = 0;
    int opcao;
    int idDentistaLogado;

    do {
        printf("\n=== SISTEMA ODONTOLOGICO ===\n");
        printf("1 - Cadastrar perfil do dentista\n");
        printf("2 - Login do dentista\n");
        printf("0 - Sair\n");

        printf("\nEscolha uma opcao: ");
        opcao = lerOpcao(0, 2);

        switch (opcao) {
            case 1:
                cadastrarDentista(dentistas, &qtdDentistas);
                break;

            case 2:
                idDentistaLogado = loginDentista(dentistas, qtdDentistas);

                if (idDentistaLogado != -1) {
                    menuDentista(
                        dentistas,
                        qtdDentistas,
                        pacientes,
                        &qtdPacientes,
                        idDentistaLogado
                    );
                }
                break;

            case 0:
                printf("\nEncerrando sistema...\n");
                break;
        }

    } while (opcao != 0);

    return 0;
}
