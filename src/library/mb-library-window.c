/* Magic Book Reader
 * Copyright (C) 2015 Yan Zlobin <yan_zlobin@agniyoga.ru>
 *
 * mb-library-window.c
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

#include "mb-library-window.h"
#include "mb-library-view.h"
#include "../types/mb-marshaller.h"

#define MB_LIBRARY_WINDOW_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), \
	MB_TYPE_LIBRARY_WINDOW, MbLibraryWindowPrivate))

struct _MbLibraryWindowPrivate
{
	MbLibraryView *view;
};

enum
{
	SIGNAL_SELECTED,
	SIGNAL_LAST
};

static guint mb_library_window_signals[SIGNAL_LAST] = { 0 };

static void mb_library_window_finalize (GObject *object);

G_DEFINE_TYPE (MbLibraryWindow, mb_library_window, G_TYPE_OBJECT);

static void
mb_library_window_init (MbLibraryWindow *window)
{
	MbLibraryWindowPrivate *priv;

	priv = MB_LIBRARY_WINDOW_GET_PRIVATE (window);
	window->priv = priv;

	priv->started = FALSE;
}

static void
mb_library_window_class_init (MbLibraryWindowClass *klass)
{
	GObjectClass *gobject_class;

	g_type_class_add_private (klass, sizeof (MbLibraryWindowPrivate));

	gobject_class = G_OBJECT_CLASS (klass);
	
	mb_library_window_signals[SIGNAL_SELECTED] =
		g_signal_new ("selected", G_TYPE_FROM_CLASS (klass),
		              G_SIGNAL_RUN_LAST,
		              G_STRUCT_OFFSET (MbLibraryWindowClass, selected),
		              NULL, NULL, mb_marshal_VOID__STRING_STRING,
		              G_TYPE_NONE, 2, G_TYPE_STRING, G_TYPE_STRING);

	gobject_class->finalize = mb_library_window_finalize;
}

static void
mb_library_window_finalize (GObject *object)
{
	MbLibraryWindow *library;
	MbLibraryWindowPrivate *priv;

	library = MB_LIBRARY_WINDOW (object);
	priv = library->priv;

	g_object_unref (priv->view);
	
	G_OBJECT_CLASS (mb_library_window_parent_class)->finalize (object);
}

MbLibraryWindow *
mb_library_window_new (void)
{
	MbLibraryWindow *window;
	MbLibraryWindowPrivate *priv;

	window = g_object_new (MB_TYPE_LIBRARY_WINDOW, NULL);
	priv = window->priv;

	return library;
}

void
mb_library_window_open (void)
{
}

