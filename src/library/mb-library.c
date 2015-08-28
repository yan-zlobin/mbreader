/* Magic Book Reader
 * Copyright (C) 2015 Yan Zlobin <yan_zlobin@agniyoga.ru>
 *
 * mb-library.c
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

#include "mb-library.h"
#include "mb-storage.h"

#define COPY(obj,str)	   (g_string_chunk_insert (obj->priv->chunk, str))

#define MB_LIBRARY_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), \
	MB_TYPE_LIBRARY, MbLibraryPrivate))

struct _MbLibraryPrivate
{
	MbReference *reference;
	MbStorage *storage;
};

enum
{
	SIGNAL_READY,
	SIGNAL_LAST
};

static guint mb_library_signals[SIGNAL_LAST] = { 0 };

static void mb_library_finalize (GObject *object);

static void storage_ready_callback (MbStorage *storage, MbLibrary *library);

G_DEFINE_TYPE (MbLibrary, mb_library, G_TYPE_OBJECT);

static void
mb_library_init (MbLibrary *library)
{
	MbLibraryPrivate *priv;

	priv = MB_LIBRARY_GET_PRIVATE (library);
	library->priv = priv;
}

static void
mb_library_class_init (MbLibraryClass *klass)
{
	GObjectClass *gobject_class;

	g_type_class_add_private (klass, sizeof (MbLibraryPrivate));

	gobject_class = G_OBJECT_CLASS (klass);
	
	mb_library_signals[SIGNAL_READY] =
		g_signal_new ("ready", G_TYPE_FROM_CLASS (klass),
		              G_SIGNAL_RUN_LAST,
		              G_STRUCT_OFFSET (MbLibraryClass, ready),
		              NULL, NULL, g_cclosure_marshal_VOID__VOID,
		              G_TYPE_NONE, 0);

	gobject_class->finalize = mb_library_finalize;
}

static void
mb_library_finalize (GObject *object)
{
	MbLibrary *library;
	MbLibraryPrivate *priv;

	library = MB_LIBRARY (object);
	priv = library->priv;

	g_object_unref (priv->storage);
	
	G_OBJECT_CLASS (mb_library_parent_class)->finalize (object);
}

MbLibrary *
mb_library_new (MbReference *reference)
{
	MbLibrary *library;
	MbLibraryPrivate *priv;

	library = g_object_new (MB_TYPE_LIBRARY, NULL);
	priv = library->priv;

	priv->reference = reference;
	priv->storage = mb_storage_new (reference);

	g_signal_connect (priv->storage, "ready",
	                  G_CALLBACK (storage_ready_callback), library);

	return library;
}

void
mb_library_start (MbLibrary *library)
{
	mb_storage_open (library->priv->storage);
}

void
mb_library_append_file (MbLibrary *library, gchar *filename)
{
	mb_storage_append_file (library->priv->storage, filename);
}

void
mb_library_append_file_list (MbLibrary *library, GSList *file_list)
{
	mb_storage_append_file_list (library->priv->storage, file_list);
}

static void
storage_ready_callback (MbStorage *storage, MbLibrary *library)
{
	g_signal_emit (library, mb_library_signals[SIGNAL_READY], 0);
}

