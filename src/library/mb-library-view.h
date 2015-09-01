/* Magic Book Reader
 * Copyright (C) 2015 Yan Zlobin <yan_zlobin@agniyoga.ru>
 *
 * mb-library-view.h
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

#ifndef __MB_LIBRARY_VIEW__
#define __MB_LIBRARY_VIEW__

#include <gtk/gtkwidget.h>

G_BEGIN_DECLS

#define MB_TYPE_REVIEW_WIDGET				(mb_library_view_get_type ())
#define MB_LIBRARY_VIEW(obj)				(G_TYPE_CHECK_INSTANCE_CAST ((obj), MB_TYPE_REVIEW_WIDGET, MbLibraryView))
#define MB_IS_REVIEW_WIDGET(obj)      		(G_TYPE_CHECK_INSTANCE_TYPE ((obj), MB_TYPE_REVIEW_WIDGET))
#define MB_LIBRARY_VIEW_CLASS(klass) 		(G_TYPE_CHECK_CLASS_CAST ((klass), MB_TYPE_REVIEW_WIDGET, MbLibraryViewClass))
#define MB_IS_REVIEW_WIDGET_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE ((klass), MB_TYPE_REVIEW_WIDGET))
#define MB_LIBRARY_VIEW_GET_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS ((obj), MB_TYPE_REVIEW_WIDGET, MbLibraryViewClass))

typedef struct _MbLibraryView MbLibraryView;
typedef struct _MbLibraryViewClass MbLibraryViewClass;
typedef struct _MbLibraryViewPrivate MbLibraryViewPrivate;

struct _MbLibraryView
{
	GtkWidget parent_instance;

	MbLibraryViewPrivate *priv;
};

struct _MbLibraryViewClass
{
	GtkWidgetClass parent_class;
};

GType mb_library_view_get_type (void) G_GNUC_CONST;

GtkWidget *mb_library_view_new (void);

G_END_DECLS

#endif
