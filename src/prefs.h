/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * SPDX-FileCopyrightText: 2006-2011, 2013 Philip Chimento <philip.chimento@gmail.com>
 */

#ifndef PREFS_H
#define PREFS_H

#include "config.h"

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>
#include <gtksourceview/gtksource.h>

typedef struct {
	/* Global pointer to preferences window */
	GtkWidget *window;
	/* Global pointers to various widgets */
	GtkWidget *prefs_notebook;
	GtkTreeView *schemes_view;
	GtkWidget *style_remove;
	GtkSourceView *tab_example;
	GtkSourceView *source_example;
	GtkWidget *auto_number;
	GtkWidget *clean_index_files;
	GtkListStore *schemes_list;
} I7PrefsWidgets;

I7PrefsWidgets *create_prefs_window(GSettings *prefs, GtkBuilder *builder);
void populate_schemes_list(GtkListStore *list);

gboolean update_style(GtkSourceBuffer *buffer);
gboolean update_tabs(GtkSourceView *view);
void select_style_scheme(GtkTreeView *view, const gchar *id);

#endif
