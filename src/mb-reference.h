/* Magic Book Reader
 * Copyright (C) 2015 Yan Zlobin <yan_zlobin@agniyoga.ru>
 *
 * mb-reference.h
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

#ifndef __MB_REFERENCE__
#define __MB_REFERENCE__

#include <glib-object.h>

G_BEGIN_DECLS

#define MB_TYPE_REFERENCE				(mb_reference_get_type ())
#define MB_REFERENCE(obj)				(G_TYPE_CHECK_INSTANCE_CAST ((obj), MB_TYPE_REFERENCE, MbReference))
#define MB_IS_REFERENCE(obj)      		(G_TYPE_CHECK_INSTANCE_TYPE ((obj), MB_TYPE_REFERENCE))
#define MB_REFERENCE_CLASS(klass) 		(G_TYPE_CHECK_CLASS_CAST ((klass), MB_TYPE_REFERENCE, MbReferenceClass))
#define MB_IS_REFERENCE_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE ((klass), MB_TYPE_REFERENCE))
#define MB_REFERENCE_GET_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS ((obj), MB_TYPE_REFERENCE, MbReferenceClass))

typedef struct _MbReference MbReference;
typedef struct _MbReferenceClass MbReferenceClass;
typedef struct _MbReferencePrivate MbReferencePrivate;

struct _MbReference
{
	GObject parent_instance;

	MbReferencePrivate *priv;
};

struct _MbReferenceClass
{
	GObjectClass parent_class;
};

GType mb_reference_get_type (void) G_GNUC_CONST;

MbReference *mb_reference_new (void);

gchar *mb_reference_get_genre_name (MbReference *reference, gchar *key);

gchar *mb_reference_get_genre_name_from_list (MbReference *reference,
                                              GSList *key_list);

GList *mb_reference_get_genre_list (MbReference *reference);

G_END_DECLS

#endif

