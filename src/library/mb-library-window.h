/* Magic Book Reader
 * Copyright (C) 2015 Yan Zlobin <yan_zlobin@agniyoga.ru>
 *
 * mb-library-window.h
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

#ifndef __MB_LIBRARY_WINDOW__
#define __MB_LIBRARY_WINDOW__

#include <glib-object.h>

G_BEGIN_DECLS

#define MB_TYPE_LIBRARY_WINDOW				(mb_library_window_get_type ())
#define MB_LIBRARY_WINDOW(obj)				(G_TYPE_CHECK_INSTANCE_CAST ((obj), MB_TYPE_LIBRARY_WINDOW, MbLibraryWindow))
#define MB_IS_LIBRARY_WINDOW(obj)      		(G_TYPE_CHECK_INSTANCE_TYPE ((obj), MB_TYPE_LIBRARY_WINDOW))
#define MB_LIBRARY_WINDOW_CLASS(klass) 		(G_TYPE_CHECK_CLASS_CAST ((klass), MB_TYPE_LIBRARY_WINDOW, MbLibraryWindowClass))
#define MB_IS_LIBRARY_WINDOW_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE ((klass), MB_TYPE_LIBRARY_WINDOW))
#define MB_LIBRARY_WINDOW_GET_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS ((obj), MB_TYPE_LIBRARY_WINDOW, MbLibraryWindowClass))

typedef struct _MbLibraryWindow MbLibraryWindow;
typedef struct _MbLibraryWindowClass MbLibraryWindowClass;
typedef struct _MbLibraryWindowPrivate MbLibraryWindowPrivate;

struct _MbLibraryWindow
{
	GObject parent_instance;

	MbLibraryWindowPrivate *priv;
};

struct _MbLibraryWindowClass
{
	GObjectClass parent_class;

	/* Signals */
	void (*selected)						(MbLibraryWindow *library, gchar *filename, gchar *intern_filename);
};

GType mb_library_window_get_type (void) G_GNUC_CONST;

MbLibraryWindow *mb_library_window_new (void);

void mb_library_window_open (void);

G_END_DECLS

#endif
