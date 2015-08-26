/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * mb-file.h
 * Copyright (C) 2014 Yan Zlobin <yan_zlobin@agniyoga.ru>
 * 
 */

#ifndef __MB_FILE__
#define __MB_FILE__

#include <glib-object.h>

#define SINGLE_FILE(obj)			(mb_file_get_count (obj) == 1)

G_BEGIN_DECLS

#include "../types/mb-types.h"
#include "../preview/mb-book-preview.h"

#define MB_TYPE_FILE				(mb_file_get_type ())
#define MB_FILE(obj)				(G_TYPE_CHECK_INSTANCE_CAST ((obj), MB_TYPE_FILE, MbFile))
#define MB_IS_FILE(obj)      		(G_TYPE_CHECK_INSTANCE_TYPE ((obj), MB_TYPE_FILE))
#define MB_FILE_CLASS(klass) 		(G_TYPE_CHECK_CLASS_CAST ((klass), MB_TYPE_FILE, MbFileClass))
#define MB_IS_FILE_CLASS(klass)		(G_TYPE_CHECK_CLASS_TYPE ((klass), MB_TYPE_FILE))
#define MB_FILE_GET_CLASS(obj)		(G_TYPE_INSTANCE_GET_CLASS ((obj), MB_TYPE_FILE, MbFileClass))

typedef enum _MbFileType MbFileType;

typedef struct _MbFile MbFile;
typedef struct _MbFileClass MbFileClass;
typedef struct _MbFilePrivate MbFilePrivate;

enum _MbFileType
{
	MB_FILE_TYPE_UNKNOWN,
	MB_FILE_TYPE_FB2,
	MB_FILE_TYPE_FB2_ZIP,
	MB_FILE_TYPE_EPUB,
	MB_FILE_TYPE_ZIP
};

struct _MbFile
{
	GObject parent_instance;

	MbFilePrivate *priv;
};

struct _MbFileClass
{
	GObjectClass parent_class;
};

GType mb_file_get_type (void) G_GNUC_CONST;

MbFile *mb_file_new (gchar *filename);

guint mb_file_get_count (MbFile *file);

MbBookPreview *mb_file_get_preview (MbFile *file, gchar *filename);

MbBookPreview *mb_file_get_preview_full (MbFile *file, gchar *filename);

gchar *mb_file_get_checksum (MbFile *file, gchar *filename);

gchar *mb_file_get_intern_filename (MbFile *file);

G_END_DECLS

#endif
