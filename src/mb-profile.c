/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * mb-profile.c
 * Copyright (C) 2014 Yan Zlobin <yan_zlobin@agniyoga.ru>
 * 
 */

#include "mb-profile.h"

static void mb_profile_finalize (GObject *object);

G_DEFINE_TYPE (MbProfile, mb_profile, G_TYPE_OBJECT);

static void
mb_profile_init (MbProfile *profile)
{
}

static void
mb_profile_class_init (MbProfileClass *klass)
{
	GObjectClass *gobject_class;
	
	gobject_class = G_OBJECT_CLASS (klass);

	gobject_class->finalize = mb_profile_finalize;
}

static void
mb_profile_finalize (GObject *object)
{
	G_OBJECT_CLASS (mb_profile_parent_class)->finalize (object);
}

MbProfile *
mb_profile_new (void)
{
	MbProfile *profile;

	profile = g_object_new (MB_TYPE_PROFILE, NULL);

	return profile;
}

void
mb_profile_get_fragment_style (MbBookBufferFragmentType type,
                               MbFragmentStyle *style)
{
}

