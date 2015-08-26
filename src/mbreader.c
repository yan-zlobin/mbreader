/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * mbreader.c
 * Copyright (C) 2014 Yan Zlobin <yan_zlobin@agniyoga.ru>
 * 
 */

#include <config.h>
#include <glib/gi18n.h>

#include "mb-application.h"

int
main (int argc, char *argv[])
{
	int status;

	MbApplication *application;
	GError *error = NULL;
	GOptionContext *context;

	status = 0;
	static gchar **filename = NULL;

	static GOptionEntry entries[] =
	{
		{ G_OPTION_REMAINING, 'f', 0, G_OPTION_ARG_STRING_ARRAY, &filename,
			"E-book filename", NULL },
		{ NULL }
	};

#ifdef ENABLE_NLS
	bindtextdomain (GETTEXT_PACKAGE, PACKAGE_LOCALE_DIR);
	bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
	textdomain (GETTEXT_PACKAGE);
#endif

	context = g_option_context_new ("- E-Book reading program");
	g_option_context_add_main_entries (context, entries, GETTEXT_PACKAGE);
	g_option_context_add_group (context, gtk_get_option_group (TRUE));

	if (!g_option_context_parse (context, &argc, &argv, &error))
	{
		g_print ("Option parsing failed: %s\n", error->message);
		
		return 1;
	}

	g_option_context_free (context);

	if (filename)
	{
		application = mb_application_new (filename[0]);
	}
	else
	{
		application = mb_application_new (NULL);
	}

	status = g_application_run (G_APPLICATION (application), argc, argv);

	return status;
}
