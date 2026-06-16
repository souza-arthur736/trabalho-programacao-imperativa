#include <gtk/gtk.h>
#include <stdio.h>
#include <string.h>
#include "mcnamara.h"

typedef struct {
    GtkApplication *aplicacao;
    GtkWidget *janela;
    GtkWidget *nome, *idade, *data;
    GtkWidget *cpf, *telefonePaciente, *emailPaciente, *endereco, *dataNascimento, *observacoes;
    GtkWidget *maxila, *mmMaxila, *perfil;
    GtkWidget *coa, *cogn, *afai, *anb, *sngogn;
    GtkWidget *dist1na, *ang1na, *dist1nb, *ang1nb;
    GtkWidget *resultado, *lista;

    char ultimoLaudo[4000];
    char ultimaClasse[80];
    char ultimaMaxila[80];
    char ultimaMandibula[80];
    char ultimoCrescimento[80];
    char ultimoPerfil[80];
} App;

static void abrir_painel_diagnostico(GtkApplication *aplicacao);
static void ativar_login(GtkApplication *aplicacao, gpointer dados);
GtkWidget *criar_texto(GtkWidget *grid, const char *label, int linha);

typedef struct {
    GtkApplication *aplicacao;
    GtkWidget *janela;

    GtkWidget *emailLogin;
    GtkWidget *senhaLogin;
    GtkWidget *mensagemLogin;

    GtkWidget *nomeCadastro;
    GtkWidget *emailCadastro;
    GtkWidget *croCadastro;
    GtkWidget *telefoneCadastro;
    GtkWidget *senhaCadastro;
    GtkWidget *confirmarSenhaCadastro;
    GtkWidget *mensagemCadastro;
} LoginApp;

static void mostrar_mensagem_label(GtkWidget *label, const char *texto) {
    gtk_label_set_text(GTK_LABEL(label), texto);
}

static int limpar_texto_csv(char *texto) {
    int i;

    if (texto == NULL) {
        return 0;
    }

    for (i = 0; texto[i] != '\0'; i++) {
        if (texto[i] == ';' || texto[i] == '\n' || texto[i] == '\r') {
            texto[i] = ' ';
        }
    }

    return 1;
}

static int email_ja_cadastrado(const char *email) {
    FILE *arquivo;
    char linha[500];

    arquivo = fopen("dentistas.csv", "r");

    if (arquivo == NULL) {
        return 0;
    }

    while (fgets(linha, sizeof(linha), arquivo) != NULL) {
        char copia[500];
        char *nomeSalvo;
        char *emailSalvo;

        linha[strcspn(linha, "\r\n")] = '\0';
        strcpy(copia, linha);

        nomeSalvo = strtok(copia, ";");
        emailSalvo = strtok(NULL, ";");

        if (nomeSalvo != NULL && emailSalvo != NULL && strcmp(email, emailSalvo) == 0) {
            fclose(arquivo);
            return 1;
        }
    }

    fclose(arquivo);
    return 0;
}

static int validar_login_dentista(const char *email, const char *senha) {
    FILE *arquivo;
    char linha[500];

    arquivo = fopen("dentistas.csv", "r");

    if (arquivo == NULL) {
        return 0;
    }

    while (fgets(linha, sizeof(linha), arquivo) != NULL) {
        char copia[500];
        char *nomeSalvo;
        char *emailSalvo;
        char *croSalvo;
        char *telefoneSalvo;
        char *senhaSalva;

        linha[strcspn(linha, "\r\n")] = '\0';
        strcpy(copia, linha);

        nomeSalvo = strtok(copia, ";");
        emailSalvo = strtok(NULL, ";");
        croSalvo = strtok(NULL, ";");
        telefoneSalvo = strtok(NULL, ";");
        senhaSalva = strtok(NULL, ";");

        if (nomeSalvo != NULL &&
            emailSalvo != NULL &&
            croSalvo != NULL &&
            telefoneSalvo != NULL &&
            senhaSalva != NULL &&
            strcmp(email, emailSalvo) == 0 &&
            strcmp(senha, senhaSalva) == 0) {

            fclose(arquivo);
            return 1;
        }
    }

    fclose(arquivo);
    return 0;
}

static GtkWidget *criar_campo_senha(GtkWidget *grid, const char *label, int linha) {
    GtkWidget *texto = gtk_label_new(label);
    GtkWidget *entry = gtk_entry_new();

    gtk_entry_set_visibility(GTK_ENTRY(entry), FALSE);
    gtk_entry_set_invisible_char(GTK_ENTRY(entry), '*');

    gtk_widget_set_halign(texto, GTK_ALIGN_START);
    gtk_widget_set_hexpand(entry, TRUE);

    gtk_grid_attach(GTK_GRID(grid), texto, 0, linha, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), entry, 1, linha, 1, 1);

    return entry;
}

static void ao_login_dentista(GtkButton *botao, gpointer dados) {
    LoginApp *login = (LoginApp *) dados;

    const char *email = gtk_editable_get_text(GTK_EDITABLE(login->emailLogin));
    const char *senha = gtk_editable_get_text(GTK_EDITABLE(login->senhaLogin));

    if (strlen(email) == 0 || strlen(senha) == 0) {
        mostrar_mensagem_label(login->mensagemLogin, "Digite email e senha para entrar.");
        return;
    }

    if (!emailValido(email)) {
        mostrar_mensagem_label(login->mensagemLogin, "EMAIL INVALIDO | USE UM FORMATO COMO nome@dominio.com");
        return;
    }

    if (!validar_login_dentista(email, senha)) {
        mostrar_mensagem_label(login->mensagemLogin, "EMAIL OU SENHA INCORRETOS | TENTE NOVAMENTE");
        return;
    }

    gtk_window_destroy(GTK_WINDOW(login->janela));
    abrir_painel_diagnostico(login->aplicacao);
}

static void ao_cadastrar_dentista(GtkButton *botao, gpointer dados) {
    LoginApp *login = (LoginApp *) dados;
    FILE *arquivo;

    char nome[120];
    char email[120];
    char cro[60];
    char telefone[60];
    char senha[80];
    char confirmarSenha[80];

    strcpy(nome, gtk_editable_get_text(GTK_EDITABLE(login->nomeCadastro)));
    strcpy(email, gtk_editable_get_text(GTK_EDITABLE(login->emailCadastro)));
    strcpy(cro, gtk_editable_get_text(GTK_EDITABLE(login->croCadastro)));
    strcpy(telefone, gtk_editable_get_text(GTK_EDITABLE(login->telefoneCadastro)));
    strcpy(senha, gtk_editable_get_text(GTK_EDITABLE(login->senhaCadastro)));
    strcpy(confirmarSenha, gtk_editable_get_text(GTK_EDITABLE(login->confirmarSenhaCadastro)));

    limpar_texto_csv(nome);
    limpar_texto_csv(email);
    limpar_texto_csv(cro);
    limpar_texto_csv(telefone);
    limpar_texto_csv(senha);
    limpar_texto_csv(confirmarSenha);

    if (strlen(nome) == 0 || strlen(email) == 0 || strlen(cro) == 0 ||
        strlen(telefone) == 0 || strlen(senha) == 0 || strlen(confirmarSenha) == 0) {
        mostrar_mensagem_label(login->mensagemCadastro, "Preencha todos os campos para cadastrar.");
        return;
    }

    if (!nomeValido(nome)) {
        mostrar_mensagem_label(login->mensagemCadastro, "O NOME DEVE CONTER APENAS LETRAS | TENTE NOVAMENTE");
        return;
    }

    if (!emailValido(email)) {
        mostrar_mensagem_label(login->mensagemCadastro, "EMAIL INVALIDO | USE UM FORMATO COMO nome@dominio.com");
        return;
    }

    if (email_ja_cadastrado(email)) {
        mostrar_mensagem_label(login->mensagemCadastro, "EMAIL JA CADASTRADO | TENTE OUTRO EMAIL");
        return;
    }

    if (!senhaValida(senha)) {
        mostrar_mensagem_label(login->mensagemCadastro,
            "ESSA SENHA NAO SEGUE AOS CRITERIOS REQUISITADOS | TENTE NOVAMENTE");
        return;
    }

    if (strcmp(senha, confirmarSenha) != 0) {
        mostrar_mensagem_label(login->mensagemCadastro, "AS SENHAS NAO COINCIDEM | TENTE NOVAMENTE");
        return;
    }

    arquivo = fopen("dentistas.csv", "a");

    if (arquivo == NULL) {
        mostrar_mensagem_label(login->mensagemCadastro, "Erro ao salvar dentista em dentistas.csv.");
        return;
    }

    fprintf(arquivo, "%s;%s;%s;%s;%s\n", nome, email, cro, telefone, senha);
    fclose(arquivo);

    gtk_editable_set_text(GTK_EDITABLE(login->nomeCadastro), "");
    gtk_editable_set_text(GTK_EDITABLE(login->emailCadastro), "");
    gtk_editable_set_text(GTK_EDITABLE(login->croCadastro), "");
    gtk_editable_set_text(GTK_EDITABLE(login->telefoneCadastro), "");
    gtk_editable_set_text(GTK_EDITABLE(login->senhaCadastro), "");
    gtk_editable_set_text(GTK_EDITABLE(login->confirmarSenhaCadastro), "");

    mostrar_mensagem_label(login->mensagemCadastro,
        "Dentista cadastrado com sucesso. Agora faca login com email e senha.");
}

static void ativar_login(GtkApplication *aplicacao, gpointer dados) {
    LoginApp *login = g_new0(LoginApp, 1);

    login->aplicacao = aplicacao;

    login->janela = gtk_application_window_new(aplicacao);
    gtk_window_set_title(GTK_WINDOW(login->janela), "Login do Dentista");
    gtk_window_set_default_size(GTK_WINDOW(login->janela), 560, 520);

    GtkWidget *principal = gtk_box_new(GTK_ORIENTATION_VERTICAL, 12);
    gtk_widget_set_margin_top(principal, 18);
    gtk_widget_set_margin_bottom(principal, 18);
    gtk_widget_set_margin_start(principal, 18);
    gtk_widget_set_margin_end(principal, 18);
    gtk_window_set_child(GTK_WINDOW(login->janela), principal);

    GtkWidget *titulo = gtk_label_new("Sistema Odontologico - Acesso do Dentista");
    gtk_box_append(GTK_BOX(principal), titulo);

    GtkWidget *abas = gtk_notebook_new();
    gtk_widget_set_vexpand(abas, TRUE);
    gtk_box_append(GTK_BOX(principal), abas);

    GtkWidget *abaLogin = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_widget_set_margin_top(abaLogin, 14);
    gtk_widget_set_margin_bottom(abaLogin, 14);
    gtk_widget_set_margin_start(abaLogin, 14);
    gtk_widget_set_margin_end(abaLogin, 14);

    GtkWidget *gridLogin = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(gridLogin), 10);
    gtk_grid_set_column_spacing(GTK_GRID(gridLogin), 10);
    gtk_box_append(GTK_BOX(abaLogin), gridLogin);

    login->emailLogin = criar_texto(gridLogin, "Email:", 0);
    login->senhaLogin = criar_campo_senha(gridLogin, "Senha:", 1);

    GtkWidget *botaoLogin = gtk_button_new_with_label("Entrar");
    gtk_box_append(GTK_BOX(abaLogin), botaoLogin);

    login->mensagemLogin = gtk_label_new("");
    gtk_label_set_wrap(GTK_LABEL(login->mensagemLogin), TRUE);
    gtk_box_append(GTK_BOX(abaLogin), login->mensagemLogin);

    gtk_notebook_append_page(GTK_NOTEBOOK(abas), abaLogin, gtk_label_new("Login"));

    GtkWidget *abaCadastro = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_widget_set_margin_top(abaCadastro, 14);
    gtk_widget_set_margin_bottom(abaCadastro, 14);
    gtk_widget_set_margin_start(abaCadastro, 14);
    gtk_widget_set_margin_end(abaCadastro, 14);

    GtkWidget *gridCadastro = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(gridCadastro), 10);
    gtk_grid_set_column_spacing(GTK_GRID(gridCadastro), 10);
    gtk_box_append(GTK_BOX(abaCadastro), gridCadastro);

    login->nomeCadastro = criar_texto(gridCadastro, "Nome:", 0);
    login->emailCadastro = criar_texto(gridCadastro, "Email:", 1);
    login->croCadastro = criar_texto(gridCadastro, "CRO:", 2);
    login->telefoneCadastro = criar_texto(gridCadastro, "Telefone:", 3);

    GtkWidget *criterios = gtk_label_new(
        "Defina sua senha. Ela deve conter:\n"
        "- No minimo 8 caracteres\n"
        "- Ao menos 1 letra maiuscula\n"
        "- Ao menos 1 letra minuscula\n"
        "- Ao menos 1 numero\n"
        "- Ao menos 1 caractere especial (Ex: #, $, *, @, !)"
    );
    gtk_widget_set_halign(criterios, GTK_ALIGN_START);
    gtk_box_append(GTK_BOX(abaCadastro), criterios);

    GtkWidget *gridSenha = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(gridSenha), 10);
    gtk_grid_set_column_spacing(GTK_GRID(gridSenha), 10);
    gtk_box_append(GTK_BOX(abaCadastro), gridSenha);

    login->senhaCadastro = criar_campo_senha(gridSenha, "Senha:", 0);
    login->confirmarSenhaCadastro = criar_campo_senha(gridSenha, "Confirmar senha:", 1);

    GtkWidget *botaoCadastrar = gtk_button_new_with_label("Cadastrar dentista");
    gtk_box_append(GTK_BOX(abaCadastro), botaoCadastrar);

    login->mensagemCadastro = gtk_label_new("");
    gtk_label_set_wrap(GTK_LABEL(login->mensagemCadastro), TRUE);
    gtk_box_append(GTK_BOX(abaCadastro), login->mensagemCadastro);

    gtk_notebook_append_page(GTK_NOTEBOOK(abas), abaCadastro, gtk_label_new("Cadastrar dentista"));

    g_signal_connect(botaoLogin, "clicked", G_CALLBACK(ao_login_dentista), login);
    g_signal_connect(botaoCadastrar, "clicked", G_CALLBACK(ao_cadastrar_dentista), login);

    gtk_window_present(GTK_WINDOW(login->janela));
}
double pegar_numero(GtkWidget *w) {
    return gtk_spin_button_get_value(GTK_SPIN_BUTTON(w));
}

int pegar_int(GtkWidget *w) {
    return gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(w));
}

void mostrar_resultado(GtkWidget *view, const char *texto) {
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(view));
    gtk_text_buffer_set_text(buffer, texto, -1);
}

void limpar_csv(char *texto) {
    int i;
    for (i = 0; texto[i] != '\0'; i++) {
        if (texto[i] == ';' || texto[i] == '\n' || texto[i] == '\r') {
            texto[i] = ' ';
        }
    }
}

GtkWidget *criar_numero(GtkWidget *grid, const char *label, int linha,
                        double inicial, double min, double max,
                        double passo, int casas) {
    GtkWidget *texto = gtk_label_new(label);
    GtkAdjustment *adj = gtk_adjustment_new(inicial, min, max, passo, passo * 10, 0);
    GtkWidget *spin = gtk_spin_button_new(adj, passo, casas);

    gtk_widget_set_halign(texto, GTK_ALIGN_START);
    gtk_widget_set_hexpand(spin, TRUE);

    gtk_grid_attach(GTK_GRID(grid), texto, 0, linha, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), spin, 1, linha, 1, 1);

    return spin;
}

GtkWidget *criar_texto(GtkWidget *grid, const char *label, int linha) {
    GtkWidget *texto = gtk_label_new(label);
    GtkWidget *entry = gtk_entry_new();

    gtk_widget_set_halign(texto, GTK_ALIGN_START);
    gtk_widget_set_hexpand(entry, TRUE);

    gtk_grid_attach(GTK_GRID(grid), texto, 0, linha, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), entry, 1, linha, 1, 1);

    return entry;
}

GtkWidget *criar_combo(GtkWidget *grid, const char *label, int linha,
                       const char **opcoes, int total) {
    int i;
    GtkWidget *texto = gtk_label_new(label);
    GtkWidget *combo = gtk_combo_box_text_new();

    gtk_widget_set_halign(texto, GTK_ALIGN_START);

    for (i = 0; i < total; i++) {
        gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo), opcoes[i]);
    }

    gtk_combo_box_set_active(GTK_COMBO_BOX(combo), 0);
    gtk_widget_set_hexpand(combo, TRUE);

    gtk_grid_attach(GTK_GRID(grid), texto, 0, linha, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), combo, 1, linha, 1, 1);

    return combo;
}

void limpar_lista(GtkWidget *lista) {
    GtkWidget *filho;

    while ((filho = gtk_widget_get_first_child(lista)) != NULL) {
        gtk_list_box_remove(GTK_LIST_BOX(lista), filho);
    }
}

void carregar_lista(App *app) {
    FILE *arquivo;
    char linha[3000];

    limpar_lista(app->lista);

    arquivo = fopen("pacientes.csv", "r");

    if (arquivo == NULL) {
        GtkWidget *row = gtk_list_box_row_new();
        GtkWidget *label = gtk_label_new("Nenhum paciente salvo ainda.");
        gtk_list_box_row_set_child(GTK_LIST_BOX_ROW(row), label);
        gtk_list_box_append(GTK_LIST_BOX(app->lista), row);
        return;
    }

    while (fgets(linha, sizeof(linha), arquivo) != NULL) {
        char copia[3000];
        char *nome, *idade, *data, *classe;
        char textoLinha[300];
        GtkWidget *row, *label;

        linha[strcspn(linha, "\r\n")] = '\0';
        strcpy(copia, linha);

        nome = strtok(copia, ";");
        idade = strtok(NULL, ";");
        data = strtok(NULL, ";");

        /* Formato antigo:
           nome;idade;data;classe;...
           Formato novo:
           nome;idade;data;cpf;telefone;email;endereco;dataNascimento;observacoes;classe;...
        */
        classe = strtok(NULL, ";");

        if (classe != NULL && strncmp(classe, "Classe", 6) != 0) {
            int p;
            for (p = 0; p < 6; p++) {
                classe = strtok(NULL, ";");
                if (classe == NULL) {
                    break;
                }
            }
        }

        if (nome == NULL || idade == NULL || data == NULL || classe == NULL) {
            continue;
        }

        sprintf(textoLinha, "%s | %s anos | %s | %s", nome, idade, data, classe);

        row = gtk_list_box_row_new();
        label = gtk_label_new(textoLinha);
        gtk_widget_set_halign(label, GTK_ALIGN_START);
        gtk_list_box_row_set_child(GTK_LIST_BOX_ROW(row), label);
        gtk_list_box_append(GTK_LIST_BOX(app->lista), row);
    }

    fclose(arquivo);
}

int calcular(App *app) {
    MedidasOrtodonticas medidas;
    ResultadoOrtodontico resultado;
    const char *nomePaciente;

    nomePaciente = gtk_editable_get_text(GTK_EDITABLE(app->nome));

    if (!nomeValido(nomePaciente)) {
        mostrar_resultado(app->resultado, "NOME INVALIDO | O nome do paciente deve conter apenas letras.");
        return 0;
    }

    medidas.tipoMaxila = gtk_combo_box_get_active(GTK_COMBO_BOX(app->maxila)) + 1;
    medidas.perfilTegumentar = gtk_combo_box_get_active(GTK_COMBO_BOX(app->perfil)) + 1;
    medidas.mmMaxila = pegar_int(app->mmMaxila);
    medidas.CoAOriginal = pegar_int(app->coa);
    medidas.CoGnReal = pegar_int(app->cogn);
    medidas.AFAIReal = pegar_int(app->afai);
    medidas.ANB = pegar_numero(app->anb);
    medidas.SNGoGn = pegar_numero(app->sngogn);
    medidas.dist1NA = pegar_numero(app->dist1na);
    medidas.ang1NA = pegar_numero(app->ang1na);
    medidas.dist1NB = pegar_numero(app->dist1nb);
    medidas.ang1NB = pegar_numero(app->ang1nb);

    if (!calcularAnaliseOrtodontica(nomePaciente, 0, &medidas, &resultado)) {
        strcpy(app->ultimoLaudo, resultado.laudo);
        mostrar_resultado(app->resultado, app->ultimoLaudo);
        return 0;
    }

    strcpy(app->ultimoLaudo, resultado.laudo);

    sprintf(app->ultimaClasse, "Classe %s", resultado.classe);
    strcpy(app->ultimaMaxila, resultado.maxila);
    strcpy(app->ultimaMandibula, resultado.mandibula);
    strcpy(app->ultimoCrescimento, resultado.crescimento);
    strcpy(app->ultimoPerfil, resultado.perfil);

    mostrar_resultado(app->resultado, app->ultimoLaudo);
    return 1;
}

void ao_calcular(GtkButton *botao, gpointer dados) {
    App *app = (App *) dados;
    calcular(app);
}

void ao_salvar(GtkButton *botao, gpointer dados) {
    App *app = (App *) dados;
    FILE *arquivo;
    char nome[120];
    char data[40];
    char cpf[40];
    char telefone[60];
    char email[120];
    char endereco[180];
    char dataNascimento[50];
    char observacoes[400];
    char laudo[4000];

    strcpy(nome, gtk_editable_get_text(GTK_EDITABLE(app->nome)));
    strcpy(data, gtk_editable_get_text(GTK_EDITABLE(app->data)));
    strcpy(cpf, gtk_editable_get_text(GTK_EDITABLE(app->cpf)));
    strcpy(telefone, gtk_editable_get_text(GTK_EDITABLE(app->telefonePaciente)));
    strcpy(email, gtk_editable_get_text(GTK_EDITABLE(app->emailPaciente)));
    strcpy(endereco, gtk_editable_get_text(GTK_EDITABLE(app->endereco)));
    strcpy(dataNascimento, gtk_editable_get_text(GTK_EDITABLE(app->dataNascimento)));
    strcpy(observacoes, gtk_editable_get_text(GTK_EDITABLE(app->observacoes)));

    limpar_csv(nome);
    limpar_csv(data);
    limpar_csv(cpf);
    limpar_csv(telefone);
    limpar_csv(email);
    limpar_csv(endereco);
    limpar_csv(dataNascimento);
    limpar_csv(observacoes);

    if (!nomeValido(nome)) {
        mostrar_resultado(app->resultado, "NOME INVALIDO | O nome do paciente deve conter apenas letras.");
        return;
    }

    if (strlen(email) > 0 && !emailValido(email)) {
        mostrar_resultado(app->resultado, "EMAIL INVALIDO | USE UM FORMATO COMO nome@dominio.com");
        return;
    }

    if (strlen(data) == 0) {
        strcpy(data, "sem data");
    }

    if (!calcular(app)) {
        return;
    }

    strcpy(laudo, app->ultimoLaudo);
    limpar_csv(laudo);

    arquivo = fopen("pacientes.csv", "a");

    if (arquivo == NULL) {
        mostrar_resultado(app->resultado, "Erro ao salvar o arquivo pacientes.csv.");
        return;
    }

    fprintf(arquivo,
            "%s;%d;%s;%s;%s;%s;%s;%s;%s;%s;%s;%s;%s;%s;%s\n",
            nome,
            pegar_int(app->idade),
            data,
            cpf,
            telefone,
            email,
            endereco,
            dataNascimento,
            observacoes,
            app->ultimaClasse,
            app->ultimaMaxila,
            app->ultimaMandibula,
            app->ultimoCrescimento,
            app->ultimoPerfil,
            laudo);

    fclose(arquivo);

    carregar_lista(app);
    mostrar_resultado(app->resultado, "Paciente salvo com sucesso em pacientes.csv.");
}


void ao_sair_conta(GtkButton *botao, gpointer dados) {
    App *app = (App *) dados;

    gtk_window_destroy(GTK_WINDOW(app->janela));
    g_application_activate(G_APPLICATION(app->aplicacao));
}

void ao_mudar_maxila(GtkComboBox *combo, gpointer dados) {
    App *app = (App *) dados;
    int ativo = gtk_combo_box_get_active(combo);

    gtk_widget_set_sensitive(app->mmMaxila, ativo != 0);

    if (ativo == 0) {
        gtk_spin_button_set_value(GTK_SPIN_BUTTON(app->mmMaxila), 0);
    }
}

static void abrir_painel_diagnostico(GtkApplication *aplicacao) {
    App *app = g_new0(App, 1);
    app->aplicacao = aplicacao;

    const char *opcoesMaxila[] = {
        "Bem posicionada",
        "Protruida",
        "Retruida"
    };

    const char *opcoesPerfil[] = {
        "Convexo suave",
        "Reto",
        "Convexo",
        "Concavo"
    };

    GtkWidget *janela = gtk_application_window_new(aplicacao);
    app->janela = janela;
    gtk_window_set_title(GTK_WINDOW(janela), "Cadastro e Diagnostico Ortodontico");
    gtk_window_set_default_size(GTK_WINDOW(janela), 1050, 700);

    GtkWidget *principal = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 12);
    gtk_widget_set_margin_top(principal, 12);
    gtk_widget_set_margin_bottom(principal, 12);
    gtk_widget_set_margin_start(principal, 12);
    gtk_widget_set_margin_end(principal, 12);
    gtk_window_set_child(GTK_WINDOW(janela), principal);

    GtkWidget *lateral = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    gtk_widget_set_size_request(lateral, 320, -1);
    gtk_box_append(GTK_BOX(principal), lateral);

    GtkWidget *labelLista = gtk_label_new("Pacientes salvos");
    gtk_box_append(GTK_BOX(lateral), labelLista);

    app->lista = gtk_list_box_new();

    GtkWidget *scrollLista = gtk_scrolled_window_new();
    gtk_widget_set_vexpand(scrollLista, TRUE);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrollLista), app->lista);
    gtk_box_append(GTK_BOX(lateral), scrollLista);

    GtkWidget *conteudo = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_widget_set_hexpand(conteudo, TRUE);
    gtk_box_append(GTK_BOX(principal), conteudo);

    GtkWidget *titulo = gtk_label_new("Diagnostico Ortodontico Esqueletico-Facial");
    gtk_box_append(GTK_BOX(conteudo), titulo);

    GtkWidget *gridPaciente = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(gridPaciente), 8);
    gtk_grid_set_column_spacing(GTK_GRID(gridPaciente), 10);
    gtk_box_append(GTK_BOX(conteudo), gridPaciente);

    app->nome = criar_texto(gridPaciente, "Nome:", 0);
    app->idade = criar_numero(gridPaciente, "Idade:", 1, 18, 0, 120, 1, 0);
    app->data = criar_texto(gridPaciente, "Data do exame:", 2);

    app->cpf = criar_texto(gridPaciente, "CPF:", 3);
    app->telefonePaciente = criar_texto(gridPaciente, "Telefone:", 4);
    app->emailPaciente = criar_texto(gridPaciente, "Email:", 5);
    app->endereco = criar_texto(gridPaciente, "Endereco:", 6);
    app->dataNascimento = criar_texto(gridPaciente, "Data de nascimento:", 7);
    app->observacoes = criar_texto(gridPaciente, "Observacoes:", 8);

    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 8);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 10);
    gtk_box_append(GTK_BOX(conteudo), grid);

    app->maxila = criar_combo(grid, "Maxila:", 0, opcoesMaxila, 3);
    app->mmMaxila = criar_numero(grid, "Grau da maxila em mm:", 1, 0, 0, 99, 1, 0);
    gtk_widget_set_sensitive(app->mmMaxila, FALSE);

    app->perfil = criar_combo(grid, "Perfil tegumentar:", 2, opcoesPerfil, 4);
    app->coa = criar_numero(grid, "CoA - maxila:", 3, 90, 0, 200, 1, 0);
    app->cogn = criar_numero(grid, "CoGn real:", 4, 116, 0, 250, 1, 0);
    app->afai = criar_numero(grid, "AFAI real:", 5, 64, 0, 200, 1, 0);
    app->anb = criar_numero(grid, "ANB:", 6, 2, -20, 30, 0.1, 1);
    app->sngogn = criar_numero(grid, "SN.GoGn:", 7, 32, 0, 80, 0.1, 1);
    app->dist1na = criar_numero(grid, "1-NA distancia:", 8, 4, -20, 30, 0.1, 1);
    app->ang1na = criar_numero(grid, "1.NA angulo:", 9, 24, 0, 80, 0.1, 1);
    app->dist1nb = criar_numero(grid, "1-NB distancia:", 10, 4, -20, 30, 0.1, 1);
    app->ang1nb = criar_numero(grid, "1.NB angulo:", 11, 25, 0, 80, 0.1, 1);

    GtkWidget *botoes = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    gtk_box_append(GTK_BOX(conteudo), botoes);

    GtkWidget *botaoCalcular = gtk_button_new_with_label("Calcular diagnostico");
    GtkWidget *botaoSalvar = gtk_button_new_with_label("Salvar paciente");
    GtkWidget *botaoSairConta = gtk_button_new_with_label("Sair da conta");

    gtk_box_append(GTK_BOX(botoes), botaoCalcular);
    gtk_box_append(GTK_BOX(botoes), botaoSalvar);
    gtk_box_append(GTK_BOX(botoes), botaoSairConta);

    app->resultado = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(app->resultado), FALSE);
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(app->resultado), GTK_WRAP_WORD);

    GtkWidget *scrollResultado = gtk_scrolled_window_new();
    gtk_widget_set_vexpand(scrollResultado, TRUE);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrollResultado), app->resultado);
    gtk_box_append(GTK_BOX(conteudo), scrollResultado);

    g_signal_connect(app->maxila, "changed", G_CALLBACK(ao_mudar_maxila), app);
    g_signal_connect(botaoCalcular, "clicked", G_CALLBACK(ao_calcular), app);
    g_signal_connect(botaoSalvar, "clicked", G_CALLBACK(ao_salvar), app);
    g_signal_connect(botaoSairConta, "clicked", G_CALLBACK(ao_sair_conta), app);

    carregar_lista(app);

    gtk_window_present(GTK_WINDOW(janela));
}

int main(int argc, char *argv[]) {
    GtkApplication *aplicacao;
    int status;

    aplicacao = gtk_application_new("com.ortodontia.cadastro", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(aplicacao, "activate", G_CALLBACK(ativar_login), NULL);

    status = g_application_run(G_APPLICATION(aplicacao), argc, argv);
    g_object_unref(aplicacao);

    return status;
}
