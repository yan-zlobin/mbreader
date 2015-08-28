/* Magic Book Reader
 * Copyright (C) 2015 Yan Zlobin <yan_zlobin@agniyoga.ru>
 *
 * mb-book-preview.c
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

#include <libxml/parser.h>
#include "mb-book-preview.h"

#if defined(EQUALS)
#undef EQUALS
#endif

#define EQUALS(name,str)			((gboolean) (!xmlStrcmp ((name), (const xmlChar *) (str))))
#define ATTRS_LEN(obj)				(g_strv_length ((gchar **) obj))
#define NAME_EQUALS(str)			(EQUALS (name, str))

typedef enum _ParserState ParserState;
typedef struct _BookParserState BookParserState;

enum _ParserState
{
	PARSER_START,
	PARSER_FICTIONBOOK,
	PARSER_DESCRIPTION,
	PARSER_TITLE_INFO,
	PARSER_DOCUMENT_INFO,
	PARSER_ID,
	PARSER_GENRE,
	PARSER_AUTHOR,
	PARSER_AUTHOR_NICKNAME,
	PARSER_AUTHOR_FIRST_NAME,
	PARSER_AUTHOR_MIDDLE_NAME,
	PARSER_AUTHOR_LAST_NAME,
	PARSER_BOOK_TITLE,
	PARSER_ANNOTATION,
	PARSER_ANNOTATION_P,
	PARSER_COVERPAGE,
	PARSER_IMAGE,
	PARSER_BINARY,
	PARSER_COVER_IMAGE,
	PARSER_FINISH,
	PARSER_UNKNOWN
};

struct _BookParserState
{
	ParserState state;

	gboolean full_data;

	GSList *genres;
	GSList *authors;
	GSList *sequences;

	GString *genre;
	GString *author_nick_name;
	GString *author_first_name;
	GString *author_middle_name;
	GString *author_last_name;

	GString *id;
	GString *title;
	GString *annotation;

	GString *image_marker_name;
	GString *image_body;
};

static gboolean parse_file (const char *filename, MbBookPreview *data,
                            gboolean full_data);

static gboolean parse_memory (char *buffer, guint64 size, MbBookPreview *data,
                              gboolean full_data);

static MbBookPreview *preview_new (void);

static gboolean preview_update (int rc, BookParserState *state,
                                MbBookPreview *data);

static void parser_start_document (BookParserState *state);

static void parser_end_document (BookParserState *state);

static void parser_start_element (BookParserState *state, const xmlChar *name,
                                  const xmlChar **attrs);

static void parser_end_element (BookParserState *state, const xmlChar *name);

static void parser_characters (BookParserState *state, const xmlChar *chars,
                               int len);

static GdkPixbuf *image_decode (gchar *encoded_image);

static GSList *append_to_list (GSList *list, GString *element);

static GSList *author_append (GSList *list, GString *nick_name,
                              GString *first_name, GString *middle_name,
                              GString *last_name);

static GSList *sequence_append (GSList *list, gchar *number, gchar *name);

MbBookAuthor *author_new (void);

MbBookSequence *sequence_new (void);

static void author_free (gpointer data);

static void sequence_free (gpointer data);

static xmlSAXHandler parser =
{
	(internalSubsetSAXFunc) NULL,
    (isStandaloneSAXFunc) NULL,
    (hasInternalSubsetSAXFunc) NULL,
    (hasExternalSubsetSAXFunc) NULL,
    (resolveEntitySAXFunc) NULL,
    (getEntitySAXFunc) NULL,
    (entityDeclSAXFunc) NULL,
    (notationDeclSAXFunc) NULL,
    (attributeDeclSAXFunc) NULL,
    (elementDeclSAXFunc) NULL,
    (unparsedEntityDeclSAXFunc) NULL,
    (setDocumentLocatorSAXFunc) NULL,
    (startDocumentSAXFunc) parser_start_document,
    (endDocumentSAXFunc) parser_end_document,
    (startElementSAXFunc) parser_start_element,
    (endElementSAXFunc) parser_end_element,
    (referenceSAXFunc) NULL,
    (charactersSAXFunc) parser_characters,
    (ignorableWhitespaceSAXFunc) NULL,
    (processingInstructionSAXFunc) NULL,
    (commentSAXFunc) NULL,
    (warningSAXFunc) NULL,
    (errorSAXFunc) NULL,
    (fatalErrorSAXFunc) NULL,
};

MbBookPreview *
mb_book_preview_new_from_file (const gchar *filename, gboolean full_data)
{
	MbBookPreview *preview;

	preview = preview_new ();

	if (!parse_file (filename, preview, full_data))
	{
		mb_book_preview_free (preview);

		preview = NULL;
	}

	return preview;
}

MbBookPreview *
mb_book_preview_new_from_memory (gchar *buffer, guint64 size,
                                 gboolean full_data)
{
	MbBookPreview *preview;

	preview = preview_new ();

	if (!parse_memory ((char *) buffer, (int) size, preview, full_data))
	{
		mb_book_preview_free (preview);

		preview = NULL;
	}

	return preview;
}

void
mb_book_preview_free (MbBookPreview *preview)
{
	if (!preview)
	{
		return;
	}
	
	if (preview->genres)
	{
		g_slist_free_full (preview->genres, g_free);

		preview->genres = NULL;
	}

	if (preview->authors)
	{
		g_slist_free_full (preview->authors, author_free);

		preview->authors = NULL;
	}

	if (preview->sequences)
	{
		g_slist_free_full (preview->sequences, sequence_free);

		preview->sequences = NULL;
	}

	if (preview->id)
	{
		g_free (preview->id);

		preview->id = NULL;
	}

	if (preview->title)
	{
		g_free (preview->title);

		preview->title = NULL;
	}

	if (preview->annotation)
	{
		g_free (preview->annotation);

		preview->annotation = NULL;
	}

	if (preview->cover)
	{
		g_object_unref (preview->cover);

		preview->cover = NULL;
	}

	g_slice_free1 (sizeof (MbBookPreview), preview);
}

gchar *
mb_book_preview_get_authors_as_string (MbBookPreview *preview)
{
	GString *author_string;
	MbBookAuthor *author;
	GSList *list;

	if (!preview->authors)
	{
		return NULL;
	}
	
	author_string = g_string_new ("");

	for (list = preview->authors; list; list = list->next)
	{
		author = (MbBookAuthor *) list->data;

		if (author_string->len > 0)
		{
			g_string_append (author_string, ", ");
		}

		if (author->nick_name)
		{
			g_string_append (author_string, author->nick_name);
		}
		else
		{
			if (author->first_name)
			{
				g_string_append (author_string, author->first_name);
			}

			if (author->middle_name)
			{
				if (author_string->len > 0)
				{
					g_string_append (author_string, " ");
				}

				g_string_append (author_string, author->middle_name);
			}

			if (author->last_name)
			{
				if (author_string->len > 0)
				{
					g_string_append (author_string, " ");
				}

				g_string_append (author_string, author->last_name);
			}
		}
	}

	return g_string_free (author_string, FALSE);
}

gchar *
mb_book_preview_get_sequences_as_string (MbBookPreview *preview)
{
	GString *sequence_string;
	MbBookSequence *sequence;
	GSList *list;

	if (!preview->sequences)
	{
		return NULL;
	}

	sequence_string = g_string_new ("");

	for (list = preview->sequences; list; list = list->next)
	{
		sequence = (MbBookSequence *) list->data;

		if (sequence_string->len > 0)
		{
			g_string_append (sequence_string, ", ");
		}

		if (sequence->name && sequence->number > 0)
		{
			g_string_printf (sequence_string, "%s #%u", sequence->name,
			                 sequence->number);
		}
	}

	return g_string_free (sequence_string, FALSE);
}

GdkPixbuf *
mb_book_preview_scale_cover (GdkPixbuf *scr_image, gint dest_width)
{
	GdkPixbuf *dest_image = NULL;

	float source_size;
	float source_width;
	float source_height;
	float dest_height;

	if (!GDK_IS_PIXBUF (scr_image))
	{
		return NULL;
	}

	source_height = (float) gdk_pixbuf_get_height (scr_image);
	source_width = (float) gdk_pixbuf_get_width (scr_image);
	source_size = source_height / source_width;
	dest_height = (int) dest_width * source_size;

	dest_image = gdk_pixbuf_scale_simple (scr_image, (int) dest_width,
	                                      (int) dest_height, GDK_INTERP_HYPER);

	return dest_image;	
}

static gboolean
parse_file (const char *filename, MbBookPreview *data, gboolean full_data)
{
	int rc;
	BookParserState state = { 0 };

	rc = xmlSAXUserParseFile (&parser, &state, filename);

	return preview_update (rc, &state, data);
}

static gboolean
parse_memory (char *buffer, guint64 size, MbBookPreview *data,
              gboolean full_data)
{
	int rc;
	BookParserState state = { 0 };
	
	state.full_data = full_data;
	rc = xmlSAXUserParseMemory (&parser, &state, buffer, size);

	return preview_update (rc, &state, data);
}

static MbBookPreview *
preview_new (void)
{
	MbBookPreview *preview;

	preview = (MbBookPreview *) g_slice_alloc (sizeof (MbBookPreview));

	preview->genres = NULL;
	preview->authors = NULL;
	preview->sequences = NULL;

	preview->id = NULL;
	preview->title = NULL;
	preview->annotation = NULL;

	preview->cover = NULL;

	return preview;
}

static gboolean
preview_update (int rc, BookParserState *state, MbBookPreview *data)
{
	gboolean result = FALSE;

	if (rc == 0 && state->state == PARSER_FINISH)
	{
		data->genres = g_slist_reverse (state->genres);
		data->authors = g_slist_reverse (state->authors);
		data->sequences = g_slist_reverse (state->sequences);

		if (state->full_data)
		{
			if (state->id->len > 0)
			{
				data->id = g_string_free (state->id, FALSE);
			}
			else
			{
				g_string_free (state->id, TRUE);
			}

			if (state->annotation->len > 0)
			{
				data->annotation = g_string_free (state->annotation, FALSE);
			}
			else
			{
				g_string_free (state->annotation, TRUE);
			}
		}
		
		if (state->title->len > 0)
		{
			data->title = g_string_free (state->title, FALSE);
		}
		else
		{
			g_string_free (state->title, TRUE);
		}

		if (state->image_body->len > 0)
		{
			data->cover = image_decode (state->image_body->str);
		}

		result = TRUE;
	}

	g_string_free (state->genre, TRUE);
	g_string_free (state->author_nick_name, TRUE);
	g_string_free (state->author_first_name, TRUE);
	g_string_free (state->author_middle_name, TRUE);
	g_string_free (state->author_last_name, TRUE);

	g_string_free (state->image_marker_name, TRUE);
	g_string_free (state->image_body, TRUE);

	return result;
}

static void
parser_start_document (BookParserState *state)
{
	state->state = PARSER_START;

	state->genres = NULL;
	state->authors = NULL;
	state->sequences = NULL;

	state->genre = g_string_new ("");
	state->author_nick_name = g_string_new ("");
	state->author_first_name = g_string_new ("");
	state->author_middle_name = g_string_new ("");
	state->author_last_name = g_string_new ("");

	state->id = g_string_new ("");
	state->title = g_string_new ("");
	state->annotation = g_string_new ("");

	state->image_marker_name = g_string_new ("");
	state->image_body = g_string_new ("");
}

static void
parser_end_document (BookParserState *state)
{
	state->state = PARSER_FINISH;
}

static void
parser_start_element (BookParserState *state, const xmlChar *name,
                      const xmlChar **attrs)
{
	switch (state->state)
	{
		case PARSER_START:
		{
			if (NAME_EQUALS ("FictionBook"))
			{
				state->state = PARSER_FICTIONBOOK;
			}
			else
			{
				state->state = PARSER_UNKNOWN;
			}
			
			break;
		}
		case PARSER_FICTIONBOOK:
		{
			if (NAME_EQUALS ("description"))
			{
				state->state = PARSER_DESCRIPTION;
			}
			else if (NAME_EQUALS ("binary"))
			{
				gint i;
				
				state->state = PARSER_BINARY;

				if (attrs)
				{
					for (i = 0; attrs[i] && i <= ATTRS_LEN (attrs); i += 2)
					{
						if (EQUALS (attrs[i], "id")
						    && EQUALS (attrs[i + 1],
						               state->image_marker_name->str))
						{
							state->state = PARSER_COVER_IMAGE;
						
							break;
						}
					}
				}
			}
			
			break;
		}
		case PARSER_DESCRIPTION:
		{
			if (NAME_EQUALS ("title-info"))
			{
				state->state = PARSER_TITLE_INFO;
			}
			else if (NAME_EQUALS ("document-info") && state->full_data)
			{
				state->state = PARSER_DOCUMENT_INFO;
			}
			
			break;
		}
		case PARSER_TITLE_INFO:
		{
			if (NAME_EQUALS ("genre"))
			{
				state->state = PARSER_GENRE;
			}
			else if (NAME_EQUALS ("author"))
			{
				state->state = PARSER_AUTHOR;
			}
			else if (NAME_EQUALS ("book-title"))
			{
				state->state = PARSER_BOOK_TITLE;
			}
			else if (NAME_EQUALS ("annotation") && state->full_data)
			{
				state->state = PARSER_ANNOTATION;
			}
			else if (NAME_EQUALS ("coverpage"))
			{
				state->state = PARSER_COVERPAGE;
			}
			else if (NAME_EQUALS ("sequence"))
			{
				gint i;
				gchar *number = NULL;
				gchar *name = NULL;

				if (attrs)
				{
					for (i = 0; attrs[i] && i <= ATTRS_LEN (attrs); i += 2)
					{
						if (EQUALS (attrs[i], "number"))
						{
							number = (gchar *) attrs[i+1];
						}
						else if (EQUALS (attrs[i], "name"))
						{
							name = (gchar *) attrs[i+1];
						}
					}
				}

				state->sequences = sequence_append (state->sequences, number,
				                                    name);
			}
			
			break;
		}
		case PARSER_DOCUMENT_INFO:
		{
			if (NAME_EQUALS ("id") && state->full_data)
			{
				state->state = PARSER_ID;
			}

			break;
		}
		case PARSER_AUTHOR:
		{
			if (NAME_EQUALS ("nickname"))
			{
				state->state = PARSER_AUTHOR_NICKNAME;
			}
			else if (NAME_EQUALS ("first-name"))
			{
				state->state = PARSER_AUTHOR_FIRST_NAME;
			}
			else if (NAME_EQUALS ("middle-name"))
			{
				state->state = PARSER_AUTHOR_MIDDLE_NAME;
			}
			else if (NAME_EQUALS ("last-name"))
			{
				state->state = PARSER_AUTHOR_LAST_NAME;
			}
			
			break;
		}
		case PARSER_ANNOTATION:
		{
			if (NAME_EQUALS ("p") && state->full_data)
			{
				state->state = PARSER_ANNOTATION_P;
			}
			
			break;
		}
		case PARSER_COVERPAGE:
		{
			if (NAME_EQUALS ("image"))
			{
				if (g_str_has_prefix ((const gchar *) attrs[1], "#"))
				{
					gchar *marker;

					marker = (gchar *) attrs[1];

					g_string_append (state->image_marker_name,
					                 (const gchar *) (&marker[1]));
				}
				else
				{
					g_string_append (state->image_marker_name,
					                 (const gchar *) attrs[1]);
				}
			}
			else
			{
				state->state = PARSER_UNKNOWN;
			}
			
			break;
		}
		default:
		{
			break;
		}
	}
}

static void
parser_end_element (BookParserState *state, const xmlChar *name)
{
	switch (state->state)
	{
		case PARSER_DESCRIPTION:
		{
			if (NAME_EQUALS ("description"))
			{
				state->state = PARSER_FICTIONBOOK;
			}
		
			break;
		}
		case PARSER_TITLE_INFO:
		{
			if (NAME_EQUALS ("title-info"))
			{
				state->state = PARSER_DESCRIPTION;
			}
			
			break;
		}
		case PARSER_DOCUMENT_INFO:
		{
			if (NAME_EQUALS ("document-info") && state->full_data)
			{
				state->state = PARSER_DESCRIPTION;
			}
			
			break;
		}
		case PARSER_ID:
		{
			if (NAME_EQUALS ("id") && state->full_data)
			{
				state->state = PARSER_DOCUMENT_INFO;
			}
			
			break;
		}
		case PARSER_GENRE:
		{
			if (NAME_EQUALS ("genre"))
			{
				state->state = PARSER_TITLE_INFO;

				state->genres = append_to_list (state->genres, state->genre);
			}
			
			break;
		}
		case PARSER_AUTHOR:
		{
			if (NAME_EQUALS ("author"))
			{
				state->state = PARSER_TITLE_INFO;

				state->authors = author_append (state->authors,
				                                state->author_nick_name,
				                                state->author_first_name,
				                                state->author_middle_name,
				                                state->author_last_name);
			}
			
			break;
		}
		case PARSER_AUTHOR_NICKNAME:
		{
			if (NAME_EQUALS ("nickname"))
			{
				state->state = PARSER_AUTHOR;
			}
			
			break;
		}
		case PARSER_AUTHOR_FIRST_NAME:
		{
			if (NAME_EQUALS ("first-name"))
			{
				state->state = PARSER_AUTHOR;
			}
			
			break;
		}
		case PARSER_AUTHOR_MIDDLE_NAME:
		{
			if (NAME_EQUALS ("middle-name"))
			{
				state->state = PARSER_AUTHOR;
			}
			
			break;
		}
		case PARSER_AUTHOR_LAST_NAME:
		{
			if (NAME_EQUALS ("last-name"))
			{
				state->state = PARSER_AUTHOR;
			}
			
			break;
		}
		case PARSER_BOOK_TITLE:
		{
			if (NAME_EQUALS ("book-title"))
			{
				state->state = PARSER_TITLE_INFO;
			}
			
			break;
		}
		case PARSER_ANNOTATION:
		{
			if (NAME_EQUALS ("annotation") && state->full_data)
			{
				state->state = PARSER_TITLE_INFO;
			}
			
			break;
		}
		case PARSER_ANNOTATION_P:
		{
			if (NAME_EQUALS ("p") && state->full_data)
			{
				state->state = PARSER_ANNOTATION;
			}
			
			break;
		}
		case PARSER_COVERPAGE:
		{
			if (NAME_EQUALS ("coverpage"))
			{
				state->state = PARSER_TITLE_INFO;
			}
			
			break;
		}
		case PARSER_IMAGE:
		{
			if (NAME_EQUALS ("image"))
			{
				state->state = PARSER_COVERPAGE;
			}
			
			break;
		}
		case PARSER_BINARY:
		{
			if (NAME_EQUALS ("binary"))
			{
				state->state = PARSER_FICTIONBOOK;
			}
			
			break;
		}
		default:
		{
			break;
		}
	}
}

static void
parser_characters (BookParserState *state, const xmlChar *chars, int len)
{
	switch (state->state)
	{
		case PARSER_GENRE:
		{
			g_string_append_len (state->genre, (const gchar *) chars, len);
		
			break;
		}
		case PARSER_AUTHOR_NICKNAME:
		{
			g_string_append_len (state->author_nick_name, (const gchar *) chars,
			                     len);
		
			break;
		}
		case PARSER_AUTHOR_FIRST_NAME:
		{
			g_string_append_len (state->author_first_name,
			                     (const gchar *) chars, len);
		
			break;
		}
		case PARSER_AUTHOR_MIDDLE_NAME:
		{
			g_string_append_len (state->author_middle_name,
			                     (const gchar *) chars, len);
		
			break;
		}
		case PARSER_AUTHOR_LAST_NAME:
		{
			g_string_append_len (state->author_last_name, (const gchar *) chars,
			                     len);
		
			break;
		}
		case PARSER_BOOK_TITLE:
		{
			g_string_append_len (state->title, (const gchar *) chars, len);
		
			break;
		}
		case PARSER_ANNOTATION_P:
		{
			if (state->full_data)
			{
				g_string_append_len (state->annotation, (const gchar *) chars,
				                     len);
			}
		
			break;
		}
		case PARSER_COVER_IMAGE:
		{
			g_string_append_len (state->image_body, (const gchar *) chars, len);
		
			break;
		}
		case PARSER_ID:
		{
			if (state->full_data)
			{
				g_string_append_len (state->id, (const gchar *) chars, len);
			}
		
			break;
		}
		default:
		{
			break;
		}
	}
}

static GdkPixbuf *
image_decode (gchar *encoded_image)
{
	gsize len = 0;
	GError *error = NULL;
	GdkPixbuf *image = NULL;
	guchar *decoded_image = NULL;
	GdkPixbufLoader *loader;

	decoded_image = g_base64_decode ((const gchar *) encoded_image, &len);

	if (len > 0)
	{
		loader = gdk_pixbuf_loader_new ();

		gdk_pixbuf_loader_write (loader, decoded_image, len, &error);
		gdk_pixbuf_loader_close (loader, &error);

		image = gdk_pixbuf_loader_get_pixbuf (loader);
	}

	if (error)
	{
		g_error_free (error);
	}

	g_free (decoded_image);

	return image;
}

static GSList *
append_to_list (GSList *list, GString *element)
{
	GSList *new_list;
	
	new_list = g_slist_prepend (list, g_strdup (element->str));

	g_string_truncate (element, 0);

	return new_list;
}

static GSList *
author_append (GSList *list, GString *nick_name, GString *first_name,
               GString *middle_name, GString *last_name)
{
	MbBookAuthor *author;

	author = author_new ();

	if (nick_name->len > 0)
	{
		author->nick_name = g_strdup (nick_name->str);
	}

	if (first_name->len > 0)
	{
		author->first_name = g_strdup (first_name->str);
	}

	if (middle_name->len > 0)
	{
		author->middle_name = g_strdup (middle_name->str);
	}

	if (last_name->len > 0)
	{
		author->last_name = g_strdup (last_name->str);
	}

	g_string_truncate (nick_name, 0);
	g_string_truncate (first_name, 0);
	g_string_truncate (middle_name, 0);
	g_string_truncate (last_name, 0);

	return g_slist_prepend (list, author);
}

static GSList *
sequence_append (GSList *list, gchar *number, gchar *name)
{
	MbBookSequence *sequence;
	gint sequence_number = 0;

	if (!name)
	{
		return list;
	}

	sequence = sequence_new ();

	if (number)
	{
		sequence_number = (gint) g_ascii_strtod (number, NULL);
	}

	sequence->number = sequence_number > 0 ? (guint) sequence_number : 0;
	sequence->name = g_strdup (name);

	return g_slist_prepend (list, sequence);
}

MbBookAuthor *
author_new (void)
{
	MbBookAuthor *author;

	author = (MbBookAuthor *) g_slice_alloc (sizeof (MbBookAuthor));

	author->nick_name = NULL;
	author->first_name = NULL;
	author->middle_name = NULL;
	author->last_name = NULL;

	return author;
}

MbBookSequence *
sequence_new (void)
{
	MbBookSequence *sequence;

	sequence = (MbBookSequence *) g_slice_alloc (sizeof (MbBookSequence));

	sequence->number = 0;
	sequence->name = NULL;

	return sequence;
}

static void
author_free (gpointer data)
{
	MbBookAuthor *author;

	author = (MbBookAuthor *) data;

	if (author->nick_name)
	{
		g_free (author->nick_name);
	}

	if (author->first_name)
	{
		g_free (author->first_name);
	}

	if (author->middle_name)
	{
		g_free (author->middle_name);
	}

	if (author->last_name)
	{
		g_free (author->last_name);
	}

	g_slice_free1 (sizeof (MbBookAuthor), data);
}

static void
sequence_free (gpointer data)
{
	MbBookSequence *sequence;

	sequence = (MbBookSequence *) data;

	if (sequence->name)
	{
		g_free (sequence->name);
	}

	g_slice_free1 (sizeof (MbBookSequence), data);
}

