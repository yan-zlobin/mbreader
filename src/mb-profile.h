/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * mb-profile.h
 * Copyright (C) 2014 Yan Zlobin <yan_zlobin@agniyoga.ru>
 * 
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
