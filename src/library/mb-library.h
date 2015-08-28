/* Magic Book Reader
 * Copyright (C) 2015 Yan Zlobin <yan_zlobin@agniyoga.ru>
 *
 * mb-library.h
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

#ifndef __MB_LIBRARY__
#define __MB_LIBRARY__

#include <glib-object.h>

G_BEGIN_DECLS

#include "../mb-reference.h"

#define MB_TYPE_LIBRARY					(mb_library_get_type ())
#define MB_LIBRARY(obj)					(G_TYPE_CHECK_INSTANCE_CAST ((obj), MB_TYPE_LIBRARY, MbLibrary))
#define MB_IS_LIBRARY(obj)      		(G_TYPE_CHECK_INSTANCE_TYPE ((obj), MB_TYPE_LIBRARY))
#define MB_LIBRARY_CLASS(klass) 		(G_TYPE_CHECK_CLASS_CAST ((klass), MB_TYPE_LIBRARY, MbLibraryClass))
#define MB_IS_LIBRARY_CLASS(klass)		(G_TYPE_CHECK_CLASS_TYPE ((klass), MB_TYPE_LIBRARY))
#define MB_LIBRARY_GET_CLASS(obj)		(G_TYPE_INSTANCE_GET_CLASS ((obj), MB_TYPE_LIBRARY, MbLibraryClass))

typedef struct _MbLibrary MbLibrary;
typedef struct _MbLibraryClass MbLibraryClass;
typedef struct _MbLibraryPrivate MbLibraryPrivate;

struct _MbLibrary
{
	GObject parent_instance;

	MbLibraryPrivate *priv;
};

struct _MbLibraryClass
{
	GObjectClass parent_class;

	/* Signals */
	void (*ready)						(MbLibrary *library);
};

GType mb_library_get_type (void) G_GNUC_CONST;

MbLibrary *mb_library_new (MbReference *reference);

void mb_library_start (MbLibrary *library);

void mb_library_append_file (MbLibrary *library, gchar *filename);

void mb_library_append_file_list (MbLibrary *library, GSList *file_list);

G_END_DECLS

#endif
