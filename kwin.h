/* gtkmm example Copyright (C) 2002 gtkmm development team
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2
 * as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#pragma once

#include <gtkmm.h>

#define KVSIZE 10

class kbEnt {
public:
    kbEnt();
    virtual ~kbEnt();
    Glib::ustring getK();
    void setK( Glib::ustring s );
    Glib::ustring getV();
    void setV( Glib::ustring s );
protected:
private:
    Glib::ustring kbk;
    Glib::ustring kbv;
};

class KVBase {
public:
    KVBase();
    virtual ~KVBase();
    void setIdx( gint idx );
    void dspIdx( Gtk::Label *lbl );
    void dspIdxC( Gtk::Label *lbl );
    void dspKval( Gtk::Label *lbl );
    void dspKval( Gtk::Entry *ent );
    void dspVval( Gtk::Entry *ent );
    gchar getKoV();
    void setKoV( gchar c );
    void setKval( Glib::ustring s );
    Glib::ustring getVval();
    void setVval( Glib::ustring s );
protected:
private:
    gint nkv;
    kbEnt *ke[KVSIZE];
    gint kidx;
    gchar kov;
    gint encflag;	/* 0plain 1encrypt */
};

class KboxGrid : public Gtk::Grid
{
public:
    KboxGrid();
    virtual ~KboxGrid();
    bool procKey( guint keyval, gint state3 );
    void vEntered();
protected:
    Gtk::Label vTitle;
    Gtk::Entry vEntry;
    Gtk::Label kIndex;
    Gtk::Label kValue;
    Glib::RefPtr<Gtk::Clipboard> m_Clip;
private:
    KVBase kvb;
};

class KboxWin : public Gtk::ApplicationWindow
{
public:
  KboxWin();
  virtual ~KboxWin();

protected:
  //Signal handlers:
  bool on_key_press_event( GdkEventKey *event) override;

  void on_menu_others();

  void on_menu_choices(const Glib::ustring& parameter);
  void on_menu_choices_other(int parameter);
  void on_menu_toggle();

  //Child widgets:
  Gtk::Box m_Box;
  KboxGrid m_Grid;

  Glib::RefPtr<Gtk::Builder> m_refBuilder;

  //Two sets of choices:
  Glib::RefPtr<Gio::SimpleAction> m_refChoice;
  Glib::RefPtr<Gio::SimpleAction> m_refChoiceOther;

  Glib::RefPtr<Gio::SimpleAction> m_refToggle;
};

