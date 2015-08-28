/* Magic Book Reader
 * Copyright (C) 2015 Yan Zlobin <yan_zlobin@agniyoga.ru>
 *
 * mb-file-system.c
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

#include <zip.h>
#include <string.h>
#include <unistd.h>
#include <glib/gstdio.h>
#include "mb-file-system.h"

#define SKIP_HIDDEN_DIRS				TRUE

#define COPY(obj,str)					(g_string_chunk_insert (obj, str))
#define NO_ACCESS(f)					((gboolean) (g_access (f, R_OK)))
#define NOT_HIDDEN(f)					(SKIP_HIDDEN_DIRS ? (!g_str_has_prefix (f, ".")) : TRUE)

#define MB_FILE_SYSTEM_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), \
	MB_TYPE_FILE_SYSTEM, MbFileSystemPrivate))

struct _MbFileSystemPrivate
{
	gchar *filename;
};

enum
{
	SIGNAL_FILE_FOUND,
	SIGNAL_FILE_UPDATED,
	SIGNAL_LAST
};

static guint mb_file_system_signals[SIGNAL_LAST] = { 0 };

static void mb_file_system_finalize (GObject *object);

static gpointer file_system_open (gpointer data);

static GSList *direcotry_open (GSList *file_list, GStringChunk *chunk,
                         gchar *filename);

G_DEFINE_TYPE (MbFileSystem, mb_file_system, G_TYPE_OBJECT);

static void
mb_file_system_init (MbFileSystem *fs)
{
	MbFileSystemPrivate *priv;

	priv = MB_FILE_SYSTEM_GET_PRIVATE (fs);
	fs->priv = priv;

	priv->filename = NULL;
}

static void
mb_file_system_class_init (MbFileSystemClass *klass)
{
	GObjectClass *gobject_class;

	g_type_class_add_private (klass, sizeof (MbFileSystemPrivate));

	gobject_class = G_OBJECT_CLASS (klass);
	
	mb_file_system_signals[SIGNAL_FILE_FOUND] =
		g_signal_new ("file-found", G_TYPE_FROM_CLASS (klass),
		              G_SIGNAL_RUN_LAST,
		              G_STRUCT_OFFSET (MbFileSystemClass, file_found),
		              NULL, NULL, g_cclosure_marshal_VOID__POINTER,
		              G_TYPE_NONE, 1, G_TYPE_POINTER);

	mb_file_system_signals[SIGNAL_FILE_UPDATED] =
		g_signal_new ("file-updated", G_TYPE_FROM_CLASS (klass),
		              G_SIGNAL_RUN_LAST,
		              G_STRUCT_OFFSET (MbFileSystemClass, file_updated),
		              NULL, NULL, g_cclosure_marshal_VOID__STRING,
		              G_TYPE_NONE, 1, G_TYPE_STRING);

	gobject_class->finalize = mb_file_system_finalize;
}

static void
mb_file_system_finalize (GObject *object)
{
	MbFileSystem *fs;
	MbFileSystemPrivate *priv;

	fs = MB_FILE_SYSTEM (object);
	priv = fs->priv;

	if (priv->filename)
	{
		g_free (priv->filename);
	}

	G_OBJECT_CLASS (mb_file_system_parent_class)->finalize (object);
}

MbFileSystem *
mb_file_system_new (void)
{
	MbFileSystem *fs;

	fs = g_object_new (MB_TYPE_FILE_SYSTEM, NULL);

	return fs;
}

void
mb_file_system_open (MbFileSystem *fs, gchar *filename)
{
	MbFileSystemPrivate *priv;

	if (!g_file_test (filename, G_FILE_TEST_IS_DIR))
	{
		return;
	}

	priv = fs->priv;
	priv->filename = g_strdup (filename);

	g_thread_new (NULL, (GThreadFunc) file_system_open, fs);
}

static gpointer
file_system_open (gpointer data)
{
	MbFileSystem *fs;
	MbFileSystemPrivate *priv;
	GStringChunk *chunk;
	GSList *file_list = NULL;

	fs = MB_FILE_SYSTEM (data);
	priv = fs->priv;
	chunk = g_string_chunk_new (1);
	file_list = direcotry_open (file_list, chunk, priv->filename);

	if (file_list)
	{
		g_signal_emit (fs, mb_file_system_signals[SIGNAL_FILE_FOUND], 0,
		               file_list);

		g_slist_free (file_list);
	}

	g_string_chunk_free (chunk);

	return (gpointer) file_list;
}

static GSList *
direcotry_open (GSList *file_list, GStringChunk *chunk, gchar *filename)
{
	GDir *dir;
	gchar *dir_filename;
	gchar *tmp;
	GError *error = NULL;
	guint count = 0;

	if (NO_ACCESS (filename))
	{
		return file_list;
	}
	
	dir = g_dir_open (filename, 0, &error);

	if (!dir)
	{
		g_warning ("Cannot open directory. %s\n", error->message);
		
		if (error)
		{
			g_error_free (error);
		}

		return file_list;
	}

	do
	{
		dir_filename = (gchar *) g_dir_read_name (dir);

		if (dir_filename)
		{
			tmp = g_build_filename (filename, dir_filename, NULL);

			if (g_file_test (tmp, G_FILE_TEST_IS_DIR))
			{
				if (NOT_HIDDEN (dir_filename))
				{
					file_list = direcotry_open (file_list, chunk, tmp);
				}
			}
			else
			{
				if (g_str_has_suffix (tmp, ".fb2")
					|| g_str_has_suffix (tmp, ".fb2.gz")
					|| g_str_has_suffix (tmp, ".fb2.zip"))
				{
					file_list = g_slist_prepend (file_list, COPY (chunk, tmp));

					count++;
				}
			}

			g_free (tmp);
		}
	}
	while (dir_filename);

	g_dir_close (dir);

	return file_list;
}

