#include <gtk/gtk.h>
#include "mcnamara.h"

static void ativar(GtkApplication *app, gpointer dados) {
    iniciar_login(app);
}

int main(int argc, char **argv) {
    GtkApplication *app = gtk_application_new("com.ortodontia.sistema", G_APPLICATION_DEFAULT_FLAGS);
    int status;
    g_signal_connect(app, "activate", G_CALLBACK(ativar), NULL);
    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);
    return status;
}
