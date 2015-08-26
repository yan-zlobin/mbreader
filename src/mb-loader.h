/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * mb-loader.h
 * Copyright (C) 2014 Yan Zlobin <yan_zlobin@agniyoga.ru>
 * 
 */

#ifndef __MB_LOADER__
#define __MB_LOADER__

#include <glib-object.h>

G_BEGIN_DECLS

#include "mb-book-buffer.h"

#define MB_TYPE_LOADER					(mb_loader_get_type ())
#define MB_LOADER(obj)					(G_TYPE_CHECK_INSTANCE_CAST ((obj), MB_TYPE_LOADER, MbLoader))
#define MB_IS_LOADER(obj)         		(G_TYPE_CHECK_INSTANCE_TYPE ((obj), MB_TYPE_LOADER))
#define MB_LOADER_CLASS(klass)   		(G_TYPE_CHECK_CLASS_CAST ((klass), MB_TYPE_LOADER, MbLoaderClass))
#define MB_IS_LOADER_CLASS(klass)		(G_TYPE_CHECK_CLASS_TYPE ((klass), MB_TYPE_LOADER))
#define MB_LOADER_GET_CLASS(obj)		(G_TYPE_INSTANCE_GET_CLASS ((obj), MB_TYPE_LOADER, MbLoaderClass))

typedef struct _MbLoader MbLoader;
typedef struct _MbLoaderClass MbLoaderClass;
typedef struct _MbLoaderPrivate MbLoaderPrivate;

struct _MbLoader
{
	GObject parent_instance;

	MbLoaderPrivate *priv;
};

struct _MbLoaderClass
{
	GObjectClass parent_class;

	/* Signals */
	void (*loading_completed)			(MbLoader *loader, MbBookBuffer *buffer);
	void (*loading_error)				(MbLoader *loader, gchar *message);
};

GType mb_loader_get_type (void) G_GNUC_CONST;

MbLoader *mb_loader_new (gchar *filename);

void mb_loader_load_book (MbLoader *loader);

G_END_DECLS

#endif
