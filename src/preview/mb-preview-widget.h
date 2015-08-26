/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * mb-preview-widget.h
 * Copyright (C) 2014 Yan Zlobin <yan_zlobin@agniyoga.ru>
 * 
 */

#ifndef __MB_PREVIEW_WIDGET__
#define __MB_PREVIEW_WIDGET__

#include <gtk/gtkwidget.h>

G_BEGIN_DECLS

#include "mb-reference.h"

#define MB_TYPE_REVIEW_WIDGET				(mb_preview_widget_get_type ())
#define MB_PREVIEW_WIDGET(obj)				(G_TYPE_CHECK_INSTANCE_CAST ((obj), MB_TYPE_REVIEW_WIDGET, MbPreviewWidget))
#define MB_IS_REVIEW_WIDGET(obj)      		(G_TYPE_CHECK_INSTANCE_TYPE ((obj), MB_TYPE_REVIEW_WIDGET))
#define MB_PREVIEW_WIDGET_CLASS(klass) 		(G_TYPE_CHECK_CLASS_CAST ((klass), MB_TYPE_REVIEW_WIDGET, MbPreviewWidgetClass))
#define MB_IS_REVIEW_WIDGET_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE ((klass), MB_TYPE_REVIEW_WIDGET))
#define MB_PREVIEW_WIDGET_GET_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS ((obj), MB_TYPE_REVIEW_WIDGET, MbPreviewWidgetClass))

typedef struct _MbPreviewWidget MbPreviewWidget;
typedef struct _MbPreviewWidgetClass MbPreviewWidgetClass;
typedef struct _MbPreviewWidgetPrivate MbPreviewWidgetPrivate;

struct _MbPreviewWidget
{
	GtkWidget parent_instance;

	MbPreviewWidgetPrivate *priv;
};

struct _MbPreviewWidgetClass
{
	GtkWidgetClass parent_class;
};

GType mb_preview_widget_get_type (void) G_GNUC_CONST;

GtkWidget *mb_preview_widget_new (MbReference *reference);

gboolean mb_preview_widget_set_filename (MbPreviewWidget *widget,
                                         gchar *filename);

G_END_DECLS

#endif
