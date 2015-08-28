/* Magic Book Reader
 * Copyright (C) 2015 Yan Zlobin <yan_zlobin@agniyoga.ru>
 *
 * mb-zip-file.c
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
#include "mb-zip-file.h"

#define COPY(obj,str)	   (g_string_chunk_insert (obj->priv->chunk, str))

#define MB_ZIP_FILE_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), \
	MB_TYPE_ZIP_FILE, MbZipFilePrivate))

struct _MbZipFilePrivate
{
	GStringChunk *chunk;

	struct zip *archive;

	GSList *files;
	gulong file_count;
};

static void mb_zip_file_finalize (GObject *object);

static void file_open (MbZipFile *file, gchar *filename);

G_DEFINE_TYPE (MbZipFile, mb_zip_file, G_TYPE_OBJECT);

static void
mb_zip_file_init (MbZipFile *file)
{
	MbZipFilePrivate *priv;

	priv = MB_ZIP_FILE_GET_PRIVATE (file);
	file->priv = priv;

	priv->chunk = g_string_chunk_new (1);

	priv->archive = NULL;
	priv->files = NULL;
	priv->file_count = 0;
}

static void
mb_zip_file_class_init (MbZipFileClass *klass)
{
	GObjectClass *gobject_class;

	g_type_class_add_private (klass, sizeof (MbZipFilePrivate));

	gobject_class = G_OBJECT_CLASS (klass);
	
	gobject_class->finalize = mb_zip_file_finalize;
}

static void
mb_zip_file_finalize (GObject *object)
{
	MbZipFile *file;
	MbZipFilePrivate *priv;

	file = MB_ZIP_FILE (object);
	priv = file->priv;

	g_slist_free (priv->files);
	
	g_string_chunk_free (priv->chunk);

	if (priv->archive)
	{
		zip_close (priv->archive);
	}

	G_OBJECT_CLASS (mb_zip_file_parent_class)->finalize (object);
}

MbZipFile *
mb_zip_file_new (gchar *filename)
{
	MbZipFile *file;

	file = g_object_new (MB_TYPE_ZIP_FILE, NULL);

	if (g_file_test (filename, G_FILE_TEST_IS_REGULAR))
	{
		file_open (file, filename);
	}

	return file;
}

gulong
mb_zip_file_get_count (MbZipFile *file)
{
	return file->priv->file_count;
}

gchar *
mb_zip_file_get_file (MbZipFile *file, gchar *filename, gsize *size)
{
	MbZipFilePrivate *priv;
	struct zip_stat stat;
	struct zip_file *zip_file;
	zip_int64_t read_result;
	gchar *result = NULL;
	int stat_rc;

	priv = file->priv;
	
	stat_rc = zip_stat (priv->archive, filename, 0, &stat);

	if (stat_rc == -1)
	{
		g_warning ("Error reading %s", filename);
		
		return result;
	}

	char buffer[stat.size];
	zip_file = zip_fopen (priv->archive, filename, 0);

	if (zip_file)
	{
		read_result = zip_fread (zip_file, buffer, sizeof (buffer));

		if (read_result > 0)
		{
			result = g_malloc (stat.size);

			memcpy (result, buffer, stat.size);
		
			if (size)
			{
				if (read_result != -1)
				{
					*size = (gsize) stat.size;
				}
				else
				{
					*size = 0;
				}
			}
		}

		zip_fclose (zip_file);
	}

	return result;
}

GSList *
mb_zip_file_get_list (MbZipFile *file)
{
	return file->priv->files;
}

static void
file_open (MbZipFile *file, gchar *filename)
{
	MbZipFilePrivate *priv;
	struct zip *archive;
	struct zip_stat stat;
	zip_int64_t file_count;
	zip_int64_t i;
	int error = 0;

	priv = file->priv;
	archive = zip_open (filename, 0, &error);

	if (error)
	{
		return;
	}
	
	priv->archive = archive;
	file_count = zip_get_num_entries (priv->archive, ZIP_FL_UNCHANGED);

	if (file_count > 0)
	{
		for (i = 0; i < file_count; i++)
		{
			zip_stat_index (priv->archive, i, 0, &stat);

			if (g_str_has_suffix (stat.name, ".fb2")
			    || g_str_has_suffix (stat.name, ".fb2.gz")
			    || g_str_has_suffix (stat.name, ".fb2.zip")
			    || g_str_has_suffix (stat.name, ".epub")
			    || g_str_has_suffix (stat.name, ".zip"))
			{
				priv->files = g_slist_prepend (priv->files, COPY (file,
				                                                  stat.name));

				priv->file_count++;
			}
		}

		if (priv->file_count > 0)
		{
			priv->files = g_slist_reverse (priv->files);
		}
	}
}

