/* Magic Book Reader
 * Copyright (C) 2015 Yan Zlobin <yan_zlobin@agniyoga.ru>
 *
 * mb-zip-file.h
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

#ifndef __MB_ZIP_FILE__
#define __MB_ZIP_FILE__

#include <glib-object.h>

#define ZIP_FIRST_FILENAME(obj)			(mb_zip_file_get_list (obj)->data)
#define ZIP_FILE_COUNT(obj)				(mb_zip_file_get_count (obj))

G_BEGIN_DECLS

#define MB_TYPE_ZIP_FILE				(mb_zip_file_get_type ())
#define MB_ZIP_FILE(obj)				(G_TYPE_CHECK_INSTANCE_CAST ((obj), MB_TYPE_ZIP_FILE, MbZipFile))
#define MB_IS_ZIP_FILE(obj)      		(G_TYPE_CHECK_INSTANCE_TYPE ((obj), MB_TYPE_ZIP_FILE))
#define MB_ZIP_FILE_CLASS(klass) 		(G_TYPE_CHECK_CLASS_CAST ((klass), MB_TYPE_ZIP_FILE, MbZipFileClass))
#define MB_IS_ZIP_FILE_CLASS(klass)		(G_TYPE_CHECK_CLASS_TYPE ((klass), MB_TYPE_ZIP_FILE))
#define MB_ZIP_FILE_GET_CLASS(obj)		(G_TYPE_INSTANCE_GET_CLASS ((obj), MB_TYPE_ZIP_FILE, MbZipFileClass))

typedef struct _MbZipFile MbZipFile;
typedef struct _MbZipFileClass MbZipFileClass;
typedef struct _MbZipFilePrivate MbZipFilePrivate;

struct _MbZipFile
{
	GObject parent_instance;

	MbZipFilePrivate *priv;
};

struct _MbZipFileClass
{
	GObjectClass parent_class;
};

GType mb_zip_file_get_type (void) G_GNUC_CONST;

MbZipFile *mb_zip_file_new (gchar *filename);

gulong mb_zip_file_get_count (MbZipFile *file);

gchar *mb_zip_file_get_file (MbZipFile *file, gchar *filename, gsize *size);

GSList *mb_zip_file_get_list (MbZipFile *file);

G_END_DECLS

#endif
