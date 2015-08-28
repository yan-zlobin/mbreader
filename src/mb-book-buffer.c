/* Magic Book Reader
 * Copyright (C) 2015 Yan Zlobin <yan_zlobin@agniyoga.ru>
 *
 * mb-book-buffer.c
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

#include <string.h>
#include "mb-book-buffer.h"

#define MB_BOOK_BUFFER_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), \
	MB_TYPE_BOOK_BUFFER, MbBookBufferPrivate))

struct _MbBookBufferPrivate
{
	MbBookDescription *description;
	GHashTable *fragments;
	GHashTable *binaries;

	// GSList *stylesheets;

	MbBookBufferFragment *first_fragment;
	MbBookBufferFragment *last_fragment;
	MbBookBufferFragment *current_fragment;

	guint fragment_count;
	guint binary_count;

	gint 
};

static void mb_book_buffer_finalize (GObject *object);

static MbBookBufferFragment *buffer_fragment_new (MbBookBufferFragmentType type,
                                                  MbBookBufferFragment *parent,
                                                  MbBookBufferFragment *prev,
                                                  gchar *body, gint length);

static MbBookBufferFragment *buffer_fragment_get_parent (MbBookBuffer *buffer,
                                                         MbBookBufferFragment *self);

static void buffer_fragment_free (gpointer data);

static void buffer_binary_free (gpointer data);

G_DEFINE_TYPE (MbBookBuffer, mb_book_buffer, G_TYPE_OBJECT);

static void
mb_book_buffer_init (MbBookBuffer *buffer)
{
	MbBookBufferPrivate *priv;

	priv = MB_BOOK_BUFFER_GET_PRIVATE (buffer);
	buffer->priv = priv;

	priv->description = NULL;

	priv->fragments = g_hash_table_new_full (g_int_hash, g_int_equal, g_free,
	                                         buffer_fragment_free);

	priv->binaries = g_hash_table_new_full (g_int_hash, g_int_equal, g_free,
	                                        buffer_binary_free);

	// priv->stylesheets = NULL;
	priv->first_fragment = NULL;
	priv->last_fragment = NULL;
	priv->fragment_count = 0;
	priv->binary_count = 0;
}

static void
mb_book_buffer_class_init (MbBookBufferClass *klass)
{
	GObjectClass *gobject_class;
	
	g_type_class_add_private (klass, sizeof (MbBookBufferPrivate));

	gobject_class = G_OBJECT_CLASS (klass);

	gobject_class->finalize = mb_book_buffer_finalize;
}

static void
mb_book_buffer_finalize (GObject *object)
{
	MbBookBuffer *buffer;
	MbBookBufferPrivate *priv;

	buffer = MB_BOOK_BUFFER (object);
	priv = buffer->priv;

	g_hash_table_destroy (priv->fragments);
	g_hash_table_destroy (priv->binaries);
	g_hash_table_unref (priv->fragments);
	g_hash_table_unref (priv->binaries);
	
	G_OBJECT_CLASS (mb_book_buffer_parent_class)->finalize (object);
}

MbBookBuffer *
mb_book_buffer_new (void)
{
	MbBookBuffer *buffer;

	buffer = g_object_new (MB_TYPE_BOOK_BUFFER, NULL);

	return buffer;
}


void
mb_book_buffer_fragment_insert (MbBookBuffer *buffer, gchar *body, gint length,
                                MbBookBufferFragmentType type)
{
	MbBookBufferPrivate *priv;
	// MbBookBufferFragment *fragment;

	priv = buffer->priv;
	// fragment = buffer_fragment_new (type, NULL, priv->last_fragment, body, length);



	// priv->fragment_count++;
}

void
mb_book_buffer_paragraph_append (MbBookBuffer *buffer, gchar *body, gint length)
{
	MbBookBufferPrivate *priv;
	MbBookBufferFragment *fragment;
	gint *key;

	priv = buffer->priv;

	fragment = buffer_fragment_new (MB_BOOK_BUFFER_FRAGMENT_TYPE_PARAGRAPH,
	                                NULL, priv->last_fragment, body, length);

	if (fragment)
	{
		key = (gint *) g_malloc (sizeof (gint));
		*key = priv->fragment_count;

		g_hash_table_insert (priv->fragments, key, fragment);
	}
	
	if (!priv->first_fragment)
	{
		priv->first_fragment = fragment;
		priv->current_fragment = fragment;
	}

	if (priv->last_fragment)
	{
		priv->last_fragment->next = fragment;
	}

	priv->last_fragment = fragment;

	priv->fragment_count++;
}

MbBookBufferFragment *
mb_book_buffer_get_paragraph (MbBookBuffer *buffer, gint index)
{
	MbBookBufferPrivate *priv;
	MbBookBufferFragment *fragment;

	priv = buffer->priv;
	fragment = g_hash_table_lookup (priv->fragments, &index);

	return fragment;
}

MbBookBufferFragment *
mb_book_buffer_paragraph_get_next (MbBookBuffer *buffer)
{
	MbBookBufferPrivate *priv;
	MbBookBufferFragment *fragment;

	priv = buffer->priv;
	fragment = priv->current_fragment;

	if (fragment->next)
	{
		priv->current_fragment = fragment->next;
	}
	else
	{
		priv->current_fragment = NULL;
	}

	return fragment;
}

gchar *
mb_book_buffer_get_title (MbBookBuffer *buffer)
{
	return "Неукратимая планета";
}

MbBookBufferFragment *
mb_book_buffer_paragraph_get_current (MbBookBuffer *buffer)
{
	MbBookBufferFragment *fragment = NULL;

	return fragment;
}

guint
mb_book_buffer_paragraph_get_fragment_count (MbBookBuffer *buffer)
{
	return buffer->priv->fragment_count;
}

static MbBookBufferFragment *
buffer_fragment_new (MbBookBufferFragmentType type, MbBookBufferFragment *parent,
                     MbBookBufferFragment *prev, gchar *body, gint length)
{
	// gsize length;
	MbBookBufferFragment *fragment;

	length = sizeof (MbBookBufferFragment);
	fragment = (MbBookBufferFragment *) g_slice_alloc (length);

	fragment->type = type;
	fragment->parent = parent;
	fragment->prev = prev;
	fragment->next = NULL;

	fragment->child_count = 0;

	if (fragment->prev)
	{
		fragment->order = fragment->prev->order + 1;
	}
	else
	{
		fragment->order = 0;
	}
	
	fragment->length = length;
	fragment->body = g_strdup (body);

	return fragment;
}

static MbBookBufferFragment *
buffer_fragment_get_parent (MbBookBuffer *buffer, MbBookBufferFragment *self)
{
	MbBookBufferFragment *fragment = NULL;
	
	return fragment;
}

static void
buffer_fragment_free (gpointer data)
{
	MbBookBufferFragment *fragment;

	fragment = (MbBookBufferFragment *) data;
	
	if (fragment->body)
	{
		g_free (fragment->body);
	}

	g_slice_free1 (sizeof (MbBookBufferFragment), fragment);
}

static void
buffer_binary_free (gpointer data)
{
	MbBookBufferBinary *binary;

	binary = (MbBookBufferBinary *) data;
	
}
