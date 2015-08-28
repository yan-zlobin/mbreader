/* Magic Book Reader
 * Copyright (C) 2015 Yan Zlobin <yan_zlobin@agniyoga.ru>
 *
 * mb-epub-loader.c
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

#include "mb-epub-loader.h"

enum
{
	SIGNAL_LOADING_COMPLETED,
	SIGNAL_LOADING_ERROR,
	SIGNAL_LAST
};

static guint mb_epub_loader_signals[SIGNAL_LAST] = { 0 };

static void mb_epub_loader_finalize (GObject *object);

G_DEFINE_TYPE (MbEpubLoader, mb_epub_loader, G_TYPE_OBJECT);

static void
mb_epub_loader_init (MbEpubLoader *loader)
{
}

static void
mb_epub_loader_class_init (MbEpubLoaderClass *klass)
{
	GObjectClass *gobject_class;
	
	gobject_class = G_OBJECT_CLASS (klass);

	mb_epub_loader_signals[SIGNAL_LOADING_COMPLETED] =
		g_signal_new ("loading-completed", G_TYPE_FROM_CLASS (klass),
		              G_SIGNAL_RUN_LAST,
		              G_STRUCT_OFFSET (MbEpubLoaderClass, loading_completed),
		              NULL, NULL, g_cclosure_marshal_VOID__OBJECT,
		              G_TYPE_NONE, 1, MB_TYPE_BOOK_BUFFER);

	mb_epub_loader_signals[SIGNAL_LOADING_ERROR] =
		g_signal_new ("loading-error", G_TYPE_FROM_CLASS (klass),
		              G_SIGNAL_RUN_LAST,
		              G_STRUCT_OFFSET (MbEpubLoaderClass, loading_error),
		              NULL, NULL, g_cclosure_marshal_VOID__OBJECT,
		              G_TYPE_NONE, 1, G_TYPE_UINT);

	gobject_class->finalize = mb_epub_loader_finalize;
}

static void
mb_epub_loader_finalize (GObject *object)
{
	G_OBJECT_CLASS (mb_epub_loader_parent_class)->finalize (object);
}

MbEpubLoader *
mb_epub_loader_new (void)
{
	MbEpubLoader *loader;

	loader = g_object_new (MB_TYPE_EPUB_LOADER, NULL);

	return loader;
}

