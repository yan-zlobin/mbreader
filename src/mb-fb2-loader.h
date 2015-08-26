/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * mb-fb2-loader.h
 * Copyright (C) 2014 Yan Zlobin <yan_zlobin@agniyoga.ru>
 * 
 */

#ifndef __MB_FB2_LOADER__
#define __MB_FB2_LOADER__

#include <glib-object.h>
#include "mb-book-buffer.h"

#define FB2_LOADER_ERROR_NAME(obj)			(mb_fb2_loader_get_error_name (obj))

G_BEGIN_DECLS

#define MB_TYPE_FB2_LOADER					(mb_fb2_loader_get_type ())
#define MB_FB2_LOADER(obj)					(G_TYPE_CHECK_INSTANCE_CAST ((obj), MB_TYPE_FB2_LOADER, MbFb2Loader))
#define MB_IS_FB2_LOADER(obj)         		(G_TYPE_CHECK_INSTANCE_TYPE ((obj), MB_TYPE_FB2_LOADER))
#define MB_FB2_LOADER_CLASS(klass)   		(G_TYPE_CHECK_CLASS_CAST ((klass), MB_TYPE_FB2_LOADER, MbFb2LoaderClass))
#define MB_IS_FB2_LOADER_CLASS(klass)		(G_TYPE_CHECK_CLASS_TYPE ((klass), MB_TYPE_FB2_LOADER))
#define MB_FB2_LOADER_GET_CLASS(obj)		(G_TYPE_INSTANCE_GET_CLASS ((obj), MB_TYPE_FB2_LOADER, MbFb2LoaderClass))

typedef enum _MbFb2LoaderError MbFb2LoaderError;

typedef struct _MbFb2Loader MbFb2Loader;
typedef struct _MbFb2LoaderClass MbFb2LoaderClass;

enum _MbFb2LoaderError
{
	MB_FB2_LOADER_ERROR_PARSE,
	MB_FB2_LOADER_ERROR_NOT_VALID_XML_FILE,
	MB_FB2_LOADER_ERROR_NOT_WELL_FORMED_DOCUMENT
};

struct _MbFb2Loader
{
	GObject parent_instance;

	gchar *filename;
};

struct _MbFb2LoaderClass
{
	GObjectClass parent_class;

	/* Signals */
	void (*loading_completed)				(MbFb2Loader *loader, MbBookBuffer *buffer);
	void (*loading_error)					(MbFb2Loader *loader, MbFb2LoaderError error);
};

GType mb_fb2_loader_get_type (void) G_GNUC_CONST;

MbFb2Loader *mb_fb2_loader_new (gchar *filename);

void mb_fb2_loader_load_from_fb2_file (MbFb2Loader *loader);

void mb_fb2_loader_load_from_zip_file (MbFb2Loader *loader);

gchar *mb_fb2_loader_get_error_name (MbFb2LoaderError error);

G_END_DECLS

#endif
