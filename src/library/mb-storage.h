/* Magic Book Reader
 * Copyright (C) 2015 Yan Zlobin <yan_zlobin@agniyoga.ru>
 *
 * mb-storage.h
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

#ifndef __MB_STORAGE__
#define __MB_STORAGE__

#include <glib-object.h>

G_BEGIN_DECLS

#include "../mb-reference.h"

#define MB_TYPE_STORAGE					(mb_storage_get_type ())
#define MB_STORAGE(obj)					(G_TYPE_CHECK_INSTANCE_CAST ((obj), MB_TYPE_STORAGE, MbStorage))
#define MB_IS_STORAGE(obj)      		(G_TYPE_CHECK_INSTANCE_TYPE ((obj), MB_TYPE_STORAGE))
#define MB_STORAGE_CLASS(klass) 		(G_TYPE_CHECK_CLASS_CAST ((klass), MB_TYPE_STORAGE, MbStorageClass))
#define MB_IS_STORAGE_CLASS(klass)		(G_TYPE_CHECK_CLASS_TYPE ((klass), MB_TYPE_STORAGE))
#define MB_STORAGE_GET_CLASS(obj)		(G_TYPE_INSTANCE_GET_CLASS ((obj), MB_TYPE_STORAGE, MbStorageClass))

typedef struct _MbStorage MbStorage;
typedef struct _MbStorageClass MbStorageClass;
typedef struct _MbStoragePrivate MbStoragePrivate;

struct _MbStorage
{
	GObject parent_instance;

	MbStoragePrivate *priv;
};

struct _MbStorageClass
{
	GObjectClass parent_class;

	/* Signals */
	void (*ready)						(MbStorage *storage);
};

GType mb_storage_get_type (void) G_GNUC_CONST;

MbStorage *mb_storage_new (MbReference *reference);

void mb_storage_open (MbStorage *storage);

void mb_storage_append_file (MbStorage *storage, gchar *filename);

void mb_storage_append_file_list (MbStorage *storage, GList *file_list);

G_END_DECLS

#endif
