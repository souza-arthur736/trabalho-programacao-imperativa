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
    GtkWidget *stack;


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
void ao_mudar_nome(GtkEditable *editable, gpointer dados) {
    const char *texto = gtk_editable_get_text(editable);
    char sem_numeros[120] = "";
    int j = 0;


    for (int i = 0; texto[i] != '\0' && j < 119; i++) {
        if (!g_ascii_isdigit(texto[i])) {
            sem_numeros[j++] = texto[i];
        }
    }
    sem_numeros[j] = '\0';

    if (g_strcmp0(texto, sem_numeros) != 0) {
        gtk_editable_set_text(editable, sem_numeros);
        gtk_editable_set_position(editable, -1); // Mantém o cursor no final
    }
}

void ao_mudar_data(GtkEditable *editable, gpointer dados) {
    const char *texto = gtk_editable_get_text(editable);
    char apenas_numeros[9] = "";
    int j = 0;

    for (int i = 0; texto[i] != '\0' && j < 8; i++) {
        if (g_ascii_isdigit(texto[i])) {
            apenas_numeros[j++] = texto[i];
        }
    }
    apenas_numeros[j] = '\0';

    char formatado[11] = "";
    int pos = 0;
    for (int i = 0; i < j; i++) {
        if (i == 2 || i == 4) {
            formatado[pos++] = '/';
        }
        formatado[pos++] = apenas_numeros[i];
    }
    formatado[pos] = '\0';

    if (g_strcmp0(texto, formatado) != 0) {
        gtk_editable_set_text(editable, formatado);
        gtk_editable_set_position(editable, -1);
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

    gtk_spin_button_set_numeric(GTK_SPIN_BUTTON(spin), TRUE);

    gtk_grid_attach(GTK_GRID(grid), texto, 0, linha, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), spin, 1, linha, 1, 1);

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


void carregar_lista(App *app) {
    FILE *arquivo;
    char linha[3000];


    GtkWidget *filho = gtk_widget_get_first_child(app->lista);
    while (filho != NULL) {
        GtkWidget *proximo = gtk_widget_get_next_sibling(filho);
        gtk_list_box_remove(GTK_LIST_BOX(app->lista), filho);
        filho = proximo;
    }

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

        nome   = strtok(copia, ";");
        idade  = strtok(NULL, ";");
        data   = strtok(NULL, ";");
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
    int tipoMaxila = gtk_combo_box_get_active(GTK_COMBO_BOX(app->maxila)) + 1;
    int tipoPerfil = gtk_combo_box_get_active(GTK_COMBO_BOX(app->perfil)) + 1;

    int mmMaxila     = pegar_int(app->mmMaxila);
    int coaOriginal  = pegar_int(app->coa);
    int cogn         = pegar_int(app->cogn);
    int afai         = pegar_int(app->afai);
    int coaCorrigido = coaOriginal;

    double anb    = pegar_numero(app->anb);
    double sngogn = pegar_numero(app->sngogn);
    double dist1na = pegar_numero(app->dist1na);
    double ang1na  = pegar_numero(app->ang1na);
    double dist1nb = pegar_numero(app->dist1nb);
    double ang1nb  = pegar_numero(app->ang1nb);

    const TabelaMcNamara *ref;

    char classe[80];
    char maxila[80]    = "bem posicionada";
    char mandibula[80];
    char crescimento[80];
    char perfil[80];
    char supPos[80], supAng[80], infPos[80], infAng[80];

    if (tipoMaxila == 2) {
        coaCorrigido = coaOriginal - mmMaxila;
        strcpy(maxila, "protruida");
    } else if (tipoMaxila == 3) {
        coaCorrigido = coaOriginal + mmMaxila;
        strcpy(maxila, "retruida");
    }

    if      (tipoPerfil == 1) strcpy(perfil, "convexo suave");
    else if (tipoPerfil == 2) strcpy(perfil, "reto");
    else if (tipoPerfil == 3) strcpy(perfil, "convexo");
    else                      strcpy(perfil, "concavo");

    ref = buscarTabela(coaCorrigido);

    if (ref == NULL) {
        sprintf(app->ultimoLaudo,
                "Erro: CoA corrigido ficou em %d.\n\n"
                "A tabela de McNamara aceita valores de 80 a 108.",
                coaCorrigido);
        mostrar_resultado(app->resultado, app->ultimoLaudo);
        return 0;
    }

    if      (cogn < ref->coGnMin) strcpy(mandibula, "diminuido");
    else if (cogn > ref->coGnMax) strcpy(mandibula, "aumentado");
    else                          strcpy(mandibula, "normal");

    if      (afai < ref->afaiMin && sngogn < 29) strcpy(crescimento, "horizontal");
    else if (afai > ref->afaiMax && sngogn > 35) strcpy(crescimento, "vertical");
    else if (afai >= ref->afaiMin && afai <= ref->afaiMax &&
             sngogn >= 29 && sngogn <= 35)        strcpy(crescimento, "equilibrado");
    else                                           strcpy(crescimento, "misto/indefinido");

    if      (anb >= 1 && anb <= 4) strcpy(classe, "Classe I");
    else if (anb > 4)              strcpy(classe, "Classe II");
    else                           strcpy(classe, "Classe III");

    if      (dist1na < 3) strcpy(supPos, "retraidos");
    else if (dist1na > 5) strcpy(supPos, "protruidos");
    else                  strcpy(supPos, "bem posicionados");

    if      (ang1na < 23) strcpy(supAng, "verticalizados");
    else if (ang1na > 25) strcpy(supAng, "inclinados para vestibular");
    else                  strcpy(supAng, "com inclinacao normal");

    if      (dist1nb < 3) strcpy(infPos, "retraidos");
    else if (dist1nb > 5) strcpy(infPos, "protruidos");
    else                  strcpy(infPos, "bem posicionados");

    if      (ang1nb < 24) strcpy(infAng, "verticalizados");
    else if (ang1nb > 26) strcpy(infAng, "inclinados para vestibular");
    else                  strcpy(infAng, "com inclinacao normal");

    strcpy(app->ultimaClasse,      classe);
    strcpy(app->ultimaMaxila,      maxila);
    strcpy(app->ultimaMandibula,   mandibula);
    strcpy(app->ultimoCrescimento, crescimento);
    strcpy(app->ultimoPerfil,      perfil);

    sprintf(app->ultimoLaudo,
            "DIAGNOSTICO ORTODONTICO\n\n"
            "CoA original: %d mm\n"
            "CoA corrigido: %d mm\n"
            "CoGn ideal: %d a %d mm\n"
            "AFAI ideal: %d a %d mm\n\n"
            "Classe esqueletica: %s\n"
            "Maxila: %s\n"
            "Tamanho mandibular: %s\n"
            "Padrao de crescimento: %s\n"
            "Perfil tegumentar: %s\n\n"
            "Incisivos superiores: %s e %s.\n"
            "Incisivos inferiores: %s e %s.\n\n"
            "LAUDO FINAL\n\n"
            "Paciente padrao de %s, com maxila %s e tamanho mandibular %s. "
            "Padrao de crescimento %s. Incisivos superiores %s e %s. "
            "Incisivos inferiores %s e %s. Perfil facial %s.",
            coaOriginal, coaCorrigido,
            ref->coGnMin, ref->coGnMax,
            ref->afaiMin, ref->afaiMax,
            classe, maxila, mandibula, crescimento, perfil,
            supPos, supAng, infPos, infAng,
            classe, maxila, mandibula, crescimento,
            supPos, supAng, infPos, infAng, perfil);

    mostrar_resultado(app->resultado, app->ultimoLaudo);
    return 1;
}

void ao_calcular(GtkButton *botao, gpointer dados) {
    App *app = (App *)dados;
    if (calcular(app)) {
        gtk_stack_set_visible_child_name(GTK_STACK(app->stack), "laudo");
    }
}

void ao_clicar_cadastrar(GtkButton *botao, gpointer dados) {
    App *app = (App *)dados;

    gtk_editable_set_text(GTK_EDITABLE(app->nome), "");
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(app->idade), 18);
    gtk_editable_set_text(GTK_EDITABLE(app->data), "");


    gtk_combo_box_set_active(GTK_COMBO_BOX(app->maxila), 0);
    gtk_combo_box_set_active(GTK_COMBO_BOX(app->perfil), 0);


    gtk_spin_button_set_value(GTK_SPIN_BUTTON(app->mmMaxila), 0);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(app->coa), 90);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(app->cogn), 116);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(app->afai), 64);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(app->anb), 2.0);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(app->sngogn), 32.0);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(app->dist1na), 4.0);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(app->ang1na), 24.0);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(app->dist1nb), 4.0);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(app->ang1nb), 25.0);


    mostrar_resultado(app->resultado, "");
    gtk_stack_set_visible_child_name(GTK_STACK(app->stack), "formulario");
}

void ao_salvar(GtkButton *botao, gpointer dados) {
    (void)botao;
    App *app = (App *)dados;
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

void ao_excluir(GtkButton *botao, gpointer dados) {
    App *app = (App *) dados;
    FILE *arquivo;
    FILE *temp;
    char nome[120];
    char linha[3000];
    int achou = 0;


    strcpy(nome, gtk_editable_get_text(GTK_EDITABLE(app->nome)));
    limpar_csv(nome);

    if (strlen(nome) == 0) {
        mostrar_resultado(app->resultado, "Digite o nome do paciente que deseja excluir.");
        return;
    }

    arquivo = fopen("pacientes.csv", "r");
    temp = fopen("temp.csv", "w");

    if (arquivo == NULL || temp == NULL) {
        mostrar_resultado(app->resultado, "Erro ao abrir o arquivo.");
        if (arquivo) fclose(arquivo);
        if (temp) fclose(temp);
        return;
    }


    while (fgets(linha, sizeof(linha), arquivo) != NULL) {
        char copia[3000];
        strcpy(copia, linha);
        char *nome_csv = strtok(copia, ";");

        if (nome_csv && g_strcmp0(nome_csv, nome) == 0) {
            achou = 1;
        } else {
            fputs(linha, temp);
        }
    }

    fclose(arquivo);
    fclose(temp);

    remove("pacientes.csv");
    rename("temp.csv", "pacientes.csv");


    carregar_lista(app);

    if (achou) {
        mostrar_resultado(app->resultado, "Paciente excluido com sucesso.");

        gtk_editable_set_text(GTK_EDITABLE(app->nome), "");
        gtk_editable_set_text(GTK_EDITABLE(app->data), "");
    } else {
        mostrar_resultado(app->resultado, "Paciente nao encontrado no sistema.");
    }
}

void ao_voltar(GtkButton *botao, gpointer dados) {
    App *app = (App *)dados;
    gtk_stack_set_visible_child_name(GTK_STACK(app->stack), "formulario");
}

void ao_selecionar_paciente(GtkListBox *box, GtkListBoxRow *row, gpointer dados) {
    if (row == NULL) return;

    App *app = (App *)dados;
    int index_alvo = gtk_list_box_row_get_index(row);

    FILE *arquivo = fopen("pacientes.csv", "r");
    if (arquivo == NULL) return;

    char linha[3000];
    int index_atual = 0;
    int achou = 0;

    while (fgets(linha, sizeof(linha), arquivo) != NULL) {
        if (index_atual == index_alvo) {
            achou = 1;
            break;
        }
        index_atual++;
    }
    fclose(arquivo);

    if (!achou) return;

    linha[strcspn(linha, "\r\n")] = '\0';

    char *nome = strtok(linha, ";");
    char *idade = strtok(NULL, ";");
    char *data = strtok(NULL, ";");
    char *classe = strtok(NULL, ";");
    char *maxila = strtok(NULL, ";");
    char *mandibula = strtok(NULL, ";");
    char *crescimento = strtok(NULL, ";");
    char *perfil = strtok(NULL, ";");
    char *laudo = strtok(NULL, ";");

    if (nome && idade && data) {

        gtk_editable_set_text(GTK_EDITABLE(app->nome), nome);
        gtk_spin_button_set_value(GTK_SPIN_BUTTON(app->idade), atoi(idade));
        gtk_editable_set_text(GTK_EDITABLE(app->data), data);

        if (laudo) {
            mostrar_resultado(app->resultado, laudo);
        }
    }
}


void ao_mudar_maxila(GtkComboBox *combo, gpointer dados) {
    App *app = (App *)dados;
    int ativo = gtk_combo_box_get_active(combo);

    gtk_widget_set_sensitive(app->mmMaxila, ativo != 0);

    if (ativo == 0) {
        gtk_spin_button_set_value(GTK_SPIN_BUTTON(app->mmMaxila), 0);
    }
}

void ao_sair(GtkButton *botao, gpointer dados) {

    GtkWindow *janela = GTK_WINDOW(gtk_widget_get_root(GTK_WIDGET(botao)));
    gtk_window_close(janela);
}


static void activate(GtkApplication *gtkapp, gpointer dados) {
    App *app = (App *)dados;

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

    GtkWidget *janela = gtk_application_window_new(gtkapp);
    gtk_window_set_title(GTK_WINDOW(janela), "Cadastro e Diagnostico Ortodontico");
    gtk_window_set_default_size(GTK_WINDOW(janela), 1050, 700);

    GtkWidget *principal = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 12);
    gtk_widget_set_margin_start(principal, 12);
    gtk_widget_set_margin_end(principal, 12);
    gtk_widget_set_margin_top(principal, 12);
    gtk_widget_set_margin_bottom(principal, 12);
    gtk_window_set_child(GTK_WINDOW(janela), principal);

    /* --- Painel lateral com lista de pacientes --- */
    GtkWidget *lateral = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    gtk_widget_set_size_request(lateral, 320, -1);
    gtk_box_append(GTK_BOX(principal), lateral);

    GtkWidget *labelLista = gtk_label_new("Pacientes salvos");
    gtk_box_append(GTK_BOX(lateral), labelLista);

    app->lista = gtk_list_box_new();
    g_signal_connect(app->lista, "row-selected", G_CALLBACK(ao_selecionar_paciente), app);

    GtkWidget *scrollLista = gtk_scrolled_window_new();
    gtk_widget_set_vexpand(scrollLista, TRUE);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrollLista), app->lista);
    gtk_box_append(GTK_BOX(lateral), scrollLista);

    /* --- Painel principal de conteudo --- */

    // 1. Criação do Gerenciador de Telas (Stack)
    app->stack = gtk_stack_new();
    gtk_widget_set_hexpand(app->stack, TRUE);
    // Efeito suave de transição entre as páginas
    gtk_stack_set_transition_type(GTK_STACK(app->stack), GTK_STACK_TRANSITION_TYPE_CROSSFADE);
    gtk_box_append(GTK_BOX(principal), app->stack);

    // =========================================================
    // PÁGINA 1: FORMULÁRIO DE DADOS
    // =========================================================
    GtkWidget *tela_form = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_stack_add_named(GTK_STACK(app->stack), tela_form, "formulario");

    GtkWidget *titulo = gtk_label_new("Diagnostico Ortodontico Esqueletico-Facial");
    gtk_box_append(GTK_BOX(tela_form), titulo);

    // Grid de dados do paciente
    GtkWidget *gridPaciente = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(gridPaciente), 8);
    gtk_grid_set_column_spacing(GTK_GRID(gridPaciente), 10);
    gtk_box_append(GTK_BOX(tela_form), gridPaciente);

    app->nome  = criar_texto(gridPaciente, "Nome:", 0);
    app->idade = criar_numero(gridPaciente, "Idade:", 1, 18, 0, 120, 1, 0);
    app->data  = criar_texto(gridPaciente, "Data do exame:", 2);

    // Grid de medidas cefalometricas
    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 8);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 10);
    gtk_box_append(GTK_BOX(tela_form), grid);

    app->maxila   = criar_combo(grid, "Maxila:", 0, opcoesMaxila, 3);
    app->mmMaxila = criar_numero(grid, "Grau da maxila em mm:", 1, 0, 0, 20, 1, 0);
    gtk_widget_set_sensitive(app->mmMaxila, FALSE);

    app->perfil  = criar_combo(grid, "Perfil tegumentar:", 2, opcoesPerfil, 4);
    app->coa     = criar_numero(grid, "CoA - maxila:",   3, 90,  0,   200, 1,   0);
    app->cogn    = criar_numero(grid, "CoGn real:",      4, 116, 0,   250, 1,   0);
    app->afai    = criar_numero(grid, "AFAI real:",      5, 64,  0,   200, 1,   0);
    app->anb     = criar_numero(grid, "ANB:",            6, 2,   -20, 30,  0.1, 1);
    app->sngogn  = criar_numero(grid, "SN.GoGn:",        7, 32,  0,   80,  0.1, 1);
    app->dist1na = criar_numero(grid, "1-NA distancia:", 8, 4,   -20, 30,  0.1, 1);
    app->ang1na  = criar_numero(grid, "1.NA angulo:",    9, 24,  0,   80,  0.1, 1);
    app->dist1nb = criar_numero(grid, "1-NB distancia:", 10, 4,  -20, 30,  0.1, 1);
    app->ang1nb  = criar_numero(grid, "1.NB angulo:",    11, 25, 0,   80,  0.1, 1);

    // Botoes do formulário
    GtkWidget *botoes = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    gtk_box_append(GTK_BOX(tela_form), botoes);

    GtkWidget *botaoCalcular = gtk_button_new_with_label("Calcular diagnostico");
    GtkWidget *botaoSalvar   = gtk_button_new_with_label("Salvar paciente");
    GtkWidget *botaoDeletar  = gtk_button_new_with_label("Excluir paciente");

    gtk_box_append(GTK_BOX(botoes), botaoCalcular);
    gtk_box_append(GTK_BOX(botoes), botaoSalvar);
    gtk_box_append(GTK_BOX(botoes), botaoDeletar);

    g_signal_connect(app->maxila, "changed", G_CALLBACK(ao_mudar_maxila), app);
    g_signal_connect(botaoCalcular, "clicked", G_CALLBACK(ao_calcular), app);
    g_signal_connect(botaoSalvar,   "clicked", G_CALLBACK(ao_salvar),   app);
    g_signal_connect(botaoDeletar,  "clicked", G_CALLBACK(ao_excluir),  app);


    // =========================================================
    // PÁGINA 2: EXIBIÇÃO DO LAUDO (TERMINAL GIGANTE)
    // =========================================================
    GtkWidget *tela_laudo = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_stack_add_named(GTK_STACK(app->stack), tela_laudo, "laudo");

    GtkWidget *titulo_laudo = gtk_label_new("Resultado do Diagnóstico");
    gtk_box_append(GTK_BOX(tela_laudo), titulo_laudo);


    app->resultado = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(app->resultado), FALSE);
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(app->resultado), GTK_WRAP_WORD);
    gtk_text_view_set_monospace(GTK_TEXT_VIEW(app->resultado), TRUE);
    gtk_widget_set_vexpand(app->resultado, TRUE);
    gtk_text_view_set_top_margin(GTK_TEXT_VIEW(app->resultado), 15);
    gtk_text_view_set_bottom_margin(GTK_TEXT_VIEW(app->resultado), 15);
    gtk_text_view_set_left_margin(GTK_TEXT_VIEW(app->resultado), 15);
    gtk_text_view_set_right_margin(GTK_TEXT_VIEW(app->resultado), 15);

    GtkWidget *scrollResultado = gtk_scrolled_window_new();
    gtk_widget_set_vexpand(scrollResultado, TRUE);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrollResultado), app->resultado);
    gtk_box_append(GTK_BOX(tela_laudo), scrollResultado);


    GtkWidget *botoes_laudo = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    gtk_widget_set_halign(botoes_laudo, GTK_ALIGN_CENTER);
    gtk_box_append(GTK_BOX(tela_laudo), botoes_laudo);

    GtkWidget *botaoVoltar    = gtk_button_new_with_label("Alterar dados");
    GtkWidget *botaoCadastrar = gtk_button_new_with_label("Cadastrar");
    GtkWidget *botaoSair      = gtk_button_new_with_label("Sair");

    gtk_box_append(GTK_BOX(botoes_laudo), botaoVoltar);
    gtk_box_append(GTK_BOX(botoes_laudo), botaoCadastrar);
    gtk_box_append(GTK_BOX(botoes_laudo), botaoSair);

    g_signal_connect(botaoVoltar,    "clicked", G_CALLBACK(ao_voltar), app);
    g_signal_connect(botaoCadastrar, "clicked", G_CALLBACK(ao_clicar_cadastrar), app);
    g_signal_connect(botaoSair,      "clicked", G_CALLBACK(ao_sair),   app);

    // =========================================================

    carregar_lista(app);

    gtk_stack_set_visible_child_name(GTK_STACK(app->stack), "laudo");

    gtk_window_present(GTK_WINDOW(janela));
}


int main(int argc, char *argv[]) {
    App app;
    memset(&app, 0, sizeof(app));

    GtkApplication *gtkapp = gtk_application_new(
        "br.com.ortodontia.diagnostico",
        G_APPLICATION_DEFAULT_FLAGS
    );

    g_signal_connect(gtkapp, "activate", G_CALLBACK(activate), &app);

    int status = g_application_run(G_APPLICATION(gtkapp), argc, argv);
    g_object_unref(gtkapp);

    return status;
}
