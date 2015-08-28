/* Magic Book Reader
 * Copyright (C) 2015 Yan Zlobin <yan_zlobin@agniyoga.ru>
 *
 * mbreader.c
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
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
