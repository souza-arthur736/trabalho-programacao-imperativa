#include <gtk/gtk.h>
#include <string.h>
#include "mcnamara.h"

typedef struct {
    GtkApplication *app;
    GtkWidget *janela;
    GtkWidget *emailLogin, *senhaLogin, *msgLogin;
    GtkWidget *nomeCad, *emailCad, *croCad, *telCad, *senhaCad, *confCad, *msgCad;
} LoginUI;

static void filtro_nome_login(GtkEditable *e, gpointer d) { filtrar_entry(GTK_WIDGET(e), " '-", 1, 0); }
static void filtro_numero_login(GtkEditable *e, gpointer d) { filtrar_entry(GTK_WIDGET(e), ".-/()+ ", 0, 1); }
static void filtro_cro_login(GtkEditable *e, gpointer d) { filtrar_entry(GTK_WIDGET(e), ".", 0, 1); }

static GtkWidget *campo(GtkWidget *grid, const char *rotulo, int linha, int senha) {
    GtkWidget *l = gtk_label_new(rotulo), *e = gtk_entry_new();
    gtk_widget_set_halign(l, GTK_ALIGN_START);
    gtk_widget_set_hexpand(e, TRUE);
    if (senha) { gtk_entry_set_visibility(GTK_ENTRY(e), FALSE); gtk_entry_set_invisible_char(GTK_ENTRY(e), '*'); }
    gtk_grid_attach(GTK_GRID(grid), l, 0, linha, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), e, 1, linha, 1, 1);
    return e;
}

static void ao_login(GtkButton *b, gpointer dados) {
    LoginUI *ui = (LoginUI *)dados;
    Dentista d;
    const char *email = gtk_editable_get_text(GTK_EDITABLE(ui->emailLogin));
    const char *senha = gtk_editable_get_text(GTK_EDITABLE(ui->senhaLogin));
    if (!emailValido(email) || strlen(senha) == 0) { gtk_label_set_text(GTK_LABEL(ui->msgLogin), "Digite email valido e senha."); return; }
    if (!carregar_dentista_por_login(email, senha, &d)) { gtk_label_set_text(GTK_LABEL(ui->msgLogin), "Email ou senha incorretos."); return; }
    gtk_window_destroy(GTK_WINDOW(ui->janela));
    abrir_painel(ui->app, &d);
}

static void ao_cadastrar(GtkButton *b, gpointer dados) {
    LoginUI *ui = (LoginUI *)dados;
    Dentista d;
    char erro[200];
    snprintf(d.nome, sizeof d.nome, "%s", gtk_editable_get_text(GTK_EDITABLE(ui->nomeCad)));
    snprintf(d.email, sizeof d.email, "%s", gtk_editable_get_text(GTK_EDITABLE(ui->emailCad)));
    snprintf(d.cro, sizeof d.cro, "%s", gtk_editable_get_text(GTK_EDITABLE(ui->croCad)));
    snprintf(d.telefone, sizeof d.telefone, "%s", gtk_editable_get_text(GTK_EDITABLE(ui->telCad)));
    snprintf(d.senha, sizeof d.senha, "%s", gtk_editable_get_text(GTK_EDITABLE(ui->senhaCad)));
    const char *conf = gtk_editable_get_text(GTK_EDITABLE(ui->confCad));
    if (!nomeValido(d.nome)) { gtk_label_set_text(GTK_LABEL(ui->msgCad), "Nome invalido."); return; }
    if (!emailValido(d.email)) { gtk_label_set_text(GTK_LABEL(ui->msgCad), "Email invalido."); return; }
    if (!croValidoSimples(d.cro)) { gtk_label_set_text(GTK_LABEL(ui->msgCad), "CRO invalido. Use exatamente o formato 12.345."); return; }
    if (!telefoneValidoSimples(d.telefone)) { gtk_label_set_text(GTK_LABEL(ui->msgCad), "Telefone invalido: nao use letras."); return; }
    if (!senhaValida(d.senha)) { gtk_label_set_text(GTK_LABEL(ui->msgCad), "Senha fraca: use 8+ caracteres, maiuscula, minuscula, numero e especial."); return; }
    if (strcmp(d.senha, conf) != 0) { gtk_label_set_text(GTK_LABEL(ui->msgCad), "As senhas nao coincidem."); return; }
    if (!salvar_dentista(&d, erro, sizeof erro)) { gtk_label_set_text(GTK_LABEL(ui->msgCad), erro); return; }
    gtk_editable_set_text(GTK_EDITABLE(ui->nomeCad), ""); gtk_editable_set_text(GTK_EDITABLE(ui->emailCad), "");
    gtk_editable_set_text(GTK_EDITABLE(ui->croCad), ""); gtk_editable_set_text(GTK_EDITABLE(ui->telCad), "");
    gtk_editable_set_text(GTK_EDITABLE(ui->senhaCad), ""); gtk_editable_set_text(GTK_EDITABLE(ui->confCad), "");
    gtk_label_set_text(GTK_LABEL(ui->msgCad), "Dentista cadastrado. Entre pela aba Login.");
}

void iniciar_login(GtkApplication *app) {
    LoginUI *ui = g_new0(LoginUI, 1);
    ui->app = app;
    ui->janela = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(ui->janela), "Acesso do Dentista");
    gtk_window_set_default_size(GTK_WINDOW(ui->janela), 600, 540);
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 12);
    gtk_widget_set_margin_top(box, 18); gtk_widget_set_margin_bottom(box, 18); gtk_widget_set_margin_start(box, 18); gtk_widget_set_margin_end(box, 18);
    gtk_window_set_child(GTK_WINDOW(ui->janela), box);
    gtk_box_append(GTK_BOX(box), gtk_label_new("Sistema de Avaliacao Ortodontica"));
    GtkWidget *abas = gtk_notebook_new(); gtk_widget_set_vexpand(abas, TRUE); gtk_box_append(GTK_BOX(box), abas);

    GtkWidget *pgLogin = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_widget_set_margin_top(pgLogin, 14); gtk_widget_set_margin_bottom(pgLogin, 14); gtk_widget_set_margin_start(pgLogin, 14); gtk_widget_set_margin_end(pgLogin, 14);
    GtkWidget *gLogin = gtk_grid_new(); gtk_grid_set_row_spacing(GTK_GRID(gLogin), 10); gtk_grid_set_column_spacing(GTK_GRID(gLogin), 10); gtk_box_append(GTK_BOX(pgLogin), gLogin);
    ui->emailLogin = campo(gLogin, "Email:", 0, 0); gtk_entry_set_placeholder_text(GTK_ENTRY(ui->emailLogin), "conta@gmail.com"); ui->senhaLogin = campo(gLogin, "Senha:", 1, 1);
    GtkWidget *btLogin = gtk_button_new_with_label("Entrar"); gtk_box_append(GTK_BOX(pgLogin), btLogin);
    ui->msgLogin = gtk_label_new(""); gtk_label_set_wrap(GTK_LABEL(ui->msgLogin), TRUE); gtk_box_append(GTK_BOX(pgLogin), ui->msgLogin);
    gtk_notebook_append_page(GTK_NOTEBOOK(abas), pgLogin, gtk_label_new("Login"));

    GtkWidget *pgCad = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_widget_set_margin_top(pgCad, 14); gtk_widget_set_margin_bottom(pgCad, 14); gtk_widget_set_margin_start(pgCad, 14); gtk_widget_set_margin_end(pgCad, 14);
    GtkWidget *gCad = gtk_grid_new(); gtk_grid_set_row_spacing(GTK_GRID(gCad), 10); gtk_grid_set_column_spacing(GTK_GRID(gCad), 10); gtk_box_append(GTK_BOX(pgCad), gCad);
    ui->nomeCad = campo(gCad, "Nome:", 0, 0); ui->emailCad = campo(gCad, "Email:", 1, 0); gtk_entry_set_placeholder_text(GTK_ENTRY(ui->emailCad), "conta@gmail.com");
    ui->croCad = campo(gCad, "CRO:", 2, 0); gtk_entry_set_placeholder_text(GTK_ENTRY(ui->croCad), "12.345"); ui->telCad = campo(gCad, "Telefone:", 3, 0); gtk_entry_set_placeholder_text(GTK_ENTRY(ui->telCad), "81912345678");
    g_signal_connect(ui->nomeCad, "changed", G_CALLBACK(filtro_nome_login), NULL);
    g_signal_connect(ui->croCad, "changed", G_CALLBACK(filtro_cro_login), NULL);
    g_signal_connect(ui->telCad, "changed", G_CALLBACK(filtro_numero_login), NULL);
    ui->senhaCad = campo(gCad, "Senha:", 4, 1); ui->confCad = campo(gCad, "Confirmar senha:", 5, 1);
    gtk_box_append(GTK_BOX(pgCad), gtk_label_new("Senha: minimo 8 caracteres, maiuscula, minuscula, numero e especial."));
    GtkWidget *btCad = gtk_button_new_with_label("Cadastrar dentista"); gtk_box_append(GTK_BOX(pgCad), btCad);
    ui->msgCad = gtk_label_new(""); gtk_label_set_wrap(GTK_LABEL(ui->msgCad), TRUE); gtk_box_append(GTK_BOX(pgCad), ui->msgCad);
    gtk_notebook_append_page(GTK_NOTEBOOK(abas), pgCad, gtk_label_new("Cadastrar"));

    g_signal_connect(btLogin, "clicked", G_CALLBACK(ao_login), ui);
    g_signal_connect(btCad, "clicked", G_CALLBACK(ao_cadastrar), ui);
    gtk_window_present(GTK_WINDOW(ui->janela));
}
