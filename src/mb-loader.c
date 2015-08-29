/* Magic Book Reader
 * Copyright (C) 2015 Yan Zlobin <yan_zlobin@agniyoga.ru>
 *
 * mb-loader.c
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

#include "mb-loader.h"
#include "fs/mb-file.h"
#include "fb2/mb-fb2-loader.h"
#include "epub/mb-epub-loader.h"

#define MB_LOADER_GET_PRIVATE(obj)  \
	(G_TYPE_INSTANCE_GET_PRIVATE ((obj), MB_TYPE_LOADER, MbLoaderPrivate))

typedef enum _MbLoaderError MbLoaderError; 

struct _MbLoaderPrivate
{
	gchar *filename;
};

enum
{
	SIGNAL_LOADING_COMPLETED,
	SIGNAL_LOADING_ERROR,
	SIGNAL_LAST
};

enum _MbLoaderError
{
	MB_LOADER_ERROR_FILE_NOT_EXISTS,
	MB_LOADER_ERROR_OPEN,
	MB_LOADER_ERROR_FB2,
	MB_LOADER_ERROR_EPUB
};

static guint mb_loader_signals[SIGNAL_LAST] = { 0 };

static void mb_loader_finalize (GObject *object);

static void load_from_epub (MbLoader *loader);

static void load_from_fb2 (MbLoader *loader);

static void load_from_fb2_zip (MbLoader *loader, MbFile *file);

static void load_from_zip (MbLoader *loader);

static gchar *get_error_message (MbLoader *loader, MbLoaderError error);

static void loading_completed_callback (MbFb2Loader *fb2_loader,
                                        MbBookBuffer *buffer, MbLoader *loader);

static void loading_error_callback (MbFb2Loader *fb2_loader,
                                    MbFb2LoaderError error, MbLoader *loader);

G_DEFINE_TYPE (MbLoader, mb_loader, G_TYPE_OBJECT);

static void
mb_loader_init (MbLoader *loader)
{
	MbLoaderPrivate *priv;

	priv = MB_LOADER_GET_PRIVATE (loader);

	loader->priv = priv;
}

static void
mb_loader_class_init (MbLoaderClass *klass)
{
	GObjectClass *gobject_class;
	
	g_type_class_add_private (klass, sizeof (MbLoaderPrivate));

	gobject_class = G_OBJECT_CLASS (klass);

	mb_loader_signals[SIGNAL_LOADING_COMPLETED] =
		g_signal_new ("loading-completed", G_TYPE_FROM_CLASS (klass),
		              G_SIGNAL_RUN_LAST,
		              G_STRUCT_OFFSET (MbLoaderClass, loading_completed),
		              NULL, NULL, g_cclosure_marshal_VOID__OBJECT,
		              G_TYPE_NONE, 1, MB_TYPE_BOOK_BUFFER);

	mb_loader_signals[SIGNAL_LOADING_ERROR] =
		g_signal_new ("loading-error", G_TYPE_FROM_CLASS (klass),
		              G_SIGNAL_RUN_LAST,
		              G_STRUCT_OFFSET (MbLoaderClass, loading_error),
		              NULL, NULL, g_cclosure_marshal_VOID__OBJECT,
		              G_TYPE_NONE, 1, G_TYPE_STRING);

	gobject_class->finalize = mb_loader_finalize;
}

static void
mb_loader_finalize (GObject *object)
{
	G_OBJECT_CLASS (mb_loader_parent_class)->finalize (object);
}

MbLoader *
mb_loader_new (gchar *filename)
{
	MbLoader *loader;
	MbLoaderPrivate *priv;

	loader = g_object_new (MB_TYPE_LOADER, NULL);

	priv = loader->priv;
	priv->filename = filename;

	return loader;
}

void
mb_loader_load_book (MbLoader *loader)
{
	MbLoaderPrivate *priv;
	MbFile *file;

	priv = loader->priv;
	file = mb_file_new (priv->filename);

	if (file)
	{
		switch (FILE_TYPE (file))
		{
			case MB_FILE_TYPE_EPUB:
			{
				load_from_epub (loader);
				
				break;
			}
			case MB_FILE_TYPE_FB2:
			{
				load_from_fb2 (loader);
			
				break;
			}
			case MB_FILE_TYPE_FB2_ZIP:
			{
				load_from_fb2_zip (loader, file);
				
				break;
			}
			case MB_FILE_TYPE_ZIP:
			{
				load_from_zip (loader);

				break;
			}
			default:
			{
				break;
			}
		}

		g_object_unref (file);
	}
	else
	{
		gchar *message;
				
		message = get_error_message (loader, MB_LOADER_ERROR_FILE_NOT_EXISTS);
		
		g_signal_emit (loader, mb_loader_signals[SIGNAL_LOADING_ERROR], 0,
		               message, NULL);

		g_free (message);
	}
}

static void
load_from_epub (MbLoader *loader)
{
}

static void
load_from_fb2 (MbLoader *loader)
{
	MbFb2Loader *fb2_loader;

	fb2_loader = mb_fb2_loader_new (loader->priv->filename);

	g_signal_connect (fb2_loader, "loading-completed",
	                  G_CALLBACK (loading_completed_callback), loader);

	g_signal_connect (fb2_loader, "loading-error",
	                  G_CALLBACK (loading_error_callback), loader);

	mb_fb2_loader_load_from_fb2_file (fb2_loader);
}

static void
load_from_fb2_zip (MbLoader *loader, MbFile *file)
{
	MbFb2Loader *fb2_loader;
	gchar *contents;
	gsize size;

	contents = mb_file_get_contents (file, NULL, &size);

	if (contents)
	{
		fb2_loader = mb_fb2_loader_new_with_size (contents, size);

		g_signal_connect (fb2_loader, "loading-completed",
			              G_CALLBACK (loading_completed_callback), loader);

		g_signal_connect (fb2_loader, "loading-error",
			              G_CALLBACK (loading_error_callback), loader);

		mb_fb2_loader_load_from_memory (fb2_loader);
	}
}

static void
load_from_zip (MbLoader *loader)
{
}

static gchar *
get_error_message (MbLoader *loader, MbLoaderError error)
{
	MbLoaderPrivate *priv;
	gchar *message = NULL;

	priv = loader->priv;
	
	switch (error)
	{
		case MB_LOADER_ERROR_FILE_NOT_EXISTS:
		{
			message = g_strdup_printf ("Файл %s не найден", priv->filename);

			break;
		}
		case MB_LOADER_ERROR_OPEN:
		{
			message = g_strdup_printf ("Невозможно прочитать файл %s",
			                           priv->filename);

			break;
		}
		default:
		{
			break;
		}
	}

	return message;
}

static void
loading_completed_callback (MbFb2Loader *fb2_loader, MbBookBuffer *buffer,
                            MbLoader *loader)
{
	g_signal_emit (loader, mb_loader_signals[SIGNAL_LOADING_COMPLETED], 0,
	               buffer, NULL);

	g_object_unref (fb2_loader);
}

static void
loading_error_callback (MbFb2Loader *fb2_loader, MbFb2LoaderError error,
                        MbLoader *loader)
{
	g_signal_emit (loader, mb_loader_signals[SIGNAL_LOADING_ERROR], 0,
	               FB2_LOADER_ERROR_NAME (error), NULL);
}

