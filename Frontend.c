#include <gtk/gtk.h>
#include <stdio.h>
#include "mcnamara.h"

typedef struct {
    GtkWidget *tipoMaxila;
    GtkWidget *mmMaxila;
    GtkWidget *perfil;
    GtkWidget *coa;
    GtkWidget *cogn;
    GtkWidget *afai;
    GtkWidget *anb;
    GtkWidget *sngogn;
    GtkWidget *dist1na;
    GtkWidget *ang1na;
    GtkWidget *dist1nb;
    GtkWidget *ang1nb;
    GtkWidget *resultado;
} App;

double valor(GtkWidget *campo) {
    return gtk_spin_button_get_value(GTK_SPIN_BUTTON(campo));
}

void mostrar_resultado(GtkWidget *view, const char *texto) {
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(view));
    gtk_text_buffer_set_text(buffer, texto, -1);
}

GtkWidget *criar_numero(GtkWidget *grid, const char *texto, int linha, double min, double max) {
    GtkWidget *label = gtk_label_new(texto);
    GtkAdjustment *adj = gtk_adjustment_new(0, min, max, 0.1, 1, 0);
    GtkWidget *spin = gtk_spin_button_new(adj, 0.1, 1);

    gtk_widget_set_halign(label, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), label, 0, linha, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), spin, 1, linha, 1, 1);

    return spin;
}

GtkWidget *criar_combo(GtkWidget *grid, const char *texto, int linha, const char **opcoes, int total) {
    int i;
    GtkWidget *label = gtk_label_new(texto);
    GtkWidget *combo = gtk_combo_box_text_new();

    gtk_widget_set_halign(label, GTK_ALIGN_START);

    for (i = 0; i < total; i++) {
        gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo), opcoes[i]);
    }

    gtk_combo_box_set_active(GTK_COMBO_BOX(combo), 0);

    gtk_grid_attach(GTK_GRID(grid), label, 0, linha, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), combo, 1, linha, 1, 1);

    return combo;
}

void calcular(GtkButton *botao, gpointer dados) {
    App *app = (App *) dados;

    int tipoMaxila = gtk_combo_box_get_active(GTK_COMBO_BOX(app->tipoMaxila)) + 1;
    int perfil = gtk_combo_box_get_active(GTK_COMBO_BOX(app->perfil)) + 1;

    int mmMaxila = (int) valor(app->mmMaxila);
    int coaOriginal = (int) valor(app->coa);
    int cogn = (int) valor(app->cogn);
    int afai = (int) valor(app->afai);

    double anb = valor(app->anb);
    double sngogn = valor(app->sngogn);
    double dist1na = valor(app->dist1na);
    double ang1na = valor(app->ang1na);
    double dist1nb = valor(app->dist1nb);
    double ang1nb = valor(app->ang1nb);

    int coaCorrigido = coaOriginal;

    const char *txtMaxila = "bem posicionada";
    const char *txtPerfil = "normal";
    const char *txtMandibula;
    const char *txtCrescimento;
    const char *txtClasse;
    const char *txtSupPos;
    const char *txtSupAng;
    const char *txtInfPos;
    const char *txtInfAng;

    char laudo[3000];

    if (tipoMaxila == 2) {
        coaCorrigido = coaOriginal - mmMaxila;
        txtMaxila = "protruida";
    } else if (tipoMaxila == 3) {
        coaCorrigido = coaOriginal + mmMaxila;
        txtMaxila = "retruida";
    }

    if (perfil == 1) txtPerfil = "convexo suave";
    else if (perfil == 2) txtPerfil = "reto";
    else if (perfil == 3) txtPerfil = "convexo";
    else txtPerfil = "concavo";

    const TabelaMcNamara *ref = buscarTabela(coaCorrigido);

    if (ref == NULL) {
        sprintf(laudo,
            "Erro: CoA corrigido ficou em %d mm.\n\n"
            "A tabela de McNamara aceita valores de 80 a 108 mm.",
            coaCorrigido
        );

        mostrar_resultado(app->resultado, laudo);
        return;
    }

    if (cogn < ref->coGnMin) txtMandibula = "diminuido";
    else if (cogn > ref->coGnMax) txtMandibula = "aumentado";
    else txtMandibula = "normal";

    if (afai < ref->afaiMin && sngogn < 29) {
        txtCrescimento = "horizontal";
    } else if (afai > ref->afaiMax && sngogn > 35) {
        txtCrescimento = "vertical";
    } else if (afai >= ref->afaiMin && afai <= ref->afaiMax &&
               sngogn >= 29 && sngogn <= 35) {
        txtCrescimento = "equilibrado";
    } else {
        txtCrescimento = "misto/indefinido";
    }

    if (anb >= 1 && anb <= 4) txtClasse = "Classe I esqueletica";
    else if (anb > 4) txtClasse = "Classe II esqueletica";
    else txtClasse = "Classe III esqueletica";

    if (dist1na < 3) txtSupPos = "retraidos";
    else if (dist1na > 5) txtSupPos = "protruidos";
    else txtSupPos = "bem posicionados";

    if (ang1na < 23) txtSupAng = "verticalizados";
    else if (ang1na > 25) txtSupAng = "inclinados para vestibular";
    else txtSupAng = "com inclinacao normal";

    if (dist1nb < 3) txtInfPos = "retraidos";
    else if (dist1nb > 5) txtInfPos = "protruidos";
    else txtInfPos = "bem posicionados";

    if (ang1nb < 24) txtInfAng = "verticalizados";
    else if (ang1nb > 26) txtInfAng = "inclinados para vestibular";
    else txtInfAng = "com inclinacao normal";

    sprintf(laudo,
        "DIAGNOSTICO ORTODONTICO\n\n"
        "CoA original: %d mm\n"
        "CoA corrigido: %d mm\n"
        "CoGn ideal pela tabela: %d a %d mm\n"
        "AFAI ideal pela tabela: %d a %d mm\n\n"
        "Resultado:\n"
        "%s\n"
        "Maxila: %s\n"
        "Tamanho mandibular: %s\n"
        "Padrao de crescimento: %s\n"
        "Perfil tegumentar: %s\n\n"
        "Incisivos superiores: %s e %s\n"
        "Incisivos inferiores: %s e %s\n\n"
        "Laudo final:\n"
        "Paciente com %s, maxila %s, tamanho mandibular %s, "
        "padrao de crescimento %s. Incisivos superiores %s e %s. "
        "Incisivos inferiores %s e %s. Perfil facial %s.",
        coaOriginal,
        coaCorrigido,
        ref->coGnMin,
        ref->coGnMax,
        ref->afaiMin,
        ref->afaiMax,
        txtClasse,
        txtMaxila,
        txtMandibula,
        txtCrescimento,
        txtPerfil,
        txtSupPos,
        txtSupAng,
        txtInfPos,
        txtInfAng,
        txtClasse,
        txtMaxila,
        txtMandibula,
        txtCrescimento,
        txtSupPos,
        txtSupAng,
        txtInfPos,
        txtInfAng,
        txtPerfil
    );

    mostrar_resultado(app->resultado, laudo);
}

int main(int argc, char *argv[]) {
    App app;

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

    gtk_init(&argc, &argv);

    GtkWidget *janela = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(janela), "Diagnostico Ortodontico");
    gtk_window_set_default_size(GTK_WINDOW(janela), 850, 650);

    GtkWidget *layout = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_set_border_width(GTK_CONTAINER(layout), 15);
    gtk_container_add(GTK_CONTAINER(janela), layout);

    GtkWidget *titulo = gtk_label_new("Diagnostico Ortodontico Esqueletico-Facial");
    gtk_box_pack_start(GTK_BOX(layout), titulo, FALSE, FALSE, 5);

    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 8);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 10);
    gtk_box_pack_start(GTK_BOX(layout), grid, FALSE, FALSE, 5);

    app.tipoMaxila = criar_combo(grid, "Maxila:", 0, opcoesMaxila, 3);
    app.mmMaxila = criar_numero(grid, "Grau da maxila em mm:", 1, 0, 20);
    app.perfil = criar_combo(grid, "Perfil tegumentar:", 2, opcoesPerfil, 4);

    app.coa = criar_numero(grid, "CoA - maxila:", 3, 0, 200);
    app.cogn = criar_numero(grid, "CoGn real:", 4, 0, 250);
    app.afai = criar_numero(grid, "AFAI real:", 5, 0, 200);
    app.anb = criar_numero(grid, "ANB:", 6, -20, 30);
    app.sngogn = criar_numero(grid, "SN.GoGn:", 7, 0, 80);

    app.dist1na = criar_numero(grid, "1-NA distancia:", 8, -20, 30);
    app.ang1na = criar_numero(grid, "1.NA angulo:", 9, 0, 80);
    app.dist1nb = criar_numero(grid, "1-NB distancia:", 10, -20, 30);
    app.ang1nb = criar_numero(grid, "1.NB angulo:", 11, 0, 80);

    GtkWidget *botao = gtk_button_new_with_label("Calcular diagnostico");
    gtk_box_pack_start(GTK_BOX(layout), botao, FALSE, FALSE, 5);

    app.resultado = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(app.resultado), FALSE);
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(app.resultado), GTK_WRAP_WORD);

    GtkWidget *scroll = gtk_scrolled_window_new(NULL, NULL);
    gtk_widget_set_vexpand(scroll, TRUE);
    gtk_container_add(GTK_CONTAINER(scroll), app.resultado);
    gtk_box_pack_start(GTK_BOX(layout), scroll, TRUE, TRUE, 5);

    g_signal_connect(botao, "clicked", G_CALLBACK(calcular), &app);
    g_signal_connect(janela, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    gtk_widget_show_all(janela);
    gtk_main();

    return 0;
}
