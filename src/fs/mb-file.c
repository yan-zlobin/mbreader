/* Magic Book Reader
 * Copyright (C) 2015 Yan Zlobin <yan_zlobin@agniyoga.ru>
 *
 * mb-file.c
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

#include "mb-file.h"
#include "mb-zip-file.h"

#define MB_FILE_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), \
	MB_TYPE_FILE, MbFilePrivate))

struct _MbFilePrivate
{
	gchar *filename;

	MbFileType type;

	MbZipFile *zip_file;
};

static void mb_file_finalize (GObject *object);

static void file_check (MbFile *file);

static void file_open (MbFile *file);

static MbBookPreview *get_preview (MbFile *file, gchar *filename,
                                   gboolean full_data);

G_DEFINE_TYPE (MbFile, mb_file, G_TYPE_OBJECT);

static void
mb_file_init (MbFile *file)
{
	MbFilePrivate *priv;

	priv = MB_FILE_GET_PRIVATE (file);
	file->priv = priv;

	priv->filename = NULL;
	priv->zip_file = NULL;
}

static void
mb_file_class_init (MbFileClass *klass)
{
	GObjectClass *gobject_class;

	g_type_class_add_private (klass, sizeof (MbFilePrivate));

	gobject_class = G_OBJECT_CLASS (klass);
	
	gobject_class->finalize = mb_file_finalize;
}

static void
mb_file_finalize (GObject *object)
{
	MbFile *file;
	MbFilePrivate *priv;

	file = MB_FILE (object);
	priv = file->priv;

	if (priv->filename)
	{
		g_free (priv->filename);
	}

	if (priv->zip_file)
	{
		g_object_unref (priv->zip_file);
	}

	G_OBJECT_CLASS (mb_file_parent_class)->finalize (object);
}

MbFile *
mb_file_new (gchar *filename)
{
	MbFile *file = NULL;
	MbFilePrivate *priv;

	if (filename && g_file_test (filename, G_FILE_TEST_IS_REGULAR))
	{
		file = g_object_new (MB_TYPE_FILE, NULL);
		priv = file->priv;
		priv->filename = g_strdup (filename);

		file_check (file);
		
		if (priv->type == MB_FILE_TYPE_UNKNOWN)
		{
			g_object_unref (file);
			g_warning ("Cannot open unknown file type");

			file = NULL;
		}
		else
		{
			file_open (file);

			if (priv->type == MB_FILE_TYPE_ZIP && SINGLE_FILE (file))
			{
				if (g_str_has_suffix (INTERN_FILENAME (file), ".fb2"))
				{
					priv->type = MB_FILE_TYPE_FB2_ZIP;
				}
			}
		}
	}

	return file;
}

guint
mb_file_get_count (MbFile *file)
{
	MbFilePrivate *priv;

	priv = file->priv;

	if (priv->type == MB_FILE_TYPE_FB2 || priv->type == MB_FILE_TYPE_EPUB)
	{
		return 1;
	}
	else
	{
		return (guint) mb_zip_file_get_count (priv->zip_file);
	}
}

MbFileType
mb_file_get_file_type (MbFile *file)
{
	return file->priv->type;
}

GSList *
mb_file_get_list (MbFile *file)
{
	MbFilePrivate *priv;
	GSList *list = NULL;

	priv = file->priv;

	if (SINGLE_FILE (file))
	{
		list = g_slist_append (list, priv->filename);
	}
	else
	{
		list = mb_zip_file_get_list (priv->zip_file);
	}

	return list;
}

gchar *
mb_file_get_contents (MbFile *file, gchar *filename, gsize *size)
{
	MbFilePrivate *priv;
	gchar *contents = NULL;

	priv = file->priv;
	
	if (priv->type == MB_FILE_TYPE_FB2)
	{
		g_file_get_contents (filename, &contents, size, NULL);
	}
	else if (priv->zip_file)
	{
		if (filename)
		{
			contents = mb_zip_file_get_file (priv->zip_file, filename, size);
		}
		else
		{
			contents = mb_zip_file_get_file (priv->zip_file,
			                                 INTERN_FILENAME (file), size);
		}
	}
	
	return contents;
}

MbBookPreview *
mb_file_get_preview (MbFile *file, gchar *filename)
{
	return get_preview (file, filename, FALSE);
}

MbBookPreview *
mb_file_get_preview_full (MbFile *file, gchar *filename)
{
	return get_preview (file, filename, TRUE);
}

gchar *
mb_file_get_checksum (MbFile *file, gchar *filename)
{
	MbFilePrivate *priv;
	gchar *result = NULL;
	gchar *contents;
	gsize size;

	priv = file->priv;

	if (SINGLE_FILE (file))
	{
		g_file_get_contents (filename, &contents, &size, NULL);
	}
	else
	{
		contents = mb_zip_file_get_file (priv->zip_file, filename, &size);
	}

	if (contents)
	{
		result = g_compute_checksum_for_data (G_CHECKSUM_MD5,
		                                      (const guchar *) contents, size);
	}

	g_free (contents);

	return result;
}

gchar *
mb_file_get_intern_filename (MbFile *file)
{
	MbFilePrivate *priv;
	gchar *intern_filename = NULL;

	priv = file->priv;

	if (SINGLE_FILE (file))
	{
		if (priv->type == MB_FILE_TYPE_FB2_ZIP
		    || priv->type == MB_FILE_TYPE_ZIP)
		{
			intern_filename = ZIP_FIRST_FILENAME (priv->zip_file);
		}
	}

	return intern_filename;
}

static void
file_check (MbFile *file)
{
	MbFilePrivate *priv;

	priv = file->priv;

	if (g_str_has_suffix (priv->filename, ".fb2")
	    || g_str_has_suffix (priv->filename, ".fb2.gz"))
	{
		priv->type = MB_FILE_TYPE_FB2;
	}
	else if (g_str_has_suffix (priv->filename, ".zip"))
	{
		priv->type = MB_FILE_TYPE_ZIP;
	}
	else if (g_str_has_suffix (priv->filename, ".epub"))
	{
		priv->type = MB_FILE_TYPE_EPUB;
	}
	else
	{
		priv->type = MB_FILE_TYPE_UNKNOWN;
	}
}

static void
file_open (MbFile *file)
{
	MbFilePrivate *priv;

	priv = file->priv;

	switch (priv->type)
	{
		case MB_FILE_TYPE_FB2:
		{

			break;
		}
		case MB_FILE_TYPE_FB2_ZIP:
		{
		}
		case MB_FILE_TYPE_EPUB:
		{
		}
		case MB_FILE_TYPE_ZIP:
		{
			priv->zip_file = mb_zip_file_new (priv->filename);

			break;
		}
		default:
		{
			break;
		}
	}
}

static MbBookPreview *
get_preview (MbFile *file, gchar *filename, gboolean full_data)
{
	MbFilePrivate *priv;
	MbBookPreview *preview = NULL;
	gchar *contents;
	gsize size = 0;

	priv = file->priv;

	switch (FILE_TYPE (file))
	{
		case MB_FILE_TYPE_FB2:
		{
			preview = mb_book_preview_new_from_file (priv->filename, full_data);

			break;
		}
		case MB_FILE_TYPE_FB2_ZIP:
		{
			contents = mb_zip_file_get_file (priv->zip_file,
			                                 INTERN_FILENAME (file), &size);

			preview = mb_book_preview_new_from_memory (contents, size,
			                                           full_data);
			
			g_free (contents);

			break;
		}
		default:
		{
			break;
		}
	}

	return preview;
}

