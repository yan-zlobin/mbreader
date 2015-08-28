/* Magic Book Reader
 * Copyright (C) 2015 Yan Zlobin <yan_zlobin@agniyoga.ru>
 *
 * mb-book-view-stack.c
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <gtk/gtk.h>
#include "mb-book-view-stack.h"
#include "mb-book-buffer.h"
#include "mb-book-view.h"

#define CHILD_1_NAME				"1"
#define CHILD_2_NAME				"2"
#define PAGE_FLIPPING_DURATION		400

#define EQUALS(a,b)					(!((gboolean) g_strcmp0 (a, b)))
#define VISIBLE_CHILD(obj)			(gtk_stack_get_visible_child (GTK_STACK ((obj)->priv->gtk_stack)))
#define VISIBLE_CHILD_NAME(obj)		(gtk_stack_get_visible_child_name (GTK_STACK ((obj)->priv->gtk_stack)))
#define NEXT_CHILD_NAME(obj)		(get_next_child_name (obj))
#define NEXT_CHILD(obj)				(gtk_stack_get_child_by_name (GTK_STACK ((obj)->priv->gtk_stack), NEXT_CHILD_NAME (obj)))

#define C_ENUM(v)					((gint) v)

#define MB_BOOK_VIEW_STACK_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), \
	MB_TYPE_BOOK_VIEW_STACK, MbBookViewStackPrivate))

struct _MbBookViewStackPrivate
{
	MbBookBuffer *buffer;

	MbScrollOrientation scroll_orientation;
	
	GtkWidget *gtk_stack;

	GtkWidget *book_view_1;
	GtkWidget *book_view_2;
};

enum
{
	PROP_0,
	PROP_BOOK_BUFFER,
	PROP_SCROLL_ORIENTATION,
	PROP_LAST
};

static void get_property (GObject *object, guint propid, GValue *value,
                          GParamSpec *pspec);

static void set_property (GObject *object, guint propid, const GValue *value,
                          GParamSpec *pspec);

static void mb_book_view_stack_finalize (GObject *object);

static gchar *get_next_child_name (MbBookViewStack *stack);

static void book_view_synchronize (MbBookViewStack *stack);

G_DEFINE_TYPE (MbBookViewStack, mb_book_view_stack, G_TYPE_OBJECT);

GType
mb_scroll_orientation_get_type (void)
{
	static gsize id = 0;
	
	static const GEnumValue values[] = {
		{C_ENUM (MB_SCROLL_ORIENTATION_HORIZONTAL), "MB_SCROLL_ORIENTATION_HORIZONTAL", "horizontal"},
		{C_ENUM (MB_SCROLL_ORIENTATION_VERTICAL), "MB_SCROLL_ORIENTATION_VERTICAL", "vertical"},
		{0, NULL, NULL}
	};

	if (g_once_init_enter (&id))
	{
		GType tmp = g_enum_register_static ("MbScrollOrientation", values);
		g_once_init_leave (&id, tmp);
	}

	return (GType) id;
}

static gboolean
configure_event_callback (GtkWidget *widget, GdkEvent *event,
                          gpointer user_data)
{
	return TRUE;
}

static void
mb_book_view_stack_init (MbBookViewStack *stack)
{
	MbBookViewStackPrivate *priv;

	priv = MB_BOOK_VIEW_STACK_GET_PRIVATE (stack);
	stack->priv = priv;

	priv->gtk_stack = gtk_stack_new ();
	priv->book_view_1 = mb_book_view_new ();
	priv->book_view_2 = mb_book_view_new ();

	gtk_stack_set_transition_duration (GTK_STACK (priv->gtk_stack),
	                                   PAGE_FLIPPING_DURATION);

	gtk_stack_add_named (GTK_STACK (priv->gtk_stack), priv->book_view_1,
	                     CHILD_1_NAME);

	gtk_stack_add_named (GTK_STACK (priv->gtk_stack), priv->book_view_2,
	                     CHILD_2_NAME);
	
	g_signal_connect (priv->gtk_stack, "configure-event",
	                  G_CALLBACK (configure_event_callback), NULL);
}

static void
mb_book_view_stack_class_init (MbBookViewStackClass *klass)
{
	GObjectClass *gobject_class;
	
	g_type_class_add_private (klass, sizeof (MbBookViewStackPrivate));

	gobject_class = G_OBJECT_CLASS (klass);

	gobject_class->get_property = get_property;
	gobject_class->set_property = set_property;
	gobject_class->finalize = mb_book_view_stack_finalize;

	g_object_class_install_property (gobject_class, PROP_BOOK_BUFFER,
		g_param_spec_object ("book-buffer", "Book Buffer",
		                     "The Book Buffer",
		                     MB_TYPE_BOOK_BUFFER,
		                     G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

	g_object_class_install_property (gobject_class, PROP_SCROLL_ORIENTATION,
		g_param_spec_enum ("scroll-orientation", "Scroll Orientation",
		                   "Book View Scroll Orientation",
		                   MB_TYPE_SCROLL_ORIENTATION,
		                   MB_SCROLL_ORIENTATION_HORIZONTAL,
		                   G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
}

static void
get_property (GObject *object, guint propid, GValue *value, GParamSpec *pspec)
{
	MbBookViewStack *stack = MB_BOOK_VIEW_STACK (object);

	switch (propid)
	{
		case PROP_BOOK_BUFFER:
		{
			g_value_set_object (value, stack->priv->buffer);

			break;
		}
		case PROP_SCROLL_ORIENTATION:
		{
			g_value_set_enum (value,
			                  mb_book_view_stack_get_scroll_orientation (stack));

			break;
		}
		default:
		{
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, propid, pspec);
		}
	}
}

static void
set_property (GObject *object, guint propid, const GValue *value,
              GParamSpec *pspec)
{
	MbBookViewStack *stack = MB_BOOK_VIEW_STACK (object);

	switch (propid)
	{
		case PROP_BOOK_BUFFER:
		{
			mb_book_view_stack_set_buffer (stack, g_value_get_object (value));

			break;
		}
		case PROP_SCROLL_ORIENTATION:
		{
			mb_book_view_stack_set_scroll_orientation (stack,
			                                           g_value_get_enum (value));
			
			break;
		}
		default:
		{
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, propid, pspec);
		}
	}
}

static void
mb_book_view_stack_finalize (GObject *object)
{
	MbBookViewStack *stack;
	MbBookViewStackPrivate *priv;

	stack = MB_BOOK_VIEW_STACK (object);
	priv = stack->priv;

	g_object_unref (priv->buffer);

	G_OBJECT_CLASS (mb_book_view_stack_parent_class)->finalize (object);
}

MbBookViewStack *
mb_book_view_stack_new (void)
{
	MbBookViewStack *stack;

	stack = g_object_new (MB_TYPE_BOOK_VIEW_STACK, NULL);

	return stack;
}

MbScrollOrientation
mb_book_view_stack_get_scroll_orientation (MbBookViewStack *stack)
{
	return stack->priv->scroll_orientation;
}

void
mb_book_view_stack_set_scroll_orientation (MbBookViewStack *stack,
                                           MbScrollOrientation orientation)
{
	g_return_if_fail (MB_IS_BOOK_VIEW_STACK (stack));

	stack->priv->scroll_orientation = orientation;
}

void
mb_book_view_stack_set_buffer (MbBookViewStack *stack, gpointer buffer)
{
	MbBookViewStackPrivate *priv;
	
	g_return_if_fail (MB_IS_BOOK_VIEW_STACK (stack));
	g_return_if_fail (MB_IS_BOOK_BUFFER (buffer));

	priv = stack->priv;

	if (priv->buffer)
	{
		g_object_unref (priv->buffer);

		priv->buffer = NULL;
	}
	
	priv->buffer = MB_BOOK_BUFFER (buffer);

	mb_book_view_set_buffer (MB_BOOK_VIEW (priv->book_view_1), priv->buffer);
	mb_book_view_set_buffer (MB_BOOK_VIEW (priv->book_view_2), priv->buffer);
}

GtkWidget *
mb_book_view_stack_get_widget (MbBookViewStack *stack)
{
	return stack->priv->gtk_stack;
}

gchar *
mb_book_view_stack_get_title (MbBookViewStack *stack)
{
	return mb_book_buffer_get_title (stack->priv->buffer);
}

void
mb_book_view_stack_next_page (MbBookViewStack *stack)
{
	MbBookViewStackPrivate *priv;

	priv = stack->priv;

	book_view_synchronize (stack);
	mb_book_view_show_scroll_next (MB_BOOK_VIEW (NEXT_CHILD (stack)));

	gtk_stack_set_visible_child_full (GTK_STACK (priv->gtk_stack),
	                                  NEXT_CHILD_NAME (stack),
	                                  GTK_STACK_TRANSITION_TYPE_SLIDE_LEFT);
}

void
mb_book_view_stack_prev_page (MbBookViewStack *stack)
{
	MbBookViewStackPrivate *priv;

	priv = stack->priv;

	book_view_synchronize (stack);
	mb_book_view_show_scroll_prev (MB_BOOK_VIEW (priv->book_view_1));

	gtk_stack_set_visible_child_full (GTK_STACK (priv->gtk_stack),
	                                  NEXT_CHILD_NAME (stack),
	                                  GTK_STACK_TRANSITION_TYPE_SLIDE_RIGHT);
}

static gchar *
get_next_child_name (MbBookViewStack *stack)
{
	if (EQUALS (VISIBLE_CHILD_NAME (stack), CHILD_1_NAME))
	{
		return CHILD_2_NAME;
	}
	else
	{
		return CHILD_1_NAME;
	}
}

static void
book_view_synchronize (MbBookViewStack *stack)
{
	MbBookViewStackPrivate *priv;
	GtkWidget *src_book_view;
	GtkWidget *dst_book_view;
	guint first_section;
	guint last_section;
	guint first_line;
	guint last_line;
	guint next_line;
	gint start_index;
	
	priv = stack->priv;

	if (EQUALS (VISIBLE_CHILD_NAME (stack), CHILD_1_NAME))
	{
		src_book_view = priv->book_view_1;
		dst_book_view = priv->book_view_2;
	}
	else
	{
		src_book_view = priv->book_view_2;
		dst_book_view = priv->book_view_1;
	}
	
	g_object_get (G_OBJECT (src_book_view),
	              "first-section", &first_section,
	              "last-section", &last_section,
	              "first-line", &first_line,
	              "last-line", &last_line,
	              "start-index", &start_index,
	              NULL);

	next_line = last_line > 0 ? last_line : 0;
	first_section = last_section;
	first_line = next_line;

	g_object_set (G_OBJECT (src_book_view),
	              "first-section", first_section,
	              "last-section", last_section,
	              "first-line", first_line,
	              "last-line", last_line,
	              "start-index", start_index,
	              NULL);

	g_object_set (G_OBJECT (dst_book_view),
	              "first-section", first_section,
	              "last-section", last_section,
	              "first-line", first_line,
	              "last-line", last_line,
	              "start-index", start_index,
	              NULL);
}
