/* Magic Book Reader
 * Copyright (C) 2015 Yan Zlobin <yan_zlobin@agniyoga.ru>
 *
 * mb-preview-widget.c
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
#include "mb-preview-widget.h"

#define DEFAULT_WIDGET_WIDTH	160
#define DEFAULT_COVER_WIDTH		160

#define MB_PREVIEW_WIDGET_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), \
	MB_TYPE_REVIEW_WIDGET, MbPreviewWidgetPrivate))

struct _MbPreviewWidgetPrivate
{
	MbFile *file;
	MbBookPreview *preview;
	MbReference *reference;
};

static void mb_preview_widget_realize (GtkWidget *widget);

static void mb_preview_widget_unrealize (GtkWidget *widget);

static gboolean mb_preview_widget_draw (GtkWidget *widget, cairo_t *cr);

static void mb_preview_widget_size_allocate (GtkWidget *widget,
                                             GtkAllocation *allocation);

static void mb_preview_widget_destroy (GtkWidget *widget);

static void mb_preview_widget_finalize (GObject *object);

static gint print_text (GtkWidget *widget, cairo_t *cr, gchar *text, gint top,
                        gboolean center, gboolean strong);

G_DEFINE_TYPE (MbPreviewWidget, mb_preview_widget, GTK_TYPE_WIDGET);

static void
mb_preview_widget_init (MbPreviewWidget *widget)
{
	MbPreviewWidgetPrivate *priv;

	priv = MB_PREVIEW_WIDGET_GET_PRIVATE (widget);
	widget->priv = priv;

	priv->file = NULL;
	priv->preview = NULL;
}

static void
mb_preview_widget_class_init (MbPreviewWidgetClass *klass)
{
	GObjectClass *gobject_class;
	GtkWidgetClass *widget_class;

	g_type_class_add_private (klass, sizeof (MbPreviewWidgetPrivate));

	gobject_class = G_OBJECT_CLASS (klass);
	widget_class = GTK_WIDGET_CLASS (klass);
	
	widget_class->realize = mb_preview_widget_realize;
	widget_class->unrealize = mb_preview_widget_unrealize;
	widget_class->draw = mb_preview_widget_draw;
	widget_class->size_allocate = mb_preview_widget_size_allocate;
	widget_class->destroy = mb_preview_widget_destroy;
	
	gobject_class->finalize = mb_preview_widget_finalize;
}

static void
mb_preview_widget_realize (GtkWidget *widget)
{
	g_return_if_fail (widget != NULL);
	g_return_if_fail (MB_IS_REVIEW_WIDGET (widget));

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

	gdk_window_show (window);
}

static void
mb_preview_widget_unrealize (GtkWidget *widget)
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
mb_preview_widget_draw (GtkWidget *widget, cairo_t *cr)
{
	MbBookPreview *preview;
	MbPreviewWidgetPrivate *priv;
	GdkPixbuf *cover;
	gchar *genres;
	gchar *authors;
	gchar *sequences;
	gint top;
	gint height;
	gint margin;

	priv = MB_PREVIEW_WIDGET (widget)->priv;
	margin = 8;
	top = 0;

	if (priv->preview)
	{
		preview = priv->preview;
		authors = mb_book_preview_get_authors_as_string (preview);

		if (preview->cover)
		{
			cover = SCALE_COVER (preview->cover, DEFAULT_COVER_WIDTH);
		}
		else
		{
			cover = SCALE_COVER (BLANK_COVER (preview->title, authors),
			                     DEFAULT_COVER_WIDTH);
		}
		
		if (cover)
		{
			gdk_cairo_set_source_pixbuf (cr, cover, 0, 0);
			cairo_paint (cr);
			cairo_set_source_rgb (cr, 0.0, 0.0, 0.0);

			top = top + (margin * 2) + gdk_pixbuf_get_height (cover);

			g_object_unref (cover);
		}

		height = print_text (widget, cr, preview->title, top, TRUE, TRUE);
		top = top + height;

		if (authors)
		{
			height = print_text (widget, cr, authors, top, TRUE, FALSE);
			top = top + height + margin;

			g_free (authors);
		}

		sequences = mb_book_preview_get_sequences_as_string (preview);

		if (sequences)
		{
			height = print_text (widget, cr, sequences, top, TRUE, FALSE);
			top = top + height + margin;

			g_free (sequences);
		}
		
		genres = mb_reference_get_genre_name_from_list (priv->reference,
		                                                preview->genres);
		if (genres)
		{
			print_text (widget, cr, genres, top, TRUE, FALSE);

			g_free (genres);
		}
	}
	else if (FILE_COUNT (priv->file) > 0)
	{
		gchar *book_count;

		height = print_text (widget, cr, "Архив ZIP", top, TRUE, TRUE);
		top = top + height + margin;

		book_count = g_strdup_printf ("Книг в архиве: %u",
		                              FILE_COUNT (priv->file));

		print_text (widget, cr, book_count, top, TRUE, FALSE);

		g_free (book_count);
	}
	
	return TRUE;
}

static void
mb_preview_widget_size_allocate (GtkWidget *widget, GtkAllocation *allocation)
{
	GdkWindow *window;
	
	gtk_widget_set_allocation (widget, allocation);
	window = gtk_widget_get_window (widget);
	
	if (gtk_widget_get_realized (widget))
	{
		gdk_window_move_resize (window, allocation->x, allocation->y,
		                        allocation->width, allocation->height);
	}
}

static void
mb_preview_widget_destroy (GtkWidget *widget)
{
	GTK_WIDGET_CLASS (mb_preview_widget_parent_class)->destroy (widget);
}

static void
mb_preview_widget_finalize (GObject *object)
{
	MbPreviewWidget *widget;
	MbPreviewWidgetPrivate *priv;

	widget = MB_PREVIEW_WIDGET (object);
	priv = widget->priv;

	if (priv->file)
	{
		g_object_unref (priv->file);
	}
	
	mb_book_preview_free (priv->preview);

	G_OBJECT_CLASS (mb_preview_widget_parent_class)->finalize (object);
}

GtkWidget *
mb_preview_widget_new (MbReference *reference)
{
	MbPreviewWidget *preview_widget;
	GtkWidget *widget;

	preview_widget = g_object_new (MB_TYPE_REVIEW_WIDGET, NULL);
	widget = GTK_WIDGET (preview_widget);
	preview_widget->priv->reference = reference;

	gtk_widget_set_size_request (widget, DEFAULT_WIDGET_WIDTH, -1);

	gtk_widget_set_margin_start (widget, 6);
	gtk_widget_set_margin_end (widget, 20);
	gtk_widget_set_margin_top (widget, 18);
	gtk_widget_set_margin_bottom (widget, 6);

	return widget;
}

void
mb_preview_widget_set_file (MbPreviewWidget *widget, MbFile *file)
{
	MbPreviewWidgetPrivate *priv;
	MbBookPreview *preview;

	priv = widget->priv;

	if (priv->file)
	{
		g_object_unref (priv->file);

		priv->file = NULL;
	}

	if (priv->preview)
	{
		mb_book_preview_free (priv->preview);

		priv->preview = NULL;
	}

	priv->file = file;

	if (SINGLE_FILE (file)) 
	{
		preview = mb_file_get_preview (file, NULL);

		if (preview)
		{
			priv->preview = preview;
		}
	}

	gtk_widget_queue_resize (GTK_WIDGET (widget));
}

static gint
print_text (GtkWidget *widget, cairo_t *cr, gchar *text, gint top,
            gboolean center, gboolean strong)
{
	GtkStyleContext *context;
	PangoLayout *layout;
	PangoFontDescription *font;
	gint layout_height;
	gint font_size;

	if (!text)
	{
		return 0;
	}

	layout = pango_cairo_create_layout (cr);
	context = gtk_widget_get_style_context (widget);

	gtk_style_context_get (context, GTK_STATE_FLAG_NORMAL, "font", &font, NULL);

	font_size = pango_font_description_get_size (font) / PANGO_SCALE;

	if (strong)
	{
		pango_font_description_set_weight (font, PANGO_WEIGHT_BOLD);

		pango_font_description_set_absolute_size (font, (font_size + 6)
		                                          * PANGO_SCALE);
	}

	if (center)
	{
		pango_layout_set_alignment (layout, PANGO_ALIGN_CENTER);
	}
	else
	{
		pango_layout_set_alignment (layout, PANGO_ALIGN_LEFT);
	}
	
	pango_layout_set_wrap (layout, PANGO_WRAP_WORD_CHAR);
	pango_layout_set_width (layout, DEFAULT_WIDGET_WIDTH * PANGO_SCALE);
	pango_layout_set_font_description (layout, font);
	pango_layout_set_text (layout, text, -1);

	cairo_move_to (cr, (double) 0, (double) top);
	pango_cairo_show_layout (cr, layout);

	pango_layout_get_size (layout, NULL, &layout_height);

	g_object_unref (layout);

	return layout_height / PANGO_SCALE;
}

