/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * mb-application.h
 * Copyright (C) 2014 Yan Zlobin <yan_zlobin@agniyoga.ru>
 * 
 */

#ifndef _MB_APPLICATION_
#define _MB_APPLICATION_

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define MB_TYPE_APPLICATION             (mb_application_get_type ())
#define MB_APPLICATION(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), MB_TYPE_APPLICATION, MbApplication))
#define MB_APPLICATION_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), MB_TYPE_APPLICATION, MbApplicationClass))
#define MB_IS_APPLICATION(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), MB_TYPE_APPLICATION))
#define MB_IS_APPLICATION_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), MB_TYPE_APPLICATION))
#define MB_APPLICATION_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), MB_TYPE_APPLICATION, MbApplicationClass))

typedef struct _MbApplication MbApplication;
typedef struct _MbApplicationClass MbApplicationClass;
typedef struct _MbApplicationPrivate MbApplicationPrivate;

struct _MbApplication
{
	GtkApplication parent_instance;

	MbApplicationPrivate *priv;
};

struct _MbApplicationClass
{
	GtkApplicationClass parent_class;
};

GType mb_application_get_type (void) G_GNUC_CONST;

MbApplication *mb_application_new (gchar *filename);

G_END_DECLS

#endif
