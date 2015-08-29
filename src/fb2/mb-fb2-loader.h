/* Magic Book Reader
 * Copyright (C) 2015 Yan Zlobin <yan_zlobin@agniyoga.ru>
 *
 * mb-fb2-loader.h
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

#ifndef __MB_FB2_LOADER__
#define __MB_FB2_LOADER__

#include <glib-object.h>
#include "mb-book-buffer.h"

#define FB2_LOADER_ERROR_NAME(obj)			(mb_fb2_loader_get_error_name (obj))

G_BEGIN_DECLS

#define MB_TYPE_FB2_LOADER					(mb_fb2_loader_get_type ())
#define MB_FB2_LOADER(obj)					(G_TYPE_CHECK_INSTANCE_CAST ((obj), MB_TYPE_FB2_LOADER, MbFb2Loader))
#define MB_IS_FB2_LOADER(obj)         		(G_TYPE_CHECK_INSTANCE_TYPE ((obj), MB_TYPE_FB2_LOADER))
#define MB_FB2_LOADER_CLASS(klass)   		(G_TYPE_CHECK_CLASS_CAST ((klass), MB_TYPE_FB2_LOADER, MbFb2LoaderClass))
#define MB_IS_FB2_LOADER_CLASS(klass)		(G_TYPE_CHECK_CLASS_TYPE ((klass), MB_TYPE_FB2_LOADER))
#define MB_FB2_LOADER_GET_CLASS(obj)		(G_TYPE_INSTANCE_GET_CLASS ((obj), MB_TYPE_FB2_LOADER, MbFb2LoaderClass))

typedef enum _MbFb2LoaderError MbFb2LoaderError;

typedef struct _MbFb2Loader MbFb2Loader;
typedef struct _MbFb2LoaderClass MbFb2LoaderClass;
typedef struct _MbFb2LoaderPrivate MbFb2LoaderPrivate;

enum _MbFb2LoaderError
{
	MB_FB2_LOADER_ERROR_PARSE,
	MB_FB2_LOADER_ERROR_NOT_VALID_XML_FILE,
	MB_FB2_LOADER_ERROR_NOT_WELL_FORMED_DOCUMENT
};

struct _MbFb2Loader
{
	GObject parent_instance;

	MbFb2LoaderPrivate *priv;
};

struct _MbFb2LoaderClass
{
	GObjectClass parent_class;

	/* Signals */
	void (*loading_completed)				(MbFb2Loader *loader, MbBookBuffer *buffer);
	void (*loading_error)					(MbFb2Loader *loader, MbFb2LoaderError error);
};

GType mb_fb2_loader_get_type (void) G_GNUC_CONST;

MbFb2Loader *mb_fb2_loader_new (gchar *file);

MbFb2Loader *mb_fb2_loader_new_with_size (gchar *file, gsize size);

void mb_fb2_loader_load_from_fb2_file (MbFb2Loader *loader);

void mb_fb2_loader_load_from_memory (MbFb2Loader *loader);

gchar *mb_fb2_loader_get_error_name (MbFb2LoaderError error);

G_END_DECLS

#endif
