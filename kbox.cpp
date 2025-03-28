/* gtkmm example Copyright (C) 2014 gtkmm development team
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
 * along with this program; if not, see <http://www.gnu.org/licenses/>.
 */

#include "kbox.h"
#include "kwin.h"
#include <iostream>

KboxApp::KboxApp()
: Gtk::Application("org.gtkmm.example.main_menu")
{
  Glib::set_application_name("Main Menu Example");
}

Glib::RefPtr<KboxApp> KboxApp::create()
{
  return Glib::RefPtr<KboxApp>(new KboxApp());
}

void KboxApp::on_startup()
{
  //Call the base class's implementation:
  Gtk::Application::on_startup();

  //Create actions for menus and toolbars.
  //We can use add_action() because Gtk::Application derives from Gio::ActionMap.

  //File|New sub menu:
  add_action("newstandard",
    sigc::mem_fun(*this, &KboxApp::on_menu_file_new_generic));

  add_action("newfoo",
    sigc::mem_fun(*this, &KboxApp::on_menu_file_new_generic));

  add_action("newgoo",
    sigc::mem_fun(*this, &KboxApp::on_menu_file_new_generic));

  //File menu:
  add_action("quit", sigc::mem_fun(*this, &KboxApp::on_menu_file_quit));

  //Help menu:
  add_action("about", sigc::mem_fun(*this, &KboxApp::on_menu_help_about));

  m_refBuilder = Gtk::Builder::create();

  //Layout the actions in a menubar and an application menu:
  Glib::ustring ui_info =
    "<interface>"
    "  <!-- menubar -->"
    "  <menu id='menu-example'>"
    "    <submenu>"
    "      <attribute name='label' translatable='yes'>_File</attribute>"
    "      <section>"
    "        <item>"
    "          <attribute name='label' translatable='yes'>New _Standard</attribute>"
    "          <attribute name='action'>app.newstandard</attribute>"
    "          <attribute name='accel'>&lt;Primary&gt;n</attribute>"
    "        </item>"
    "        <item>"
    "          <attribute name='label' translatable='yes'>New _Foo</attribute>"
    "          <attribute name='action'>app.newfoo</attribute>"
    "        </item>"
    "        <item>"
    "          <attribute name='label' translatable='yes'>New _Goo</attribute>"
    "          <attribute name='action'>app.newgoo</attribute>"
    "        </item>"
    "      </section>"
    "      <section>"
    "        <item>"
    "          <attribute name='label' translatable='yes'>_Quit</attribute>"
    "          <attribute name='action'>app.quit</attribute>"
    "          <attribute name='accel'>&lt;Primary&gt;q</attribute>"
    "        </item>"
    "      </section>"
    "    </submenu>"
    "    <submenu>"
    "      <attribute name='label' translatable='yes'>_Edit</attribute>"
    "      <section>"
    "        <item>"
    "          <attribute name='label' translatable='yes'>_Copy</attribute>"
    "          <attribute name='action'>win.copy</attribute>"
    "          <attribute name='accel'>&lt;Primary&gt;c</attribute>"
    "        </item>"
    "        <item>"
    "          <attribute name='label' translatable='yes'>_Paste</attribute>"
    "          <attribute name='action'>win.paste</attribute>"
    "          <attribute name='accel'>&lt;Primary&gt;v</attribute>"
    "        </item>"
    "        <item>"
    "          <attribute name='label' translatable='yes'>_Something</attribute>"
    "          <attribute name='action'>win.something</attribute>"
    "        </item>"
    "      </section>"
    "    </submenu>"
    "    <submenu>"
    "      <attribute name='label' translatable='yes'>_Choices</attribute>"
    "      <section>"
    "        <item>"
    "          <attribute name='label' translatable='yes'>Choice _A</attribute>"
    "          <attribute name='action'>win.choice</attribute>"
    "          <attribute name='target'>a</attribute>"
    "        </item>"
    "        <item>"
    "          <attribute name='label' translatable='yes'>Choice _B</attribute>"
    "          <attribute name='action'>win.choice</attribute>"
    "          <attribute name='target'>b</attribute>"
    "        </item>"
    "      </section>"
    "    </submenu>"
    "    <submenu>"
    "      <attribute name='label' translatable='yes'>_Other Choices</attribute>"
    "      <section>"
    "        <item>"
    "          <attribute name='label' translatable='yes'>Choice 1</attribute>"
    "          <attribute name='action'>win.choiceother</attribute>"
    "          <attribute name='target' type='i'>1</attribute>"
    "        </item>"
    "        <item>"
    "          <attribute name='label' translatable='yes'>Choice 2</attribute>"
    "          <attribute name='action'>win.choiceother</attribute>"
    "          <attribute name='target' type='i'>2</attribute>"
    "        </item>"
    "      </section>"
    "      <section>"
    "        <item>"
    "          <attribute name='label' translatable='yes'>Some Toggle</attribute>"
    "          <attribute name='action'>win.sometoggle</attribute>"
    "        </item>"
    "      </section>"
    "    </submenu>"
    "    <submenu>"
    "      <attribute name='label' translatable='yes'>_Help</attribute>"
    "      <section>"
    "        <item>"
    "          <attribute name='label' translatable='yes'>_About</attribute>"
    "          <attribute name='action'>win.about</attribute>"
    "        </item>"
    "      </section>"
    "    </submenu>"
    "  </menu>"
    ""
    "  <!-- application menu -->"
    "  <menu id='appmenu'>"
    "    <submenu>"
    "      <attribute name='label' translatable='yes'>_File</attribute>"
    "      <section>"
    "        <item>"
    "          <attribute name='label' translatable='yes'>New _Standard</attribute>"
    "          <attribute name='action'>app.newstandard</attribute>"
    "          <attribute name='accel'>&lt;Primary&gt;n</attribute>"
    "        </item>"
    "        <item>"
    "          <attribute name='label' translatable='yes'>New _Foo</attribute>"
    "          <attribute name='action'>app.newfoo</attribute>"
    "        </item>"
    "        <item>"
    "          <attribute name='label' translatable='yes'>New _Goo</attribute>"
    "          <attribute name='action'>app.newgoo</attribute>"
    "        </item>"
    "      </section>"
    "      <section>"
    "        <item>"
    "          <attribute name='label' translatable='yes'>_Quit</attribute>"
    "          <attribute name='action'>app.quit</attribute>"
    "          <attribute name='accel'>&lt;Primary&gt;q</attribute>"
    "        </item>"
    "      </section>"
    "    </submenu>"
    "    <submenu>"
    "      <attribute name='label' translatable='yes'>_Help</attribute>"
    "      <section>"
    "        <item>"
    "          <attribute name='label' translatable='yes'>_About</attribute>"
    "          <attribute name='action'>app.about</attribute>"
    "        </item>"
    "      </section>"
    "    </submenu>"
    "  </menu>"
    "</interface>";

  try
  {
    m_refBuilder->add_from_string(ui_info);
  }
  catch (const Glib::Error& ex)
  {
    std::cerr << "Building menus failed: " << ex.what();
  }

  //Get the menubar and the app menu, and add them to the application:
  auto object = m_refBuilder->get_object("menu-example");
  auto gmenu = Glib::RefPtr<Gio::Menu>::cast_dynamic(object);
  object = m_refBuilder->get_object("appmenu");
  auto appMenu = Glib::RefPtr<Gio::Menu>::cast_dynamic(object);
  if (!(gmenu && appMenu)) {
    g_warning("GMenu or AppMenu not found");
  }
  else
  {
    set_app_menu(appMenu);
    set_menubar(gmenu);
  }
}

void KboxApp::on_activate()
{
  //std::cout << "debug1: " << G_STRFUNC << std::endl;
  // The application has been started, so let's show a window.
  // A real application might want to reuse this window in on_open(),
  // when asked to open a file, if no changes have been made yet.
  create_window();
}

void KboxApp::create_window()
{
  auto win = new KboxWin();

  //Make sure that the application runs for as long this window is still open:
  add_window(*win);

  //Delete the window when it is hidden.
  //That's enough for this simple example.
  win->signal_hide().connect(sigc::bind<Gtk::Window*>(
    sigc::mem_fun(*this, &KboxApp::on_window_hide), win));

  win->show_all();
}

void KboxApp::on_window_hide(Gtk::Window* window)
{
  delete window;
}

void KboxApp::on_menu_file_new_generic()
{
  std::cout << "A File|New menu item was selected." << std::endl;
}

void KboxApp::on_menu_file_quit()
{
  std::cout << G_STRFUNC << std::endl;
  quit(); // Not really necessary, when Gtk::Widget::hide() is called.

  // Gio::Application::quit() will make Gio::Application::run() return,
  // but it's a crude way of ending the program. The window is not removed
  // from the application. Neither the window's nor the application's
  // destructors will be called, because there will be remaining reference
  // counts in both of them. If we want the destructors to be called, we
  // must remove the window from the application. One way of doing this
  // is to hide the window.
  std::vector<Gtk::Window*> windows = get_windows();
  if (windows.size() > 0)
    windows[0]->hide(); // In this simple case, we know there is only one window.
}

void KboxApp::on_menu_help_about()
{
  std::cout << "App|Help|About was selected." << std::endl;
}
