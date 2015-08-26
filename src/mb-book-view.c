/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * mb-book-view.c
 * Copyright (C) 2014 Yan Zlobin <yan_zlobin@agniyoga.ru>
 * 
 */

#include <gtk/gtk.h>
#include "mb-book-view.h"
#include "mb-profile.h"

// For 1366x768 width: 620
#define DEFAULT_MAX_ONE_COLON_WIDTH			1020

#define MB_BOOK_VIEW_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), \
	MB_TYPE_BOOK_VIEW, MbBookViewPrivate))

struct _MbBookViewPrivate
{
	MbBookBuffer *buffer;

	guint first_section;
	guint last_section;
	guint first_line;
	guint last_line;

	gint start_index;

	gboolean scrolled;
};

enum
{
	SIGNAL_PAGE_DRAW,
	SIGNAL_LAST
};

enum
{
	PROP_0,
	PROP_FIRST_SECTION,
	PROP_LAST_SECTION,
	PROP_FIRST_LINE,
	PROP_LAST_LINE,
	PROP_START_INDEX,
	PROP_LAST
};

guint mb_book_view_signals[SIGNAL_LAST] = { 0 };

static void get_property (GObject *object, guint propid, GValue *value,
                          GParamSpec *pspec);

static void set_property (GObject *object, guint propid, const GValue *value,
                          GParamSpec *pspec);

static guint get_first_section (MbBookView *view);

static guint get_last_section (MbBookView *view);

static guint get_first_line (MbBookView *view);

static guint get_last_line (MbBookView *view);

static gint get_start_index (MbBookView *view);

static void set_first_section (MbBookView *view, guint index);

static void set_last_section (MbBookView *view, guint index);

static void set_first_line (MbBookView *view, guint index);

static void set_last_line (MbBookView *view, guint index);

static void set_start_index (MbBookView *view, gint index);

static void mb_book_view_realize (GtkWidget *widget);

static void mb_book_view_unrealize (GtkWidget *widget);

static gboolean mb_book_view_draw (GtkWidget *widget, cairo_t *cr);

static void mb_book_view_size_allocate (GtkWidget *widget,
                                        GtkAllocation *allocation);

static void mb_book_view_finalize (GObject *object);

static void mb_book_view_destroy (GtkWidget *widget);

static void view_init (GtkWidget *view);

static void print_page (MbBookView *view, cairo_t *cr);

static gboolean print_paragraph (MbBookView *view, cairo_t *cr,
                                 PangoFontDescription *font, gint width,
                                 gint top, gint left, gint step,
                                 gint max_height, guint start_line, gchar *text,
                                 gint *height, guint *last_line);

G_DEFINE_TYPE (MbBookView, mb_book_view, GTK_TYPE_WIDGET);

static void
mb_book_view_init (MbBookView *view)
{
	MbBookViewPrivate *priv;

	priv = MB_BOOK_VIEW_GET_PRIVATE (view);
	view->priv = priv;

	priv->scrolled = FALSE;
	priv->buffer = NULL;

	g_object_set (G_OBJECT (view),
	              "double-buffered", FALSE,
	              "app-paintable", FALSE,
	              NULL);
}

static void
mb_book_view_class_init (MbBookViewClass *klass)
{
	GObjectClass *gobject_class;
	GtkWidgetClass *widget_class;
	
	g_type_class_add_private (klass, sizeof (MbBookViewPrivate));

	gobject_class = G_OBJECT_CLASS (klass);
	widget_class = GTK_WIDGET_CLASS (klass);

	mb_book_view_signals[SIGNAL_PAGE_DRAW] =
		g_signal_new ("page-draw", G_TYPE_FROM_CLASS (klass),
		              G_SIGNAL_RUN_LAST,
		              G_STRUCT_OFFSET (MbBookViewClass, page_draw),
		              NULL, NULL, g_cclosure_marshal_VOID__VOID,
		              G_TYPE_NONE, 0);

	widget_class->realize = mb_book_view_realize;
	widget_class->unrealize = mb_book_view_unrealize;
	widget_class->draw = mb_book_view_draw;
	widget_class->size_allocate = mb_book_view_size_allocate;
	widget_class->destroy = mb_book_view_destroy;

	gobject_class->get_property = get_property;
	gobject_class->set_property = set_property;
	gobject_class->finalize = mb_book_view_finalize;

	g_object_class_install_property (gobject_class, PROP_FIRST_SECTION,
		g_param_spec_uint ("first-section", "First section",
		                   "First page section index",
		                   0, G_MAXUINT, 0,
		                   G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

	g_object_class_install_property (gobject_class, PROP_LAST_SECTION,
		g_param_spec_uint ("last-section", "Last section",
		                   "Last page section index",
		                   0, G_MAXUINT, 0,
		                   G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

	g_object_class_install_property (gobject_class, PROP_FIRST_LINE,
		g_param_spec_uint ("first-line", "First line",
		                   "First line index in the first section",
		                   0, G_MAXUINT, 0,
		                   G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

	g_object_class_install_property (gobject_class, PROP_LAST_LINE,
		g_param_spec_uint ("last-line", "Last line",
		                   "Last line index in the last section",
		                   0, G_MAXUINT, 0,
		                   G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

	g_object_class_install_property (gobject_class, PROP_START_INDEX,
		g_param_spec_int ("start-index", "Start index",
		                   "Start index byte in the line",
		                   0, G_MAXINT, 0,
		                  G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
}

static void
get_property (GObject *object, guint propid, GValue *value, GParamSpec *pspec)
{
	MbBookView *view;

	view = MB_BOOK_VIEW (object);

	switch (propid)
	{
		case PROP_FIRST_SECTION:
		{
			g_value_set_uint (value, get_first_section (view));

			break;
		}
		case PROP_LAST_SECTION:
		{
			g_value_set_uint (value, get_last_section (view));

			break;
		}
		case PROP_FIRST_LINE:
		{
			g_value_set_uint (value, get_first_line (view));

			break;
		}
		case PROP_LAST_LINE:
		{
			g_value_set_uint (value, get_last_line (view));

			break;
		}
		case PROP_START_INDEX:
		{
			g_value_set_int (value, get_start_index (view));

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
	MbBookView *view;

	view = MB_BOOK_VIEW (object);

	switch (propid)
	{
		case PROP_FIRST_SECTION:
		{
			set_first_section (view, g_value_get_uint (value));

			break;
		}
		case PROP_LAST_SECTION:
		{
			set_last_section (view, g_value_get_uint (value));
			
			break;
		}
		case PROP_FIRST_LINE:
		{
			set_first_line (view, g_value_get_uint (value));

			break;
		}
		case PROP_LAST_LINE:
		{
			set_last_line (view, g_value_get_uint (value));

			break;
		}
		case PROP_START_INDEX:
		{
			set_start_index (view, g_value_get_int (value));

			break;
		}
		default:
		{
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, propid, pspec);
		}
	}
}

static guint
get_first_section (MbBookView *view)
{
	g_return_val_if_fail (MB_IS_BOOK_VIEW (view), 0);
	
	return view->priv->first_section;
}

static guint
get_last_section (MbBookView *view)
{
	g_return_val_if_fail (MB_IS_BOOK_VIEW (view), 0);
	
	return view->priv->last_section;
}

static guint
get_first_line (MbBookView *view)
{
	g_return_val_if_fail (MB_IS_BOOK_VIEW (view), 0);
	
	return view->priv->first_line;
}

static guint
get_last_line (MbBookView *view)
{
	g_return_val_if_fail (MB_IS_BOOK_VIEW (view), 0);
	
	return view->priv->last_line;
}

static gint
get_start_index (MbBookView *view)
{
	g_return_val_if_fail (MB_IS_BOOK_VIEW (view), 0);
	
	return view->priv->start_index;
}

static void
set_first_section (MbBookView *view, guint index)
{
	g_return_if_fail (MB_IS_BOOK_VIEW (view));

	view->priv->first_section = index;
}

static void
set_last_section (MbBookView *view, guint index)
{
	g_return_if_fail (MB_IS_BOOK_VIEW (view));

	view->priv->last_section = index;
}

static void
set_first_line (MbBookView *view, guint index)
{
	g_return_if_fail (MB_IS_BOOK_VIEW (view));

	view->priv->first_line = index;
}

static void
set_last_line (MbBookView *view, guint index)
{
	g_return_if_fail (MB_IS_BOOK_VIEW (view));

	view->priv->last_line = index;
}

static void
set_start_index (MbBookView *view, gint index)
{
	g_return_if_fail (MB_IS_BOOK_VIEW (view));

	view->priv->start_index = index;
}

static void
mb_book_view_realize (GtkWidget *widget)
{
	g_return_if_fail (widget != NULL);
	g_return_if_fail (MB_IS_BOOK_VIEW (widget));
	
	GdkWindow *window;
	GdkWindowAttr attributes;
	GtkAllocation allocation;
	gint attr_mask;

	gtk_widget_set_realized (widget, TRUE);
	gtk_widget_get_allocation (widget, &allocation);

	attributes.x = allocation.x;
	attributes.y = allocation.y;
	attributes.width = allocation.width;
	attributes.height = allocation.height;
	attributes.wclass = GDK_INPUT_OUTPUT;
	attributes.window_type = GDK_WINDOW_CHILD;
	attributes.event_mask = gtk_widget_get_events (widget);
	attributes.event_mask |= (GDK_EXPOSURE_MASK);

	attributes.visual = gtk_widget_get_visual (widget);

	attr_mask = GDK_WA_X | GDK_WA_Y | GDK_WA_VISUAL;
	window = gdk_window_new (gtk_widget_get_parent_window (widget), &attributes,
	                         attr_mask);

	gtk_widget_set_window (widget, window);
	gtk_widget_register_window (widget, window);
}

static void
mb_book_view_unrealize (GtkWidget *widget)
{
	GdkWindow *window;

	window = gtk_widget_get_window (widget);

	if (GDK_IS_WINDOW (window))
	{
		gtk_widget_unregister_window (widget, window);
	}

	gtk_selection_remove_all (widget);
	gtk_widget_set_realized (widget, FALSE);
}

static gboolean
mb_book_view_draw (GtkWidget *widget, cairo_t *cr)
{
	g_return_val_if_fail (widget != NULL, FALSE);
	g_return_val_if_fail (MB_IS_BOOK_VIEW (widget), FALSE);

	print_page (MB_BOOK_VIEW (widget), cr);

	return TRUE;
}

static void
mb_book_view_size_allocate (GtkWidget *widget, GtkAllocation *allocation)
{
	GdkWindow *window;

	g_return_if_fail (widget != NULL);
	g_return_if_fail (allocation != NULL);
	g_return_if_fail (MB_IS_BOOK_VIEW (widget));

	gtk_widget_set_allocation (widget, allocation);
	window = gtk_widget_get_window (widget);
	
	if (gtk_widget_get_realized (widget))
	{
		gdk_window_move_resize (window, allocation->x, allocation->y,
		                        allocation->width, allocation->height);
	}
}

static void
mb_book_view_finalize (GObject *object)
{
	G_OBJECT_CLASS (mb_book_view_parent_class)->finalize (object);
}

static void
mb_book_view_destroy (GtkWidget *widget)
{
	GTK_WIDGET_CLASS (mb_book_view_parent_class)->destroy (widget);
}

GtkWidget *
mb_book_view_new (void)
{
	GtkWidget *view;

	view = GTK_WIDGET (g_object_new (MB_TYPE_BOOK_VIEW, NULL));

	view_init (view);

	return view;
}

void
mb_book_view_set_buffer (MbBookView *view, MbBookBuffer *buffer)
{
	MbBookViewPrivate *priv;
	
	g_return_if_fail (MB_IS_BOOK_VIEW (view));
	g_return_if_fail (MB_IS_BOOK_BUFFER (buffer));

	priv = view->priv;

	if (priv->buffer)
	{
		g_object_unref (priv->buffer);
		
		priv->buffer = NULL;
	}

	priv->buffer = buffer;

	priv->first_section = 0;
	priv->last_section = 0;
	priv->first_line = 0;
	priv->last_line = 0;
	priv->start_index = 0;
	priv->scrolled = FALSE;
}

void
mb_book_view_show_scroll_next (MbBookView *view)
{
	MbBookViewPrivate *priv;

	priv = view->priv;

	priv->start_index = 0;
	priv->scrolled = TRUE;
}

void
mb_book_view_show_scroll_prev (MbBookView *view)
{
	g_object_set (G_OBJECT (view), "first-section", 0, "first-line", 0, NULL);
}

static void
view_init (GtkWidget *view)
{
	gtk_widget_set_hexpand_set (view, TRUE);
	gtk_widget_set_hexpand (view, TRUE);

	gtk_widget_set_vexpand_set (view, TRUE);
	gtk_widget_set_vexpand (view, TRUE);
}

static void
print_page (MbBookView *view, cairo_t *cr)
{
	if (!gtk_widget_get_realized (GTK_WIDGET (view)))
	{
		return;
	}

	GtkWidget *widget;
	MbBookViewPrivate *priv;
	MbBookBufferFragment *fragment;

	GtkAllocation allocation;
	PangoFontDescription *font;
	gint column_count;
	guint left;
	guint column_width;
	guint section_indent_x;
	guint section_indent_y;
	gint fragment_top;
	gint fragment_height;
	guint start_line;
	guint next_line;
	guint page_height;
	gboolean height_limit;
	gboolean second_column;
	gboolean page_start;

	gint current_section;
	gchar *section;

	priv = view->priv;

	if (!priv->buffer)
	{
		return;
	}
	
	widget = GTK_WIDGET (view);
	
	if (gtk_widget_get_allocated_width (widget) > DEFAULT_MAX_ONE_COLON_WIDTH)
	{
		column_count = 2;
	}
	else
	{
		column_count = 1;
	}

	left = 0;
	next_line = 0;
	fragment_height = 0;
	second_column = FALSE;
	page_start = TRUE;
	section_indent_x = 60;
	section_indent_y = 12;
	height_limit = FALSE;

	start_line = priv->first_line;
	current_section = priv->first_section;
	fragment_top = section_indent_y;

	gtk_widget_get_allocation (widget, &allocation);

	page_height = allocation.height - section_indent_x - fragment_top
		+ section_indent_y;

	if (column_count == 2)
	{
		column_width = (allocation.width - (section_indent_x * 3)) / 2;
	}
	else
	{
		column_width = (allocation.width - (section_indent_x * 2));
	}

	cairo_translate (cr, (double) section_indent_x, (double) fragment_top);
	cairo_set_source_rgb (cr, 0.0, 0.0, 0.0);

	font = pango_font_description_from_string ("Verdana 10");

	while (!height_limit)
	{
		fragment = mb_book_buffer_get_paragraph (priv->buffer, current_section);

		if (fragment)
		{
			if (page_start)
			{
				section = (gchar *) (&fragment->body[priv->start_index]);

				if (!priv->scrolled)
				{
					start_line = 0;
				}
				
				page_start = FALSE;
			}
			else
			{
				section = fragment->body;
			}

			height_limit = print_paragraph (view, cr, font, column_width,
			                                fragment_top, left, 16, page_height,
			                                start_line, section,
			                                &fragment_height, &next_line);

			if (height_limit && column_count == 2 && !second_column)
			{
				left = column_width + section_indent_x;
				start_line = next_line;
				fragment_top = section_indent_y;
				height_limit = FALSE;
				second_column = TRUE;

				current_section--;
			}
			else
			{
				start_line = 0;
				fragment_top = fragment_top + fragment_height
					+ section_indent_y;
			}

			current_section++;
		}
		else
		{
			break;
		}
	}
	
	g_object_set (G_OBJECT (view),
	              "last-section", current_section - 1,
	              "last-line", next_line,
	              NULL);

	pango_font_description_free (font);
}

static gboolean
print_paragraph (MbBookView *view, cairo_t *cr, PangoFontDescription *font,
                 gint width, gint top, gint left, gint step, gint max_height,
                 guint start_line, gchar *text, gint *height, guint *last_line)
{
	PangoLayout *layout;
	PangoLayoutLine *line;
	gint line_count;
	gint position;
	gboolean result;
	guint i;
	
	layout = pango_cairo_create_layout (cr);
	
	pango_layout_set_justify (layout, TRUE);
	pango_layout_set_wrap (layout, PANGO_WRAP_WORD_CHAR);
	pango_layout_set_width (layout, width * PANGO_SCALE);
	pango_layout_set_font_description (layout, font);
	pango_layout_set_text (layout, text, -1);

	line_count = pango_layout_get_line_count (layout);
	position = top;
	result = FALSE;
	
	for (i = start_line; i < line_count; i++)
	{
		position = position + step;
		
		if (position > max_height)
		{
			result = TRUE;
			
			break;
		}

		line = pango_layout_get_line_readonly (layout, i);
	
		if (i == start_line)
		{
			if (line->is_paragraph_start)
			{
			}
			else
			{
				if (view->priv->scrolled)
				{
					g_object_set (G_OBJECT (view), "start-index",
					              line->start_index, NULL);

					view->priv->scrolled = FALSE;
				}
			}
		}	

		cairo_move_to (cr, (double) left, (double) position);
		pango_cairo_show_layout_line (cr, line);
	}

	if (last_line)
	{
		*last_line = i > 0 ? i : 0;
	}

	if (height)
	{
		*height = position - top;
	}

	g_object_unref (layout);

	return result;
}

