/* Magic Book Reader
 * Copyright (C) 2015 Yan Zlobin <yan_zlobin@agniyoga.ru>
 *
 * mb-loader.h
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

#ifndef __MB_LOADER__
#define __MB_LOADER__

#include <glib-object.h>

G_BEGIN_DECLS

#include "mb-book-buffer.h"

#define MB_TYPE_LOADER					(mb_loader_get_type ())
#define MB_LOADER(obj)					(G_TYPE_CHECK_INSTANCE_CAST ((obj), MB_TYPE_LOADER, MbLoader))
#define MB_IS_LOADER(obj)         		(G_TYPE_CHECK_INSTANCE_TYPE ((obj), MB_TYPE_LOADER))
#define MB_LOADER_CLASS(klass)   		(G_TYPE_CHECK_CLASS_CAST ((klass), MB_TYPE_LOADER, MbLoaderClass))
#define MB_IS_LOADER_CLASS(klass)		(G_TYPE_CHECK_CLASS_TYPE ((klass), MB_TYPE_LOADER))
#define MB_LOADER_GET_CLASS(obj)		(G_TYPE_INSTANCE_GET_CLASS ((obj), MB_TYPE_LOADER, MbLoaderClass))

typedef struct _MbLoader MbLoader;
typedef struct _MbLoaderClass MbLoaderClass;
typedef struct _MbLoaderPrivate MbLoaderPrivate;

struct _MbLoader
{
	GObject parent_instance;

	MbLoaderPrivate *priv;
};

struct _MbLoaderClass
{
	GObjectClass parent_class;

	/* Signals */
	void (*loading_completed)			(MbLoader *loader, MbBookBuffer *buffer);
	void (*loading_error)				(MbLoader *loader, gchar *message);
};

GType mb_loader_get_type (void) G_GNUC_CONST;

MbLoader *mb_loader_new (gchar *filename);

void mb_loader_load_book (MbLoader *loader);

G_END_DECLS

#endif
