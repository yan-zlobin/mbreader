/* Magic Book Reader
 * Copyright (C) 2015 Yan Zlobin <yan_zlobin@agniyoga.ru>
 *
 * mb-profile.h
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

#ifndef __MB_PROFILE__
#define __MB_PROFILE__

#include <glib-object.h>

G_BEGIN_DECLS

#include "mb-book-buffer.h"

#define MB_TYPE_PROFILE					(mb_profile_get_type ())
#define MB_PROFILE(obj)					(G_TYPE_CHECK_INSTANCE_CAST ((obj), MB_TYPE_PROFILE, MbProfile))
#define MB_IS_PROFILE(obj)         		(G_TYPE_CHECK_INSTANCE_TYPE ((obj), MB_TYPE_PROFILE))
#define MB_PROFILE_CLASS(klass)   		(G_TYPE_CHECK_CLASS_CAST ((klass), MB_TYPE_PROFILE, MbProfileClass))
#define MB_IS_PROFILE_CLASS(klass)		(G_TYPE_CHECK_CLASS_TYPE ((klass), MB_TYPE_PROFILE))
#define MB_PROFILE_GET_CLASS(obj)		(G_TYPE_INSTANCE_GET_CLASS ((obj), MB_TYPE_PROFILE, MbProfileClass))

typedef struct _MbProfile MbProfile;
typedef struct _MbProfileClass MbProfileClass;
typedef struct _MbFragmentStyle MbFragmentStyle;

struct _MbProfile
{
	GObject parent_instance;
};

struct _MbProfileClass
{
	GObjectClass parent_class;
};

struct _MbFragmentStyle
{
	
};

GType mb_profile_get_type (void) G_GNUC_CONST;

MbProfile *mb_profile_new (void);

void mb_profile_get_fragment_style (MbBookBufferFragmentType type,
                                    MbFragmentStyle *style);

G_END_DECLS

#endif
