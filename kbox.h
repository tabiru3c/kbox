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

#pragma once

#include <gtkmm.h>

class KboxApp : public Gtk::Application
{
protected:
  KboxApp();

public:
  static Glib::RefPtr<KboxApp> create();

protected:
  //Overrides of default signal handlers:
  void on_startup() override;
  void on_activate() override;

private:
  void create_window();

  void on_window_hide(Gtk::Window* window);
  void on_menu_file_new_generic();
  void on_menu_file_quit();
  void on_menu_help_about();

  Glib::RefPtr<Gtk::Builder> m_refBuilder;
};

