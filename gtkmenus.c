#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>

#include <stdio.h>

static GtkWidget *treeview;
static GtkWidget *notebook;
static GtkWidget *info_view;
static GtkWidget *source_view;

static gchar *current_file = NULL;
static GtkWidget *headerbar;

static GtkWidget *
create_menu (gint depth)
{
  GtkWidget *menu;
  GtkRadioMenuItem *last_item;
  char buf[32];
  int i, j;

  if (depth < 1)
    return NULL;

  menu = gtk_menu_new ();
  last_item = NULL;

  for (i = 0, j = 1; i < 5; i++, j++)
    {
      GtkWidget *menu_item;

      sprintf (buf, "item %2d - %d", depth, j);

      menu_item = gtk_radio_menu_item_new_with_label_from_widget (NULL, buf);
      gtk_radio_menu_item_join_group (GTK_RADIO_MENU_ITEM (menu_item), last_item);
      last_item = GTK_RADIO_MENU_ITEM (menu_item);

      gtk_menu_shell_append (GTK_MENU_SHELL (menu), menu_item);
      gtk_widget_show (menu_item);
      if (i == 3)
        gtk_widget_set_sensitive (menu_item, FALSE);

      gtk_menu_item_set_submenu (GTK_MENU_ITEM (menu_item), create_menu (depth - 1));
    }

  return menu;
}

static void
change_orientation (GtkWidget *button,
                    GtkWidget *menubar)
{
  GtkWidget *parent;
  GtkOrientation orientation;

  parent = gtk_widget_get_parent (menubar);
  orientation = gtk_orientable_get_orientation (GTK_ORIENTABLE (parent));
  gtk_orientable_set_orientation (GTK_ORIENTABLE (parent), 1 - orientation);

  if (orientation == GTK_ORIENTATION_VERTICAL)
    g_object_set (menubar, "pack-direction", GTK_PACK_DIRECTION_TTB, NULL);
  else
    g_object_set (menubar, "pack-direction", GTK_PACK_DIRECTION_LTR, NULL);

}

static GtkWidget *window = NULL;

GtkWidget *
do_menus (GtkWidget *do_widget)
{
  GtkWidget *box;
  GtkWidget *box1;
  GtkWidget *box2;
  GtkWidget *button;

  if (!window)
    {
      GtkWidget *menubar;
      GtkWidget *menu;
      GtkWidget *menuitem;
      GtkAccelGroup *accel_group;

      window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
      gtk_window_set_screen (GTK_WINDOW (window),
                             gtk_widget_get_screen (do_widget));
      gtk_window_set_title (GTK_WINDOW (window), "Menus");
      g_signal_connect (window, "destroy",
                        G_CALLBACK(gtk_widget_destroyed), &window);

      accel_group = gtk_accel_group_new ();
      gtk_window_add_accel_group (GTK_WINDOW (window), accel_group);

      gtk_container_set_border_width (GTK_CONTAINER (window), 0);

      box = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
      gtk_container_add (GTK_CONTAINER (window), box);
      gtk_widget_show (box);

      box1 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
      gtk_container_add (GTK_CONTAINER (box), box1);
      gtk_widget_show (box1);

      menubar = gtk_menu_bar_new ();
      gtk_widget_set_hexpand (menubar, TRUE);
      gtk_box_pack_start (GTK_BOX (box1), menubar, FALSE, TRUE, 0);
      gtk_widget_show (menubar);

      menu = create_menu (2);

      menuitem = gtk_menu_item_new_with_label ("test\nline2");
      gtk_menu_item_set_submenu (GTK_MENU_ITEM (menuitem), menu);
      gtk_menu_shell_append (GTK_MENU_SHELL (menubar), menuitem);
      gtk_widget_show (menuitem);

      menuitem = gtk_menu_item_new_with_label ("foo");
      gtk_menu_item_set_submenu (GTK_MENU_ITEM (menuitem), create_menu (3));
      gtk_menu_shell_append (GTK_MENU_SHELL (menubar), menuitem);
      gtk_widget_show (menuitem);

      menuitem = gtk_menu_item_new_with_label ("bar");
      gtk_menu_item_set_submenu (GTK_MENU_ITEM (menuitem), create_menu (4));
      gtk_menu_shell_append (GTK_MENU_SHELL (menubar), menuitem);
      gtk_widget_show (menuitem);

      box2 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 10);
      gtk_container_set_border_width (GTK_CONTAINER (box2), 10);
      gtk_box_pack_start (GTK_BOX (box1), box2, FALSE, TRUE, 0);
      gtk_widget_show (box2);

      button = gtk_button_new_with_label ("Flip");
      g_signal_connect (button, "clicked",
                        G_CALLBACK (change_orientation), menubar);
      gtk_box_pack_start (GTK_BOX (box2), button, TRUE, TRUE, 0);
      gtk_widget_show (button);

      button = gtk_button_new_with_label ("Close");
      g_signal_connect_swapped (button, "clicked",
                                G_CALLBACK(gtk_widget_destroy), window);
      gtk_box_pack_start (GTK_BOX (box2), button, TRUE, TRUE, 0);
      gtk_widget_set_can_default (button, TRUE);
      gtk_widget_grab_default (button);
      gtk_widget_show (button);
    }

  if (!gtk_widget_get_visible (window))
    gtk_widget_show (window);
  else
    gtk_widget_destroy (window);

  return window;
}

static void
activate_run (GSimpleAction *action,
              GVariant      *parameter,
              gpointer       user_data)
{
  GtkWidget *window = user_data;
  GtkTreeSelection *selection;
  GtkTreeModel *model;
  GtkTreeIter iter;

  selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (treeview));
#if 0
  if (gtk_tree_selection_get_selected (selection, &model, &iter))
    run_example_for_row (window, model, &iter);
#endif
}

static void
startup (GApplication *app)
{
  GtkBuilder *builder;
  GMenuModel *appmenu;
  gchar *ids[] = { "appmenu", NULL };

  builder = gtk_builder_new ();
  gtk_builder_add_objects_from_resource (builder, "/ui/appmenu.ui", ids, NULL);

  appmenu = (GMenuModel *)gtk_builder_get_object (builder, "appmenu");

  gtk_application_set_app_menu (GTK_APPLICATION (app), appmenu);

  g_object_unref (builder);
}

static void
run_example_for_row (GtkWidget    *window,
                     GtkTreeModel *model,
                     GtkTreeIter  *iter)
{
#if 0
  PangoStyle style;
  GDoDemoFunc func;
  GtkWidget *demo;

  gtk_tree_model_get (GTK_TREE_MODEL (model),
                      iter,
                      FUNC_COLUMN, &func,
                      STYLE_COLUMN, &style,
                      -1);

  if (func)
    {
      gtk_tree_store_set (GTK_TREE_STORE (model),
                          iter,
                          STYLE_COLUMN, (style == PANGO_STYLE_ITALIC ? PANGO_STYLE_NORMAL : PANGO_STYLE_ITALIC),
                          -1);
      demo = (func) (window);

      if (demo != NULL)
        {
          CallbackData *cbdata;

          cbdata = g_new (CallbackData, 1);
          cbdata->model = model;
          cbdata->path = gtk_tree_model_get_path (model, iter);

          if (gtk_widget_is_toplevel (demo))
            {
              gtk_window_set_transient_for (GTK_WINDOW (demo), GTK_WINDOW (window));
              gtk_window_set_modal (GTK_WINDOW (demo), TRUE);
            }

          g_signal_connect (demo, "destroy",
                            G_CALLBACK (window_closed_cb), cbdata);
        }
    }
#endif
}

static void
row_activated_cb (GtkWidget         *tree_view,
                  GtkTreePath       *path,
                  GtkTreeViewColumn *column)
{
  GtkTreeIter iter;
  GtkWidget *window;
  GtkTreeModel *model;

  window = gtk_widget_get_toplevel (tree_view);
  model = gtk_tree_view_get_model (GTK_TREE_VIEW (tree_view));
  gtk_tree_model_get_iter (model, &iter, path);

  run_example_for_row (window, model, &iter);
}

static void
start_cb (GtkMenuItem *item, GtkWidget *scrollbar)
{
  GtkAdjustment *adj;

  adj = gtk_range_get_adjustment (GTK_RANGE (scrollbar));
  gtk_adjustment_set_value (adj, gtk_adjustment_get_lower (adj));
}

static void
end_cb (GtkMenuItem *item, GtkWidget *scrollbar)
{
  GtkAdjustment *adj;

  adj = gtk_range_get_adjustment (GTK_RANGE (scrollbar));
  gtk_adjustment_set_value (adj, gtk_adjustment_get_upper (adj) - gtk_adjustment_get_page_size (adj));
}

static gboolean
scrollbar_popup (GtkWidget *scrollbar, GtkWidget *menu)
{
  gtk_menu_popup_at_pointer (GTK_MENU (menu), NULL);

  return TRUE;
}

static void
activate (GApplication *app)
{
  GtkBuilder *builder;
  GtkWindow *window;
  GtkWidget *widget;
  GtkTreeModel *model;
  GtkTreeIter iter;
  GError *error = NULL;
  GtkWidget *sw;
  GtkWidget *scrollbar;
  GtkWidget *menu;
  GtkWidget *item;

  static GActionEntry win_entries[] = {
    { "run", activate_run, NULL, NULL, NULL }
  };

  builder = gtk_builder_new ();
  gtk_builder_add_from_resource (builder, "/ui/main.ui", &error);
  if (error != NULL)
    {
      g_critical ("%s", error->message);
      exit (1);
    }

  window = (GtkWindow *)gtk_builder_get_object (builder, "window");
  gtk_application_add_window (GTK_APPLICATION (app), window);
  g_action_map_add_action_entries (G_ACTION_MAP (window),
                                   win_entries, G_N_ELEMENTS (win_entries),
                                   window);

  notebook = (GtkWidget *)gtk_builder_get_object (builder, "notebook");

  info_view = (GtkWidget *)gtk_builder_get_object (builder, "info-textview");
  source_view = (GtkWidget *)gtk_builder_get_object (builder, "source-textview");
  headerbar = (GtkWidget *)gtk_builder_get_object (builder, "headerbar");
  treeview = (GtkWidget *)gtk_builder_get_object (builder, "treeview");
  model = gtk_tree_view_get_model (GTK_TREE_VIEW (treeview));

  sw = (GtkWidget *)gtk_builder_get_object (builder, "source-scrolledwindow");
  scrollbar = gtk_scrolled_window_get_vscrollbar (GTK_SCROLLED_WINDOW (sw));

  menu = gtk_menu_new ();

  item = gtk_menu_item_new_with_label ("Start");
  g_signal_connect (item, "activate", G_CALLBACK (start_cb), scrollbar);
  gtk_menu_shell_append (GTK_MENU_SHELL (menu), item);

  item = gtk_menu_item_new_with_label ("End");
  g_signal_connect (item, "activate", G_CALLBACK (end_cb), scrollbar);
  gtk_menu_shell_append (GTK_MENU_SHELL (menu), item);

  gtk_widget_show_all (menu);

  g_signal_connect (scrollbar, "popup-menu", G_CALLBACK (scrollbar_popup), menu);

  load_file (gtk_demos[0].name, gtk_demos[0].filename);

  populate_model (model);

  g_signal_connect (treeview, "row-activated", G_CALLBACK (row_activated_cb), model);

  widget = (GtkWidget *)gtk_builder_get_object (builder, "treeview-selection");
  g_signal_connect (widget, "changed", G_CALLBACK (selection_cb), model);

  gtk_tree_model_get_iter_first (gtk_tree_view_get_model (GTK_TREE_VIEW (treeview)), &iter);
  gtk_tree_selection_select_iter (GTK_TREE_SELECTION (widget), &iter);

  gtk_tree_view_collapse_all (GTK_TREE_VIEW (treeview));

  gtk_widget_show_all (GTK_WIDGET (window));

  g_object_unref (builder);
}


static gint
command_line (GApplication            *app,
              GApplicationCommandLine *cmdline)
{
  GVariantDict *options;
  const gchar *name = NULL;
  gboolean autoquit = FALSE;
  gboolean list = FALSE;
#if 0
  Demo *d, *c;
  GDoDemoFunc func = 0;
#endif
  GtkWidget *window, *demo;

  activate (app);

  options = g_application_command_line_get_options_dict (cmdline);
  g_variant_dict_lookup (options, "run", "&s", &name);
  g_variant_dict_lookup (options, "autoquit", "b", &autoquit);
  g_variant_dict_lookup (options, "list", "b", &list);

  if (list)
    {
      list_demos ();
      g_application_quit (app);
      return 0;
    }

  if (name == NULL)
    goto out;

  window = gtk_application_get_windows (GTK_APPLICATION (app))->data;

  do_menus(window);
#if 0
  d = gtk_demos;

  while (d->title)
    {
      c = d->children;
      if (g_strcmp0 (d->name, name) == 0)
        {
          func = d->func;
          goto out;
        }
      d++;
      while (c && c->title)
        {
          if (g_strcmp0 (c->name, name) == 0)
            {
              func = c->func;
              goto out;
            }
          c++;
        }
    }

out:
  if (func)
    {
      demo = (func) (window);

      gtk_window_set_transient_for (GTK_WINDOW (demo), GTK_WINDOW (window));
      gtk_window_set_modal (GTK_WINDOW (demo), TRUE);
    }

  if (autoquit)
    g_timeout_add_seconds (1, auto_quit, app);
#endif

  return 0;
}
int
main (int argc, char **argv)
{
  GtkApplication *app;
#if 1
  static GActionEntry app_entries[] = {
    { "about", activate_about, NULL, NULL, NULL },
    { "quit", activate_quit, NULL, NULL, NULL },
  };

  /* Most code in gtk-demo is intended to be exemplary, but not
   * these few lines, which are just a hack so gtk-demo will work
   * in the GTK tree without installing it.
   */
  if (g_file_test ("../../modules/input/immodules.cache", G_FILE_TEST_EXISTS))
    {
      g_setenv ("GTK_IM_MODULE_FILE", "../../modules/input/immodules.cache", TRUE);
    }
  /* -- End of hack -- */
#endif
  app = gtk_application_new ("org.gtk.Demo", G_APPLICATION_NON_UNIQUE|G_APPLICATION_HANDLES_COMMAND_LINE);

#if 0
  g_action_map_add_action_entries (G_ACTION_MAP (app),
                                   app_entries, G_N_ELEMENTS (app_entries),
                                   app);

  g_application_add_main_option (G_APPLICATION (app), "version", 0, 0, G_OPTION_ARG_NONE, "Show program version", NULL);
  g_application_add_main_option (G_APPLICATION (app), "run", 0, 0, G_OPTION_ARG_STRING, "Run an example", "EXAMPLE");
  g_application_add_main_option (G_APPLICATION (app), "list", 0, 0, G_OPTION_ARG_NONE, "List examples", NULL);
  g_application_add_main_option (G_APPLICATION (app), "autoquit", 0, 0, G_OPTION_ARG_NONE, "Quit after a delay", NULL);

  g_signal_connect (app, "startup", G_CALLBACK (startup), NULL);
  g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);
  g_signal_connect (app, "command-line", G_CALLBACK (command_line), NULL);
  g_signal_connect (app, "handle-local-options", G_CALLBACK (local_options), NULL);

  g_application_run (G_APPLICATION (app), argc, argv);

  return 0;
}

