/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * mb-application.c
 * Copyright (C) 2014 Yan Zlobin <yan_zlobin@agniyoga.ru>
 * 
 */

#include "mb-application.h"
#include "mb-window.h"
#include "mb-loader.h"
#include "mb-reference.h"
#include "library/mb-library.h"
#include "fs/mb-file-system.h"
#include "preview/mb-preview-widget.h"

#define DEFAULT_OPEN_FOLDER		"/home/yan_zlobin/Библиотека/Книги"
#define DEFAULT_LIBRARY_FOLDER  "/home/yan_zlobin/Библиотека/Книги"

#define APP_WINDOW(obj)			(GTK_WINDOW (MB_WINDOW_WIDGET (obj->priv->window)))

#define MB_APPLICATION_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), \
	MB_TYPE_APPLICATION, MbApplicationPrivate))

struct _MbApplicationPrivate
{
	MbWindow *window;

	MbReference *reference;
	MbLibrary *library;
	MbFileSystem *fs;

	gchar *filename;
};

static void mb_application_activate (GApplication *application);

static void mb_application_open (GApplication *application, GFile **files,
                                 gint n_files, const gchar *hint);

static void mb_application_startup (GApplication *application);

static void mb_application_shutdown (GApplication *application);

static void mb_application_finalize (GObject *object);

static GMenuModel *mb_application_menu_new (void);

static void load_book (MbApplication *application);

static void open_file (MbApplication *application);

static void loading_completed_callback (MbLoader *loader, MbBookBuffer *buffer,
                                        MbApplication *application);

static void loading_error_callback (MbLoader *loader, gchar *message,
                                    MbApplication *application);

static void window_show_callback (MbWindow *window, MbApplication *application);

static void window_destroy_callback (MbWindow *window,
                                     MbApplication *application);

static void key_open_callback (MbWindow *window, MbApplication *application);

static void menu_open_callback (GSimpleAction *action, GVariant *parameter,
                                gpointer data);

static void menu_about_callback (GSimpleAction *action, GVariant *parameter,
                                 gpointer data);

static void menu_quit_callback (GSimpleAction *action, GVariant *parameter,
                                gpointer data);

static void update_preview_callback (GtkFileChooser *chooser,
                                     GtkWidget *preview);

static void fs_file_found_callback (MbFileSystem *fs, GSList *file_list,
                                    MbApplication *application);

static void fs_file_updated_callback (MbFileSystem *fs, gchar *filename,
                                      MbApplication *application);

static void library_ready_callback (MbLibrary *library,
                                    MbApplication *application);

const GActionEntry app_entries[] =
{
	{ "open", menu_open_callback, NULL, NULL, NULL },
	{ "about", menu_about_callback, NULL, NULL, NULL },
	{ "quit", menu_quit_callback, NULL, NULL, NULL }
};

G_DEFINE_TYPE (MbApplication, mb_application, GTK_TYPE_APPLICATION);

static void
mb_application_init (MbApplication *application)
{
	MbApplicationPrivate *priv;

	priv = MB_APPLICATION_GET_PRIVATE (application);
	application->priv = priv;

	priv->filename = NULL;
	priv->reference = mb_reference_new ();
	priv->library = mb_library_new (priv->reference);
	priv->fs = mb_file_system_new ();

	g_signal_connect (priv->library, "ready",
	                  G_CALLBACK (library_ready_callback), application);

	g_signal_connect (priv->fs, "file-found",
	                  G_CALLBACK (fs_file_found_callback), application);

	g_signal_connect (priv->fs, "file-updated",
	                  G_CALLBACK (fs_file_updated_callback), application);
}

static void
mb_application_class_init (MbApplicationClass *klass)
{
	GApplicationClass *application_klass;

	g_type_class_add_private (klass, sizeof (MbApplicationPrivate));

	application_klass = G_APPLICATION_CLASS (klass);
	
	application_klass->activate = mb_application_activate;
	application_klass->open = mb_application_open;
	application_klass->startup = mb_application_startup;
	application_klass->shutdown = mb_application_shutdown;

	G_OBJECT_CLASS (klass)->finalize = mb_application_finalize;
}

static void
mb_application_activate (GApplication *application)
{
	MbApplication *mb_application;
	MbApplicationPrivate *priv;
	
	mb_application = MB_APPLICATION (application);

	priv = mb_application->priv;
	priv->window = mb_window_new (application);

	g_signal_connect (priv->window, "window-show",
	                  G_CALLBACK (window_show_callback), application);

	g_signal_connect (priv->window, "window-destroy",
	                  G_CALLBACK (window_destroy_callback), application);

	g_signal_connect (priv->window, "key-open",
	                  G_CALLBACK (key_open_callback), application);

	mb_library_start (priv->library);
	mb_window_show (priv->window);
}

static void
mb_application_open (GApplication *application, GFile **files, gint n_files,
                     const gchar *hint)
{
}

static void
mb_application_startup (GApplication *application)
{
	G_APPLICATION_CLASS (mb_application_parent_class)->startup (application);

	g_action_map_add_action_entries (G_ACTION_MAP (application), app_entries,
	                                 G_N_ELEMENTS (app_entries), application);

	gtk_application_set_app_menu (GTK_APPLICATION (application),
	                              mb_application_menu_new ());
}

static void
mb_application_shutdown (GApplication *application)
{
	G_APPLICATION_CLASS (mb_application_parent_class)->shutdown (application);
}

static void
mb_application_finalize (GObject *object)
{
	MbApplication *application;
	MbApplicationPrivate *priv;

	application = MB_APPLICATION (object);
	priv = application->priv;

	if (priv->window)
	{
		g_object_unref (priv->window);
	}

	if (priv->filename)
	{
		g_free (priv->filename);
	}

	g_object_unref (priv->reference);
	g_object_unref (priv->library);
	g_object_unref (priv->fs);
	
	G_OBJECT_CLASS (mb_application_parent_class)->finalize (object);
}

MbApplication *
mb_application_new (gchar *filename)
{
	MbApplication *application;
	MbApplicationPrivate *priv;

	g_set_application_name ((const gchar *) "Magic Book Reader");
	g_set_prgname (g_get_application_name ());

	application = g_object_new (MB_TYPE_APPLICATION,
	                            "application-id", "org.gtk.mb_application",
	                            "flags", G_APPLICATION_HANDLES_OPEN,
	                            NULL);

	priv = application->priv;

	if (filename)
	{
		priv->filename = g_strdup (filename);
	}

	return application;
}

static GMenuModel *
mb_application_menu_new (void)
{
	GMenu *menu;
	GMenu *open_section;
	GMenu *about_section;
	GMenu *exit_section;

	menu = g_menu_new ();
	open_section = g_menu_new ();
	about_section = g_menu_new ();
	exit_section = g_menu_new ();

	g_menu_append (open_section, "Открыть...", "app.open");
	g_menu_append (about_section, "О программе...", "app.about");
	g_menu_append (exit_section, "Выход", "app.quit");

	g_menu_append_section (menu, NULL, G_MENU_MODEL (open_section));
	g_menu_append_section (menu, NULL, G_MENU_MODEL (about_section));
	g_menu_append_section (menu, NULL, G_MENU_MODEL (exit_section));

	return G_MENU_MODEL (menu);
}

static void
load_book (MbApplication *application)
{
	MbApplicationPrivate *priv;
	MbLoader *loader;

	priv = application->priv;

	if (!priv->filename)
	{
		return;
	}

	loader = mb_loader_new (priv->filename);

	g_signal_connect (loader, "loading-completed",
	                  G_CALLBACK (loading_completed_callback), application);

	g_signal_connect (loader, "loading-error",
	                  G_CALLBACK (loading_error_callback), application);

	mb_loader_load_book (loader);
}

static void
open_file (MbApplication *application)
{
	MbApplicationPrivate *priv;
	GtkWidget *dialog;
	GtkWidget *preview;
	GtkFileChooser *chooser;
	GtkFileFilter *fb2_filter;
	GtkFileFilter *comics_filter;
	gchar *filename;
	gint res;

	priv = application->priv;

	dialog = gtk_file_chooser_dialog_new ("Открыть", APP_WINDOW (application),
	                                      GTK_FILE_CHOOSER_ACTION_OPEN,
	                                      "Отмена", GTK_RESPONSE_CANCEL,
	                                      "Открыть", GTK_RESPONSE_ACCEPT, NULL);

	chooser = GTK_FILE_CHOOSER (dialog);
	fb2_filter = gtk_file_filter_new ();
	comics_filter = gtk_file_filter_new ();
	preview = mb_preview_widget_new (priv->reference);

	gtk_file_filter_set_name (fb2_filter, "Книги fb2");
	gtk_file_filter_add_pattern (fb2_filter, "*.fb2");
	gtk_file_filter_add_pattern (fb2_filter, "*.fb2.zip");
	gtk_file_filter_add_pattern (fb2_filter, "*.fb2.gz");
	gtk_file_filter_add_pattern (fb2_filter, "*.fb2.zip");

	gtk_file_filter_set_name (comics_filter, "Комиксы");
	gtk_file_filter_add_pattern (comics_filter, "*.cbr");

	gtk_file_chooser_add_filter (chooser, fb2_filter);
	gtk_file_chooser_add_filter (chooser, comics_filter);
	gtk_file_chooser_set_filter (chooser, fb2_filter);

	gtk_file_chooser_set_use_preview_label (chooser, FALSE);
	gtk_file_chooser_set_preview_widget (chooser, preview);
	gtk_file_chooser_set_current_folder (chooser, DEFAULT_OPEN_FOLDER);

	g_signal_connect (chooser, "update-preview",
	                  G_CALLBACK (update_preview_callback), preview);
	
	res = gtk_dialog_run (GTK_DIALOG (dialog));

	if (res == GTK_RESPONSE_ACCEPT)
	{
		filename = gtk_file_chooser_get_filename (chooser);

		if (filename)
		{
			if (priv->filename)
			{
				g_free (priv->filename);

				priv->filename = NULL;
			}

			priv->filename = filename;

			load_book (application);
		}
	}

	gtk_widget_destroy (dialog);
}

static void
loading_completed_callback (MbLoader *loader, MbBookBuffer *buffer,
                            MbApplication *application)
{
	mb_window_set_book_buffer (application->priv->window, (gpointer) buffer);

	g_object_unref (loader);
}

static void
loading_error_callback (MbLoader *loader, gchar *message,
                        MbApplication *application)
{
	MbApplicationPrivate *priv;
	MbWindow *window;

	priv = application->priv;
	window = priv->window;
	
	mb_window_show_error (window, message);

	g_object_unref (loader);
}

static void
window_show_callback (MbWindow *window, MbApplication *application)
{
	load_book (application);
}

static void
window_destroy_callback (MbWindow *window, MbApplication *application)
{
	MbApplicationPrivate *priv;

	priv = application->priv;

	g_object_unref (window);
	priv->window = NULL;
}

static void
key_open_callback (MbWindow *window, MbApplication *application)
{
	open_file (application);
}

static void
menu_open_callback (GSimpleAction *action, GVariant *parameter, gpointer data)
{
	open_file (MB_APPLICATION (data));
}

static void
menu_about_callback (GSimpleAction *action, GVariant *parameter, gpointer data)
{
}

static void
menu_quit_callback (GSimpleAction *action, GVariant *parameter, gpointer data)
{
	g_application_quit (G_APPLICATION (data));
}

static void
update_preview_callback (GtkFileChooser *chooser, GtkWidget *preview)
{
	gchar *filename;

	filename = (gchar *) gtk_file_chooser_get_preview_filename (chooser);

	if (g_file_test (filename, G_FILE_TEST_IS_REGULAR))
	{
		if (mb_preview_widget_set_filename (MB_PREVIEW_WIDGET (preview),
		                                    filename))
		{
			gtk_file_chooser_set_preview_widget_active (chooser, TRUE);
		}
		else
		{
			gtk_file_chooser_set_preview_widget_active (chooser, FALSE);
		}
	}
	else
	{
		gtk_file_chooser_set_preview_widget_active (chooser, FALSE);

		g_free (filename);
	}
}

static void
fs_file_found_callback (MbFileSystem *fs, GSList *file_list,
                        MbApplication *application)
{
	mb_library_append_file_list (application->priv->library, file_list);
}

static void
fs_file_updated_callback (MbFileSystem *fs, gchar *filename,
                          MbApplication *application)
{
}

static void
library_ready_callback (MbLibrary *library, MbApplication *application)
{
	mb_file_system_open (application->priv->fs, DEFAULT_LIBRARY_FOLDER);
}

