/* Magic Book Reader
 * Copyright (C) 2015 Yan Zlobin <yan_zlobin@agniyoga.ru>
 *
 * mb-file-system.h
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

#ifndef __MB_FILE_SYSTEM__
#define __MB_FILE_SYSTEM__

#include <glib-object.h>

G_BEGIN_DECLS

#define MB_TYPE_FILE_SYSTEM					(mb_file_system_get_type ())
#define MB_FILE_SYSTEM(obj)					(G_TYPE_CHECK_INSTANCE_CAST ((obj), MB_TYPE_FILE_SYSTEM, MbFileSystem))
#define MB_IS_FILE_SYSTEM(obj)      		(G_TYPE_CHECK_INSTANCE_TYPE ((obj), MB_TYPE_FILE_SYSTEM))
#define MB_FILE_SYSTEM_CLASS(klass) 		(G_TYPE_CHECK_CLASS_CAST ((klass), MB_TYPE_FILE_SYSTEM, MbFileSystemClass))
#define MB_IS_FILE_SYSTEM_CLASS(klass)		(G_TYPE_CHECK_CLASS_TYPE ((klass), MB_TYPE_FILE_SYSTEM))
#define MB_FILE_SYSTEM_GET_CLASS(obj)		(G_TYPE_INSTANCE_GET_CLASS ((obj), MB_TYPE_FILE_SYSTEM, MbFileSystemClass))

typedef struct _MbFileSystem MbFileSystem;
typedef struct _MbFileSystemClass MbFileSystemClass;
typedef struct _MbFileSystemPrivate MbFileSystemPrivate;

struct _MbFileSystem
{
	GObject parent_instance;

	MbFileSystemPrivate *priv;
};

struct _MbFileSystemClass
{
	GObjectClass parent_class;

	/* Signals */
	void (*file_found)						(MbFileSystem *fs, GList *file_list);
	void (*file_updated)					(MbFileSystem *fs, gchar *filename);
};

GType mb_file_system_get_type (void) G_GNUC_CONST;

MbFileSystem *mb_file_system_new (void);

void mb_file_system_open (MbFileSystem *fs, gchar *filename);

G_END_DECLS

#endif
