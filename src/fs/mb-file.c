/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * mb-file.c
 * Copyright (C) 2014 Yan Zlobin <yan_zlobin@agniyoga.ru>
 * 
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
			intern_filename = ZIP_FIRST_FILE (priv->zip_file);
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
	else if (g_str_has_suffix (priv->filename, ".fb2.zip"))
	{
		priv->type = MB_FILE_TYPE_FB2_ZIP;
	}
	else if (g_str_has_suffix (priv->filename, ".epub"))
	{
		priv->type = MB_FILE_TYPE_EPUB;
	}
	else if (g_str_has_suffix (priv->filename, ".zip"))
	{
		priv->type = MB_FILE_TYPE_ZIP;
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

	if (SINGLE_FILE (file))
	{
		if (priv->type == MB_FILE_TYPE_FB2)
		{
			preview = mb_book_preview_new_from_file (priv->filename, full_data);
		}
		else if (priv->type == MB_FILE_TYPE_FB2_ZIP)
		{
			gchar *intern_filename;

			intern_filename = ZIP_FIRST_FILE (priv->zip_file);
			
			contents = mb_zip_file_get_file (priv->zip_file, intern_filename,
			                                 &size);

			preview = mb_book_preview_new_from_memory (contents, size,
			                                           full_data);

			g_free (contents);
		}
	}
	else
	{
		contents = mb_zip_file_get_file (priv->zip_file, filename, &size);
		preview = mb_book_preview_new_from_memory (contents, size, full_data);

		g_free (contents);
	}

	return preview;
}

