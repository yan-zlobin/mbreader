/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * mb-fb2-parser.h
 * Copyright (C) 2014 Yan Zlobin <yan_zlobin@agniyoga.ru>
 * 
 */

#include <glib.h>
#include <libxml/parser.h>
#include "mb-book-buffer.h"

#define PARSER_PREV_STATE(obj)				(get_state_from_history (obj, 0))

typedef enum _ParserSAXResult ParserSAXResult; 
typedef enum _SAXState SAXState;

typedef struct _ParserState ParserState;

enum _ParserSAXResult
{
	PARSER_SAX_RESULT_SUCCESS,
	PARSER_SAX_RESULT_NOT_FINISH_STATE,
	PARSER_SAX_RESULT_NOT_VALID_XML_FILE,
	PARSER_SAX_RESULT_NOT_WELL_FORMED_DOCUMENT
};

enum _SAXState
{
	PARSER_START,
	PARSER_FICTIONBOOK,
	PARSER_A,
	PARSER_ANNOTATION,
	PARSER_AUTHOR,
	PARSER_BINARY,
	PARSER_BODY,
	PARSER_BOOK_NAME,
	PARSER_BOOK_TITLE,
	PARSER_CITE,
	PARSER_CITY,
	PARSER_CODE,
	PARSER_COVERPAGE,
	PARSER_CUSTOM_INFO,
	PARSER_DATE,
	PARSER_DESCRIPTION,
	PARSER_DOCUMENT_INFO,
	PARSER_EMAIL,
	PARSER_EMPHASIS,
	PARSER_EMPTY_LINE,
	PARSER_EPIGRAPH,
	PARSER_FIRST_NAME,
	PARSER_GENRE,
	PARSER_HISTORY,
	PARSER_HOME_PAGE,
	PARSER_ID,
	PARSER_ISBN,
	PARSER_IMAGE,
	PARSER_KEYWORDS,
	PARSER_LANG,
	PARSER_LAST_NAME,
	PARSER_MIDDLE_NAME,
	PARSER_NICKNAME,
	PARSER_OUTPUT_DOCUMENT_CLASS,
	PARSER_OUTPUT,
	PARSER_P,
	PARSER_PART,
	PARSER_POEM,
	PARSER_PROGRAM_USED,
	PARSER_PUBLISH_INFO,
	PARSER_PUBLISHER,
	PARSER_SECTION,
	PARSER_SEQUENCE,
	PARSER_SRC_LANG,
	PARSER_SRC_OCR,
	PARSER_SRC_TITLE_INFO,
	PARSER_SRC_URL,
	PARSER_STANZA,
	PARSER_STRIKETHROUGH,
	PARSER_STRONG,
	PARSER_STYLE,
	PARSER_STYLESHEET,
	PARSER_SUB,
	PARSER_SUBTITLE,
	PARSER_SUP,
	PARSER_TABLE,
	PARSER_TD,
	PARSER_TEXT_AUTHOR,
	PARSER_TH,
	PARSER_TITLE,
	PARSER_TITLE_INFO,
	PARSER_TR,
	PARSER_TRANSLATOR,
	PARSER_V,
	PARSER_VERSION,
	PARSER_YEAR,
	PARSER_FINISH,
	PARSER_UNKNOWN
};

struct _ParserState
{
	SAXState state;

	// MbBookBuffer *buffer;

	gboolean book_body;

	GList *history;

	GString *paragraph;
};

MbBookBuffer *book_buffer;

ParserSAXResult mb_fb2_parser_parse_file (MbBookBuffer *buffer, gchar *filename);

ParserSAXResult mb_fb2_parser_parse_memory (const gchar *buffer, gint len);

void save_state_to_history (ParserState *state);

SAXState get_state_from_history (ParserState *state, guint n);

void remove_state_from_history (ParserState *state);

