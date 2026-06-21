#ifndef MCNAMARA_H
#define MCNAMARA_H

#include <gtk/gtk.h>

#define TAM_DIAGNOSTICO 4000
#define MAX_CAMPO 256

#define ARQ_DENTISTAS "dentistas.csv"
#define ARQ_PACIENTES "pacientes.csv"

typedef struct {
    int coa;
    int coGnMin;
    int coGnMax;
    int afaiMin;
    int afaiMax;
} TabelaMcNamara;

typedef struct {
    int tipoMaxila;
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
    int perfilTegumentar;
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

typedef struct {
    char nome[120];
    char email[120];
    char cro[60];
    char telefone[60];
    char senha[80];
} Dentista;

typedef struct {
    int linhaArquivo;
    char dentistaEmail[120];
    char dentistaNome[120];
    char nome[120];
    int idade;
    char dataExame[40];
    char cpf[40];
    char telefone[60];
    char email[120];
    char endereco[180];
    char dataNascimento[50];
    char observacoes[400];
    MedidasOrtodonticas medidas;
    char classe[80];
    char maxila[80];
    char mandibula[80];
    char crescimento[80];
    char perfil[80];
    char laudo[TAM_DIAGNOSTICO];
} Paciente;

const TabelaMcNamara *buscarTabela(int coa);
int calcularAnaliseOrtodontica(const char *nomePaciente, int idPaciente,
                               const MedidasOrtodonticas *medidas,
                               ResultadoOrtodontico *resultado);

int senhaValida(const char *senha);
int emailValido(const char *email);
int nomeValido(const char *nome);
int somenteDigitos(const char *texto);
int cpfValidoSimples(const char *cpf);
int telefoneValidoSimples(const char *telefone);
int dataValidaSimples(const char *data);
int croValidoSimples(const char *cro);
void limpar_csv(char *texto);
void filtrar_entry(GtkWidget *entry, const char *permitidos, int permitirLetras, int permitirDigitos);

int salvar_dentista(const Dentista *dentista, char *erro, int tamErro);
int carregar_dentista_por_login(const char *email, const char *senha, Dentista *dentista);
int carregar_dentista_por_email(const char *email, Dentista *dentista);
int atualizar_dentista(const char *emailOriginal, const Dentista *novo, char *erro, int tamErro);
int email_ja_cadastrado(const char *email);

int salvar_paciente(const Paciente *paciente, char *erro, int tamErro);
int atualizar_paciente_linha(int linhaArquivo, const Paciente *paciente, char *erro, int tamErro);
int carregar_paciente_por_linha(int linhaArquivo, Paciente *paciente);

void iniciar_login(GtkApplication *app);
void abrir_painel(GtkApplication *app, const Dentista *dentista);

#endif
