/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * mb-storage.h
 * Copyright (C) 2014 Yan Zlobin <yan_zlobin@agniyoga.ru>
 * 
 */

#ifndef __MB_STORAGE__
#define __MB_STORAGE__

#include <glib-object.h>

G_BEGIN_DECLS

#include "../mb-reference.h"

#define MB_TYPE_STORAGE					(mb_storage_get_type ())
#define MB_STORAGE(obj)					(G_TYPE_CHECK_INSTANCE_CAST ((obj), MB_TYPE_STORAGE, MbStorage))
#define MB_IS_STORAGE(obj)      		(G_TYPE_CHECK_INSTANCE_TYPE ((obj), MB_TYPE_STORAGE))
#define MB_STORAGE_CLASS(klass) 		(G_TYPE_CHECK_CLASS_CAST ((klass), MB_TYPE_STORAGE, MbStorageClass))
#define MB_IS_STORAGE_CLASS(klass)		(G_TYPE_CHECK_CLASS_TYPE ((klass), MB_TYPE_STORAGE))
#define MB_STORAGE_GET_CLASS(obj)		(G_TYPE_INSTANCE_GET_CLASS ((obj), MB_TYPE_STORAGE, MbStorageClass))

typedef struct _MbStorage MbStorage;
typedef struct _MbStorageClass MbStorageClass;
typedef struct _MbStoragePrivate MbStoragePrivate;

struct _MbStorage
{
	GObject parent_instance;

	MbStoragePrivate *priv;
};

struct _MbStorageClass
{
	GObjectClass parent_class;

	/* Signals */
	void (*ready)						(MbStorage *storage);
};

GType mb_storage_get_type (void) G_GNUC_CONST;

MbStorage *mb_storage_new (MbReference *reference);

void mb_storage_open (MbStorage *storage);

void mb_storage_append_file (MbStorage *storage, gchar *filename);

void mb_storage_append_file_list (MbStorage *storage, GSList *file_list);

G_END_DECLS

#endif
