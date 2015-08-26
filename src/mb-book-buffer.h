/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * mb-book-buffer.h
 * Copyright (C) 2014 Yan Zlobin <yan_zlobin@agniyoga.ru>
 * 
 */

#ifndef __MB_BOOK_BUFFER__
#define __MB_BOOK_BUFFER__

#include <glib-object.h>

G_BEGIN_DECLS

#define MB_TYPE_BOOK_BUFFER					(mb_book_buffer_get_type ())
#define MB_BOOK_BUFFER(obj)					(G_TYPE_CHECK_INSTANCE_CAST ((obj), MB_TYPE_BOOK_BUFFER, MbBookBuffer))
#define MB_IS_BOOK_BUFFER(obj)      		(G_TYPE_CHECK_INSTANCE_TYPE ((obj), MB_TYPE_BOOK_BUFFER))
#define MB_BOOK_BUFFER_CLASS(klass) 		(G_TYPE_CHECK_CLASS_CAST ((klass), MB_TYPE_BOOK_BUFFER, MbBookBufferClass))
#define MB_IS_BOOK_BUFFER_CLASS(klass)		(G_TYPE_CHECK_CLASS_TYPE ((klass), MB_TYPE_BOOK_BUFFER))
#define MB_BOOK_BUFFER_GET_CLASS(obj)		(G_TYPE_INSTANCE_GET_CLASS ((obj), MB_TYPE_BOOK_BUFFER, MbBookBufferClass))

typedef enum _MbBookBufferFragmentType MbBookBufferFragmentType;

typedef struct _MbBookBufferFragment MbBookBufferFragment;
typedef struct _MbBookBufferBinary MbBookBufferBinary;
typedef struct _MbBookBufferStylesheet _MbBookBufferStylesheet;
typedef struct _MbBookBufferAuthor MbBookBufferAuthor;
typedef struct _MbBookDescription MbBookDescription;
typedef struct _MbBookGenre MbBookGenre;
typedef struct _MbBookSequences MbBookSequences;
typedef struct _MbBookImage MbBookImage;
typedef struct _MbBookDate MbBookDate;
typedef struct _MbBookTitle MbBookTitle;
typedef struct _MbBookDocument MbBookDocument;
typedef struct _MbBookPublish MbBookPublish;
typedef struct _MbBookCustomInfo MbBookCustomInfo;
typedef struct _MbBookPart MbBookPart;
typedef struct _MbBookOutputDocumentClass MbBookOutputDocumentClass;
typedef struct _MbBookOutput MbBookOutput;

typedef struct _MbBookBuffer MbBookBuffer;
typedef struct _MbBookBufferClass MbBookBufferClass;
typedef struct _MbBookBufferPrivate MbBookBufferPrivate;

/*
enum _MbBookBufferFragmentType
{
	MB_BOOK_BUFFER_FRAGMENT_TYPE_SECTION,
	MB_BOOK_BUFFER_FRAGMENT_TYPE_PARAGRAPH,
	MB_BOOK_BUFFER_FRAGMENT_TYPE_IMAGE,
	MB_BOOK_BUFFER_FRAGMENT_TYPE_POEM,
	MB_BOOK_BUFFER_FRAGMENT_TYPE_SUBTITLE,
	MB_BOOK_BUFFER_FRAGMENT_TYPE_CITE,
	MB_BOOK_BUFFER_FRAGMENT_TYPE_EMPTY_LINE,
	MB_BOOK_BUFFER_FRAGMENT_TYPE_TABLE
};
*/

enum _MbBookBufferFragmentType
{
	MB_BOOK_BUFFER_FRAGMENT_TYPE_PARAGRAPH,
	MB_BOOK_BUFFER_FRAGMENT_TYPE_EMPHASIS
};

struct _MbBookBufferFragment
{
	MbBookBufferFragmentType type;

	MbBookBufferFragment *parent;
	MbBookBufferFragment *prev;
	MbBookBufferFragment *next;

	guint child_count;
	guint order;
	gsize length;

	gchar *body;
};

struct _MbBookBufferBinary
{
	gchar *body;
	gchar *content_type;
	// gchar *id; // id в качестве ключа в GHashTable
};

struct _MbBookBufferStylesheet
{
	gchar *stylesheet;
	gchar *type;
};

struct _MbBookBufferAuthor
{
	gchar *id;
	gchar *first_name;
	gchar *second_name;
	gchar *last_name;
	gchar *nick_name;
	
	GSList *home_pages;
	GSList *emails;
};

struct _MbBookGenre
{
	gchar *genre;
	guint match;
};

struct _MbBookSequences
{
	gchar *name;
	gchar *number;
	gchar *lang;
};

struct _MbBookImage
{
	gchar *type;
	gchar *href;
	gchar *alt;
	gchar *title;
	gchar *id;
};

struct _MbBookDate
{
	gchar *date;
	gchar *lang;
	gchar *value;
};

struct _MbBookTitle
{
	gchar *title;
	gchar *title_lang;
	gchar *annotation;
	gchar *keywords;
	gchar *lang;
	gchar *src_lang;
	
	MbBookDate *date;

	GSList *authors;
	GSList *genres;
	GSList *coverpages;
	GSList *translators;
	GSList *sequences;
};

struct _MbBookDocument
{
	gchar *program;
	gchar *program_lang;
	gchar *src_ocr;
	gchar *src_ocr_lang;
	gchar *id;
	gchar *version;
	gchar *history; // Возможно, лучше использовать MbBookBufferFragment

	MbBookDate *date;
	MbBookBufferAuthor *publisher;

	GSList *authors;
	GSList *src_urls;
};

struct _MbBookPublish
{
	gchar *book_name;
	gchar *book_name_lang;
	gchar *publisher;
	gchar *publisher_lang;
	gchar *city;
	gchar *city_lang;
	gchar *year;
	gchar *isbn;
	gchar *isbn_lang;

	GSList *sequences;
};

struct _MbBookCustomInfo
{
	gchar *body;
	gchar *into_type;
};

struct _MbBookPart
{
	gchar *type;
	gchar *href;
	gchar *include;
};

struct _MbBookOutputDocumentClass
{
	gchar *name;
	gchar *create;
	gchar *price;

	MbBookPart *part;
};

struct _MbBookOutput
{
	gchar *mode;
	gchar *include_all;
	gchar *price;
	gchar *currency;

	MbBookPart *part;
	MbBookOutputDocumentClass *document_class;
};

struct _MbBookDescription
{
	MbBookTitle *title;
	MbBookTitle *src_title;
	MbBookDocument *document;
	MbBookPublish *publish;

	GSList *custom_info;
	GSList *output;
};

struct _MbBookBuffer
{
	GObject parent_instance;

	MbBookBufferPrivate *priv;
};

struct _MbBookBufferClass
{
	GObjectClass parent_class;
};

GType mb_book_buffer_get_type (void) G_GNUC_CONST;

MbBookBuffer *mb_book_buffer_new (void);

void mb_book_buffer_fragment_insert (MbBookBuffer *buffer, gchar *body,
                                     gint length, MbBookBufferFragmentType type);

void mb_book_buffer_paragraph_append (MbBookBuffer *buffer, gchar *body,
                                      gint length);

MbBookBufferFragment *mb_book_buffer_get_paragraph (MbBookBuffer *buffer,
                                                    gint index);

MbBookBufferFragment *mb_book_buffer_paragraph_get_next (MbBookBuffer *buffer);

gchar *mb_book_buffer_get_title (MbBookBuffer *buffer);

guint mb_book_buffer_paragraph_get_fragment_count (MbBookBuffer *buffer);

G_END_DECLS

#endif
