/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * mb-loader.c
 * Copyright (C) 2014 Yan Zlobin <yan_zlobin@agniyoga.ru>
 * 
 */

#include "mb-loader.h"
#include "mb-fb2-loader.h"
#include "mb-epub-loader.h"

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

static gchar *mb_loader_get_error_message (MbLoader *loader,
                                           MbLoaderError error);

static gboolean file_exists (MbLoader *loader);

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
	if (!file_exists (loader))
	{
		return;
	}

	MbLoaderPrivate *priv;
	gchar *filename;

	priv = loader->priv;
	filename = priv->filename;

	if (g_str_has_suffix (filename, ".fb2")
	    || g_str_has_suffix (filename, ".fb2.gz"))
	{
		MbFb2Loader *fb2_loader;

		fb2_loader = mb_fb2_loader_new (filename);

		g_signal_connect (fb2_loader, "loading-completed",
		                  G_CALLBACK (loading_completed_callback), loader);

		g_signal_connect (fb2_loader, "loading-error",
		                  G_CALLBACK (loading_error_callback), loader);

		mb_fb2_loader_load_from_fb2_file (fb2_loader);

	}
	else if (g_str_has_suffix (filename, ".fb2.zip"))
	{
		MbFb2Loader *fb2_loader;

		fb2_loader = mb_fb2_loader_new (filename);

		g_signal_connect (fb2_loader, "loading-completed",
		                  G_CALLBACK (loading_completed_callback), loader);

		mb_fb2_loader_load_from_zip_file (fb2_loader);
	}
}

static gchar *
mb_loader_get_error_message (MbLoader *loader, MbLoaderError error)
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

static gboolean
file_exists (MbLoader *loader)
{
	MbLoaderPrivate *priv;
	gchar *message;
	gboolean result;

	priv = loader->priv;
	result = g_file_test (priv->filename, G_FILE_TEST_IS_REGULAR);

	if (!result)
	{
		message = mb_loader_get_error_message (loader,
		                                       MB_LOADER_ERROR_FILE_NOT_EXISTS);
		
		g_signal_emit (loader, mb_loader_signals[SIGNAL_LOADING_ERROR], 0,
		               message, NULL);

		g_free (message);
	}

	return result;
}

static void
loading_completed_callback (MbFb2Loader *fb2_loader, MbBookBuffer *buffer,
                            MbLoader *loader)
{
	g_object_unref (fb2_loader);
	
	g_signal_emit (loader, mb_loader_signals[SIGNAL_LOADING_COMPLETED], 0,
	               buffer, NULL);
}

static void
loading_error_callback (MbFb2Loader *fb2_loader, MbFb2LoaderError error,
                        MbLoader *loader)
{
	g_signal_emit (loader, mb_loader_signals[SIGNAL_LOADING_ERROR], 0,
	               FB2_LOADER_ERROR_NAME (error), NULL);
}

