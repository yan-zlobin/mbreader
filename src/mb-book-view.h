/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * mb-book-view.h
 * Copyright (C) 2014 Yan Zlobin <yan_zlobin@agniyoga.ru>
 * 
 */

#ifndef __MB_BOOK_VIEW__
#define __MB_BOOK_VIEW__

#include <gtk/gtkwidget.h>

G_BEGIN_DECLS

#include "mb-book-buffer.h"

#define MB_TYPE_BOOK_VIEW					(mb_book_view_get_type ())
#define MB_BOOK_VIEW(obj)					(G_TYPE_CHECK_INSTANCE_CAST ((obj), MB_TYPE_BOOK_VIEW, MbBookView))
#define MB_IS_BOOK_VIEW(obj)      			(G_TYPE_CHECK_INSTANCE_TYPE ((obj), MB_TYPE_BOOK_VIEW))
#define MB_BOOK_VIEW_CLASS(klass) 			(G_TYPE_CHECK_CLASS_CAST ((klass), MB_TYPE_BOOK_VIEW, MbBookViewClass))
#define MB_IS_BOOK_VIEW_CLASS(klass)		(G_TYPE_CHECK_CLASS_TYPE ((klass), MB_TYPE_BOOK_VIEW))
#define MB_BOOK_VIEW_GET_CLASS(obj)			(G_TYPE_INSTANCE_GET_CLASS ((obj), MB_TYPE_BOOK_VIEW, MbBookViewClass))

typedef enum _MbBookViewFlippingDirection MbBookViewFlippingDirection;

typedef struct _MbBookView MbBookView;
typedef struct _MbBookViewClass MbBookViewClass;
typedef struct _MbBookViewPrivate MbBookViewPrivate;

enum _MbBookViewFlippingDirection
{
	MB_BOOK_VIEW_FLIPPING_DIRECTION_FORWARD,
	MB_BOOK_VIEW_FLIPPING_DIRECTION_BACKWARD
};

struct _MbBookView
{
	GtkWidget parent_instance;

	MbBookViewPrivate *priv;
};

struct _MbBookViewClass
{
	GtkWidgetClass parent_class;

	/* Signals */
	void (*page_draw)						(MbBookView *view);
};

GType mb_book_view_get_type (void) G_GNUC_CONST;

GtkWidget *mb_book_view_new (void);

void mb_book_view_set_buffer (MbBookView *view, MbBookBuffer *buffer);

void mb_book_view_show_scroll_next (MbBookView *view);

void mb_book_view_show_scroll_prev (MbBookView *view);

G_END_DECLS

#endif
