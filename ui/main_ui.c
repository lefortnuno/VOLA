#include <gtk/gtk.h>
#include "../modules/depenses.h"

void on_add_depense(GtkWidget *widget, gpointer data) {
    add_depense();   // ton code métier
}

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "VOLA");
    gtk_window_set_default_size(GTK_WINDOW(window), 600, 400);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    GtkWidget *button = gtk_button_new_with_label("Ajouter dépense");
    g_signal_connect(button, "clicked", G_CALLBACK(on_add_depense), NULL);

    gtk_container_add(GTK_CONTAINER(window), button);

    gtk_widget_show_all(window);
    gtk_main();
    return 0;
}
