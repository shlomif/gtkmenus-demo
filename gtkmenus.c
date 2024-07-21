#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>

#include <stdio.h>

static GtkWidget *treeview;
static GtkWidget *notebook;
static GtkWidget *info_view;
static GtkWidget *source_view;

static gchar *current_file = NULL;
static GtkWidget *headerbar;

enum {
  NAME_COLUMN,
  TITLE_COLUMN,
  FILENAME_COLUMN,
  FUNC_COLUMN,
  STYLE_COLUMN,
  NUM_COLUMNS
};

#define PACKAGE_VERSION "0.0.1"

static void
activate_about (GSimpleAction *,
                GVariant      *,
                gpointer       user_data)
{
  GtkApplication *app = user_data;
  const gchar *authors[] = {
    "The GTK+ Team",
    NULL
  };

  gtk_show_about_dialog (GTK_WINDOW (gtk_application_get_active_window (app)),
                         "program-name", "GTK+ Demo",
                         "version", g_strdup_printf ("%s,\nRunning against GTK+ %d.%d.%d",
                                                     PACKAGE_VERSION,
                                                     gtk_get_major_version (),
                                                     gtk_get_minor_version (),
                                                     gtk_get_micro_version ()),
                         "copyright", "(C) 1997-2013 The GTK+ Team",
                         "license-type", GTK_LICENSE_LGPL_2_1,
                         "website", "http://www.gtk.org",
                         "comments", "Program to demonstrate GTK+ widgets",
                         "authors", authors,
                         "logo-icon-name", "gtk3-demo",
                         "title", "About GTK+ Demo",
                         NULL);
}

static void
activate_quit (GSimpleAction *,
               GVariant      *,
               gpointer       user_data)
{
  GtkApplication *app = user_data;
  GtkWidget *win;
  GList *list, *next;

  list = gtk_application_get_windows (app);
  while (list)
    {
      win = list->data;
      next = list->next;

      gtk_widget_destroy (GTK_WIDGET (win));

      list = next;
    }
}

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
change_orientation (GtkWidget *,
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

      menuitem = gtk_menu_item_new_with_mnemonic ("_File");
      gtk_menu_item_set_submenu (GTK_MENU_ITEM (menuitem), menu);
      gtk_menu_shell_append (GTK_MENU_SHELL (menubar), menuitem);
      gtk_widget_show (menuitem);

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

#if 0
static void
activate_run (GSimpleAction *action,
              GVariant      *parameter,
              gpointer       user_data)
#else
static void
activate_run (GSimpleAction *,
              GVariant      *,
              gpointer)
#endif
{
#if 0
  GtkWidget *window = user_data;
  GtkTreeSelection *selection;
  GtkTreeModel *model;
  GtkTreeIter iter;
#endif

#if 0
  selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (treeview));
#else
  gtk_tree_view_get_selection (GTK_TREE_VIEW (treeview));
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


#if 0
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
}
#endif

static void
start_cb (GtkMenuItem *, GtkWidget *scrollbar)
{
  GtkAdjustment *adj;

  adj = gtk_range_get_adjustment (GTK_RANGE (scrollbar));
  gtk_adjustment_set_value (adj, gtk_adjustment_get_lower (adj));
}

static void
end_cb (GtkMenuItem *, GtkWidget *scrollbar)
{
  GtkAdjustment *adj;

  adj = gtk_range_get_adjustment (GTK_RANGE (scrollbar));
  gtk_adjustment_set_value (adj, gtk_adjustment_get_upper (adj) - gtk_adjustment_get_page_size (adj));
}

static gboolean
scrollbar_popup (GtkWidget *, GtkWidget *menu)
{
  gtk_menu_popup_at_pointer (GTK_MENU (menu), NULL);

  return TRUE;
}

static GtkWidget *create_text (GtkWidget **text_view, gboolean is_source);

enum {
  STATE_NORMAL,
  STATE_IN_COMMENT
};

static void
add_data_tab (const gchar *demoname)
{
  gchar *resource_dir, *resource_name;
  gchar **resources;
  GtkWidget *widget, *label;
  guint i;

  resource_dir = g_strconcat ("/", demoname, NULL);
  resources = g_resources_enumerate_children (resource_dir, 0, NULL);
  if (resources == NULL)
    {
      g_free (resource_dir);
      return;
    }

  for (i = 0; resources[i]; i++)
    {
      resource_name = g_strconcat (resource_dir, "/", resources[i], NULL);

      widget = gtk_image_new_from_resource (resource_name);
      if (gtk_image_get_pixbuf (GTK_IMAGE (widget)) == NULL &&
          gtk_image_get_animation (GTK_IMAGE (widget)) == NULL)
        {
          GBytes *bytes;

          /* So we've used the best API available to figure out it's
           * not an image. Let's try something else then.
           */
          g_object_ref_sink (widget);
          g_object_unref (widget);

          bytes = g_resources_lookup_data (resource_name, 0, NULL);
          g_assert (bytes);

          if (g_utf8_validate (g_bytes_get_data (bytes, NULL), g_bytes_get_size (bytes), NULL))
            {
              /* Looks like it parses as text. Dump it into a textview then! */
              GtkTextBuffer *buffer;
              GtkWidget *textview;

              widget = create_text (&textview, FALSE);
              buffer = gtk_text_buffer_new (NULL);
              gtk_text_buffer_set_text (buffer, g_bytes_get_data (bytes, NULL), g_bytes_get_size (bytes));
              gtk_text_view_set_buffer (GTK_TEXT_VIEW (textview), buffer);
            }
          else
            {
              g_warning ("Don't know how to display resource '%s'", resource_name);
              widget = NULL;
            }

          g_bytes_unref (bytes);
        }

      gtk_widget_show_all (widget);
      label = gtk_label_new (resources[i]);
      gtk_widget_show (label);
      gtk_notebook_append_page (GTK_NOTEBOOK (notebook), widget, label);
      gtk_container_child_set (GTK_CONTAINER (notebook),
                               GTK_WIDGET (widget),
                               "tab-expand", TRUE,
                               NULL);

      g_free (resource_name);
    }

  g_strfreev (resources);
  g_free (resource_dir);
}

static void
remove_data_tabs (void)
{
  gint i;

  for (i = gtk_notebook_get_n_pages (GTK_NOTEBOOK (notebook)) - 1; i > 1; i--)
    gtk_notebook_remove_page (GTK_NOTEBOOK (notebook), i);
}

static GtkWidget *
create_text (GtkWidget **view,
             gboolean    is_source)
{
  GtkWidget *scrolled_window;
  GtkWidget *text_view;

  scrolled_window = gtk_scrolled_window_new (NULL, NULL);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled_window),
                                  GTK_POLICY_AUTOMATIC,
                                  GTK_POLICY_AUTOMATIC);
  gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolled_window),
                                       GTK_SHADOW_NONE);

  *view = text_view = gtk_text_view_new ();
  g_object_set (text_view,
                "left-margin", 20,
                "right-margin", 20,
                "top-margin", 20,
                "bottom-margin", 20,
                NULL);

  gtk_text_view_set_editable (GTK_TEXT_VIEW (text_view), FALSE);
  gtk_text_view_set_cursor_visible (GTK_TEXT_VIEW (text_view), FALSE);

  gtk_container_add (GTK_CONTAINER (scrolled_window), text_view);

  if (is_source)
    {
      gtk_text_view_set_monospace (GTK_TEXT_VIEW (text_view), TRUE);
      gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW (text_view), GTK_WRAP_NONE);
    }
  else
    {
      /* Make it a bit nicer for text. */
      gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW (text_view), GTK_WRAP_WORD);
      gtk_text_view_set_pixels_above_lines (GTK_TEXT_VIEW (text_view), 2);
      gtk_text_view_set_pixels_below_lines (GTK_TEXT_VIEW (text_view), 2);
    }

  return scrolled_window;
}

static void
selection_cb (GtkTreeSelection *selection,
              GtkTreeModel     *model)
{
  GtkTreeIter iter;
  char *name;
  char *filename;
  char *title;

  if (! gtk_tree_selection_get_selected (selection, NULL, &iter))
    return;

  gtk_tree_model_get (model, &iter,
                      NAME_COLUMN, &name,
                      TITLE_COLUMN, &title,
                      FILENAME_COLUMN, &filename,
                      -1);

#if 0
  if (filename)
    load_file (name, filename);
#endif

  gtk_header_bar_set_title (GTK_HEADER_BAR (headerbar), title);

  g_free (name);
  g_free (title);
  g_free (filename);
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
#if 1
  gtk_builder_add_from_resource (builder, "/ui/main.ui", &error);
  if (error != NULL)
    {
      g_critical ("%s", error->message);
      exit (1);
    }
#endif

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

#if 0
  load_file (gtk_demos[0].name, gtk_demos[0].filename);
  populate_model (model);

  g_signal_connect (treeview, "row-activated", G_CALLBACK (row_activated_cb), model);
#endif

  widget = (GtkWidget *)gtk_builder_get_object (builder, "treeview-selection");
  g_signal_connect (widget, "changed", G_CALLBACK (selection_cb), model);

  gtk_tree_model_get_iter_first (gtk_tree_view_get_model (GTK_TREE_VIEW (treeview)), &iter);
  gtk_tree_selection_select_iter (GTK_TREE_SELECTION (widget), &iter);

  gtk_tree_view_collapse_all (GTK_TREE_VIEW (treeview));

  gtk_widget_show_all (GTK_WIDGET (window));

  g_object_unref (builder);

  window = gtk_application_get_windows (GTK_APPLICATION (app))->data;

  do_menus(GTK_WIDGET (window));
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
  GtkWidget *window;

  activate (app);

  options = g_application_command_line_get_options_dict (cmdline);
  g_variant_dict_lookup (options, "run", "&s", &name);
  g_variant_dict_lookup (options, "autoquit", "b", &autoquit);
  g_variant_dict_lookup (options, "list", "b", &list);

  if (list)
    {
#if 0
      list_demos ();
#endif
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

#endif
out:

  return 0;
}

static void
print_version (void)
{
  g_print ("gtk3-demo %d.%d.%d\n",
           gtk_get_major_version (),
           gtk_get_minor_version (),
           gtk_get_micro_version ());
}

static int
local_options (GApplication *app,
               GVariantDict *options,
               gpointer      data)
{
  gboolean version = FALSE;

  g_variant_dict_lookup (options, "version", "b", &version);

  if (version)
    {
      print_version ();
      return 0;
    }

  return -1;
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
#endif

  g_signal_connect (app, "startup", G_CALLBACK (startup), NULL);
  g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);
  g_signal_connect (app, "command-line", G_CALLBACK (command_line), NULL);
  g_signal_connect (app, "handle-local-options", G_CALLBACK (local_options), NULL);

  g_application_run (G_APPLICATION (app), argc, argv);

  return 0;
}

