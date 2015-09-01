/* Magic Book Reader
 * Copyright (C) 2015 Yan Zlobin <yan_zlobin@agniyoga.ru>
 *
 * mb-library-view.c
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
#include "mb-library-view.h"

#define DEFAULT_WIDGET_WIDTH	160
#define DEFAULT_COVER_WIDTH		160

#define MB_LIBRARY_VIEW_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), \
	MB_TYPE_REVIEW_WIDGET, MbLibraryViewPrivate))

struct _MbLibraryViewPrivate
{
	guint count;

	GList *books;
};

static void mb_library_view_realize (GtkWidget *widget);

static void mb_library_view_unrealize (GtkWidget *widget);

static gboolean mb_library_view_draw (GtkWidget *widget, cairo_t *cr);

static void mb_library_view_size_allocate (GtkWidget *widget,
                                             GtkAllocation *allocation);

static void mb_library_view_destroy (GtkWidget *widget);

static void mb_library_view_finalize (GObject *object);

G_DEFINE_TYPE (MbLibraryView, mb_library_view, GTK_TYPE_WIDGET);

static void
mb_library_view_init (MbLibraryView *view)
{
	MbLibraryViewPrivate *priv;

	priv = MB_LIBRARY_VIEW_GET_PRIVATE (view);
	view->priv = priv;

	priv->count = 0;
	priv->books = NULL;
}

static void
mb_library_view_class_init (MbLibraryViewClass *klass)
{
	GObjectClass *gobject_class;
	GtkWidgetClass *widget_class;

	g_type_class_add_private (klass, sizeof (MbLibraryViewPrivate));

	gobject_class = G_OBJECT_CLASS (klass);
	widget_class = GTK_WIDGET_CLASS (klass);
	
	widget_class->realize = mb_library_view_realize;
	widget_class->unrealize = mb_library_view_unrealize;
	widget_class->draw = mb_library_view_draw;
	widget_class->size_allocate = mb_library_view_size_allocate;
	widget_class->destroy = mb_library_view_destroy;
	
	gobject_class->finalize = mb_library_view_finalize;
}

static void
mb_library_view_realize (GtkWidget *widget)
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
mb_library_view_unrealize (GtkWidget *widget)
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
mb_library_view_draw (GtkWidget *widget, cairo_t *cr)
{
	return TRUE;
}

static void
mb_library_view_size_allocate (GtkWidget *widget, GtkAllocation *allocation)
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
mb_library_view_destroy (GtkWidget *widget)
{
	GTK_WIDGET_CLASS (mb_library_view_parent_class)->destroy (widget);
}

static void
mb_library_view_finalize (GObject *object)
{
	MbLibraryView *widget;
	MbLibraryViewPrivate *priv;

	widget = MB_LIBRARY_VIEW (object);
	priv = widget->priv;

	if (priv->books)
	{
		g_list_free (priv->books);
	}
	
	G_OBJECT_CLASS (mb_library_view_parent_class)->finalize (object);
}

GtkWidget *
mb_library_view_new (void)
{
	MbLibraryView *library_view;
	GtkWidget *widget;

	library_view = g_object_new (MB_TYPE_REVIEW_WIDGET, NULL);
	widget = GTK_WIDGET (library_view);

	gtk_widget_set_margin_start (widget, 20);
	gtk_widget_set_margin_end (widget, 20);
	gtk_widget_set_margin_top (widget, 20);
	gtk_widget_set_margin_bottom (widget, 20);

	return widget;
}
