/* Magic Book Reader
 * Copyright (C) 2015 Yan Zlobin <yan_zlobin@agniyoga.ru>
 *
 * mb-file.h
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

#ifndef __MB_FILE__
#define __MB_FILE__

#include <glib-object.h>

#define FILE_TYPE(obj)				(mb_file_get_file_type (obj))
#define FILE_COUNT(obj)				(mb_file_get_count (obj))
#define SINGLE_FILE(obj)			(FILE_COUNT (obj) == 1)
#define INTERN_FILENAME(obj)		(mb_file_get_intern_filename (obj))

G_BEGIN_DECLS

#include "../types/mb-types.h"
#include "../preview/mb-book-preview.h"

#define MB_TYPE_FILE				(mb_file_get_type ())
#define MB_FILE(obj)				(G_TYPE_CHECK_INSTANCE_CAST ((obj), MB_TYPE_FILE, MbFile))
#define MB_IS_FILE(obj)      		(G_TYPE_CHECK_INSTANCE_TYPE ((obj), MB_TYPE_FILE))
#define MB_FILE_CLASS(klass) 		(G_TYPE_CHECK_CLASS_CAST ((klass), MB_TYPE_FILE, MbFileClass))
#define MB_IS_FILE_CLASS(klass)		(G_TYPE_CHECK_CLASS_TYPE ((klass), MB_TYPE_FILE))
#define MB_FILE_GET_CLASS(obj)		(G_TYPE_INSTANCE_GET_CLASS ((obj), MB_TYPE_FILE, MbFileClass))

typedef enum _MbFileType MbFileType;

typedef struct _MbFile MbFile;
typedef struct _MbFileClass MbFileClass;
typedef struct _MbFilePrivate MbFilePrivate;

enum _MbFileType
{
	MB_FILE_TYPE_UNKNOWN,
	MB_FILE_TYPE_FB2,
	MB_FILE_TYPE_FB2_ZIP,
	MB_FILE_TYPE_EPUB,
	MB_FILE_TYPE_ZIP
};

struct _MbFile
{
	GObject parent_instance;

	MbFilePrivate *priv;
};

struct _MbFileClass
{
	GObjectClass parent_class;
};

GType mb_file_get_type (void) G_GNUC_CONST;

MbFile *mb_file_new (const gchar *filename);

guint mb_file_get_count (MbFile *file);

MbFileType mb_file_get_file_type (MbFile *file);

GSList *mb_file_get_list (MbFile *file);

gchar *mb_file_get_contents (MbFile *file, gchar *filename, gsize *size);

MbBookPreview *mb_file_get_preview (MbFile *file, gchar *filename);

MbBookPreview *mb_file_get_preview_full (MbFile *file, gchar *filename);

gchar *mb_file_get_checksum (MbFile *file, gchar *filename);

gchar *mb_file_get_intern_filename (MbFile *file);

G_END_DECLS

#endif
