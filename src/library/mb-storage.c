/* Magic Book Reader
 * Copyright (C) 2015 Yan Zlobin <yan_zlobin@agniyoga.ru>
 *
 * mb-storage.c
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

#include <sqlite3.h>
#include <glib/gstdio.h>
#include "mb-storage.h"
#include "../fs/mb-file.h"

#define CURRENT_APP_VERSION			"0.6"

#define STORAGE_DIR					"mbreader"
#define STORAGE_FILENAME			"library.db"
#define LIBRARY_COVER_WIDTH			160

#define DB(obj)						(obj->priv->database)
#define BEGIN(obj)					(transaction_begin (obj->priv->database))
#define COMMIT(obj)					(transaction_commit (obj->priv->database))

#define MB_STORAGE_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), \
	MB_TYPE_STORAGE, MbStoragePrivate))

typedef struct _BookKey BookKey;

struct _MbStoragePrivate
{
	MbReference *reference;
	sqlite3 *database;

	GList *errors;

	guint error_file_count;
	guint file_count;

	gchar *filename;
};

enum
{
	SIGNAL_READY,
	SIGNAL_LAST
};

struct _BookKey
{
	guint key;
	gchar *checksum;
};

static guint mb_storage_signals[SIGNAL_LAST] = { 0 };

static void mb_storage_finalize (GObject *object);

static void create_database_dir (MbStorage *storage);

static void database_open (MbStorage *storage);

static void database_close (MbStorage *storage);

static void database_check (MbStorage *storage);

static gpointer database_init (gpointer data);

static void database_create (MbStorage *storage);

static void database_load_init_values (MbStorage *storage);

static void transaction_begin (sqlite3 *database);

static void transaction_commit (sqlite3 *database);

static gboolean book_append (MbStorage *storage, gchar *filename);

static guint book_get_key (MbStorage *storage, MbBookPreview *preview,
                           gchar *filename, gchar *intern_filename,
                           gchar *checksum);

static guint book_data_append (MbStorage *storage, MbBookPreview *preview,
                               gchar *filename, gchar *intern_filename,
                               gchar *checksum);

static void book_metadata_append (MbStorage *storage, MbBookPreview *preview,
                                  guint key);

static guint genre_get_key (MbStorage *storage, gchar *name);

static guint author_get_key (MbStorage *storage, MbBookAuthor *author);

static guint author_append (MbStorage *storage, MbBookAuthor *author);

static guint sequence_get_key (MbStorage *storage, MbBookSequence *sequence);

static guint sequence_append (MbStorage *storage, MbBookSequence *sequence);

static void book_genre_append (MbStorage *storage, guint book, guint genre);

static void book_author_append (MbStorage *storage, guint book, guint author);

static void book_sequence_append (MbStorage *storage, guint book,
                                  guint sequence, guint number);

static int get_key_callback (void *data, int col_count, char **col_text,
                             char **col_names);

static int get_book_callback (void *data, int col_count, char **col_text,
                              char **col_names);

G_DEFINE_TYPE (MbStorage, mb_storage, G_TYPE_OBJECT);

static void
mb_storage_init (MbStorage *storage)
{
	MbStoragePrivate *priv;

	priv = MB_STORAGE_GET_PRIVATE (storage);
	storage->priv = priv;

	priv->filename = NULL;
	priv->database = NULL;
	priv->errors = NULL;

	priv->error_file_count = 0;
	priv->file_count = 0;
}

static void
mb_storage_class_init (MbStorageClass *klass)
{
	GObjectClass *gobject_class;

	g_type_class_add_private (klass, sizeof (MbStoragePrivate));

	gobject_class = G_OBJECT_CLASS (klass);
	
	mb_storage_signals[SIGNAL_READY] =
		g_signal_new ("ready", G_TYPE_FROM_CLASS (klass),
		              G_SIGNAL_RUN_LAST,
		              G_STRUCT_OFFSET (MbStorageClass, ready),
		              NULL, NULL, g_cclosure_marshal_VOID__VOID,
		              G_TYPE_NONE, 0);

	gobject_class->finalize = mb_storage_finalize;
}

static void
mb_storage_finalize (GObject *object)
{
	MbStorage *storage;
	MbStoragePrivate *priv;

	storage = MB_STORAGE (object);
	priv = storage->priv;

	if (priv->errors)
	{
		g_list_free_full (priv->errors, g_free);
	}
	
	if (priv->filename)
	{
		g_free (priv->filename);
	}

	database_close (storage);

	G_OBJECT_CLASS (mb_storage_parent_class)->finalize (object);
}

MbStorage *
mb_storage_new (MbReference *reference)
{
	MbStorage *storage;
	MbStoragePrivate *priv;

	storage = g_object_new (MB_TYPE_STORAGE, NULL);
	priv = storage->priv;

	priv->reference = reference;

	return storage;
}

void
mb_storage_open (MbStorage *storage)
{
	create_database_dir (storage);
	database_open (storage);
	database_check (storage);
}

void
mb_storage_append_file (MbStorage *storage, gchar *filename)
{
	book_append (storage, filename);
}

void
mb_storage_append_file_list (MbStorage *storage, GList *file_list)
{
	MbStoragePrivate *priv;
	GList *list;
	
	g_return_if_fail (DB (storage));

	priv = storage->priv;
	
	if (file_list)
	{
		for (list = file_list; list; list = list->next)
		{
			if (book_append (storage, (gchar *) list->data))
			{
				priv->file_count++;
			}
			else
			{
				priv->error_file_count++;

				priv->errors = g_list_prepend (priv->errors,
				                               g_strdup ((gchar *) list->data));
			}
		}
	}
}

static void
create_database_dir (MbStorage *storage)
{
	MbStoragePrivate *priv;
	gchar *storage_dir;

	priv = storage->priv;
	storage_dir = g_build_filename (g_get_user_data_dir (), STORAGE_DIR, NULL); 

	if (g_file_test (storage_dir, G_FILE_TEST_EXISTS))
	{
		if (g_file_test (storage_dir, G_FILE_TEST_IS_DIR))
		{
			priv->filename = g_build_filename (storage_dir, STORAGE_FILENAME,
			                                   NULL);
		}
		else
		{
			g_warning ("Cannon create library storage directory. File "
				"already exists: %s", storage_dir);
		}
	}
	else
	{
		if (g_mkdir (storage_dir, 0770) < 0)
		{
			g_warning ("Cannon create library storage directory: %s\n",
			           storage_dir);
		}
		else
		{
			priv->filename = g_build_filename (storage_dir, STORAGE_FILENAME,
			                                   NULL);
		}
	}

	g_free (storage_dir);
}

static void
database_open (MbStorage *storage)
{
	MbStoragePrivate *priv;
	gchar *error;
	gchar *sql;
	int rc;

	priv = storage->priv;

	if (priv->filename)
	{
		rc = sqlite3_open ((const char *) priv->filename, &priv->database);

		if (rc != SQLITE_OK)
		{
			sqlite3_close (priv->database);

			g_warning ("Cannon open library database file: %s. %s\n",
			           priv->filename, sqlite3_errmsg (priv->database));

			priv->database = NULL;
		}

		sql = "PRAGMA foreign_keys = ON;";

		sqlite3_exec (priv->database, (const char *) sql, NULL, NULL, &error);

		sqlite3_free (error);
	}
}

static void
database_close (MbStorage *storage)
{
	if (DB (storage))
	{
		sqlite3_close (DB (storage));
	}
}

static void
database_check (MbStorage *storage)
{
	gchar *error;
	gchar *sql;
	int rc;


	g_return_if_fail (DB (storage));

	// etc - system table
	sql = "CREATE TABLE etc (version TEXT, last_vacuum_date TEXT);";

	rc = sqlite3_exec (DB (storage), (const char *) sql, NULL, NULL, &error);

	// Return if etc table already exists 
	if (rc)
	{
		g_signal_emit (storage, mb_storage_signals[SIGNAL_READY], 0);
		
		sqlite3_free (error);

		return;
	}
	else
	{
		g_thread_new (NULL, (GThreadFunc) database_init, storage);
		
	}

	sqlite3_free (error);
}

static gpointer
database_init (gpointer data)
{
	MbStorage *storage;
	gpointer result = NULL;

	storage = MB_STORAGE (data);
	
	database_create (storage);
	database_load_init_values (storage);

	g_signal_emit (storage, mb_storage_signals[SIGNAL_READY], 0);

	return result;
}

static void
database_create (MbStorage *storage)
{
	gchar *error;
	gchar *sql;

	g_return_if_fail (DB (storage));

	// books
	sql = "CREATE TABLE books (key INTEGER PRIMARY KEY AUTOINCREMENT, "
		"filename TEXT, intern_filename TEXT, id TEXT, checksum TEXT, "
		"title TEXT, annotation TEXT, cover BLOB);";

	sqlite3_exec (DB (storage), (const char *) sql, NULL, NULL, &error);

	sql = "CREATE UNIQUE INDEX books_key_idx ON books (key);";

	sqlite3_exec (DB (storage), (const char *) sql, NULL, NULL, &error);

	sql = "CREATE INDEX books_filename_idx ON books (filename);";

	sqlite3_exec (DB (storage), (const char *) sql, NULL, NULL, &error);

	sql = "CREATE INDEX books_intern_filename_idx ON books (intern_filename);";

	sqlite3_exec (DB (storage), (const char *) sql, NULL, NULL, &error);

	sql = "CREATE INDEX books_checksum_idx ON books (checksum);";

	sqlite3_exec (DB (storage), (const char *) sql, NULL, NULL, &error);

	sql = "CREATE INDEX books_title_idx ON books (title COLLATE NOCASE);";

	sqlite3_exec (DB (storage), (const char *) sql, NULL, NULL, &error);

	// sql = "CREATE TRIGGER books_insert_tr ;";

	// sqlite3_exec (DB (storage), (const char *) sql, NULL, NULL, &error);

	// authors
	sql = "CREATE TABLE authors (key INTEGER PRIMARY KEY AUTOINCREMENT, "
		"first_name TEXT, middle_name TEXT, last_name TEXT, nick_name TEXT, "
		"count INTEGER DEFAULT 0);";

	sqlite3_exec (DB (storage), (const char *) sql, NULL, NULL, &error);

	sql = "CREATE UNIQUE INDEX authors_idx ON authors (key);";

	sqlite3_exec (DB (storage), (const char *) sql, NULL, NULL, &error);

	sql = "CREATE INDEX authors_first_name_idx ON authors (first_name "
		"COLLATE NOCASE);";

	sqlite3_exec (DB (storage), (const char *) sql, NULL, NULL, &error);

	sql = "CREATE INDEX authors_middle_name_idx ON authors (middle_name "
		"COLLATE NOCASE);";

	sqlite3_exec (DB (storage), (const char *) sql, NULL, NULL, &error);

	sql = "CREATE INDEX authors_last_name_idx ON authors (last_name "
		"COLLATE NOCASE);";

	sqlite3_exec (DB (storage), (const char *) sql, NULL, NULL, &error);

	sql = "CREATE INDEX authors_nick_name_idx ON authors (nick_name "
		"COLLATE NOCASE);";

	sqlite3_exec (DB (storage), (const char *) sql, NULL, NULL, &error);

	// genres
	sql = "CREATE TABLE genres (key INTEGER PRIMARY KEY, name TEXT, "
		"count INTEGER DEFAULT 0);";

	sqlite3_exec (DB (storage), (const char *) sql, NULL, NULL, &error);

	sql = "CREATE UNIQUE INDEX genres_idx ON genres (key);";

	sqlite3_exec (DB (storage), (const char *) sql, NULL, NULL, &error);

	sql = "CREATE INDEX genres_name_idx ON genres (name COLLATE nocase);";

	sqlite3_exec (DB (storage), (const char *) sql, NULL, NULL, &error);

	// sequences
	sql = "CREATE TABLE sequences (key INTEGER PRIMARY KEY, name TEXT, "
		"count INTEGER DEFAULT 0);";

	sqlite3_exec (DB (storage), (const char *) sql, NULL, NULL, &error);

	sql = "CREATE UNIQUE INDEX sequences_idx ON sequences (key);";

	sqlite3_exec (DB (storage), (const char *) sql, NULL, NULL, &error);

	sql = "CREATE INDEX sequences_name_idx ON sequences (name COLLATE nocase);";

	sqlite3_exec (DB (storage), (const char *) sql, NULL, NULL, &error);
	
	// book_authors
	sql = "CREATE TABLE book_authors (book INTEGER, author INTEGER, "
		"FOREIGN KEY (book) REFERENCES books (key), "
		"FOREIGN KEY (author) REFERENCES authors (key));";

	sqlite3_exec (DB (storage), (const char *) sql, NULL, NULL, &error);

	sql = "CREATE INDEX book_authors_book_idx ON book_authors (book);";

	sqlite3_exec (DB (storage), (const char *) sql, NULL, NULL, &error);

	sql = "CREATE INDEX book_authors_author_idx ON book_authors (author);";

	sqlite3_exec (DB (storage), (const char *) sql, NULL, NULL, &error);
	
	// book_genres
	sql = "CREATE TABLE book_genres (book INTEGER, genre INTEGER, "
		"FOREIGN KEY (book) REFERENCES books (key), "
		"FOREIGN KEY (genre) REFERENCES genres (key));";
	
	sqlite3_exec (DB (storage), (const char *) sql, NULL, NULL, &error);

	sql = "CREATE INDEX book_genres_book_idx ON book_genres (book);";

	sqlite3_exec (DB (storage), (const char *) sql, NULL, NULL, &error);

	sql = "CREATE INDEX book_genres_genre_idx ON book_genres (genre);";

	sqlite3_exec (DB (storage), (const char *) sql, NULL, NULL, &error);

	// book_sequences
	sql = "CREATE TABLE book_sequences (book INTEGER, sequence INTEGER, "
		"sequence_number INTEGER, "
		"FOREIGN KEY (book) REFERENCES books (key), "
		"FOREIGN KEY (sequence) REFERENCES sequences (key));";

	sqlite3_exec (DB (storage), (const char *) sql, NULL, NULL, &error);

	sql = "CREATE INDEX book_sequences_book_idx ON book_sequences (book);";

	sqlite3_exec (DB (storage), (const char *) sql, NULL, NULL, &error);
	
	sql = "CREATE INDEX book_sequences_sequence_idx ON book_sequences (sequence);";

	sqlite3_exec (DB (storage), (const char *) sql, NULL, NULL, &error);
	
	// bookmarks
	sql = "CREATE TABLE bookmarks (key INTEGER, position INTEGER);";

	sqlite3_exec (DB (storage), (const char *) sql, NULL, NULL, &error);
	
	sql = "CREATE UNIQUE INDEX bookmarks_idx ON bookmarks (key);";

	sqlite3_exec (DB (storage), (const char *) sql, NULL, NULL, &error);

	sqlite3_free (error);
}

static void
database_load_init_values (MbStorage *storage)
{
	MbStoragePrivate *priv;
	GList *genre_list = NULL;
	GList *list;
	gchar *sql;
	guint key;

	priv = storage->priv;

	g_return_if_fail (DB (storage));

	sql = g_strdup_printf ("INSERT INTO etc (version, last_vacuum_date) "
		"VALUES ('%s', '%li');", CURRENT_APP_VERSION, g_get_real_time ());

	sqlite3_exec (DB (storage), (const char *) sql, NULL, NULL, NULL);

	g_free (sql);

	genre_list = mb_reference_get_genre_list (priv->reference);

	if (genre_list)
	{
		BEGIN (storage);
		{
			for (list = genre_list; list; list = list->next)
			{
				key = g_str_hash (list->data);

				sql = g_strdup_printf ("INSERT OR IGNORE INTO genres (key, "
					"name) VALUES (%u, '%s');", key, (gchar *) list->data);

				sqlite3_exec (DB (storage), (const char *) sql, NULL, NULL,
				              NULL);

				g_free (sql);
			}
		}
		COMMIT (storage);
	}

	g_list_free (genre_list);
}

static void
transaction_begin (sqlite3 *database)
{
	sqlite3_exec (database, (const char *) "BEGIN EXCLUSIVE TRANSACTION", NULL,
	              NULL, NULL);
}

static void
transaction_commit (sqlite3 *database)
{
	sqlite3_exec (database, (const char *) "COMMIT TRANSACTION", NULL, NULL,
	              NULL);
}

static gboolean
book_append (MbStorage *storage, gchar *filename)
{
	MbBookPreview *preview;
	gchar *intern_filename;
	gchar *checksum;
	MbFile *file;
	gboolean success = FALSE;
	guint key;

	file = mb_file_new (filename);

	if (!file)
	{
		return success;
	}

	BEGIN (storage);
	{
		if (SINGLE_FILE (file))
		{
			preview = mb_file_get_preview_full (file, filename);

			if (preview)
			{
				checksum = mb_file_get_checksum (file, filename);
				intern_filename = mb_file_get_intern_filename (file);
				key = book_get_key (storage, preview, filename, intern_filename,
					                checksum);

				if (key > 0)
				{
					book_metadata_append (storage, preview, key);

					success = TRUE;
				}

				g_free (checksum);
			}

			mb_book_preview_free (preview);
		}
		else
		{
			
		}
	}
	COMMIT (storage);

	g_object_unref (file);

	return success;
}

static guint
book_get_key (MbStorage *storage, MbBookPreview *preview, gchar *filename,
              gchar *intern_filename, gchar *checksum)
{
	BookKey book_key;
	GString *sql;
	guint key;

	key = 0;
	sql = g_string_new ("");
	book_key.key = 0;
	book_key.checksum = NULL;

	g_string_append_printf (sql, "SELECT key, checksum FROM books WHERE "
		"filename = '%s';", filename);

	sqlite3_exec (DB (storage), (const char *) sql->str, get_book_callback,
	              &book_key, NULL);
	
	if (book_key.key == 0)
	{
		key = book_data_append (storage, preview, filename, intern_filename,
		                        checksum);
	}
	else if (!EQUALS (book_key.checksum, checksum))
	{
		g_string_append_printf (sql, "DELETE FROM books WHERE filename = '%s';",
		                        filename);

		sqlite3_exec (DB (storage), (const char *) sql->str, NULL, NULL, NULL);

		key = book_data_append (storage, preview, filename, intern_filename,
		                        checksum);
	}
	else
	{
		key = book_key.key;
	}

	g_string_free (sql, TRUE);

	if (book_key.checksum)
	{
		g_free (book_key.checksum);
	}

	return key;
}

static guint
book_data_append (MbStorage *storage, MbBookPreview *preview, gchar *filename,
                  gchar *intern_filename, gchar *checksum)
{
	GString *sql;
	guint key;
	int rc;

	sql = g_string_new ("");
	key = 0;

	/*
	if (preview->cover)
	{
		GdkPixbuf *cover;
		gchar *buffer;
		gsize size;
		
		cover = SCALE_COVER (preview->cover, LIBRARY_COVER_WIDTH);

		if (cover)
		{
			gdk_pixbuf_save_to_buffer (cover, &buffer, &size, "jpeg", NULL,
			                           NULL);

			if (buffer)
			{
			}

			g_object_unref (cover);
		}

		
	}
	*/

	g_string_append_printf (sql, "INSERT INTO books (filename, "
		"intern_filename, id, checksum, title, annotation) VALUES ('%s', ",
	                        filename);

	if (intern_filename)
	{
		g_string_append_printf (sql, "'%s', ", intern_filename);
	}
	else
	{
		g_string_append (sql, "null, ");
	}

	if (preview->id)
	{
		g_string_append_printf (sql, "'%s', ", preview->id);
	}
	else
	{
		g_string_append (sql, "null, ");
	}

	g_string_append_printf (sql, "'%s', '%s', ", checksum, preview->title);

	if (preview->annotation)
	{
		g_string_append_printf (sql, "'%s');", preview->annotation);
	}
	else
	{
		g_string_append (sql, "null);");
	}

	rc = sqlite3_exec (DB (storage), (const char *) sql->str, NULL, NULL, NULL);

	if (rc == SQLITE_OK)
	{
		g_string_printf (sql, "SELECT last_insert_rowid () AS key FROM books;");

		sqlite3_exec (DB (storage), (const char *) sql->str, get_key_callback,
		              &key, NULL);
	}
	
	g_string_free (sql, TRUE);

	return key;
}

static void
book_metadata_append (MbStorage *storage, MbBookPreview *preview, guint key)
{
	MbBookAuthor *author;
	MbBookSequence *sequence;
	guint genre_key;
	guint author_key;
	guint sequence_key;
	GSList *list;

	if (preview->genres)
	{
		for (list = preview->genres; list; list = list->next)
		{
			genre_key = genre_get_key (storage, (gchar *) list->data);

			book_genre_append (storage, key, genre_key);
		}
	}

	if (preview->authors)
	{
		for (list = preview->authors; list; list = list->next)
		{
			author = (MbBookAuthor *) list->data;
			author_key = author_get_key (storage, author);

			book_author_append (storage, key, author_key);
		}
	}

	if (preview->sequences)
	{
		for (list = preview->sequences; list; list = list->next)
		{
			sequence = (MbBookSequence *) list->data;
			sequence_key = sequence_get_key (storage, sequence);

			book_sequence_append (storage, key, sequence_key, sequence->number);
		}
	}
}

static guint
genre_get_key (MbStorage *storage, gchar *name)
{
	GString *sql;
	guint key = 0;

	if (name)
	{
		sql = g_string_new ("");

		g_string_printf (sql, "SELECT key FROM genres WHERE name = '%s';",
		                 name);

		sqlite3_exec (DB (storage), (const char *) sql->str, get_key_callback,
		              &key, NULL);

		g_string_free (sql, TRUE);
	}

	return key;
}

static guint
author_get_key (MbStorage *storage, MbBookAuthor *author)
{
	GString *sql;
	guint key;
	guint sql_length;

	sql_length = 0;
	key = 0;

	if (author)
	{
		sql = g_string_new ("");

		g_string_append (sql, "SELECT key FROM authors WHERE ");
		
		if (author->nick_name)
		{
			g_string_append (sql, "nick_name = '%s';");
		}
		else
		{
			if (author->first_name)
			{
				g_string_append_printf (sql, "first_name = '%s'",
				                        author->first_name);

				sql_length++;
			}

			if (author->middle_name)
			{
				if (sql_length > 0)
				{
					g_string_append (sql, " AND ");
				}
				
				g_string_append_printf (sql, "middle_name = '%s'",
				                        author->middle_name);

				sql_length++;
			}

			if (author->last_name)
			{
				if (sql_length > 0)
				{
					g_string_append (sql, " AND ");
				}
				
				g_string_append_printf (sql, "last_name = '%s'",
				                        author->last_name);
			}

			g_string_append (sql, ";");
		}

		sqlite3_exec (DB (storage), (const char *) sql->str, get_key_callback,
		              &key, NULL);

		if (key == 0)
		{
			key = author_append (storage, author);
		}

		g_string_free (sql, TRUE);
	}

	return key;
}

static guint
author_append (MbStorage *storage, MbBookAuthor *author)
{
	GString *sql;
	guint sql_length;
	guint key;

	sql_length = 0;
	key = 0;

	if (author)
	{
		sql = g_string_new ("");

		g_string_append (sql, "INSERT INTO authors (");
		
		if (author->nick_name)
		{
			g_string_append_printf (sql, "nickname) VALUES ('%s');",
			                        author->nick_name);
		}

		else
		{
			if (author->first_name)
			{
				g_string_append (sql, "first_name");

				sql_length++;
			}

			if (author->middle_name)
			{
				if (sql_length > 0)
				{
					g_string_append (sql, ", ");
				}
				
				g_string_append (sql, "middle_name");

				sql_length++;
			}

			if (author->last_name)
			{
				if (sql_length > 0)
				{
					g_string_append (sql, ", ");
				}
				
				g_string_append (sql, "last_name");
			}

			g_string_append (sql, ") VALUES (");

			sql_length = 0;
			
			if (author->first_name)
			{
				g_string_append_printf (sql, "'%s'", author->first_name);

				sql_length++;
			}

			if (author->middle_name)
			{
				if (sql_length > 0)
				{
					g_string_append (sql, ", ");
				}

				g_string_append_printf (sql, "'%s'", author->middle_name);

				sql_length++;
			}

			if (author->last_name)
			{
				if (sql_length > 0)
				{
					g_string_append (sql, ", ");
				}

				g_string_append_printf (sql, "'%s'", author->last_name);
			}

			g_string_append (sql, ");");
		}

		sqlite3_exec (DB (storage), (const char *) sql->str, NULL, NULL, NULL);

		g_string_printf (sql, "SELECT last_insert_rowid () AS key FROM "
			"authors;");

		sqlite3_exec (DB (storage), (const char *) sql->str, get_key_callback,
		              &key, NULL);

		g_string_free (sql, TRUE);
	}

	return key;
}

static guint
sequence_get_key (MbStorage *storage, MbBookSequence *sequence)
{
	GString *sql;
	guint key = 0;

	if (sequence)
	{
		sql = g_string_new ("");

		g_string_printf (sql, "SELECT key FROM sequences WHERE name = '%s';",
		                 sequence->name);

		sqlite3_exec (DB (storage), (const char *) sql->str, get_key_callback,
		              &key, NULL);

		if (key == 0)
		{
			key = sequence_append (storage, sequence);
		}

		g_string_free (sql, TRUE);
	}

	return key;
}

static guint
sequence_append (MbStorage *storage, MbBookSequence *sequence)
{
	GString *sql;
	gchar *error;
	guint key = 0;
	int rc;

	if (sequence)
	{
		sql = g_string_new ("");
		key = g_str_hash (sequence->name);

		g_string_printf (sql, "INSERT INTO sequences (key, name) VALUES "
			"(%u, '%s');", key, sequence->name);

		rc = sqlite3_exec (DB (storage), (const char *) sql->str, NULL, NULL,
		                   &error);

		if (rc != SQLITE_OK)
		{
			g_warning ("Cannon insert sequence %u:%s\n", key, error);
		}

		sqlite3_free (error);
		g_string_free (sql, TRUE);
	}

	return key;
}

static void
book_genre_append (MbStorage *storage, guint book, guint genre)
{
	GString *sql;

	if (book > 0 && genre > 0)
	{
		sql = g_string_new ("");

		g_string_printf (sql, "INSERT INTO book_genres (book, genre) VALUES "
			"(%u, %u);", book, genre);

		sqlite3_exec (DB (storage), (const char *) sql->str, NULL, NULL, NULL);

		g_string_free (sql, TRUE);
	}
}

static void
book_author_append (MbStorage *storage, guint book, guint author)
{
	GString *sql;

	if (book > 0 && author > 0)
	{
		sql = g_string_new ("");

		g_string_printf (sql, "INSERT INTO book_authors (book, author) VALUES "
			"(%u, %u);", book, author);

		sqlite3_exec (DB (storage), (const char *) sql->str, NULL, NULL, NULL);

		g_string_free (sql, TRUE);
	}
}

static void
book_sequence_append (MbStorage *storage, guint book, guint sequence,
                      guint number)
{
	GString *sql;
	char *error;
	int rc;

	if (book > 0 && sequence > 0)
	{
		sql = g_string_new ("");

		g_string_printf (sql, "INSERT INTO book_sequences (book, sequence, "
			"sequence_number) VALUES (%u, %u, %u);", book, sequence, number);

		rc = sqlite3_exec (DB (storage), (const char *) sql->str, NULL, NULL,
		                   &error);

		if (rc)
		{
			g_warning ("Cannon insert into book_sequences %s. Sequence: %u, book: %u",
			           error, sequence, book);
		}

		sqlite3_free (error);
		g_string_free (sql, TRUE);
	}
}

static int
get_key_callback (void *data, int col_count, char **col_text, char **col_names)
{
	guint *key;

	g_return_val_if_fail (data, 1);

	key = (guint *) data;

	if (EQUALS (col_names[0], "key"))
	{
		*key = GUINT (col_text[0]);

		if (*key > 0)
		{
			return 1;
		}
	}

	return 0;
}

static int
get_book_callback (void *data, int col_count, char **col_text, char **col_names)
{
	BookKey *key;

	g_return_val_if_fail (data, 1);

	key = (BookKey *) data;

	if (EQUALS (col_names[0], "key") && EQUALS (col_names[1], "checksum"))
	{
		key->key = GUINT (col_text[0]);
		key->checksum = g_strdup (col_text[1]);
	}

	return 0;
}

