/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * mb-fb2-parser-characters.c
 * Copyright (C) 2014 Yan Zlobin <yan_zlobin@agniyoga.ru>
 * 
 */

#include "mb-fb2-parser.h"

void
mb_fb2_parser_characters (ParserState *state, const xmlChar *chars, int len)
{
	switch (state->state)
	{
		case PARSER_START:
		{
			break;
		}
		case PARSER_FICTIONBOOK:
		{
			break;
		}
		case PARSER_A:
		{
			break;
		}
		case PARSER_ANNOTATION:
		{
			break;
		}
		case PARSER_AUTHOR:
		{
			break;
		}
		case PARSER_BINARY:
		{
			/*
			if (chars != NULL)
			{
				g_string_append_len (state->image_body, (const gchar *) chars, len);
			}
			*/
						
			break;
		}
		case PARSER_BODY:
		{
			break;
		}
		case PARSER_BOOK_NAME:
		{
			break;
		}
		case PARSER_BOOK_TITLE:
		{
			/*
			if (chars != NULL)
			{
				parser_state prev_state = (parser_state) g_list_nth_data (state->history, 0);

				PARSER_PREV_STATE (state)

				if (prev_state == PARSER_TITLE_INFO)
				{
					g_string_append_len (state->book_title, (const gchar *) chars, len);
				}
			}
			*/

			break;
		}
		case PARSER_CITE:
		{
			break;
		}
		case PARSER_CITY:
		{
			break;
		}
		case PARSER_CODE:
		{
			break;
		}
		case PARSER_COVERPAGE:
		{
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
			break;
		}
		case PARSER_DOCUMENT_INFO:
		{
			break;
		}
		case PARSER_EMAIL:
		{
			break;
		}
		case PARSER_EMPHASIS:
		{
			/*
			if (state->book_body)
			{
				gtk_text_buffer_insert_with_tags_by_name (state->buffer, &state->current_iter,
														  (const gchar *) chars, len, "italic", NULL);
			}
			*/
			
			break;
		}
		case PARSER_EMPTY_LINE:
		{
			break;
		}
		case PARSER_EPIGRAPH:
		{
			break;
		}
		case PARSER_FIRST_NAME:
		{
			/*
			if (chars != NULL)
			{
				parser_state prev_state = (parser_state) g_list_nth_data (state->history, 0);
				parser_state prev_prev_state = (parser_state) g_list_nth_data (state->history, 1);

				if (prev_state == PARSER_AUTHOR)
				{
					if (prev_prev_state == PARSER_TITLE_INFO)
					{
						g_string_append_len (state->book_author_first_name,
											 (const gchar *) chars, len);
					}
				}
			}
			*/

			break;
		}
		case PARSER_GENRE:
		{
			break;
		}
		case PARSER_HISTORY:
		{
			break;
		}
		case PARSER_HOME_PAGE:
		{
			break;
		}
		case PARSER_ID:
		{
			/*
			if (chars != NULL)
			{
				parser_state prev_state = (parser_state) g_list_nth_data (state->history, 0);

				if (prev_state == PARSER_DOCUMENT_INFO)
				{
					g_string_append_len (state->book_id, (const gchar *) chars, len);
				}
			}
			*/
			
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
			/*
			if (chars != NULL)
			{
				parser_state prev_state = (parser_state) g_list_nth_data (state->history, 0);
				parser_state prev_prev_state = (parser_state) g_list_nth_data (state->history, 1);

				if (prev_state == PARSER_AUTHOR)
				{
					if (prev_prev_state == PARSER_TITLE_INFO)
					{
						g_string_append_len (state->book_author_last_name,
											 (const gchar *) chars, len);
					}
				}
			}
			*/

			break;
		}
		case PARSER_MIDDLE_NAME:
		{
			break;
		}
		case PARSER_NICKNAME:
		{
			/*
			if (chars != NULL)
			{
				parser_state prev_state = (parser_state) g_list_nth_data (state->history, 0);
				parser_state prev_prev_state = (parser_state) g_list_nth_data (state->history, 1);

				if (prev_state == PARSER_AUTHOR)
				{
					if (prev_prev_state == PARSER_TITLE_INFO)
					{
						g_string_append_len (state->book_author_nickname,
											 (const gchar *) chars, len);
					}
				}
			}
			*/

			break;
		}
		case PARSER_OUTPUT_DOCUMENT_CLASS:
		{
			break;
		}
		case PARSER_OUTPUT:
		{
			break;
		}
		case PARSER_P:
		{
			if (state->book_body)
			{
				// printf ("%.*s", len, chars);

				g_string_append_len (state->paragraph, (const gchar *) chars,
				                     (gssize) len);
			}

			// printf ("%.*s", len, chars);
			
			/*
			if (state->book_body)
			{
				parser_state prev_state = (parser_state) g_list_nth_data (state->history, 0);

				if (prev_state == PARSER_ANNOTATION)
				{
					gtk_text_buffer_insert_with_tags_by_name (state->buffer, &state->current_iter,
															  (const gchar *) chars, len,
															  "small_gap_after_line", "wide_margins", "justify",
															  NULL);
				}
				else if (prev_state == PARSER_CITE)
				{
					gtk_text_buffer_insert_with_tags_by_name (state->buffer, &state->current_iter,
															  (const gchar *) chars, len,
															  "justify", "italic", "middle_gap_left_line",
															  "small_gap_after_line", "small_gap_before_line",
															  NULL);
				}
				else if (prev_state == PARSER_EPIGRAPH)
				{
					gtk_text_buffer_insert_with_tags_by_name (state->buffer, &state->current_iter,
															  (const gchar *) chars, len,
															  "italic", "right", "big_gap_left_line", NULL);
				}
				else if (prev_state == PARSER_TITLE)
				{
					if (state->section_inside)
					{
						gchar *section_name;
						
						// section_name = g_locale_to_utf8 (g_strndup ((const gchar *) chars,
																	(gssize) len), (gssize) len, NULL, NULL, NULL);

						section_name = g_strndup ((const gchar *) chars, (gssize) len);
						
						if ((g_str_has_suffix ((const gchar *) section_name, (const gchar *) ","))
							|| (g_str_has_suffix ((const gchar *) section_name,
												  (const gchar *) ".")))
						{
							g_string_append_printf (state->section_name, (const gchar *) "%s ",
													section_name);
						}
						else if (g_str_has_suffix ((const gchar *) section_name,
												   (const gchar *) ". "))
						{
							state->section_name = g_string_append (state->section_name,
																   section_name);
						}
						else
						{
							g_string_append_printf (state->section_name, (const gchar *) "%s - ",
													section_name);
						}

						gtk_text_buffer_insert_with_tags_by_name (state->buffer,
																  &state->current_iter,
																  section_name, -1,
																  "left", "heading", NULL);
						/ *
						state->section_name = g_string_append_len (state->section_name,
																   (const gchar *) chars, (gssize) len);
						gtk_text_buffer_insert_with_tags_by_name (state->buffer,
																  &state->current_iter,
																  (const gchar *) chars, len,
																  "center", "heading", NULL);
						* /
						 g_free (section_name);
					}
				}
				else
				{
					gtk_text_buffer_insert_with_tags_by_name (state->buffer,
																  &state->current_iter,
																  (const gchar *) chars, len,
																  "justify", "small_gap_after_line",
																  "small_gap_before_line", NULL);
				}
			}
			*/

			break;
		}
		case PARSER_PART:
		{
			break;
		}
		case PARSER_POEM:
		{
			break;
		}
		case PARSER_PROGRAM_USED:
		{
			break;
		}
		case PARSER_PUBLISH_INFO:
		{
			break;
		}
		case PARSER_PUBLISHER:
		{
			break;
		}
		case PARSER_SECTION:
		{
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
			break;
		}
		case PARSER_SRC_URL:
		{
			break;
		}
		case PARSER_STANZA:
		{
			break;
		}
		case PARSER_STRIKETHROUGH:
		{
			break;
		}
		case PARSER_STRONG:
		{
			/*
			if (state->book_body)
			{
				gtk_text_buffer_insert_with_tags_by_name (state->buffer, &state->current_iter,
														  (const gchar *) chars, len, "bold", NULL);
			}
			*/

			break;
		}
		case PARSER_STYLE:
		{
			break;
		}
		case PARSER_STYLESHEET:
		{
			break;
		}
		case PARSER_SUB:
		{
			break;
		}
		case PARSER_SUBTITLE:
		{
			break;
		}
		case PARSER_SUP:
		{
			break;
		}
		case PARSER_TABLE:
		{
			break;
		}
		case PARSER_TD:
		{
			break;
		}
		case PARSER_TEXT_AUTHOR:
		{
			break;
		}
		case PARSER_TH:
		{
			break;
		}
		case PARSER_TITLE:
		{
			break;
		}
		case PARSER_TITLE_INFO:
		{
			break;
		}
		case PARSER_TR:
		{
			break;
		}
		case PARSER_TRANSLATOR:
		{
			break;
		}
		case PARSER_V:
		{
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
			break;
		}
	}
}

