/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * mb-window.c
 * Copyright (C) 2014 Yan Zlobin <yan_zlobin@agniyoga.ru>
 * 
 */

#include "mb-window.h"
#include "mb-book-view-stack.h"

#define DEFAULT_WINDOW_TITLE			"Magic Book Reader"

// For 1366x768 width: 600, height: 740
// #define DEFAUL_WINDOW_WIDTH			780
// #define DEFAUL_WINDOW_HEIGHT			860

#define DEFAUL_WINDOW_WIDTH				600
#define DEFAUL_WINDOW_HEIGHT			740
#define DEFAULT_STACK_TOP_MARGIN		32

#define STACK(obj)						(obj)->priv->stack

#define MB_WINDOW_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), \
	MB_TYPE_WINDOW, MbWindowPrivate))

struct _MbWindowPrivate
{
	GtkWidget *window;

	GtkWidget *header_bar;
	
	GtkWidget *info_bar;
	GtkWidget *info_bar_label;

	GtkWidget *tool_bar;
	GtkWidget *menu_button;
	
	MbBookViewStack *stack;

	gboolean hide_header;
	gboolean hide_toolbar;
	
	gboolean is_fullscreen;
	gboolean is_header_visible;
	gboolean is_toolbar_visible;
};

enum
{
	SIGNAL_WINDOW_SHOW,
	SIGNAL_WINDOW_DESTROY,
	SIGNAL_KEY_OPEN,
	SIGNAL_LAST
};

guint mb_window_signals[SIGNAL_LAST] = { 0 };

static void mb_window_finalize (GObject *object);

static void mb_window_create (MbWindow *mb_window, GApplication *app);

static GtkWidget *info_bar_new (MbWindow *mb_window);

static GtkWidget *header_new (MbWindow *mb_window);

static GtkWidget *popover_new (MbWindow *mb_window, GtkWidget *parent);

static void set_background_color (GtkWidget *widget);

static void toolbar_show (MbWindow *mb_window, gboolean show);

static void switch_header (MbWindow *mb_window);

static void switch_toolbar (MbWindow *mb_window);

static void switch_fullscreen (MbWindow *mb_window);

static void window_move (MbWindow *mb_window, gint x, gint y);

static void window_set_title (MbWindow *mb_window, const gchar *title);

static gboolean button_press_callback (GtkWidget *widget, GdkEvent *event,
                                       MbWindow *mb_window);

static gboolean key_release_callback (GtkWidget *widget, GdkEvent *event,
                                      MbWindow *window);

static void window_show_callback (GtkWidget *widget, MbWindow *window);

static void window_destroy_callback (GtkWidget *widget, MbWindow *window);

static void menu_profile_callback (GSimpleAction *action, GVariant *parameter,
                                   gpointer data);

static void menu_quit_callback (GSimpleAction *action, GVariant *parameter,
                                gpointer data);

const GActionEntry menu_items[] =
{
	{ "profiles", menu_profile_callback, NULL, NULL, NULL },
	{ "quit", menu_quit_callback, NULL, NULL, NULL }
};

G_DEFINE_TYPE (MbWindow, mb_window, G_TYPE_OBJECT);

static void
mb_window_init (MbWindow *window)
{
	MbWindowPrivate *priv;

	priv = MB_WINDOW_GET_PRIVATE (window);

	window->priv = priv;

	priv->hide_header = TRUE;
	priv->hide_toolbar = TRUE;

	priv->is_fullscreen = FALSE;
	priv->is_header_visible = TRUE;
	priv->is_toolbar_visible = TRUE;
}

static void
mb_window_class_init (MbWindowClass *klass)
{
	GObjectClass *gobject_class;

	g_type_class_add_private (klass, sizeof (MbWindowPrivate));

	gobject_class = G_OBJECT_CLASS (klass);
	
	mb_window_signals[SIGNAL_WINDOW_SHOW] =
		g_signal_new ("window-show", G_TYPE_FROM_CLASS (klass),
		              G_SIGNAL_RUN_LAST,
		              G_STRUCT_OFFSET (MbWindowClass, window_show),
		              NULL, NULL, g_cclosure_marshal_VOID__VOID,
		              G_TYPE_NONE, 0);

	mb_window_signals[SIGNAL_WINDOW_DESTROY] =
		g_signal_new ("window-destroy", G_TYPE_FROM_CLASS (klass),
		              G_SIGNAL_RUN_LAST,
		              G_STRUCT_OFFSET (MbWindowClass, window_destroy),
		              NULL, NULL, g_cclosure_marshal_VOID__VOID,
		              G_TYPE_NONE, 0);

	mb_window_signals[SIGNAL_KEY_OPEN] =
		g_signal_new ("key-open", G_TYPE_FROM_CLASS (klass),
		              G_SIGNAL_RUN_LAST,
		              G_STRUCT_OFFSET (MbWindowClass, key_open),
		              NULL, NULL, g_cclosure_marshal_VOID__VOID,
		              G_TYPE_NONE, 0);
	
	gobject_class->finalize = mb_window_finalize;
}

static void
mb_window_finalize (GObject *object)
{
	G_OBJECT_CLASS (mb_window_parent_class)->finalize (object);
}

MbWindow *
mb_window_new (GApplication *application)
{
	MbWindow *window;

	window = g_object_new (MB_TYPE_WINDOW, NULL);

	mb_window_create (window, application);

	return window;
}

void
mb_window_set_book_buffer (MbWindow *mb_window, gpointer buffer)
{
	MbWindowPrivate *priv;
	gchar *title;

	priv = mb_window->priv;

	mb_book_view_stack_set_buffer (priv->stack, buffer);

	title = mb_book_view_stack_get_title (priv->stack);

	window_set_title (mb_window, title);
}

void
mb_window_show (MbWindow *mb_window)
{
	MbWindowPrivate *priv;

	priv = mb_window->priv;

	gtk_widget_show_all (priv->window);

	gtk_header_bar_set_has_subtitle (GTK_HEADER_BAR (priv->header_bar), FALSE);
	gtk_widget_queue_resize (GTK_WIDGET (priv->header_bar));
}

GtkWidget *
mb_window_get_widget (MbWindow *mb_window)
{
	return mb_window->priv->window;
}

void
mb_window_hide_titlebar (MbWindow *mb_window)
{
	MbWindowPrivate *priv;
	GtkWidget *window;
	GdkWindow *gdk_window;

	priv = mb_window->priv;
	window = priv->window;
	gdk_window = gtk_widget_get_window (window);

	gdk_window_set_decorations (gdk_window, GDK_DECOR_BORDER);
}

void
mb_window_show_titlebar (MbWindow *mb_window)
{
	MbWindowPrivate *priv;
	GtkWidget *window;
	GdkWindow *gdk_window;

	priv = mb_window->priv;
	window = priv->window;
	gdk_window = gtk_widget_get_window (window);

	gdk_window_set_decorations (gdk_window, GDK_DECOR_ALL);
}

void
mb_window_show_error (MbWindow *mb_window, gchar *error_text)
{
	MbWindowPrivate *priv;

	GtkWidget *bar;
	GtkWidget *label;

	priv = mb_window->priv;
	bar = priv->info_bar;
	label = priv->info_bar_label;

	gtk_label_set_text (GTK_LABEL (label), error_text);
	gtk_info_bar_set_message_type (GTK_INFO_BAR (bar), GTK_MESSAGE_WARNING);
	gtk_widget_show (bar);
}

static void
mb_window_create (MbWindow *mb_window, GApplication *application)
{
	MbWindowPrivate *priv;

	GtkWidget *grid;
	GtkWidget *bar;
	GtkWidget *header;

	priv = mb_window->priv;

	priv->window = gtk_application_window_new (GTK_APPLICATION (application));

	gtk_window_set_icon_name (GTK_WINDOW (priv->window), "mbreader");
	gtk_window_set_default_size ((GtkWindow*) priv->window, DEFAUL_WINDOW_WIDTH,
	                             DEFAUL_WINDOW_HEIGHT);

	priv->header_bar = gtk_header_bar_new ();

	g_object_set (priv->header_bar,
	              "has-subtitle", FALSE,
	              "show-close-button", TRUE,
	              "spacing", 0,
	              "title", DEFAULT_WINDOW_TITLE,
	              "subtitle", NULL,
	              NULL);

	gtk_header_bar_set_has_subtitle (GTK_HEADER_BAR (priv->header_bar), FALSE);

	grid = gtk_grid_new ();
	set_background_color (grid);

	bar = info_bar_new (mb_window);
	header = header_new (mb_window);
	priv->stack = mb_book_view_stack_new ();

	gtk_grid_attach (GTK_GRID (grid), bar, 0, 0, 1, 1);
	gtk_grid_attach (GTK_GRID (grid), header, 0, 1, 1, 1);
	gtk_grid_attach (GTK_GRID (grid), STACK_WIDGET (priv->stack), 0, 2, 1, 1);

	gtk_container_add (GTK_CONTAINER (priv->window), grid);

	g_signal_connect (priv->window, "button-press-event",
	                  G_CALLBACK (button_press_callback), mb_window);

	g_signal_connect (priv->window, "key-release-event",
	                  G_CALLBACK (key_release_callback), mb_window);

	g_signal_connect (bar, "response", G_CALLBACK (gtk_widget_hide), NULL);
	
	g_signal_connect (priv->window, "show", G_CALLBACK (window_show_callback),
	                  mb_window);

	g_signal_connect (priv->window, "destroy",
	                  G_CALLBACK (window_destroy_callback), mb_window);
}

static GtkWidget *
info_bar_new (MbWindow *mb_window)
{
	GtkWidget *bar;
	GtkWidget *content_area;
	GtkWidget *message_label;
	MbWindowPrivate *priv;

	priv = mb_window->priv;

	bar = gtk_info_bar_new ();
	priv->info_bar = bar;

	gtk_widget_set_no_show_all (bar, TRUE);

	content_area = gtk_info_bar_get_content_area (GTK_INFO_BAR (bar));

	message_label = gtk_label_new ("");
	gtk_label_set_ellipsize (GTK_LABEL (message_label), PANGO_ELLIPSIZE_MIDDLE);
	priv->info_bar_label = message_label;

	gtk_widget_show (message_label);

	gtk_container_add (GTK_CONTAINER (content_area), message_label);
	gtk_info_bar_add_button (GTK_INFO_BAR (bar), "Закрыть", GTK_RESPONSE_OK);

	return bar;
}

static GtkWidget *
header_new (MbWindow *mb_window)
{
	MbWindowPrivate *priv;
	GtkWidget *header_grid;
	GtkWidget *library_button;
	GtkWidget *contents_button;
	GtkWidget *search_button;
	GtkWidget *bookmark_button;
	GtkWidget *menu_button;
	GtkWidget *menu_image;
	GtkWidget *popover;

	priv = mb_window->priv;

	header_grid = gtk_grid_new ();
	priv->tool_bar = header_grid;

	library_button = gtk_button_new_from_icon_name ("go-home-symbolic",
	                                                GTK_ICON_SIZE_MENU);

	g_object_set (G_OBJECT (library_button),
	              "halign", GTK_ALIGN_START,
	              "valign", GTK_ALIGN_CENTER,
	              "relief", GTK_RELIEF_NONE,
	              "can-focus", FALSE,
	              "always-show-image", TRUE,
	              "tooltip-text", "Библиотека",
	              "border-width", 8,
	              NULL);

	contents_button = gtk_button_new_from_icon_name ("view-list-symbolic",
	                                                 GTK_ICON_SIZE_MENU);

	g_object_set (G_OBJECT (contents_button),
	              "halign", GTK_ALIGN_START,
	              "valign", GTK_ALIGN_CENTER,
	              "relief", GTK_RELIEF_NONE,
	              "can-focus", FALSE,
	              "always-show-image", TRUE,
	              "tooltip-text", "Оглавление",
	              "border-width", 2,
	              NULL);

	search_button = gtk_button_new_from_icon_name ("edit-find-symbolic",
	                                               GTK_ICON_SIZE_MENU);

	g_object_set (G_OBJECT (search_button),
	              "hexpand", TRUE,
	              "halign", GTK_ALIGN_END,
	              "valign", GTK_ALIGN_CENTER,
	              "relief", GTK_RELIEF_NONE,
	              "can-focus", FALSE,
	              "always-show-image", TRUE,
	              "tooltip-text", "Найти в тексте",
	              "border-width", 10,
	              NULL);

	bookmark_button = gtk_button_new_from_icon_name ("bookmark-new-symbolic",
	                                                 GTK_ICON_SIZE_MENU);

	g_object_set (G_OBJECT (bookmark_button),
	              "halign", GTK_ALIGN_END,
	              "valign", GTK_ALIGN_CENTER,
	              "relief", GTK_RELIEF_NONE,
	              "can-focus", FALSE,
	              "always-show-image", TRUE,
	              "tooltip-text", "Сделать закладку",
	              "border-width", 2,
	              NULL);

	menu_button = gtk_menu_button_new ();
	priv->menu_button = menu_button;

	gtk_widget_set_hexpand (menu_button, FALSE);
	gtk_widget_set_halign (menu_button, GTK_ALIGN_END);
	gtk_menu_button_set_direction (GTK_MENU_BUTTON (menu_button), GTK_ARROW_DOWN);

	menu_image = gtk_image_new_from_icon_name ("emblem-system-symbolic",
	                                           GTK_ICON_SIZE_MENU);

	popover = popover_new (mb_window, NULL);

	g_object_set (G_OBJECT (menu_button),
	              "margin", 8,
	              "halign", GTK_ALIGN_END,
	              "valign", GTK_ALIGN_CENTER,
	              "relief", GTK_RELIEF_NONE,
	              "can-focus", FALSE,
	              "always-show-image", TRUE,
	              "popover", popover,
	              "image", menu_image,
	              "tooltip-text", "Настройки...",
	              "border-width", 0,
	              NULL);

	gtk_grid_attach (GTK_GRID (header_grid), library_button, 0, 0, 1, 1);
	gtk_grid_attach (GTK_GRID (header_grid), contents_button, 1, 0, 1, 1);
	gtk_grid_attach (GTK_GRID (header_grid), search_button, 2, 0, 1, 1);
	gtk_grid_attach (GTK_GRID (header_grid), bookmark_button, 3, 0, 1, 1);
	gtk_grid_attach (GTK_GRID (header_grid), menu_button, 4, 0, 1, 1);

	gtk_menu_button_set_align_widget (GTK_MENU_BUTTON (menu_button),
	                                  header_grid);

	return header_grid;
}

static GtkWidget *
popover_new (MbWindow *mb_window, GtkWidget *parent)
{
	GtkWidget *popover;
	GMenu *menu;
	GMenu *profile_section;
	GMenu *quit_section;
	GSimpleActionGroup *action_group;

	menu = g_menu_new ();
	profile_section = g_menu_new ();
	quit_section = g_menu_new ();

	g_menu_append (profile_section, "Электронная книга", "prop.profiles");
	g_menu_append (profile_section, "Бумажная книга", "prop.profiles");
	g_menu_append (profile_section, "Старая книга", "prop.profiles");
	g_menu_append (quit_section, "Закрыть", "prop.quit");

	g_menu_insert_submenu (menu, 0, "Профили", G_MENU_MODEL (profile_section));
	g_menu_append_section (menu, NULL, G_MENU_MODEL (quit_section));

	action_group = g_simple_action_group_new ();

	g_action_map_add_action_entries (G_ACTION_MAP (action_group), menu_items,
	                                 G_N_ELEMENTS (menu_items), mb_window);
	
	popover = gtk_popover_new (parent);

	g_object_set (G_OBJECT (popover),
	              "position", GTK_POS_TOP,
	              "can-focus", FALSE,
	              NULL);

	gtk_widget_insert_action_group (popover, "prop",
	                                G_ACTION_GROUP (action_group));


	GMenu *new_profile_section;
	GSimpleAction *aditional_action;
	
	aditional_action = g_simple_action_new_stateful ("additional",
	                                                 NULL,
	                                                 g_variant_new_boolean (TRUE));
	
	g_action_change_state (G_ACTION (aditional_action),
	                       g_variant_new_boolean (FALSE));
	
	g_action_map_add_action (G_ACTION_MAP (action_group),
	                         G_ACTION (aditional_action));

	new_profile_section = g_menu_new ();

	g_menu_append (new_profile_section, "Создать новый профиль...",
	               "prop.additional");

	g_menu_append_section (profile_section, NULL,
	                       G_MENU_MODEL (new_profile_section));


	
	GtkWidget *scroll_switch;
	GtkWidget *new_stack;

	scroll_switch = gtk_search_entry_new ();
	gtk_widget_set_size_request (scroll_switch, 300, -1);

	new_stack = gtk_stack_new ();
	
	gtk_stack_add_named (GTK_STACK (new_stack), scroll_switch, "main");

	gtk_container_add (GTK_CONTAINER (popover), new_stack);

	gtk_widget_show (new_stack);
	gtk_widget_show (scroll_switch);
	
	return popover;
}

static void
set_background_color (GtkWidget *widget)
{
	GtkStyleContext *context;
	GtkCssProvider *provider;

	context = gtk_widget_get_style_context (widget);
	provider = gtk_css_provider_new ();

	gtk_css_provider_load_from_data (GTK_CSS_PROVIDER (provider),
	                                 " GtkWidget {\n"
									 "   background-color: #FFFFFF;\n"
									 "}\n", -1, NULL);

	gtk_style_context_add_provider (context, GTK_STYLE_PROVIDER (provider),
	                                GTK_STYLE_PROVIDER_PRIORITY_USER);

	g_object_unref (provider);
}

static void
toolbar_show (MbWindow *mb_window, gboolean show)
{
	MbWindowPrivate *priv;

	priv = mb_window->priv;

	if (show)
	{
		gtk_widget_show (priv->tool_bar);

		gtk_widget_set_margin_top (STACK_WIDGET (priv->stack), 0);
	}
	else
	{
		gtk_widget_hide (priv->tool_bar);

		gtk_widget_set_margin_top (STACK_WIDGET (priv->stack),
		                           DEFAULT_STACK_TOP_MARGIN);
	}
}

static void
switch_header (MbWindow *mb_window)
{
	MbWindowPrivate *priv;

	priv = mb_window->priv;

	if (priv->is_fullscreen)
	{
		return;
	}
	
	GdkWindow *window;

	window = gtk_widget_get_window (priv->window);

	if (priv->hide_header)
	{
		if (priv->is_header_visible)
		{
			gdk_window_set_decorations (window, GDK_DECOR_BORDER);
		}
		else
		{
			gdk_window_set_decorations (window, GDK_DECOR_ALL);
		}

		priv->is_header_visible = !priv->is_header_visible;
	}
}

static void
switch_toolbar (MbWindow *mb_window)
{
	MbWindowPrivate *priv;

	priv = mb_window->priv;

	if (priv->hide_toolbar && priv->is_toolbar_visible)
	{
		toolbar_show (mb_window, FALSE);
	}
	else
	{
		toolbar_show (mb_window, TRUE);
	}

	priv->is_toolbar_visible = !priv->is_toolbar_visible;
}

static void
switch_fullscreen (MbWindow *mb_window)
{
	MbWindowPrivate *priv;
	
	priv = mb_window->priv;

	if (priv->is_fullscreen)
	{
		gtk_window_unfullscreen (GTK_WINDOW (priv->window));
	}
	else
	{
		gtk_window_fullscreen (GTK_WINDOW (priv->window));
	}
	
	priv->is_fullscreen = !priv->is_fullscreen;
}

static void
window_move (MbWindow *mb_window, gint x, gint y)
{
	MbWindowPrivate *priv;
	
	priv = mb_window->priv;

	if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (priv->menu_button)))
	{
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (priv->menu_button),
		                              FALSE);
	}
	
	if (!priv->is_header_visible && !priv->is_fullscreen)
	{
		gtk_window_begin_move_drag (GTK_WINDOW (priv->window), 1, x, y, 0);
	}
}

static void
window_set_title (MbWindow *mb_window, const gchar *title)
{
	gtk_header_bar_set_title (GTK_HEADER_BAR (mb_window->priv->header_bar),
	                          title);
}

static gboolean
button_press_callback (GtkWidget *widget, GdkEvent *event, MbWindow *mb_window)
{
	switch (event->type)
	{
		case GDK_BUTTON_PRESS:
		{
			window_move (mb_window, (gint) event->motion.x_root,
			             (gint) event->motion.y_root);

			break;
		}
		case GDK_2BUTTON_PRESS:
		{
			switch_header (mb_window);
			switch_toolbar (mb_window);
			
			break;
		}
		default:
		{
			break;
		}
	}

	return TRUE;
}

static gboolean
key_release_callback (GtkWidget *widget, GdkEvent *event, MbWindow *window)
{
	switch (event->key.keyval)
	{
		case GDK_KEY_space:
		{
			if (event->key.state & GDK_SHIFT_MASK)
			{
				mb_book_view_stack_prev_page (STACK (window));
			}
			else
			{
				mb_book_view_stack_next_page (STACK (window));
			}

			break;
		}
		case GDK_KEY_Page_Down:
		{
			mb_book_view_stack_next_page (STACK (window));
			
			break;
		}
		case GDK_KEY_Page_Up:
		{
			mb_book_view_stack_prev_page (STACK (window));
			
			break;
		}
		case GDK_KEY_Right:
		{
			mb_book_view_stack_next_page (STACK (window));
			
			break;
		}
		/*
		case GDK_KEY_Down:
		{
			mb_book_view_stack_next_page (STACK (window));
			
			break;
		}
		case GDK_KEY_Up:
		{
			mb_book_view_stack_prev_page (STACK (window));
			
			break;
		}
		*/
		case GDK_KEY_Left:
		{
			mb_book_view_stack_prev_page (STACK (window));
			
			break;
		}
		case GDK_KEY_F11:
		{
			switch_fullscreen (window);
			
			break;
		}
		case GDK_KEY_O:
		{
		}
		case GDK_KEY_o:
		{
			if (event->key.state & GDK_CONTROL_MASK)
			{
				g_signal_emit (window, mb_window_signals[SIGNAL_KEY_OPEN], 0);
			}
			
		}
	}

	return FALSE;
}

static void
window_show_callback (GtkWidget *widget, MbWindow *window)
{
	g_signal_emit (window, mb_window_signals[SIGNAL_WINDOW_SHOW], 0);
}

static void
window_destroy_callback (GtkWidget *widget, MbWindow *window)
{
	g_signal_emit (window, mb_window_signals[SIGNAL_WINDOW_DESTROY], 0);
}


static void
menu_profile_callback (GSimpleAction *action, GVariant *parameter, gpointer data)
{

}

static void
menu_quit_callback (GSimpleAction *action, GVariant *parameter, gpointer data)
{

}

