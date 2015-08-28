/* Magic Book Reader
 * Copyright (C) 2015 Yan Zlobin <yan_zlobin@agniyoga.ru>
 *
 * mb-window.h
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

#ifndef __MB_WINDOW__
#define __MB_WINDOW__

#include <glib-object.h>

G_BEGIN_DECLS

#include <gtk/gtk.h>

#define MB_WINDOW_WIDGET(obj)		(mb_window_get_widget (obj))

#define MB_TYPE_WINDOW				(mb_window_get_type ())
#define MB_WINDOW(obj)				(G_TYPE_CHECK_INSTANCE_CAST ((obj), MB_TYPE_WINDOW, MbWindow))
#define MB_IS_WINDOW(obj)      		(G_TYPE_CHECK_INSTANCE_TYPE ((obj), MB_TYPE_WINDOW))
#define MB_WINDOW_CLASS(klass) 		(G_TYPE_CHECK_CLASS_CAST ((klass), MB_TYPE_WINDOW, MbWindowClass))
#define MB_IS_WINDOW_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE ((klass), MB_TYPE_WINDOW))
#define MB_WINDOW_GET_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS ((obj), MB_TYPE_WINDOW, MbWindowClass))

typedef struct _MbWindow MbWindow;
typedef struct _MbWindowClass MbWindowClass;
typedef struct _MbWindowPrivate MbWindowPrivate;

struct _MbWindow
{
	GObject parent_instance;

	MbWindowPrivate *priv;
};

struct _MbWindowClass
{
	GObjectClass parent_class;

	/* Signals */
	void (*window_show)				(MbWindow *window);
	void (*window_destroy)			(MbWindow *window);
	void (*key_open)				(MbWindow *window);
};

GType mb_window_get_type (void) G_GNUC_CONST;

MbWindow *mb_window_new (GApplication *app);

void mb_window_set_book_buffer (MbWindow *mb_window, gpointer buffer);

void mb_window_show (MbWindow *mb_window);

GtkWidget *mb_window_get_widget (MbWindow *mb_window);

void mb_window_hide_titlebar (MbWindow *mb_window);

void mb_window_show_titlebar (MbWindow *mb_window);

void mb_window_show_error (MbWindow *mb_window, gchar *error_text);

G_END_DECLS

#endif
