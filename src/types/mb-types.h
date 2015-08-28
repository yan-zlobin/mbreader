/* Magic Book Reader
 * Copyright (C) 2015 Yan Zlobin <yan_zlobin@agniyoga.ru>
 *
 * mb-types.h
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

#ifndef __MB_TYPES__
#define __MB_TYPES__

#include <glib.h>
#include <gdk-pixbuf/gdk-pixbuf.h>

#define EQUALS(a,b)					(!((gboolean) g_strcmp0 (a, b)))
#define GUINT(str)					((guint) g_ascii_strtod (str, NULL))

typedef struct _MbBookPreview MbBookPreview;
typedef struct _MbBookAuthor MbBookAuthor;
typedef struct _MbBookSequence MbBookSequence;

struct _MbBookPreview
{
	GSList *genres;
	GSList *authors;
	GSList *sequences;

	gchar *id;
	gchar *title;
	gchar *annotation;
	
	GdkPixbuf *cover;
};

struct _MbBookAuthor
{
	gchar *nick_name;
	gchar *first_name;
	gchar *middle_name;
	gchar *last_name;
};

struct _MbBookSequence
{
	guint number;
	gchar *name;
};

#endif

