/* Magic Book Reader
 * Copyright (C) 2015 Yan Zlobin <yan_zlobin@agniyoga.ru>
 *
 * mb-book-preview.h
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

#include "../types/mb-types.h"

#define SCALE_COVER(pix,w)			(mb_book_preview_scale_cover (pix, w))
#define BLANK_COVER(t,a)			(mb_book_preview_get_blank_cover (t, a))

MbBookPreview *mb_book_preview_new_from_file (const gchar *filename,
                                              gboolean full_data);

MbBookPreview *mb_book_preview_new_from_memory (gchar *buffer, guint64 size,
                                                gboolean full_data);

gchar *mb_book_preview_get_authors_as_string (MbBookPreview *preview);

gchar *mb_book_preview_get_sequences_as_string (MbBookPreview *preview);

GdkPixbuf *mb_book_preview_scale_cover (GdkPixbuf *scr_image, gint dest_width);

GdkPixbuf *mb_book_preview_get_blank_cover (gchar *title, gchar *author);

void mb_book_preview_free (MbBookPreview *preview);
