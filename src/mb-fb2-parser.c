/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * mb-fb2-parser.c
 * Copyright (C) 2014 Yan Zlobin <yan_zlobin@agniyoga.ru>
 * 
 */

#include "mb-fb2-parser.h"
#include "mb-fb2-parser-start-element.h"
#include "mb-fb2-parser-end-element.h"
#include "mb-fb2-parser-characters.h"

static void start_document (ParserState *state);

static void end_document (ParserState *state);

static xmlSAXHandler sax_handler = {
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
	(startDocumentSAXFunc) start_document,
	(endDocumentSAXFunc) end_document,
	(startElementSAXFunc) mb_fb2_parser_start_element,
	(endElementSAXFunc) mb_fb2_parser_end_element,
	(referenceSAXFunc) NULL,
	(charactersSAXFunc) mb_fb2_parser_characters,
	(ignorableWhitespaceSAXFunc) NULL,
	(processingInstructionSAXFunc) NULL,
	(commentSAXFunc) NULL,
	(warningSAXFunc) NULL,
	(errorSAXFunc) NULL,
	(fatalErrorSAXFunc) NULL,
};

ParserSAXResult
mb_fb2_parser_parse_file (MbBookBuffer *buffer, gchar *filename)
{
	gint rc;
	ParserSAXResult result;

	rc = 0;
	ParserState state = { 0 };
	book_buffer = buffer;

	rc = xmlSAXUserParseFile (&sax_handler, &state, filename);

	if (rc < 0)
	{
		result = PARSER_SAX_RESULT_NOT_VALID_XML_FILE;
	}
	else if (state.state == PARSER_UNKNOWN)
	{
		result = PARSER_SAX_RESULT_NOT_WELL_FORMED_DOCUMENT;
	}
	else if (state.state != PARSER_FINISH)
	{
		result = PARSER_SAX_RESULT_NOT_FINISH_STATE;
	}
	else
	{
		result = PARSER_SAX_RESULT_SUCCESS;
	}

	return result;
}

void
save_state_to_history (ParserState *state)
{
	state->history = g_list_prepend (state->history, (gpointer) state->state);
}

SAXState
get_state_from_history (ParserState *state, guint n)
{
	SAXState result;
	
	result = (SAXState) g_list_nth_data (state->history, n);

	return result;
}

void
remove_state_from_history (ParserState *state)
{
	if (state->history->data)
	{
		state->state = (SAXState) state->history->data;
		state->history = g_list_remove (state->history, (gpointer) state->state);
	}
}

static void
start_document (ParserState *state)
{
	state->state = PARSER_START;
	
	state->history = NULL;
	state->book_body = FALSE;

	state->paragraph = g_string_new ("");
}

static void
end_document (ParserState *state)
{
	if (state->state == PARSER_FICTIONBOOK)
	{
		state->state = PARSER_FINISH;
	}

	g_list_free (state->history);
}
