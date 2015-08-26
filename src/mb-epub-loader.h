/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * mb-epub-loader.h
 * Copyright (C) 2014 Yan Zlobin <yan_zlobin@agniyoga.ru>
 * 
 */

#ifndef __MB_EPUB_LOADER__
#define __MB_EPUB_LOADER__

#include <glib-object.h>
#include "mb-book-buffer.h"

G_BEGIN_DECLS

#define MB_TYPE_EPUB_LOADER					(mb_epub_loader_get_type ())
#define MB_EPUB_LOADER(obj)					(G_TYPE_CHECK_INSTANCE_CAST ((obj), MB_TYPE_EPUB_LOADER, MbEpubLoader))
#define MB_IS_EPUB_LOADER(obj)         		(G_TYPE_CHECK_INSTANCE_TYPE ((obj), MB_TYPE_EPUB_LOADER))
#define MB_EPUB_LOADER_CLASS(klass)   		(G_TYPE_CHECK_CLASS_CAST ((klass), MB_TYPE_EPUB_LOADER, MbEpubLoaderClass))
#define MB_IS_EPUB_LOADER_CLASS(klass)		(G_TYPE_CHECK_CLASS_TYPE ((klass), MB_TYPE_EPUB_LOADER))
#define MB_EPUB_LOADER_GET_CLASS(obj)		(G_TYPE_INSTANCE_GET_CLASS ((obj), MB_TYPE_EPUB_LOADER, MbEpubLoaderClass))

typedef struct _MbEpubLoader MbEpubLoader;
typedef struct _MbEpubLoaderClass MbEpubLoaderClass;

typedef enum
{
	MB_EPUB_LOADER_ERROR_FILE_CANNOT_READ,
	MB_EPUB_LOADER_ERROR_NOT_WELL_FORMED
} MbEpubLoaderError;

struct _MbEpubLoader
{
	GObject parent_instance;
};

struct _MbEpubLoaderClass
{
	GObjectClass parent_class;

	/* Signals */
	void	(*loading_completed)	(MbEpubLoader *loader, MbBookBuffer *buffer);
	void	(*loading_error)		(MbEpubLoader *loader, MbEpubLoaderError error);
};

GType mb_epub_loader_get_type (void) G_GNUC_CONST;

MbEpubLoader *mb_epub_loader_new (void);

G_END_DECLS

#endif
