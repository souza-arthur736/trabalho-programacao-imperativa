#include <gtk/gtk.h>
#include <stdio.h>
#include <string.h>
#include "mcnamara.h"

typedef struct {
    GtkWidget *nome, *idade, *data;
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
        classe = strtok(NULL, ";");

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
    char laudo[4000];

    strcpy(nome, gtk_editable_get_text(GTK_EDITABLE(app->nome)));
    strcpy(data, gtk_editable_get_text(GTK_EDITABLE(app->data)));

    limpar_csv(nome);
    limpar_csv(data);

    if (strlen(nome) == 0) {
        mostrar_resultado(app->resultado, "Digite o nome do paciente antes de salvar.");
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
            "%s;%d;%s;%s;%s;%s;%s;%s;%s\n",
            nome,
            pegar_int(app->idade),
            data,
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

void ao_mudar_maxila(GtkComboBox *combo, gpointer dados) {
    App *app = (App *) dados;
    int ativo = gtk_combo_box_get_active(combo);

    gtk_widget_set_sensitive(app->mmMaxila, ativo != 0);

    if (ativo == 0) {
        gtk_spin_button_set_value(GTK_SPIN_BUTTON(app->mmMaxila), 0);
    }
}

static void ativar(GtkApplication *aplicacao, gpointer dados) {
    App *app = g_new0(App, 1);

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

    gtk_box_append(GTK_BOX(botoes), botaoCalcular);
    gtk_box_append(GTK_BOX(botoes), botaoSalvar);

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

    carregar_lista(app);

    gtk_window_present(GTK_WINDOW(janela));
}

int main(int argc, char *argv[]) {
    GtkApplication *aplicacao;
    int status;

    aplicacao = gtk_application_new("com.ortodontia.cadastro", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(aplicacao, "activate", G_CALLBACK(ativar), NULL);

    status = g_application_run(G_APPLICATION(aplicacao), argc, argv);
    g_object_unref(aplicacao);

    return status;
}
