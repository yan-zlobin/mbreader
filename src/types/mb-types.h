/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * mb-types.h
 * Copyright (C) 2014 Yan Zlobin <yan_zlobin@agniyoga.ru>
 *
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

