/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * mb-book-preview.h
 * Copyright (C) 2014 Yan Zlobin <yan_zlobin@agniyoga.ru>
 *
 */

#include "../types/mb-types.h"

#define SCALE_COVER(pix,w)			(mb_book_preview_scale_cover (pix, w))

MbBookPreview *mb_book_preview_new_from_file (const gchar *filename,
                                              gboolean full_data);

MbBookPreview *mb_book_preview_new_from_memory (gchar *buffer, guint64 size,
                                                gboolean full_data);

gchar *mb_book_preview_get_authors_as_string (MbBookPreview *preview);

gchar *mb_book_preview_get_sequences_as_string (MbBookPreview *preview);

GdkPixbuf *mb_book_preview_scale_cover (GdkPixbuf *scr_image, gint dest_width);

void mb_book_preview_free (MbBookPreview *preview);
