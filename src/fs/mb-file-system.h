/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * mb-file-system.h
 * Copyright (C) 2014 Yan Zlobin <yan_zlobin@agniyoga.ru>
 * 
 */

#ifndef __MB_FILE_SYSTEM__
#define __MB_FILE_SYSTEM__

#include <glib-object.h>

G_BEGIN_DECLS

#define MB_TYPE_FILE_SYSTEM					(mb_file_system_get_type ())
#define MB_FILE_SYSTEM(obj)					(G_TYPE_CHECK_INSTANCE_CAST ((obj), MB_TYPE_FILE_SYSTEM, MbFileSystem))
#define MB_IS_FILE_SYSTEM(obj)      		(G_TYPE_CHECK_INSTANCE_TYPE ((obj), MB_TYPE_FILE_SYSTEM))
#define MB_FILE_SYSTEM_CLASS(klass) 		(G_TYPE_CHECK_CLASS_CAST ((klass), MB_TYPE_FILE_SYSTEM, MbFileSystemClass))
#define MB_IS_FILE_SYSTEM_CLASS(klass)		(G_TYPE_CHECK_CLASS_TYPE ((klass), MB_TYPE_FILE_SYSTEM))
#define MB_FILE_SYSTEM_GET_CLASS(obj)		(G_TYPE_INSTANCE_GET_CLASS ((obj), MB_TYPE_FILE_SYSTEM, MbFileSystemClass))

typedef struct _MbFileSystem MbFileSystem;
typedef struct _MbFileSystemClass MbFileSystemClass;
typedef struct _MbFileSystemPrivate MbFileSystemPrivate;

struct _MbFileSystem
{
	GObject parent_instance;

	MbFileSystemPrivate *priv;
};

struct _MbFileSystemClass
{
	GObjectClass parent_class;

	/* Signals */
	void (*file_found)						(MbFileSystem *fs, GSList *file_list);
	void (*file_updated)					(MbFileSystem *fs, gchar *filename);
};

GType mb_file_system_get_type (void) G_GNUC_CONST;

MbFileSystem *mb_file_system_new (void);

void mb_file_system_open (MbFileSystem *fs, gchar *filename);

G_END_DECLS

#endif
