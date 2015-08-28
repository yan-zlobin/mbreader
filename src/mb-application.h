/* Magic Book Reader
 * Copyright (C) 2015 Yan Zlobin <yan_zlobin@agniyoga.ru>
 *
 * mb-application.h
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

#ifndef _MB_APPLICATION_
#define _MB_APPLICATION_

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define MB_TYPE_APPLICATION             (mb_application_get_type ())
#define MB_APPLICATION(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), MB_TYPE_APPLICATION, MbApplication))
#define MB_APPLICATION_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), MB_TYPE_APPLICATION, MbApplicationClass))
#define MB_IS_APPLICATION(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), MB_TYPE_APPLICATION))
#define MB_IS_APPLICATION_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), MB_TYPE_APPLICATION))
#define MB_APPLICATION_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), MB_TYPE_APPLICATION, MbApplicationClass))

typedef struct _MbApplication MbApplication;
typedef struct _MbApplicationClass MbApplicationClass;
typedef struct _MbApplicationPrivate MbApplicationPrivate;

struct _MbApplication
{
	GtkApplication parent_instance;

	MbApplicationPrivate *priv;
};

struct _MbApplicationClass
{
	GtkApplicationClass parent_class;
};

GType mb_application_get_type (void) G_GNUC_CONST;

MbApplication *mb_application_new (gchar *filename);

G_END_DECLS

#endif
