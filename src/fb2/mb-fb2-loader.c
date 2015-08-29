/* Magic Book Reader
 * Copyright (C) 2015 Yan Zlobin <yan_zlobin@agniyoga.ru>
 *
 * mb-fb2-loader.c
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

#include "mb-fb2-loader.h"
#include "mb-fb2-parser.h"

#define MB_FB2_LOADER_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), \
	MB_TYPE_FB2_LOADER, MbFb2LoaderPrivate))

struct _MbFb2LoaderPrivate
{
	gchar *file;
	gsize size;
};

enum
{
	SIGNAL_LOADING_COMPLETED,
	SIGNAL_LOADING_ERROR,
	SIGNAL_LAST
};

static guint mb_fb2_loader_signals[SIGNAL_LAST] = { 0 };

static void mb_fb2_loader_finalize (GObject *object);

static void load_book_from_file (MbFb2Loader *loader);

static void load_book_from_memory (MbFb2Loader *loader);

static void check_parse_status (MbFb2Loader *loader, MbBookBuffer *buffer,
                                ParserSAXResult rc);

G_DEFINE_TYPE (MbFb2Loader, mb_fb2_loader, G_TYPE_OBJECT);

static void
mb_fb2_loader_init (MbFb2Loader *loader)
{
	MbFb2LoaderPrivate *priv;

	priv = MB_FB2_LOADER_GET_PRIVATE (loader);
	loader->priv = priv;

	priv->file = NULL;
	priv->size = 0;
}

static void
mb_fb2_loader_class_init (MbFb2LoaderClass *klass)
{
	GObjectClass *gobject_class;

	g_type_class_add_private (klass, sizeof (MbFb2LoaderPrivate));
	
	gobject_class = G_OBJECT_CLASS (klass);

	mb_fb2_loader_signals[SIGNAL_LOADING_COMPLETED] =
		g_signal_new ("loading-completed", G_TYPE_FROM_CLASS (klass),
		              G_SIGNAL_RUN_LAST,
		              G_STRUCT_OFFSET (MbFb2LoaderClass, loading_completed),
		              NULL, NULL, g_cclosure_marshal_VOID__OBJECT,
		              G_TYPE_NONE, 1, MB_TYPE_BOOK_BUFFER);

	mb_fb2_loader_signals[SIGNAL_LOADING_ERROR] =
		g_signal_new ("loading-error", G_TYPE_FROM_CLASS (klass),
		              G_SIGNAL_RUN_LAST,
		              G_STRUCT_OFFSET (MbFb2LoaderClass, loading_error),
		              NULL, NULL, g_cclosure_marshal_VOID__OBJECT,
		              G_TYPE_NONE, 1, G_TYPE_UINT);

	gobject_class->finalize = mb_fb2_loader_finalize;
}

static void
mb_fb2_loader_finalize (GObject *object)
{
	G_OBJECT_CLASS (mb_fb2_loader_parent_class)->finalize (object);
}

MbFb2Loader *
mb_fb2_loader_new (gchar *file)
{
	MbFb2Loader *loader;

	loader = g_object_new (MB_TYPE_FB2_LOADER, NULL);
	loader->priv->file = file;

	return loader;
}

MbFb2Loader *
mb_fb2_loader_new_with_size (gchar *file, gsize size)
{
	MbFb2Loader *loader;

	loader = mb_fb2_loader_new (file);
	loader->priv->size = size;

	return loader;
}

void
mb_fb2_loader_load_from_fb2_file (MbFb2Loader *loader)
{
	g_thread_new (NULL, (GThreadFunc) load_book_from_file, (gpointer) loader);
}

void
mb_fb2_loader_load_from_memory (MbFb2Loader *loader)
{
	g_thread_new (NULL, (GThreadFunc) load_book_from_memory, (gpointer) loader);
}

gchar *
mb_fb2_loader_get_error_name (MbFb2LoaderError error)
{
	switch (error)
	{
		case MB_FB2_LOADER_ERROR_NOT_WELL_FORMED_DOCUMENT:
		{
			return "Некорректный XML-документ";
		}
		case MB_FB2_LOADER_ERROR_NOT_VALID_XML_FILE:
		{
			return "Недействительный XML-документ";
		}
		default:
		{
			return "Ошибка разбора XML-документа";
		}
	}
}

static void
load_book_from_file (MbFb2Loader *loader)
{
	MbBookBuffer *buffer;
	ParserSAXResult rc;

	buffer = mb_book_buffer_new ();
	rc = mb_fb2_parser_parse_file (buffer, loader->priv->file);

	check_parse_status (loader, buffer, rc);
}

static void
load_book_from_memory (MbFb2Loader *loader)
{
	MbFb2LoaderPrivate *priv;
	MbBookBuffer *buffer;
	ParserSAXResult rc;

	priv = loader->priv;

	buffer = mb_book_buffer_new ();
	rc = mb_fb2_parser_parse_memory (buffer, priv->file, priv->size);

	g_free (priv->file);

	check_parse_status (loader, buffer, rc);
}

static void
check_parse_status (MbFb2Loader *loader, MbBookBuffer *buffer,
                    ParserSAXResult rc)
{
	if (rc == PARSER_SAX_RESULT_SUCCESS)
	{
		g_signal_emit (loader, mb_fb2_loader_signals[SIGNAL_LOADING_COMPLETED],
		               0, buffer);
	}
	else if (rc == PARSER_SAX_RESULT_NOT_FINISH_STATE)
	{
		g_signal_emit (loader, mb_fb2_loader_signals[SIGNAL_LOADING_ERROR],
		               0, MB_FB2_LOADER_ERROR_PARSE);

		g_object_unref (buffer);
	}
	else if (rc == PARSER_SAX_RESULT_NOT_WELL_FORMED_DOCUMENT)
	{
		g_signal_emit (loader, mb_fb2_loader_signals[SIGNAL_LOADING_ERROR],
		               0, MB_FB2_LOADER_ERROR_NOT_WELL_FORMED_DOCUMENT);

		g_object_unref (buffer);
	}
	else
	{
		g_signal_emit (loader, mb_fb2_loader_signals[SIGNAL_LOADING_ERROR],
		               0, MB_FB2_LOADER_ERROR_NOT_VALID_XML_FILE);

		g_object_unref (buffer);
	}
}

