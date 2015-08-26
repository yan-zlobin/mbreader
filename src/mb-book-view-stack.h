/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * mb-book-view-stack.h
 * Copyright (C) 2014 Yan Zlobin <yan_zlobin@agniyoga.ru>
 * 
 */

#ifndef __MB_BOOK_VIEW_STACK__
#define __MB_BOOK_VIEW_STACK__

#include <glib-object.h>

#define STACK_WIDGET(obj)						(mb_book_view_stack_get_widget (obj))

G_BEGIN_DECLS

#define MB_TYPE_BOOK_VIEW_STACK					(mb_book_view_stack_get_type ())
#define MB_BOOK_VIEW_STACK(obj)					(G_TYPE_CHECK_INSTANCE_CAST ((obj), MB_TYPE_BOOK_VIEW_STACK, MbBookViewStack))
#define MB_IS_BOOK_VIEW_STACK(obj)      		(G_TYPE_CHECK_INSTANCE_TYPE ((obj), MB_TYPE_BOOK_VIEW_STACK))
#define MB_BOOK_VIEW_STACK_CLASS(klass) 		(G_TYPE_CHECK_CLASS_CAST ((klass), MB_TYPE_BOOK_VIEW_STACK, MbBookViewStackClass))
#define MB_IS_BOOK_VIEW_STACK_CLASS(klass)		(G_TYPE_CHECK_CLASS_TYPE ((klass), MB_TYPE_BOOK_VIEW_STACK))
#define MB_BOOK_VIEW_STACK_GET_CLASS(obj)		(G_TYPE_INSTANCE_GET_CLASS ((obj), MB_TYPE_BOOK_VIEW_STACK, MbBookViewStackClass))

typedef enum _MbScrollOrientation MbScrollOrientation;

#define MB_TYPE_SCROLL_ORIENTATION (mb_scroll_orientation_get_type ())
GType mb_scroll_orientation_get_type (void);

typedef struct _MbBookViewStack MbBookViewStack;
typedef struct _MbBookViewStackClass MbBookViewStackClass;
typedef struct _MbBookViewStackPrivate MbBookViewStackPrivate;

enum _MbScrollOrientation
{
	MB_SCROLL_ORIENTATION_HORIZONTAL			= 0,
	MB_SCROLL_ORIENTATION_VERTICAL				= 1
};

struct _MbBookViewStack
{
	GObject parent_instance;

	MbBookViewStackPrivate *priv;
};

struct _MbBookViewStackClass
{
	GObjectClass parent_class;
};

GType mb_book_view_stack_get_type (void) G_GNUC_CONST;

MbBookViewStack *mb_book_view_stack_new (void);

MbScrollOrientation mb_book_view_stack_get_scroll_orientation (MbBookViewStack *stack);

void mb_book_view_stack_set_scroll_orientation (MbBookViewStack *stack,
                                                MbScrollOrientation orientation);

void mb_book_view_stack_set_buffer (MbBookViewStack *stack, gpointer buffer);

GtkWidget *mb_book_view_stack_get_widget (MbBookViewStack *stack);

gchar *mb_book_view_stack_get_title (MbBookViewStack *stack);

void mb_book_view_stack_next_page (MbBookViewStack *stack);

void mb_book_view_stack_prev_page (MbBookViewStack *stack);

G_END_DECLS

#endif
