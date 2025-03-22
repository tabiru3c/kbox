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

#include "kwin.h"
#include <iostream>

Glib::ustring kbEnt::getK()
{
  return kbk;
}

void kbEnt::setK( Glib::ustring s )
{
  kbk = s;
}

Glib::ustring kbEnt::getV()
{
  return kbv;
}

void kbEnt::setV( Glib::ustring s )
{
  kbv = s;
}

kbEnt::kbEnt()
{
  kbk = "key";
  kbv = "value";
}

kbEnt::~kbEnt()
{
}

void KVBase::setKval( Glib::ustring s )
{
  ke[kidx]->setK(s);
}

Glib::ustring KVBase::getVval()
{
  return ke[kidx]->getV();
}

void KVBase::setVval( Glib::ustring s )
{
  ke[kidx]->setV(s);
}

gchar KVBase::getKoV()
{
  return kov;
}

void KVBase::setKoV( gchar c )
{
  kov = c;
}

void KVBase::setIdx( gint idx )
{
  kidx = idx;
}

void KVBase::dspIdx( Gtk::Label *lbl )
{
  lbl->set_text(Glib::ustring::sprintf("%d", kidx));
}

void KVBase::dspIdxC( Gtk::Label *lbl )
{
  lbl->set_text(Glib::ustring::sprintf("%dc", kidx));
}

void KVBase::dspKval( Gtk::Label *lbl )
{
  lbl->set_text(ke[kidx]->getK());
}

void KVBase::dspKval( Gtk::Entry *ent )
{
  ent->set_text(ke[kidx]->getK());
}

void KVBase::dspVval( Gtk::Entry *ent )
{
  ent->set_text(ke[kidx]->getV());
}

KVBase::KVBase()
{
  nkv = KVSIZE;
  for (gint i=0; i<nkv; i++){
    ke[i] = new kbEnt();
  }
  kidx = 0;
  kov = '-';
  encflag = 1;
}

KVBase::~KVBase()
{
}

bool KboxGrid::procKey( guint keyval, gint state3 )
{
  gchar kov = kvb.getKoV();
  if (kov == '-'){
    if ((keyval >= GDK_KEY_0) && (keyval <= GDK_KEY_9)){
      gint idx = keyval - GDK_KEY_0;
      kvb.setIdx(idx);
      kvb.dspIdx(&kIndex);
      kvb.dspKval(&kValue);
      return true;
    }
    if (keyval == GDK_KEY_c){
      m_Clip->set_text(kvb.getVval());
      kvb.dspIdxC(&kIndex);
      return true;
    }
    if ((keyval == GDK_KEY_k) || (keyval == GDK_KEY_v)){
      kov = keyval & 0x7f;
      kvb.setKoV(kov);
      if (kov == 'k'){
        vEntry.set_visibility(true);
        kvb.dspKval(&vEntry);
      } else {
        kvb.dspVval(&vEntry);
      }
      vEntry.grab_focus();
      return true;
    }
  }
  return false;
}

void KboxGrid::vEntered()
{
  Glib::ustring s = vEntry.get_text();
  gchar kov = kvb.getKoV();
  if (kov == 'k'){
    kvb.setKval(s);
    kvb.dspVval(&vEntry);
    kvb.dspKval(&kValue);
  } else {
    kvb.setVval(s);
  }
  kvb.setKoV('-');
  vEntry.set_text("");
  vEntry.set_visibility(false);
}

KboxGrid::KboxGrid() :
  vTitle("inp:"),
  vEntry(),
  kIndex("0"),
  kValue("#"),
  kvb()
{
  m_Clip = Gtk::Clipboard::get();
  this->attach(vTitle, 0, 0);
  this->attach(vEntry, 1, 0);
  this->attach(kIndex, 0, 1);
  this->attach(kValue, 1, 1);
  vEntry.set_visibility(false);
  vEntry.signal_activate().connect(sigc::mem_fun(*this, &KboxGrid::vEntered));
}

KboxGrid::~KboxGrid()
{
}

bool KboxWin::on_key_press_event( GdkEventKey *event )
{
  const gint state3 = GDK_SHIFT_MASK | GDK_CONTROL_MASK | GDK_MOD1_MASK;
  if (event->keyval == GDK_KEY_q &&
	(event->state & (state3)) == GDK_CONTROL_MASK){
    hide();
    return true;
  }
  if (m_Grid.procKey(event->keyval, state3)){
    return true;
  }
  return Gtk::Window::on_key_press_event(event);
}

KboxWin::KboxWin()
: Gtk::ApplicationWindow(),
  m_Box(Gtk::ORIENTATION_VERTICAL),
  m_Grid()
{
  set_title("Main menu example");
  set_default_size(300, 100);

  // ExampleApplication displays the menubar. Other stuff, such as a toolbar,
  // is put into the box.
  add(m_Box);

  // Create actions for menus and toolbars.
  // We can use add_action() because Gtk::ApplicationWindow derives from Gio::ActionMap.
  // This Action Map uses a "win." prefix for the actions.
  // Therefore, for instance, "win.copy", is used in ExampleApplication::on_startup()
  // to layout the menu.

  //Edit menu:
  add_action("copy", sigc::mem_fun(*this, &KboxWin::on_menu_others));
  add_action("paste", sigc::mem_fun(*this, &KboxWin::on_menu_others));
  add_action("something", sigc::mem_fun(*this, &KboxWin::on_menu_others));

  //Choices menus, to demonstrate Radio items,
  //using our convenience methods for string and int radio values:
  m_refChoice = add_action_radio_string("choice",
    sigc::mem_fun(*this, &KboxWin::on_menu_choices), "a");

  m_refChoiceOther = add_action_radio_integer("choiceother",
    sigc::mem_fun(*this, &KboxWin::on_menu_choices_other), 1);

  m_refToggle = add_action_bool("sometoggle",
    sigc::mem_fun(*this, &KboxWin::on_menu_toggle), false);

  //Help menu:
  add_action("about", sigc::mem_fun(*this, &KboxWin::on_menu_others));

  //Create the toolbar and add it to a container widget:

  m_refBuilder = Gtk::Builder::create();

  Glib::ustring ui_info =
    "<!-- Generated with glade 3.18.3 -->"
    "<interface>"
    "  <requires lib='gtk+' version='3.4'/>"
    "  <object class='GtkToolbar' id='toolbar'>"
    "    <property name='visible'>True</property>"
    "    <property name='can_focus'>False</property>"
    "    <child>"
    "      <object class='GtkToolButton' id='toolbutton_new'>"
    "        <property name='visible'>True</property>"
    "        <property name='can_focus'>False</property>"
    "        <property name='tooltip_text' translatable='yes'>New Standard</property>"
    "        <property name='action_name'>app.newstandard</property>"
    "        <property name='icon_name'>document-new</property>"
    "      </object>"
    "      <packing>"
    "        <property name='expand'>False</property>"
    "        <property name='homogeneous'>True</property>"
    "      </packing>"
    "    </child>"
    "    <child>"
    "      <object class='GtkToolButton' id='toolbutton_quit'>"
    "        <property name='visible'>True</property>"
    "        <property name='can_focus'>False</property>"
    "        <property name='tooltip_text' translatable='yes'>Quit</property>"
    "        <property name='action_name'>app.quit</property>"
    "        <property name='icon_name'>application-exit</property>"
    "      </object>"
    "      <packing>"
    "        <property name='expand'>False</property>"
    "        <property name='homogeneous'>True</property>"
    "      </packing>"
    "    </child>"
    "  </object>"
    "</interface>";

  try
  {
    m_refBuilder->add_from_string(ui_info);
  }
  catch (const Glib::Error& ex)
  {
    std::cerr << "Building toolbar failed: " <<  ex.what();
  }

  Gtk::Toolbar* toolbar = nullptr;
  m_refBuilder->get_widget("toolbar", toolbar);
  if (!toolbar)
    g_warning("GtkToolbar not found");
  else
    m_Box.pack_start(*toolbar, Gtk::PACK_SHRINK);

/*
  Gtk::Grid* grid = new KboxGrid();
  if (!grid)
    g_warning("KboxGrid not found");
  else
    m_Box.pack_start(*grid, Gtk::PACK_SHRINK);
*/
  m_Box.pack_start(m_Grid, Gtk::PACK_SHRINK);
}

KboxWin::~KboxWin()
{
}

void KboxWin::on_menu_others()
{
  std::cout << "A menu item was selected." << std::endl;
}

void KboxWin::on_menu_choices(const Glib::ustring& parameter)
{
  //The radio action's state does not change automatically:
  m_refChoice->change_state(parameter);

  Glib::ustring message;
  if (parameter == "a")
    message = "Choice a was selected.";
  else
    message = "Choice b was selected.";

  std::cout << message << std::endl;
}

void KboxWin::on_menu_choices_other(int parameter)
{
  //The radio action's state does not change automatically:
  m_refChoiceOther->change_state(parameter);

  Glib::ustring message;
  if (parameter == 1)
    message = "Choice 1 was selected.";
  else
    message = "Choice 2 was selected.";

  std::cout << message << std::endl;
}

void KboxWin::on_menu_toggle()
{
  bool active = false;
  m_refToggle->get_state(active);

  //The toggle action's state does not change automatically:
  active = !active;
  m_refToggle->change_state(active);

  Glib::ustring message;
  if (active)
    message = "Toggle is active.";
  else
    message = "Toggle is not active.";

  std::cout << message << std::endl;
}
