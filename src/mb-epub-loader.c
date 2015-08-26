/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * mb-epub-loader.c
 * Copyright (C) 2014 Yan Zlobin <yan_zlobin@agniyoga.ru>
 * 
 */

#include "mb-epub-loader.h"

enum
{
	SIGNAL_LOADING_COMPLETED,
	SIGNAL_LOADING_ERROR,
	SIGNAL_LAST
};

static guint mb_epub_loader_signals[SIGNAL_LAST] = { 0 };

static void mb_epub_loader_finalize (GObject *object);

G_DEFINE_TYPE (MbEpubLoader, mb_epub_loader, G_TYPE_OBJECT);

static void
mb_epub_loader_init (MbEpubLoader *loader)
{
}

static void
mb_epub_loader_class_init (MbEpubLoaderClass *klass)
{
	GObjectClass *gobject_class;
	
	gobject_class = G_OBJECT_CLASS (klass);

	mb_epub_loader_signals[SIGNAL_LOADING_COMPLETED] =
		g_signal_new ("loading-completed", G_TYPE_FROM_CLASS (klass),
		              G_SIGNAL_RUN_LAST,
		              G_STRUCT_OFFSET (MbEpubLoaderClass, loading_completed),
		              NULL, NULL, g_cclosure_marshal_VOID__OBJECT,
		              G_TYPE_NONE, 1, MB_TYPE_BOOK_BUFFER);

	mb_epub_loader_signals[SIGNAL_LOADING_ERROR] =
		g_signal_new ("loading-error", G_TYPE_FROM_CLASS (klass),
		              G_SIGNAL_RUN_LAST,
		              G_STRUCT_OFFSET (MbEpubLoaderClass, loading_error),
		              NULL, NULL, g_cclosure_marshal_VOID__OBJECT,
		              G_TYPE_NONE, 1, G_TYPE_UINT);

	gobject_class->finalize = mb_epub_loader_finalize;
}

static void
mb_epub_loader_finalize (GObject *object)
{
	G_OBJECT_CLASS (mb_epub_loader_parent_class)->finalize (object);
}

MbEpubLoader *
mb_epub_loader_new (void)
{
	MbEpubLoader *loader;

	loader = g_object_new (MB_TYPE_EPUB_LOADER, NULL);

	return loader;
}

