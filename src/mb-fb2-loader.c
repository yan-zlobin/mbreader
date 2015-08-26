/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * mb-fb2-loader.c
 * Copyright (C) 2014 Yan Zlobin <yan_zlobin@agniyoga.ru>
 * 
 */

#include "mb-fb2-loader.h"
#include "mb-fb2-parser.h"

enum
{
	SIGNAL_LOADING_COMPLETED,
	SIGNAL_LOADING_ERROR,
	SIGNAL_LAST
};

static guint mb_fb2_loader_signals[SIGNAL_LAST] = { 0 };

static void mb_fb2_loader_finalize (GObject *object);

static void load_book_from_file (MbFb2Loader *loader);

G_DEFINE_TYPE (MbFb2Loader, mb_fb2_loader, G_TYPE_OBJECT);

static void
mb_fb2_loader_init (MbFb2Loader *loader)
{
}

static void
mb_fb2_loader_class_init (MbFb2LoaderClass *klass)
{
	GObjectClass *gobject_class;
	
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
mb_fb2_loader_new (gchar *filename)
{
	MbFb2Loader *loader;

	loader = g_object_new (MB_TYPE_FB2_LOADER, NULL);
	loader->filename = filename;

	return loader;
}

void
mb_fb2_loader_load_from_fb2_file (MbFb2Loader *loader)
{
	g_thread_new (NULL, (GThreadFunc) load_book_from_file, (gpointer) loader);
}

void
mb_fb2_loader_load_from_zip_file (MbFb2Loader *loader)
{
	
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
	ParserSAXResult parser_result;
	gchar *filename;

	buffer = mb_book_buffer_new ();
	filename = loader->filename;

	parser_result = mb_fb2_parser_parse_file (buffer, filename);

	if (parser_result == PARSER_SAX_RESULT_SUCCESS)
	{
		g_signal_emit (loader, mb_fb2_loader_signals[SIGNAL_LOADING_COMPLETED],
		               0, buffer);
	}
	else if (parser_result == PARSER_SAX_RESULT_NOT_FINISH_STATE)
	{
		g_signal_emit (loader, mb_fb2_loader_signals[SIGNAL_LOADING_ERROR],
		               0, MB_FB2_LOADER_ERROR_PARSE);
	}
	else if (parser_result == PARSER_SAX_RESULT_NOT_WELL_FORMED_DOCUMENT)
	{
		g_signal_emit (loader, mb_fb2_loader_signals[SIGNAL_LOADING_ERROR],
		               0, MB_FB2_LOADER_ERROR_NOT_WELL_FORMED_DOCUMENT);
	}
	else
	{
		g_signal_emit (loader, mb_fb2_loader_signals[SIGNAL_LOADING_ERROR],
		               0, MB_FB2_LOADER_ERROR_NOT_VALID_XML_FILE);
	}
}
