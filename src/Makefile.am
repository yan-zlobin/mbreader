## Process this file with automake to produce Makefile.in

## Created by Anjuta

AM_CPPFLAGS = \
	-DPACKAGE_LOCALE_DIR=\""$(localedir)"\" \
	-DPACKAGE_SRC_DIR=\""$(srcdir)"\" \
	-DPACKAGE_DATA_DIR=\""$(pkgdatadir)"\" \
	$(MBREADER_CFLAGS)

AM_CFLAGS =\
	 -Wall\
	 -g

bin_PROGRAMS = mbreader

mbreader_SOURCES =  \
	mb-application.c \
	mb-application.h \
	mb-book-buffer.c \
	mb-book-buffer.h \
	mb-book-view.c \
	mb-book-view.h \
	mb-book-view-stack.c \
	mb-book-view-stack.h \
	mb-loader.c \
	mb-loader.h \
	mb-profile.c \
	mb-profile.h \
	mbreader.c \
	mb-window.c \
	mb-window.h \
	preview/mb-book-preview.c \
	preview/mb-book-preview.h \
	preview/mb-preview-widget.c \
	preview/mb-preview-widget.h \
	mb-reference.c \
	mb-reference.h \
	fs/mb-file-system.c \
	fs/mb-file-system.h \
	fs/mb-zip-file.c \
	fs/mb-zip-file.h \
	library/mb-library.c \
	library/mb-library.h \
	library/mb-storage.c \
	library/mb-storage.h \
	fs/mb-file.c \
	fs/mb-file.h \
	types/mb-types.h \
	fb2/mb-fb2-parser.c \
	fb2/mb-fb2-parser.h \
	fb2/mb-fb2-parser-characters.c \
	fb2/mb-fb2-parser-characters.h \
	fb2/mb-fb2-parser-end-element.c \
	fb2/mb-fb2-parser-end-element.h \
	fb2/mb-fb2-parser-start-element.c \
	fb2/mb-fb2-parser-start-element.h \
	epub/mb-epub-loader.c \
	epub/mb-epub-loader.h \
	fb2/mb-fb2-loader.c \
	fb2/mb-fb2-loader.h \
	library/mb-library-view.c \
	library/mb-library-view.h \
	types/mb-marshaller.c \
	types/mb-marshaller.h

mbreader_LDFLAGS = 

mbreader_LDADD = $(MBREADER_LIBS)

