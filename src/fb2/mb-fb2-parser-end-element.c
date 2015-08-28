/* Magic Book Reader
 * Copyright (C) 2015 Yan Zlobin <yan_zlobin@agniyoga.ru>
 *
 * mb-fb2-parser-end-element.c
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

#include "mb-fb2-parser.h"

void
mb_fb2_parser_end_element (ParserState *state, const xmlChar *name)
{
	switch (state->state)
	{
		case PARSER_START:
		{
			break;
		}
		case PARSER_FICTIONBOOK:
		{
			// state->state = PARSER_FINISH;

			break;
		}
		case PARSER_A:
		{
			break;
		}
		case PARSER_ANNOTATION:
		{
/*			
			gtk_text_buffer_get_end_iter (state->buffer, &state->current_iter);
			GtkTextMark *mark;
			GtkTextIter iter;
			
			mark = gtk_text_buffer_get_mark (state->buffer, (const gchar *) "book_annotation");

			if (mark != NULL)
			{
				gtk_text_buffer_get_iter_at_mark (state->buffer, &iter, GTK_TEXT_MARK (mark));
				gtk_text_buffer_insert_with_tags_by_name (state->buffer, &iter,
														  (const gchar *) state->annotation->str, -1,
														  "big_gap_after_line", "wide_margins", "justify",
														  NULL);
				gtk_text_buffer_get_end_iter (state->buffer, &state->current_iter);
			}
*/
			break;
		}
		case PARSER_AUTHOR:
		{
			break;
		}
		case PARSER_BINARY:
		{
			/*
//			gtk_text_buffer_insert_with_tags_by_name (state->buffer, &state->current_iter,
//													  " ", -1, "center", NULL);
			libmbreader_fb2_text_utils_insert_image (state->buffer, state->image_name->str,
								 state->image_body->str);
			g_string_truncate (state->image_name, 0);
			g_string_truncate (state->image_body, 0);
			*/
			
			break;
		}
		case PARSER_BODY:
		{
			state->book_body = FALSE;
			
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
			// printf ("PARSER_DESCRIPTION end\n");
			
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
			break;
		}
		case PARSER_EMPTY_LINE:
		{
			/*
			if (state->book_body)
			{
				libmbreader_fb2_text_utils_insert_blank_line (state->buffer, &state->current_iter);
			}
			*/
			
			break;
		}
		case PARSER_EPIGRAPH:
		{
			/*
			if (state->book_body)
			{
				libmbreader_fb2_text_utils_insert_blank_line (state->buffer, &state->current_iter);
			}
			*/
			
			break;
		}
		case PARSER_FIRST_NAME:
		{
			break;
		}
		case PARSER_GENRE:
		{
			// printf ("PARSER_GENRE end\n");
			
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
			parser_state prev_state = (parser_state) g_list_nth_data (state->history, 0);

			if (prev_state == PARSER_DOCUMENT_INFO)
			{
				if (state->book_id->len > 0)
				{
					libmbreader_fb2_book_data.book_id = g_strdup ((const gchar *) state->book_id->str);
				}
				else
				{
					libmbreader_fb2_book_data.book_id = g_strdup (
					    (const gchar *) libmbreader_fb2_book_data.book_filename);
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
			break;
		}
		case PARSER_MIDDLE_NAME:
		{
			break;
		}
		case PARSER_NICKNAME:
		{
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
				// printf ("%s\n\n", state->paragraph->str);

				mb_book_buffer_paragraph_append (book_buffer,
				                                 state->paragraph->str,
				                                 state->paragraph->len);

				state->paragraph = g_string_truncate (state->paragraph, 0);
			}

			// printf ("name: %s", name);
			
			/*
			if (state->book_body)
			{
				libmbreader_fb2_text_utils_insert_blank_line (state->buffer, &state->current_iter);
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
			/*
			if (state->book_body)
			{
//
				if (state->section_tree->data)
				{
					state->section_tree = g_list_delete_link (state->section_tree,
															  g_list_nth_data (state->section_tree, 0));
//					state->section_tree = g_list_remove (state->section_tree,
//														 (gpointer) &iter);
				}
//				
				state->section_level--;
			}
			*/

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
			/*
			if (state->book_body)
			{
				parser_state prev_state = (parser_state) g_list_nth_data (state->history, 0);
				if (prev_state == PARSER_SECTION)
				{
					GtkTreeIter iter;
					guint section_name_len;
					gchar *section_mark_name;
			
					section_name_len = state->section_name->len - 3;
					g_string_truncate (state->section_name, section_name_len);
					section_mark_name = g_strdup_printf ("%d", state->section_mark);
					
					if (state->section_level == 1)
					{
						gtk_tree_store_append (state->index_store, &iter, NULL);
					}
					else
					{
						if (state->section_level > state->prev_section_level)
						{
							state->parent_iter = state->last_iter;
						}
						else if (state->section_level < state->prev_section_level)
						{
							gtk_tree_model_iter_parent (GTK_TREE_MODEL (state->index_store), &iter,
														&state->parent_iter);
							state->parent_iter = iter;
						}

						gtk_tree_store_append (state->index_store, &iter, &state->parent_iter);
					}

					gtk_tree_store_set (state->index_store, &iter, 0, state->section_name->str,
										1, section_mark_name, -1);
					
					state->last_iter = iter;
					state->prev_section_level = state->section_level;

*					
					if (state->section_level < 2)
					{
						gtk_tree_store_append (state->index_store, &state->index_iter, NULL);
						gtk_tree_store_set (state->index_store, &state->index_iter, 0,
											state->section_name->str, 1, section_mark_name, -1);
					}
					else
					{
						gtk_tree_store_append (state->index_store, &child_iter,
											   &state->index_iter);
						gtk_tree_store_set (state->index_store, &child_iter, 0,
											state->section_name->str, 1, section_mark_name, -1);
					}
*
					
					state->section_mark++;
					state->section_name = g_string_truncate (state->section_name, 0);
					g_free (section_mark_name);
				}
			}
			*/
/*
			if (state->book_body)
			{
				if ((parser_state) g_list_nth_data (state->history, 0) == PARSER_BODY)
				{
					gtk_text_buffer_insert_with_tags_by_name (state->buffer, &state->end_iter,
															  " ", -1, "center", NULL);
					gtk_text_buffer_create_mark (state->buffer,
												 (const gchar *) state->coverpage_name,
												 &state->end_iter, TRUE);
					buffer_insert_blank_line (state->buffer, &state->end_iter);
				
					gtk_text_buffer_insert_with_tags_by_name (state->buffer, &state->end_iter,
															  (const gchar *) state->annotation->str, -1,
															  "big_gap_after_line", "wide_margins", "justify", NULL);
					buffer_insert_blank_line (state->buffer, &state->end_iter);
					buffer_insert_blank_line (state->buffer, &state->end_iter);
					buffer_insert_blank_line (state->buffer, &state->end_iter);
					buffer_insert_blank_line (state->buffer, &state->end_iter);
				}
			}
*/
			 break;
		}
		case PARSER_TITLE_INFO:
		{
			// printf ("PARSER_TITLE_INFO end\n");
			
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

	remove_state_from_history (state);
}
