/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * mb-fb2-parser-start-element.c
 * Copyright (C) 2014 Yan Zlobin <yan_zlobin@agniyoga.ru>
 * 
 */

#include "mb-fb2-parser.h"

void
mb_fb2_parser_start_element (ParserState *state, const xmlChar *name,
                             const xmlChar **attrs)
{
	switch (state->state)
	{
		case PARSER_START:
		{
			save_state_to_history (state);

			if (!xmlStrcmp (name, (const xmlChar *) "FictionBook"))
			{
				state->state = PARSER_FICTIONBOOK;

				// printf ("PARSER_FICTIONBOOK start\n");
			}
			else
			{
				state->state = PARSER_UNKNOWN;
			}

			break;
		}
		case PARSER_FICTIONBOOK:
		{
			save_state_to_history (state);

			if (!xmlStrcmp (name, (const xmlChar *) "stylesheet"))
			{
				state->state = PARSER_STYLESHEET;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "description"))
			{
				state->state = PARSER_DESCRIPTION;

				// printf ("PARSER_DESCRIPTION start\n");
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "body"))
			{
				guint i;
				
				state->state = PARSER_BODY;

				// printf ("PARSER_BODY start\n");

				/*
				if ((attrs == NULL) || (g_strv_length ((gchar **) attrs) < 1))
				{
					state->book_body = TRUE;

					printf ("state->book_body\n");
				}
				*/
				
				if ((attrs == NULL) || (g_strv_length ((gchar **) attrs) < 1))
				{
					state->book_body = TRUE;

					// printf ("state->book_body\n");
				}
				else
				{
					for (i = 0; attrs != NULL && attrs[i] != NULL
					     && i <= g_strv_length ((gchar **) attrs); i += 2)
					{
						if (!xmlStrcmp (attrs[i], (const xmlChar *) "name"))
						{
							if (xmlStrcmp (attrs[i + 1],
							               (const xmlChar *) "notes"))
							{
								state->book_body = TRUE;

								// printf ("state->book_body\n");
							}
						}
					}
				}
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "binary"))
			{
				state->state = PARSER_BINARY;

				/*
				for (i = 0; attrs != NULL && attrs[i] != NULL
				     && i <= g_strv_length ((gchar **) attrs); i += 2)
				{
					if (!xmlStrcmp (attrs[i], (const xmlChar *) "id"))
					{
						state->image_name = g_string_assign (state->image_name,
						                                     (const gchar *) attrs[i+1]);
					}
				}
				*/
			}
			else
			{
				state->state = PARSER_UNKNOWN;
			}

			//state->history = g_list_prepend (state->history, (gpointer) PARSER_FICTIONBOOK);

			break;
		}
		case PARSER_A:
		{
			save_state_to_history (state);

			if (!xmlStrcmp (name, (const xmlChar *) "strong"))
			{
				state->state = PARSER_STRONG;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "emphasis"))
			{
				state->state = PARSER_EMPHASIS;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "style"))
			{
				state->state = PARSER_STYLE;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "strikethrough"))
			{
				state->state = PARSER_STRIKETHROUGH;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "sub"))
			{
				state->state = PARSER_SUB;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "sup"))
			{
				state->state = PARSER_SUP;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "code"))
			{
				state->state = PARSER_CODE;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "image"))
			{
				state->state = PARSER_IMAGE;
			}
			else
			{
				state->state = PARSER_UNKNOWN;
			}

			// state->history = g_list_prepend (state->history, (gpointer) PARSER_A);

			break;
		}
		case PARSER_ANNOTATION:
		{
			save_state_to_history (state);

			if (!xmlStrcmp (name, (const xmlChar *) "p"))
			{
				state->state = PARSER_P;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "poem"))
			{
				state->state = PARSER_POEM;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "cite"))
			{
				state->state = PARSER_CITE;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "subtitle"))
			{
				state->state = PARSER_SUBTITLE;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "empty-line"))
			{
				state->state = PARSER_EMPTY_LINE;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "table"))
			{
				state->state = PARSER_TABLE;
			}
			else
			{
				state->state = PARSER_UNKNOWN;
			}

			break;
		}
		case PARSER_AUTHOR:
		{
			save_state_to_history (state);

			if (!xmlStrcmp (name, (const xmlChar *) "first-name"))
			{
				state->state = PARSER_FIRST_NAME;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "middle-name"))
			{
				state->state = PARSER_MIDDLE_NAME;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "last-name"))
			{
				state->state = PARSER_LAST_NAME;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "nickname"))
			{
				state->state = PARSER_NICKNAME;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "home-page"))
			{
				state->state = PARSER_HOME_PAGE;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "email"))
			{
				state->state = PARSER_EMAIL;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "id"))
			{
				state->state = PARSER_ID;
			}
			else
			{
				state->state = PARSER_UNKNOWN;
			}

			break;
		}
		case PARSER_BINARY:
		{
			break;
		}
		case PARSER_BODY:
		{
			save_state_to_history (state);

			if (!xmlStrcmp (name, (const xmlChar *) "image"))
			{
				state->state = PARSER_IMAGE;

				/*
				const gchar *image_name = NULL;

				if (g_str_has_prefix ((const gchar *) attrs[1], "#"))
				{
					image_name = g_strchug (g_strdelimit (g_strdup ((const gchar *) attrs[1]),
					                                      (const gchar *) "#", ' '));
				}
				else
				{
					image_name = g_strdup ((const gchar *) attrs[1]);
				}

				gtk_text_buffer_create_mark (state->buffer, image_name,
				                             &state->current_iter, TRUE);
				*/
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "title"))
			{
				state->state = PARSER_TITLE;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "epigraph"))
			{
				state->state = PARSER_EPIGRAPH;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "section"))
			{
				state->state = PARSER_SECTION;

				/*
				if (state->book_body)
				{
					state->section_inside = TRUE;
					state->section_level++;
				}
				*/
			}
			else
			{
				state->state = PARSER_UNKNOWN;
			}

			break;
		}
		case PARSER_BOOK_NAME:
		{
			break;
		}
		case PARSER_BOOK_TITLE:
		{
			break;
		}
		case PARSER_CITE:
		{
			save_state_to_history (state);

			if (!xmlStrcmp (name, (const xmlChar *) "p"))
			{
				state->state = PARSER_P;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "subtitle"))
			{
				state->state = PARSER_SUBTITLE;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "empty-line"))
			{
				state->state = PARSER_EMPTY_LINE;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "poem"))
			{
				state->state = PARSER_POEM;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "table"))
			{
				state->state = PARSER_TABLE;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "text-author"))
			{
				state->state = PARSER_TEXT_AUTHOR;
			}
			else
			{
				state->state = PARSER_UNKNOWN;
			}

			break;
		}
		case PARSER_CITY:
		{
			break;
		}
		case PARSER_CODE:
		{
			save_state_to_history (state);

			if (!xmlStrcmp (name, (const xmlChar *) "strong"))
			{
				state->state = PARSER_STRONG;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "emphasis"))
			{
				state->state = PARSER_EMPHASIS;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "style"))
			{
				state->state = PARSER_STYLE;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "a"))
			{
				state->state = PARSER_A;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "strikethrough"))
			{
				state->state = PARSER_STRIKETHROUGH;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "sub"))
			{
				state->state = PARSER_SUB;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "sup"))
			{
				state->state = PARSER_SUP;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "code"))
			{
				state->state = PARSER_CODE;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "image"))
			{
				state->state = PARSER_IMAGE;
			}
			else
			{
				state->state = PARSER_UNKNOWN;
			}

			break;
		}
		case PARSER_COVERPAGE:
		{
			save_state_to_history (state);

			if (!xmlStrcmp (name, (const xmlChar *) "image"))
			{
				state->state = PARSER_IMAGE;

				/*
				GtkTextIter iter;
				GtkTextMark *mark = NULL;
				gchar *coverpage_name = NULL;

				if (g_str_has_prefix ((const gchar *) attrs[1], "#"))
				{
					coverpage_name = g_strchug (g_strdelimit (g_strdup ((const gchar *) attrs[1]),
					                                          (const gchar *) "#", ' '));
				}
				else
				{
					coverpage_name = g_strdup ((const gchar *) attrs[1]);
				}

				mark = gtk_text_buffer_get_mark (state->buffer,
				                                 (const gchar *) "book_cover");

				if (mark != NULL)
				{
					gtk_text_buffer_get_iter_at_mark (state->buffer, &iter,
													  GTK_TEXT_MARK (mark));
					gtk_text_buffer_create_mark (state->buffer, coverpage_name,
												 &iter, TRUE);
					gtk_text_buffer_delete_mark (state->buffer, mark);
				}

				if (coverpage_name)
				{
					g_free (coverpage_name);
				}
				*/
			}
			else
			{
				state->state = PARSER_UNKNOWN;
			}

			break;
		}
		case PARSER_CUSTOM_INFO:
		{
			break;
		}
		case PARSER_DATE:
		{
			break;
		}
		case PARSER_DESCRIPTION:
		{
			save_state_to_history (state);

			if (!xmlStrcmp (name, (const xmlChar *) "title-info"))
			{
				state->state = PARSER_TITLE_INFO;

				// printf ("PARSER_TITLE_INFO start\n");
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "src-title-info"))
			{
				state->state = PARSER_SRC_TITLE_INFO;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "document-info"))
			{
				state->state = PARSER_DOCUMENT_INFO;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "publish-info"))
			{
				state->state = PARSER_PUBLISH_INFO;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "custom-info"))
			{
				state->state = PARSER_CUSTOM_INFO;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "output"))
			{
				state->state = PARSER_OUTPUT;
			}
			else
			{
				state->state = PARSER_UNKNOWN;
			}

			break;
		}
		case PARSER_DOCUMENT_INFO:
		{
			save_state_to_history (state);

			if (!xmlStrcmp (name, (const xmlChar *) "author"))
			{
				state->state = PARSER_AUTHOR;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "program-used"))
			{
				state->state = PARSER_PROGRAM_USED;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "date"))
			{
				state->state = PARSER_DATE;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "src-url"))
			{
				state->state = PARSER_SRC_URL;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "src-ocr"))
			{
				state->state = PARSER_SRC_OCR;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "id"))
			{
				state->state = PARSER_ID;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "version"))
			{
				state->state = PARSER_VERSION;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "history"))
			{
				state->state = PARSER_HISTORY;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "publisher"))
			{
				state->state = PARSER_PUBLISHER;
			}
			else
			{
				state->state = PARSER_UNKNOWN;
			}

			break;
		}
		case PARSER_EMAIL:
		{
			break;
		}
		case PARSER_EMPHASIS:
		{
			save_state_to_history (state);

			if (!xmlStrcmp (name, (const xmlChar *) "strong"))
			{
				state->state = PARSER_STRONG;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "emphasis"))
			{
				state->state = PARSER_EMPHASIS;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "style"))
			{
				state->state = PARSER_STYLE;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "a"))
			{
				state->state = PARSER_A;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "strikethrough"))
			{
				state->state = PARSER_STRIKETHROUGH;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "sub"))
			{
				state->state = PARSER_SUB;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "sup"))
			{
				state->state = PARSER_SUP;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "code"))
			{
				state->state = PARSER_CODE;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "image"))
			{
				state->state = PARSER_IMAGE;
			}
			else
			{
				state->state = PARSER_UNKNOWN;
			}

			break;
		}
		case PARSER_EMPTY_LINE:
		{
			break;
		}
		case PARSER_EPIGRAPH:
		{
			save_state_to_history (state);

			if (!xmlStrcmp (name, (const xmlChar *) "p"))
			{
				state->state = PARSER_P;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "poem"))
			{
				state->state = PARSER_POEM;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "cite"))
			{
				state->state = PARSER_CITE;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "empty-line"))
			{
				state->state = PARSER_EMPTY_LINE;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "text-author"))
			{
				state->state = PARSER_TEXT_AUTHOR;
			}
			else
			{
				state->state = PARSER_UNKNOWN;
			}

			break;
		}
		case PARSER_FIRST_NAME:
		{
			// state->history = g_list_prepend (state->history, (gpointer) PARSER_FIRST_NAME);
			// save_state_to_history (state);

			break;
		}
		case PARSER_GENRE:
		{
			// state->history = g_list_prepend (state->history, (gpointer) PARSER_GENRE);
			// save_state_to_history (state);

			break;
		}
		case PARSER_HISTORY:
		{
			save_state_to_history (state);

			if (!xmlStrcmp (name, (const xmlChar *) "p"))
			{
				state->state = PARSER_P;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "poem"))
			{
				state->state = PARSER_POEM;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "cite"))
			{
				state->state = PARSER_CITE;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "subtitle"))
			{
				state->state = PARSER_SUBTITLE;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "empty-line"))
			{
				state->state = PARSER_EMPTY_LINE;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "table"))
			{
				state->state = PARSER_TABLE;
			}
			else
			{
				state->state = PARSER_UNKNOWN;
			}

			break;
		}
		case PARSER_HOME_PAGE:
		{
			break;
		}
		case PARSER_ID: // Start element
		{
			// printf ("ID Start element\n");

			break;
		}
		case PARSER_ISBN:
		{
			break;
		}
		case PARSER_IMAGE:
		{
			break;
		}
		case PARSER_KEYWORDS:
		{
			break;
		}
		case PARSER_LANG:
		{
			break;
		}
		case PARSER_LAST_NAME:
		{
//			state->history = g_list_prepend (state->history, (gpointer) PARSER_LAST_NAME);
			break;
		}
		case PARSER_MIDDLE_NAME:
		{
//			state->history = g_list_prepend (state->history, (gpointer) PARSER_MIDDLE_NAME);
			break;
		}
		case PARSER_NICKNAME:
		{
//			state->history = g_list_prepend (state->history, (gpointer) PARSER_NICKNAME);
			break;
		}
		case PARSER_OUTPUT_DOCUMENT_CLASS:
		{
			save_state_to_history (state);

			if (!xmlStrcmp (name, (const xmlChar *) "part"))
			{
				state->state = PARSER_PART;
			}
			else
			{
				state->state = PARSER_UNKNOWN;
			}

			break;
		}
		case PARSER_OUTPUT:
		{
			save_state_to_history (state);

			if (!xmlStrcmp (name, (const xmlChar *) "part"))
			{
				state->state = PARSER_PART;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "output-document-class"))
			{
				state->state = PARSER_OUTPUT_DOCUMENT_CLASS;
			}
			else
			{
				state->state = PARSER_UNKNOWN;
			}

			break;
		}
		case PARSER_P:
		{
			save_state_to_history (state);

			if (!xmlStrcmp (name, (const xmlChar *) "strong"))
			{
				state->state = PARSER_STRONG;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "emphasis"))
			{
				state->state = PARSER_EMPHASIS;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "style"))
			{
				state->state = PARSER_STYLE;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "a"))
			{
				state->state = PARSER_A;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "strikethrough"))
			{
				state->state = PARSER_STRIKETHROUGH;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "sub"))
			{
				state->state = PARSER_SUB;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "sup"))
			{
				state->state = PARSER_SUP;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "code"))
			{
				state->state = PARSER_CODE;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "image"))
			{
				state->state = PARSER_IMAGE;
			}
			else
			{
				state->state = PARSER_UNKNOWN;
			}

			break;
		}
		case PARSER_PART:
		{
			break;
		}
		case PARSER_POEM:
		{
			save_state_to_history (state);

			if (!xmlStrcmp (name, (const xmlChar *) "title"))
			{
				state->state = PARSER_TITLE;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "epigraph"))
			{
				state->state = PARSER_EPIGRAPH;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "stanza"))
			{
				state->state = PARSER_STANZA;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "text-author"))
			{
				state->state = PARSER_TEXT_AUTHOR;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "date"))
			{
				state->state = PARSER_DATE;
			}
			else
			{
				state->state = PARSER_UNKNOWN;
			}

			break;
		}
		case PARSER_PROGRAM_USED:
		{
			break;
		}
		case PARSER_PUBLISH_INFO:
		{
			save_state_to_history (state);

			if (!xmlStrcmp (name, (const xmlChar *) "book-name"))
			{
				state->state = PARSER_BOOK_NAME;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "publisher"))
			{
				state->state = PARSER_PUBLISHER;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "city"))
			{
				state->state = PARSER_CITY;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "year"))
			{
				state->state = PARSER_YEAR;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "isbn"))
			{
				state->state = PARSER_ISBN;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "sequence"))
			{
				state->state = PARSER_SEQUENCE;
			}
			else
			{
				state->state = PARSER_UNKNOWN;
			}

			break;
		}
		case PARSER_PUBLISHER:
		{
			// todo: заменить на единую функцию
			// SAXState prev_state = (SAXState) g_list_nth_data (state->history, 0);
			// SAXState prev_state = get_state_from_history (state, 0);
			SAXState prev_state = PARSER_PREV_STATE (state);

			if (prev_state == PARSER_DOCUMENT_INFO)
			{
				save_state_to_history (state);

				if (!xmlStrcmp (name, (const xmlChar *) "first-name"))
				{
					state->state = PARSER_FIRST_NAME;
				}
				else if (!xmlStrcmp (name, (const xmlChar *) "middle-name"))
				{
					state->state = PARSER_MIDDLE_NAME;
				}
				else if (!xmlStrcmp (name, (const xmlChar *) "last-name"))
				{
					state->state = PARSER_LAST_NAME;
				}
				else if (!xmlStrcmp (name, (const xmlChar *) "nickname"))
				{
					state->state = PARSER_NICKNAME;
				}
				else if (!xmlStrcmp (name, (const xmlChar *) "home-page"))
				{
					state->state = PARSER_HOME_PAGE;
				}
				else if (!xmlStrcmp (name, (const xmlChar *) "email"))
				{
					state->state = PARSER_EMAIL;
				}
				else if (!xmlStrcmp (name, (const xmlChar *) "id"))
				{
					state->state = PARSER_ID;
				}
				else
				{
					state->state = PARSER_UNKNOWN;
				}
			}

			break;
		}
		case PARSER_SECTION:
		{
			save_state_to_history (state);

			if (!xmlStrcmp (name, (const xmlChar *) "title"))
			{
				state->state = PARSER_TITLE;

				/*
				gchar *section_mark_name;

				section_mark_name = g_strdup_printf ("%d", state->section_mark);
				gtk_text_buffer_create_mark (state->buffer, section_mark_name,
											 &state->current_iter, TRUE);
				gtk_text_buffer_insert_with_tags_by_name (state->buffer, &state->current_iter,
														  " \r\n", -1, "big_gap_before_line", NULL);
				g_free (section_mark_name);
				*/
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "epigraph"))
			{
				state->state = PARSER_EPIGRAPH;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "image"))
			{
				state->state = PARSER_IMAGE;

				/*
				const gchar *image_name = NULL;

				if (g_str_has_prefix ((const gchar *) attrs[1], "#"))
				{
					image_name = g_strchug (g_strdelimit (g_strdup ((const gchar *) attrs[1]),
						(const gchar *) "#", ' '));
				}
				else
				{
					image_name = g_strdup ((const gchar *) attrs[1]);
				}

				gtk_text_buffer_create_mark (state->buffer, image_name,
											 &state->current_iter, TRUE);
				*/
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "annotation"))
			{
				state->state = PARSER_ANNOTATION;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "section"))
			{
				state->state = PARSER_SECTION;

				/*
				if (state->book_body)
				{
					state->section_level++;
				}
				*/
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "p"))
			{
				state->state = PARSER_P;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "poem"))
			{
				state->state = PARSER_POEM;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "subtitle"))
			{
				state->state = PARSER_SUBTITLE;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "cite"))
			{
				state->state = PARSER_CITE;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "empty-line"))
			{
				state->state = PARSER_EMPTY_LINE;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "table"))
			{
				state->state = PARSER_TABLE;
			}
			else
			{
				state->state = PARSER_UNKNOWN;
			}

			break;
		}
		case PARSER_SEQUENCE:
		{
			break;
		}
		case PARSER_SRC_LANG:
		{
			break;
		}
		case PARSER_SRC_OCR:
		{
			break;
		}
		case PARSER_SRC_TITLE_INFO:
		{
			save_state_to_history (state);

			if (!xmlStrcmp (name, (const xmlChar *) "genre"))
			{
				state->state = PARSER_GENRE;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "author"))
			{
				state->state = PARSER_AUTHOR;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "book-title"))
			{
				state->state = PARSER_BOOK_TITLE;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "annotation"))
			{
				state->state = PARSER_ANNOTATION;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "keywords"))
			{
				state->state = PARSER_KEYWORDS;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "date"))
			{
				state->state = PARSER_DATE;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "coverpage"))
			{
				state->state = PARSER_COVERPAGE;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "lang"))
			{
				state->state = PARSER_LANG;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "src-lang"))
			{
				state->state = PARSER_SRC_LANG;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "translator"))
			{
				state->state = PARSER_TRANSLATOR;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "sequence"))
			{
				state->state = PARSER_SEQUENCE;
			}
			else
			{
				state->state = PARSER_UNKNOWN;
			}

			break;
		}
		case PARSER_SRC_URL:
		{
			break;
		}
		case PARSER_STANZA:
		{
			save_state_to_history (state);

			if (!xmlStrcmp (name, (const xmlChar *) "title"))
			{
				state->state = PARSER_TITLE;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "subtitle"))
			{
				state->state = PARSER_SUBTITLE;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "v"))
			{
				state->state = PARSER_V;
			}
			else
			{
				state->state = PARSER_UNKNOWN;
			}

			break;
		}
		case PARSER_STRIKETHROUGH:
		{
			save_state_to_history (state);

			if (!xmlStrcmp (name, (const xmlChar *) "strong"))
			{
				state->state = PARSER_STRONG;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "emphasis"))
			{
				state->state = PARSER_EMPHASIS;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "style"))
			{
				state->state = PARSER_STYLE;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "a"))
			{
				state->state = PARSER_A;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "strikethrough"))
			{
				state->state = PARSER_STRIKETHROUGH;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "sub"))
			{
				state->state = PARSER_SUB;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "sup"))
			{
				state->state = PARSER_SUP;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "code"))
			{
				state->state = PARSER_CODE;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "image"))
			{
				state->state = PARSER_IMAGE;
			}
			else
			{
				state->state = PARSER_UNKNOWN;
			}

			break;
		}
		case PARSER_STRONG:
		{
			save_state_to_history (state);

			if (!xmlStrcmp (name, (const xmlChar *) "strong"))
			{
				state->state = PARSER_STRONG;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "emphasis"))
			{
				state->state = PARSER_EMPHASIS;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "style"))
			{
				state->state = PARSER_STYLE;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "a"))
			{
				state->state = PARSER_A;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "strikethrough"))
			{
				state->state = PARSER_STRIKETHROUGH;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "sub"))
			{
				state->state = PARSER_SUB;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "sup"))
			{
				state->state = PARSER_SUP;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "code"))
			{
				state->state = PARSER_CODE;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "image"))
			{
				state->state = PARSER_IMAGE;
			}
			else
			{
				state->state = PARSER_UNKNOWN;
			}

			break;
		}
		case PARSER_STYLE:
		{
			save_state_to_history (state);

			if (!xmlStrcmp (name, (const xmlChar *) "strong"))
			{
				state->state = PARSER_STRONG;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "emphasis"))
			{
				state->state = PARSER_EMPHASIS;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "style"))
			{
				state->state = PARSER_STYLE;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "a"))
			{
				state->state = PARSER_A;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "strikethrough"))
			{
				state->state = PARSER_STRIKETHROUGH;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "sub"))
			{
				state->state = PARSER_SUB;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "sup"))
			{
				state->state = PARSER_SUP;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "code"))
			{
				state->state = PARSER_CODE;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "image"))
			{
				state->state = PARSER_IMAGE;
			}
			else
			{
				state->state = PARSER_UNKNOWN;
			}

			break;
		}
		case PARSER_STYLESHEET:
		{
			break;
		}
		case PARSER_SUB:
		{
			save_state_to_history (state);

			if (!xmlStrcmp (name, (const xmlChar *) "strong"))
			{
				state->state = PARSER_STRONG;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "emphasis"))
			{
				state->state = PARSER_EMPHASIS;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "style"))
			{
				state->state = PARSER_STYLE;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "a"))
			{
				state->state = PARSER_A;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "strikethrough"))
			{
				state->state = PARSER_STRIKETHROUGH;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "sub"))
			{
				state->state = PARSER_SUB;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "sup"))
			{
				state->state = PARSER_SUP;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "code"))
			{
				state->state = PARSER_CODE;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "image"))
			{
				state->state = PARSER_IMAGE;
			}
			else
			{
				state->state = PARSER_UNKNOWN;
			}

			break;
		}
		case PARSER_SUBTITLE:
		{
			save_state_to_history (state);

			if (!xmlStrcmp (name, (const xmlChar *) "strong"))
			{
				state->state = PARSER_STRONG;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "emphasis"))
			{
				state->state = PARSER_EMPHASIS;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "style"))
			{
				state->state = PARSER_STYLE;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "a"))
			{
				state->state = PARSER_A;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "strikethrough"))
			{
				state->state = PARSER_STRIKETHROUGH;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "sub"))
			{
				state->state = PARSER_SUB;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "sup"))
			{
				state->state = PARSER_SUP;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "code"))
			{
				state->state = PARSER_CODE;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "image"))
			{
				state->state = PARSER_IMAGE;
			}
			else
			{
				state->state = PARSER_UNKNOWN;
			}

			break;
		}
		case PARSER_SUP:
		{
			save_state_to_history (state);

			if (!xmlStrcmp (name, (const xmlChar *) "strong"))
			{
				state->state = PARSER_STRONG;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "emphasis"))
			{
				state->state = PARSER_EMPHASIS;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "style"))
			{
				state->state = PARSER_STYLE;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "a"))
			{
				state->state = PARSER_A;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "strikethrough"))
			{
				state->state = PARSER_STRIKETHROUGH;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "sub"))
			{
				state->state = PARSER_SUB;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "sup"))
			{
				state->state = PARSER_SUP;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "code"))
			{
				state->state = PARSER_CODE;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "image"))
			{
				state->state = PARSER_IMAGE;
			}
			else
			{
				state->state = PARSER_UNKNOWN;
			}

			break;
		}
		case PARSER_TABLE:
		{
			save_state_to_history (state);

			if (!xmlStrcmp (name, (const xmlChar *) "tr"))
			{
				state->state = PARSER_TR;
			}
			else
			{
				state->state = PARSER_UNKNOWN;
			}

			break;
		}
		case PARSER_TD:
		{
			save_state_to_history (state);

			if (!xmlStrcmp (name, (const xmlChar *) "strong"))
			{
				state->state = PARSER_STRONG;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "emphasis"))
			{
				state->state = PARSER_EMPHASIS;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "style"))
			{
				state->state = PARSER_STYLE;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "a"))
			{
				state->state = PARSER_A;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "strikethrough"))
			{
				state->state = PARSER_STRIKETHROUGH;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "sub"))
			{
				state->state = PARSER_SUB;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "sup"))
			{
				state->state = PARSER_SUP;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "code"))
			{
				state->state = PARSER_CODE;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "image"))
			{
				state->state = PARSER_IMAGE;
			}
			else
			{
				state->state = PARSER_UNKNOWN;
			}

			break;
		}
		case PARSER_TEXT_AUTHOR:
		{
			save_state_to_history (state);

			if (!xmlStrcmp (name, (const xmlChar *) "strong"))
			{
				state->state = PARSER_STRONG;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "emphasis"))
			{
				state->state = PARSER_EMPHASIS;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "style"))
			{
				state->state = PARSER_STYLE;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "a"))
			{
				state->state = PARSER_A;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "strikethrough"))
			{
				state->state = PARSER_STRIKETHROUGH;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "sub"))
			{
				state->state = PARSER_SUB;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "sup"))
			{
				state->state = PARSER_SUP;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "code"))
			{
				state->state = PARSER_CODE;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "image"))
			{
				state->state = PARSER_IMAGE;
			}
			else
			{
				state->state = PARSER_UNKNOWN;
			}

			break;
		}
		case PARSER_TH:
		{
			save_state_to_history (state);

			if (!xmlStrcmp (name, (const xmlChar *) "strong"))
			{
				state->state = PARSER_STRONG;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "emphasis"))
			{
				state->state = PARSER_EMPHASIS;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "style"))
			{
				state->state = PARSER_STYLE;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "a"))
			{
				state->state = PARSER_A;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "strikethrough"))
			{
				state->state = PARSER_STRIKETHROUGH;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "sub"))
			{
				state->state = PARSER_SUB;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "sup"))
			{
				state->state = PARSER_SUP;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "code"))
			{
				state->state = PARSER_CODE;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "image"))
			{
				state->state = PARSER_IMAGE;
			}
			else
			{
				state->state = PARSER_UNKNOWN;
			}

			break;
		}
		case PARSER_TITLE:
		{
			save_state_to_history (state);

			if (!xmlStrcmp (name, (const xmlChar *) "p"))
			{
				state->state = PARSER_P;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "empty-line"))
			{
				state->state = PARSER_EMPTY_LINE;
			}
			else
			{
				state->state = PARSER_UNKNOWN;
			}

			break;
		}
		case PARSER_TITLE_INFO:
		{
			save_state_to_history (state);

			if (!xmlStrcmp (name, (const xmlChar *) "genre"))
			{
				state->state = PARSER_GENRE;

				// printf ("PARSER_GENRE start\n"); 
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "author"))
			{
				state->state = PARSER_AUTHOR;

				// printf ("PARSER_AUTHOR\n"); 
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "book-title"))
			{
				state->state = PARSER_BOOK_TITLE;

				// printf ("PARSER_BOOK_TITLE\n");
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "annotation"))
			{
				state->state = PARSER_ANNOTATION;

				/*
				GtkTextMark *mark = gtk_text_buffer_get_mark (state->buffer,
															  (const gchar *) "book_annotation");

				if (mark != NULL)
				{
					gtk_text_buffer_get_iter_at_mark (state->buffer, &state->current_iter,
													  GTK_TEXT_MARK (mark));
				}
				*/
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "keywords"))
			{
				state->state = PARSER_KEYWORDS;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "date"))
			{
				state->state = PARSER_DATE;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "coverpage"))
			{
				state->state = PARSER_COVERPAGE;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "lang"))
			{
				state->state = PARSER_LANG;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "src-lang"))
			{
				state->state = PARSER_SRC_LANG;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "translator"))
			{
				state->state = PARSER_TRANSLATOR;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "sequence"))
			{
				state->state = PARSER_SEQUENCE;
			}
			else
			{
				state->state = PARSER_UNKNOWN;
			}

			break;
		}
		case PARSER_TR:
		{
			save_state_to_history (state);

			if (!xmlStrcmp (name, (const xmlChar *) "th"))
			{
				state->state = PARSER_TH;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "td"))
			{
				state->state = PARSER_TD;
			}
			else
			{
				state->state = PARSER_UNKNOWN;
			}

			break;
		}
		case PARSER_TRANSLATOR:
		{
			save_state_to_history (state);

			if (!xmlStrcmp (name, (const xmlChar *) "first-name"))
			{
				state->state = PARSER_FIRST_NAME;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "middle-name"))
			{
				state->state = PARSER_MIDDLE_NAME;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "last-name"))
			{
				state->state = PARSER_LAST_NAME;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "nickname"))
			{
				state->state = PARSER_NICKNAME;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "home-page"))
			{
				state->state = PARSER_HOME_PAGE;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "email"))
			{
				state->state = PARSER_EMAIL;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "id"))
			{
				state->state = PARSER_ID;
			}
			else
			{
				state->state = PARSER_UNKNOWN;
			}

			break;
		}
		case PARSER_V:
		{
			save_state_to_history (state);

			if (!xmlStrcmp (name, (const xmlChar *) "strong"))
			{
				state->state = PARSER_STRONG;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "emphasis"))
			{
				state->state = PARSER_EMPHASIS;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "style"))
			{
				state->state = PARSER_STYLE;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "a"))
			{
				state->state = PARSER_A;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "strikethrough"))
			{
				state->state = PARSER_STRIKETHROUGH;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "sub"))
			{
				state->state = PARSER_SUB;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "sup"))
			{
				state->state = PARSER_SUP;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "code"))
			{
				state->state = PARSER_CODE;
			}
			else if (!xmlStrcmp (name, (const xmlChar *) "image"))
			{
				state->state = PARSER_IMAGE;
			}
			else
			{
				state->state = PARSER_UNKNOWN;
			}

			break;
		}
		case PARSER_VERSION:
		{
			break;
		}
		case PARSER_YEAR:
		{
			break;
		}
		case PARSER_FINISH:
		{
			break;
		}
		case PARSER_UNKNOWN:
		{
			// printf ("PARSER_UNKNOWN\n");
			/*
			printf ("\n\nPARSER_UNKNOWN: State = %s, Prev state = %d, Prev-prev state = %d\n\n",
					name,
					(parser_state) g_list_nth_data (state->history, 0),
					(parser_state) g_list_nth_data (state->history, 1));
			 */

			break;
		}
	}

	// save_state_to_history (state);
}

