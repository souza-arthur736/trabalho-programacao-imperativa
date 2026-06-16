#ifndef MCNAMARA_H
#define MCNAMARA_H

#define MAX_DENTISTAS 10
#define MAX_PACIENTES 100
#define TAM_DIAGNOSTICO 4000

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

    int temPerfilOrtodontico;
    char perfilOrtodontico[TAM_DIAGNOSTICO];
} Paciente;

typedef struct {
    int coa;
    int coGnMin;
    int coGnMax;
    int afaiMin;
    int afaiMax;
} TabelaMcNamara;


typedef struct {
    int tipoMaxila;       /* 1=normal, 2=protruida, 3=retruida */
    int mmMaxila;
    int CoAOriginal;
    int CoGnReal;
    int AFAIReal;
    double ANB;
    double SNGoGn;
    double dist1NA;
    double ang1NA;
    double dist1NB;
    double ang1NB;
    int perfilTegumentar; /* 1=convexo suave, 2=reto, 3=convexo, 4=concavo */
} MedidasOrtodonticas;

typedef struct {
    int CoACorrigido;
    int coGnMin;
    int coGnMax;
    int afaiMin;
    int afaiMax;

    char classe[80];
    char maxila[80];
    char mandibula[80];
    char crescimento[80];
    char perfil[80];
    char incisivoSupPos[80];
    char incisivoSupAng[80];
    char incisivoInfPos[80];
    char incisivoInfAng[80];
    char laudo[TAM_DIAGNOSTICO];
} ResultadoOrtodontico;

const TabelaMcNamara *buscarTabela(int coa);

int calcularAnaliseOrtodontica(const char *nomePaciente,
                               int idPaciente,
                               const MedidasOrtodonticas *medidas,
                               ResultadoOrtodontico *resultado);

int lerOpcao(int min, int max);
int lerIntMin(const char *prompt, int minVal);
int lerIntRange(const char *prompt, int minVal, int maxVal);
double lerDouble(const char *prompt);
int senhaValida(char senha[]);

void cadastrarDentista(Dentista dentistas[], int *qtdDentistas);
int loginDentista(Dentista dentistas[], int qtdDentistas);
void editarPerfilDentista(Dentista dentistas[], int qtdDentistas, int idDentistaLogado);

void cadastrarPaciente(Paciente pacientes[], int *qtdPacientes, int idDentistaLogado);
void editarPaciente(Paciente pacientes[], int qtdPacientes, int idPaciente, int idDentistaLogado);

void visualizarPacientes(Paciente pacientes[], int qtdPacientes, int idDentistaLogado);
void menuPacienteSelecionado(Paciente pacientes[], int qtdPacientes, int idPaciente, int idDentistaLogado);
void mostrarPerfilPaciente(Paciente pacientes[], int qtdPacientes, int idPaciente, int idDentistaLogado);
void menuPerfilOrtodontico(Paciente pacientes[], int qtdPacientes, int idPaciente, int idDentistaLogado);
void cadastrarPerfilOrtodontico(Paciente pacientes[], int qtdPacientes, int idPaciente, int idDentistaLogado);
void visualizarPerfilOrtodontico(Paciente pacientes[], int qtdPacientes, int idPaciente, int idDentistaLogado);

void menuDentista(Dentista dentistas[], int qtdDentistas,
                  Paciente pacientes[], int *qtdPacientes,
                  int idDentistaLogado);

#endif
