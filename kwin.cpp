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
#include <fstream>
#include <iostream>

#define STATE3 (GDK_SHIFT_MASK|GDK_CONTROL_MASK|GDK_MOD1_MASK)
#define ROTORSZ 256

struct ENIGBASE {
char t1[ROTORSZ];
char t2[ROTORSZ];
char t3[ROTORSZ];
char deck[ROTORSZ];
char buf[13];
};

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

extern "C" {
int aes_enc( char *src, int slen, char *pwd, char *dst );
int aes_dec( char *src, int slen, char *pwd, char *dst );
}

gint kbEnt::dt2arKV( gchar *bf, gint sz, Glib::ustring kpass )
{
  gint n = 0;
  guint16 *lk = (guint16 *)bf;
  if ((*lk <= KLENMAX) && (*lk < sz-4)){
    if (kpass.length() > 0){
      gint elv = *(guint16 *)&bf[2+(*lk)];
      if (elv <= VLENMAX){
        gchar pbf[256]{0};
        gchar *pw = new gchar [kpass.length()+1];
        strcpy(pw, kpass.c_str());
        gint plv = aes_dec(&bf[4+(*lk)], elv, pw, pbf);
        if (plv < 1){
          delete[] pw;
          return plv;
	}
        if (plv <= VLENMAX){
          n = 4+(*lk)+elv;
          kbk = std::string(&bf[2]).substr(0, *lk);
          kbv = std::string(pbf).substr(0, plv);
	}
        delete[] pw;
      }
    } else {
      guint16 *lv = (guint16 *)&bf[2+(*lk)];
      if ((*lv <= VLENMAX) && (4+(*lk)+(*lv) < sz)){
        n = 4+(*lk)+(*lv);
        kbk = std::string(&bf[2]).substr(0, *lk);
        kbv = std::string(&bf[4+(*lk)]).substr(0, *lv);
      }
    }
  }
  return n;
}

gint kbEnt::ar2dtKV( gchar *bf, gint sz, Glib::ustring kpass )
{
  gint n = 0;
  guint16 lk = kbk.length();
  if ((lk <= KLENMAX) && (lk < sz-4)){
    if (kpass.length() > 0){
      gint plv = kbv.length();
      gchar *pbf = new gchar [plv+1];
      strcpy(pbf, kbv.c_str());
      gchar ebf[256];
      gchar *pw = new gchar [kpass.length()+1];
      strcpy(pw, kpass.c_str());
      guint16 elv = aes_enc(pbf, plv, pw, ebf);
      if (elv < 1){
        delete[] pw;
        return elv;
      }
      if ((elv <= VLENMAX) && (4+lk+elv < sz)){
        n = 4+lk+elv;
        *((guint16 *)bf) = lk;
        memcpy(&bf[2], kbk.c_str(), lk);
        *((guint16 *)&bf[2+lk]) = elv;
        memcpy(&bf[4+lk], ebf, elv);
      }
      delete[] pw;
      delete[] pbf;
    } else {
      guint16 lv = kbv.length();
      if ((lv <= VLENMAX) && (4+lk+lv < sz)){
        n = 4+lk+lv;
        *((guint16 *)bf) = lk;
        memcpy(&bf[2], kbk.c_str(), lk);
        *((guint16 *)&bf[2+lk]) = lv;
        memcpy(&bf[4+lk], kbv.c_str(), lv);
      }
    }
  }
  return n;
}

extern "C" {
int encb64( char *dst, const char *src, int n );
int decb64( char *dst, const char *src, int n );
}

gchar* kbEnt::b64( gchar e )
{
  gchar *bb = nullptr;
  size_t p0 = std::string("Bebd").find(e, 0);
  gint n = kbv.length();
  if ((n > 0) && (p0 != std::string::npos)){
    gint m = ((p0 < 2) ? ((n+2)/3)*4 : ((n+3)/4)*3) +1;
    bb = new gchar [m+n+1];
    gchar *b0 = &bb[m];
    strcpy(b0, kbv.c_str());
    if (p0 < 2){
      m = encb64(bb, b0, n);
    } else {
      m = decb64(bb, b0, n);
    }
    bb[m] = '\0';
  }
  return bb;
}

extern "C" {
void enisetup( struct ENIGBASE *eb, char *pw );
uint8_t eniconv( struct ENIGBASE *eb, int32_t n, uint8_t *s, uint8_t *d );
}

gchar* kbEnt::eni( gchar e, Glib::ustring kp )
{
  gchar *bb = nullptr;
  gint n = kbv.length();
  if ((n > 0) && ((e == 'E') || (e == 'e'))){
    struct ENIGBASE enib;
    gchar *pw = new gchar [kp.length()+1];
    strcpy(pw, kp.c_str());
    enisetup(&enib, pw);
    gint m = ((e == 'E') ? ((n+2)/3)*4 : ((n+3)/4)*3) +1;
    if (e == 'E'){
      bb = new gchar [m+2*(n+1)];
      gchar *b0 = &bb[m];
      gchar *b1 = &b0[n+1];
      strcpy(b0, kbv.c_str());
      eniconv(&enib, n, (uint8_t *)b0, (uint8_t *)b1);
      m = encb64(bb, b1, n);
    } else {
      bb = new gchar [2*m+n+1];
      gchar *b0 = &bb[m];
      gchar *b1 = &b0[n+1];
      strcpy(b0, kbv.c_str());
      m = decb64(b1, b0, n);
      eniconv(&enib, m, (uint8_t *)b1, (uint8_t *)bb);
    }
    delete[] pw;
    bb[m] = '\0';
  }
  return bb;
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

void KVBase::setPval( Glib::ustring s )
{
  kboxPass = s;
}

void KVBase::pasteKV( Glib::ustring s )
{
  if (kov == 'k'){
    ke[kidx]->setK(s);
  } else if (kov == 'v'){
    ke[kidx]->setV(s);
  }
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

void KVBase::dspIdxC( Gtk::Label *lbl, gchar c )
{
  lbl->set_text(Glib::ustring::sprintf("%d%c", kidx, c));
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

void KVBase::dspPval( Gtk::Entry *ent )
{
  ent->set_text(kboxPass);
}

Glib::ustring KVBase::rot13( Glib::ustring s )
{
  Glib::ustring d = "";
  gint l = s.length();
  if (l > 0){
    gchar *buf = new gchar [l+1];
    strcpy(buf, s.c_str());
    gchar c;
    for (gint i=0; i<l; i++){
      c = buf[i];
      if (((c >= 'A') && (c <= 'M')) || ((c >= 'a') && (c <= 'm'))){
        buf[i] += 13;
      } else if (((c >= 'N') && (c <= 'Z')) || ((c >= 'n') && (c <= 'z'))){
        buf[i] -= 13;
      }
    }
    d = std::string(buf);
    delete[] buf;
  }
  return d;
}

gint KVBase::loadAll( gchar *buf, gint bsz, Glib::ustring flnm )
{
  gint sz = 0;
  try {
    std::ifstream ifs(flnm, std::ios::binary);
    ifs.read(buf, bsz);
    sz = ifs.gcount();
    ifs.close();
  } catch (const std::exception& e){
    std::cerr << "exception: " << e.what() << std::endl;
  }
  return sz;
}

gint KVBase::dataToArr( gchar *bf, gint sz )
{
  gint n = 0;
  gint m = 6;
  for (gint i=0; (i<nkv)&&(m>=6); i++){
    m = ke[i]->dt2arKV(&bf[n], sz-n, kboxPass);
    if (m >= 6){
      n += m;
    }
  }
  return n;
}

bool KVBase::loadKV()
{
  bool rc = false;
  Glib::ustring flnm = rot13(ke[0]->getV());
  gchar buf[FSZMAX]{0};
  gint sz = loadAll(buf, FSZMAX, flnm);
  if ((sz > 4+KVSIZE*6) && (strncmp(buf, "kbox", 4) == 0)){
    if (dataToArr(&buf[4], sz-4) > KVSIZE*6){
      rc = true;
    }
  } else {
    std::cerr << "Invalid data file." << std::endl;
  }
  return rc;
}

gint KVBase::saveAll( gchar *buf, gint bsz, Glib::ustring flnm )
{
  gint sz = 0;
  try {
    std::ofstream ofs(flnm, std::ios::binary);
    ofs.write(buf, bsz);
    sz = ofs.tellp();
    ofs.close();
  } catch (const std::exception& e){
    std::cerr << "exception: " << e.what() << std::endl;
  }
  return sz;
}

gint KVBase::arrToData( gchar *bf, gint sz )
{
  gint n = 0;
  gint m = 6;
  for (gint i=0; (i<nkv)&&(m>=6); i++){
    m = ke[i]->ar2dtKV(&bf[n], sz-n, kboxPass);
    if (m >= 6){
      n += m;
    }
  }
  return n;
}

bool KVBase::saveKV()
{
  bool rc = false;
  gchar buf[FSZMAX]{0};
  memcpy(buf, "kbox", 4);
  gint sz = 4 + arrToData(&buf[4], FSZMAX-4);
  if ((sz > 4+KVSIZE*6) && (sz < FSZMAX)){
    if (saveAll(buf, sz, KBOXDAT) == sz){
      rc = true;
    }
  }
  return rc;
}

gchar* KVBase::b64( gchar e )
{
  return ke[kidx]->b64(e);
}

gchar* KVBase::eni( gchar e )
{
  return ke[kidx]->eni(e, kboxPass);
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
  kboxPass = "kboxPass";
}

KVBase::~KVBase()
{
}

bool KboxGrid::procKey( guint keyval, gint state )
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
    if ((keyval == GDK_KEY_B) || (keyval == GDK_KEY_b)){
      gchar *dst = kvb.b64(keyval);
      if (dst != nullptr){
        m_Clip->set_text(dst);
        delete[] dst;
        kvb.dspIdxC(&kIndex, keyval);
      }
      return true;
    }
    if ((keyval == GDK_KEY_E) || (keyval == GDK_KEY_e)){
      gchar *dst = kvb.eni(keyval);
      if (dst != nullptr){
        m_Clip->set_text(dst);
        delete[] dst;
        kvb.dspIdxC(&kIndex, keyval);
      }
      return true;
    }
    if (keyval == GDK_KEY_c){
      m_Clip->set_text(kvb.getVval());
      kvb.dspIdxC(&kIndex, 'c');
      return true;
    }
    if ((keyval == GDK_KEY_k) || (keyval == GDK_KEY_v) || (keyval == GDK_KEY_p)){
      kov = keyval & 0x7f;
      kvb.setKoV(kov);
      if (kov == 'k'){
        vEntry.set_visibility(true);
        kvb.dspKval(&vEntry);
      } else if (kov == 'v'){
        kvb.dspVval(&vEntry);
      } else if (kov == 'p'){
        kvb.dspPval(&vEntry);
      }
      vEntry.grab_focus();
      return true;
    }
    if (keyval == GDK_KEY_l){
      if (kvb.loadKV()){
        kValue.set_text("loaded");
      } else {
        kValue.set_text("### failed ###");
      }
      return true;
    }
    if (keyval == GDK_KEY_s){
      if (kvb.saveKV()){
        kValue.set_text("saved");
      } else {
        kValue.set_text("### failed ###");
      }
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
  } else if (kov == 'v'){
    kvb.setVval(s);
  } else if (kov == 'p'){
    kvb.setPval(s);
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
  if (m_Grid.procKey(event->keyval, event->state)){
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
/*  add_action("paste", sigc::mem_fun(*this, &KboxWin::on_menu_others)); */
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
