/*  Copyright 2006 P.F. Chimento
 *  This file is part of GNOME Inform 7.
 * 
 *  GNOME Inform 7 is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  GNOME Inform 7 is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with GNOME Inform 7; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */
 
#include <gnome.h>
#include <string.h>
#include <gtksourceview/gtksourceiter.h>

#include "support.h"
#include "findreplace.h"
#include "story.h"
#include "extension.h"

/* Scroll the text view so that the cursor is within the inner 25%-75%, if
possible. */
void scroll_text_view_to_cursor(GtkTextView *view) {
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(view);
    gtk_text_view_scroll_to_mark(view,
        gtk_text_buffer_get_insert(buffer),
        0.25, FALSE, 0.0, 0.0);
}

void
on_find_text_changed                   (GtkEditable     *editable,
                                        gpointer         user_data)
{
    const gchar *text = gtk_entry_get_text(GTK_ENTRY(editable));
    /* Do not free or modify the strings from gtk_entry_get_text */
    gtk_widget_set_sensitive(
      lookup_widget(GTK_WIDGET(editable), "find_find"),
      !(text == NULL || strlen(text) == 0));
}


void
on_find_close_clicked                  (GtkButton       *button,
                                        gpointer         user_data)
{
    gtk_widget_destroy(gtk_widget_get_toplevel(GTK_WIDGET(button)));
}


/* Callback for when "Find" is clicked from a Find dialog that was started from
an Inform 7 project */
void
on_find_find_clicked                   (GtkButton       *button,
                                        gpointer         user_data)
{
    GtkWidget *thiswidget = GTK_WIDGET(button);
    find(GTK_TEXT_BUFFER(((struct story *)user_data)->buffer),
      gtk_entry_get_text(GTK_ENTRY(lookup_widget(thiswidget, "find_text"))),
      gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(lookup_widget(thiswidget,
      "find_match_case"))),
      gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(lookup_widget(thiswidget,
      "find_reverse"))));
      /* Do not free or modify the strings from gtk_entry_get_text */
    scroll_text_view_to_cursor(
      GTK_TEXT_VIEW(lookup_widget(((struct story *)user_data)->window,
      "source_l")));
    scroll_text_view_to_cursor(
      GTK_TEXT_VIEW(lookup_widget(((struct story *)user_data)->window,
      "source_r")));
}

/* Callback for when "Find" is clicked from a Find dialog that was started from
an extension */
void
on_xfind_find_clicked                   (GtkButton       *button,
                                        gpointer         user_data)
{
    GtkWidget *thiswidget = GTK_WIDGET(button);
    find(GTK_TEXT_BUFFER(((struct extension *)user_data)->buffer),
      gtk_entry_get_text(GTK_ENTRY(lookup_widget(thiswidget, "find_text"))),
      gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(lookup_widget(thiswidget,
      "find_match_case"))),
      gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(lookup_widget(thiswidget,
      "find_reverse"))));
      /* Do not free or modify the strings from gtk_entry_get_text */
    scroll_text_view_to_cursor(
      GTK_TEXT_VIEW(lookup_widget(((struct extension *)user_data)->window,
      "ext_code")));
}

void
on_replace_find_text_changed           (GtkEditable     *editable,
                                        gpointer         user_data)
{
    /* Do not free or modify the strings from gtk_entry_get_text */
    const gchar *text = gtk_entry_get_text(GTK_ENTRY(editable));
    gboolean text_not_empty = !(text == NULL || strlen(text) == 0);
    gtk_widget_set_sensitive(
      lookup_widget(GTK_WIDGET(editable), "replace_find"), text_not_empty);
    gtk_widget_set_sensitive(
      lookup_widget(GTK_WIDGET(editable), "replace_replace"), text_not_empty);
    gtk_widget_set_sensitive(
      lookup_widget(GTK_WIDGET(editable), "replace_replace_all"),
      text_not_empty);
}


void
on_replace_close_clicked              (GtkButton       *button,
                                        gpointer         user_data)
{
    gtk_widget_destroy(gtk_widget_get_toplevel(GTK_WIDGET(button)));
}

/* Callback for when "Replace All" is clicked from a Replace dialog that was
started from an I7 project */
void
on_replace_replace_all_clicked        (GtkButton       *button,
                                        gpointer         user_data)
{
    GtkWidget *thiswidget = GTK_WIDGET(button);
    int replace_count = replace_all(
      GTK_TEXT_BUFFER(((struct story *)user_data)->buffer),
      gtk_entry_get_text(GTK_ENTRY(lookup_widget(thiswidget, 
        "replace_find_text"))),
      gtk_entry_get_text(GTK_ENTRY(lookup_widget(thiswidget,
        "replace_replace_text"))),
      gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(lookup_widget(thiswidget,
        "replace_match_case"))));
      /* Do not free or modify the strings from gtk_entry_get_text */

    gtk_widget_destroy(gtk_widget_get_toplevel(thiswidget));

    GtkWidget *dialog = gtk_message_dialog_new(
      GTK_WINDOW(((struct story *)user_data)->window),
      GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_INFO, GTK_BUTTONS_OK,
      "%d occurences replaced.", replace_count);
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

/* Callback for when "Replace All" is clicked from a Replace dialog that was
started from an extension */
void
on_xreplace_replace_all_clicked        (GtkButton       *button,
                                        gpointer         user_data)
{
    GtkWidget *thiswidget = GTK_WIDGET(button);
    int replace_count = replace_all(
      GTK_TEXT_BUFFER(((struct extension *)user_data)->buffer),
      gtk_entry_get_text(GTK_ENTRY(lookup_widget(thiswidget,
        "replace_find_text"))),
      gtk_entry_get_text(GTK_ENTRY(lookup_widget(thiswidget,
        "replace_replace_text"))),
      gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(lookup_widget(thiswidget,
        "replace_match_case"))));
      /* Do not free or modify the strings from gtk_entry_get_text */

    gtk_widget_destroy(gtk_widget_get_toplevel(thiswidget));

    GtkWidget *dialog = gtk_message_dialog_new(
      GTK_WINDOW(((struct extension *)user_data)->window),
      GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_INFO, GTK_BUTTONS_OK,
      "%d occurences replaced.", replace_count);
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

/* Callback for when "Replace" is clicked from a Replace dialog that was started
from an I7 project */
void
on_replace_replace_clicked            (GtkButton       *button,
                                        gpointer         user_data)
{
    GtkWidget *thiswidget = GTK_WIDGET(button);
    replace(GTK_TEXT_BUFFER(((struct story *)user_data)->buffer),
      gtk_entry_get_text(GTK_ENTRY(lookup_widget(thiswidget,
      "replace_find_text"))),
      gtk_entry_get_text(GTK_ENTRY(lookup_widget(thiswidget,
      "replace_replace_text"))),
      gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(lookup_widget(thiswidget,
      "replace_match_case"))),
      gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(lookup_widget(thiswidget,
      "replace_reverse"))));
    scroll_text_view_to_cursor(
      GTK_TEXT_VIEW(lookup_widget(((struct story *)user_data)->window,
      "source_l")));
    scroll_text_view_to_cursor(
      GTK_TEXT_VIEW(lookup_widget(((struct story *)user_data)->window,
      "source_r")));
}

/* Callback for when "Replace" is clicked from a Replace dialog that was started
from an extension */
void
on_xreplace_replace_clicked            (GtkButton       *button,
                                        gpointer         user_data)
{
    GtkWidget *thiswidget = GTK_WIDGET(button);
    replace(GTK_TEXT_BUFFER(((struct extension *)user_data)->buffer),
      gtk_entry_get_text(GTK_ENTRY(lookup_widget(thiswidget,
      "replace_find_text"))),
      gtk_entry_get_text(GTK_ENTRY(lookup_widget(thiswidget,
      "replace_replace_text"))),
      gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(lookup_widget(thiswidget,
      "replace_match_case"))),
      gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(lookup_widget(thiswidget,
      "replace_reverse"))));
    scroll_text_view_to_cursor(
      GTK_TEXT_VIEW(lookup_widget(((struct extension *)user_data)->window,
      "ext_code")));
}

/* Callback for when "Find" is clicked from a Replace dialog that was started
from an I7 project */
void
on_replace_find_clicked               (GtkButton       *button,
                                        gpointer         user_data)
{
    GtkWidget *thiswidget = GTK_WIDGET(button);
    find(GTK_TEXT_BUFFER(((struct story *)user_data)->buffer),
      gtk_entry_get_text(GTK_ENTRY(lookup_widget(thiswidget,
      "replace_find_text"))),
      gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(lookup_widget(thiswidget,
      "replace_match_case"))),
      gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(lookup_widget(thiswidget,
      "replace_reverse"))));
    scroll_text_view_to_cursor(
      GTK_TEXT_VIEW(lookup_widget(((struct story *)user_data)->window,
      "source_l")));
    scroll_text_view_to_cursor(
      GTK_TEXT_VIEW(lookup_widget(((struct story *)user_data)->window,
      "source_r")));
}

/* Callback for when "Find" is clicked from a Replace dialog that was started
from an extension */
void
on_xreplace_find_clicked               (GtkButton       *button,
                                        gpointer         user_data)
{
    GtkWidget *thiswidget = GTK_WIDGET(button);
    find(GTK_TEXT_BUFFER(((struct extension *)user_data)->buffer),
      gtk_entry_get_text(GTK_ENTRY(lookup_widget(thiswidget,
      "replace_find_text"))),
      gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(lookup_widget(thiswidget,
      "replace_match_case"))),
      gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(lookup_widget(thiswidget,
      "replace_reverse"))));
    scroll_text_view_to_cursor(
      GTK_TEXT_VIEW(lookup_widget(((struct extension *)user_data)->window,
      "ext_code")));
}

void find(GtkTextBuffer *buffer, const gchar *text, gboolean match_case,
  gboolean reverse) {
    GtkTextIter cursor, match_start, match_end;

    /* If there is a selection, start searching past it; otherwise start
    searching from the cursor. Likewise, if searching backwards, start before
    the selection. */
    gtk_text_buffer_get_iter_at_mark(buffer, &cursor, reverse?
      gtk_text_buffer_get_insert(buffer)
      : gtk_text_buffer_get_selection_bound(buffer));

    if(find_next(text, &cursor, &match_start, &match_end, match_case,
      reverse)) {
        gtk_text_buffer_select_range(buffer, &match_start, &match_end);
        return;
    } else {
        /* Here it gets a little convoluted, because we don't want to keep
        asking if they want to wrap around a million times. */
        GtkWidget *dialog = gtk_message_dialog_new(NULL, 0,
          GTK_MESSAGE_QUESTION, GTK_BUTTONS_YES_NO, "Search text not found. "
          "Do you want to continue searching%s from the %s of the document?",
          reverse? " backwards" : "", reverse? "end" : "beginning");
        gint result = gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        if(result == GTK_RESPONSE_YES) {
            /* Wrap the search around to the beginning/end */
            if(reverse)
                gtk_text_buffer_get_end_iter(buffer, &cursor);
            else
                gtk_text_buffer_get_start_iter(buffer, &cursor);
            if(find_next(text, &cursor, &match_start, &match_end, match_case,
              reverse)) {
                gtk_text_buffer_select_range(buffer, &match_start, &match_end);
                return;
            } else {
                dialog = gtk_message_dialog_new(
                  NULL, 0, GTK_MESSAGE_INFO, GTK_BUTTONS_OK,
                  "Search text not found.");
                gtk_dialog_run(GTK_DIALOG(dialog));
                gtk_widget_destroy(dialog);
                return;
            }
        }
    }
}

/* Searches for the next (or previous) occurrence of the string search_text
and puts it in the text iterators match_start and match_end */
gboolean find_next(const gchar *search_text, GtkTextIter *search_from,
  GtkTextIter *match_start, GtkTextIter *match_end, gboolean match_case,
  gboolean reverse) {
    if(reverse)
        return gtk_source_iter_backward_search(search_from, search_text,
          GTK_SOURCE_SEARCH_VISIBLE_ONLY | GTK_SOURCE_SEARCH_TEXT_ONLY |
          (match_case? 0 : GTK_SOURCE_SEARCH_CASE_INSENSITIVE),
          match_start, match_end, NULL);
    else
        return gtk_source_iter_forward_search(search_from, search_text,
          GTK_SOURCE_SEARCH_VISIBLE_ONLY | GTK_SOURCE_SEARCH_TEXT_ONLY |
          (match_case? 0 : GTK_SOURCE_SEARCH_CASE_INSENSITIVE),
          match_start, match_end, NULL);
}

int replace_all(GtkTextBuffer *buffer, const gchar *search_text,
const gchar *replace_text, gboolean match_case) {
    GtkTextIter cursor, match_start, match_end;
    
    /* Replace All counts as one action for Undo */
    gtk_text_buffer_begin_user_action(buffer);

    gtk_text_buffer_get_start_iter(buffer, &cursor);
    int replace_count = 0;

    while(find_next(search_text, &cursor, &match_start, &match_end, match_case,
      FALSE)) {
        gtk_text_buffer_delete(buffer, &match_start, &match_end);
        gtk_text_buffer_insert(buffer, &match_start, replace_text, -1);
        gtk_text_buffer_get_start_iter(buffer, &cursor);
        replace_count++;
    }

    gtk_text_buffer_end_user_action(buffer);
    
    return replace_count;
}

void replace(GtkTextBuffer *buffer, const gchar *search_text,
const gchar *replace_text, gboolean match_case, gboolean reverse) {
    GtkTextIter insert, selection_bound;

    gtk_text_buffer_get_selection_bounds(buffer, &insert, &selection_bound);
    gchar *selected = gtk_text_buffer_get_text(buffer, &insert,
      &selection_bound, FALSE);

    if(match_case? strcmp(selected, search_text)
      : strcasecmp(selected, search_text)) {
        /* if the text is NOT selected, "find" again to select the text */
        find(buffer, search_text, match_case, reverse);
        g_free(selected);
        return; /* do nothing, wait for the user to click replace again */
    }

    /* Replacing counts as one action for Undo */
    gtk_text_buffer_begin_user_action(buffer);
    gtk_text_buffer_delete(buffer, &insert, &selection_bound);
    gtk_text_buffer_insert(buffer, &insert, replace_text, -1);
    gtk_text_buffer_end_user_action(buffer);

    /* Find the next occurrence of the text */
    find(buffer, search_text, match_case, reverse);
    g_free(selected);
}
