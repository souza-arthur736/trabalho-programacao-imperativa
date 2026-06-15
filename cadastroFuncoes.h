#ifndef FUNCOES_H
#define FUNCOES_H

#define MAX_DENTISTAS 10
#define MAX_PACIENTES 100

typedef struct {
    int id;
    char nome[100];
    char email[100];
    char senha[50];
    char cro[30];
    char telefone[20];
} Dentista;

typedef struct {
    int id;
    int idDentista;

    char nome[100];
    char cpf[20];
    char telefone[20];
    char email[100];
    char endereco[150];
    char dataNascimento[20];
    char observacoes[300];
} Paciente;

int lerOpcao(int min, int max);
int senhaValida(char senha[]);

void cadastrarDentista(Dentista dentistas[], int *qtdDentistas);
int loginDentista(Dentista dentistas[], int qtdDentistas);
void editarPerfilDentista(Dentista dentistas[], int qtdDentistas, int idDentistaLogado);

void cadastrarPaciente(Paciente pacientes[], int *qtdPacientes, int idDentistaLogado);
void editarPaciente(Paciente pacientes[], int qtdPacientes, int idPaciente, int idDentistaLogado);

void visualizarPacientes(Paciente pacientes[], int qtdPacientes, int idDentistaLogado);
void menuPacienteSelecionado(Paciente pacientes[], int qtdPacientes, int idPaciente, int idDentistaLogado);
void mostrarPerfilPaciente(Paciente pacientes[], int qtdPacientes, int idPaciente, int idDentistaLogado);
void mostrarPerfilOrtodontico(Paciente pacientes[], int qtdPacientes, int idPaciente, int idDentistaLogado);

void menuDentista(Dentista dentistas[], int qtdDentistas,
                  Paciente pacientes[], int *qtdPacientes,
                  int idDentistaLogado);

#endif
