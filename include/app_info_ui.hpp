#ifndef APP_INFO_UI_HPP
#define APP_INFO_UI_HPP

#include <gtkmm/builder.h>
#include <gtkmm/checkbutton.h>
#include <gtkmm/grid.h>
#include <gtkmm/image.h>
#include <gtkmm/label.h>
#include <gtkmm/scale.h>
#include <gtkmm/switch.h>
#include <glibmm/i18n.h>
#include <gtkmm/togglebutton.h>
#include "blocklist_settings_ui.hpp"
#include "plugin_ui_base.hpp"
#include "preset_type.hpp"
#include "pulse_manager.hpp"
#include "util.hpp"

class AppInfoUi : public Gtk::Grid {
 public:
  AppInfoUi(BaseObjectType* cobject,
            const Glib::RefPtr<Gtk::Builder>& builder,
            std::shared_ptr<AppInfo> info,
            PulseManager* pulse_manager);
  AppInfoUi(const AppInfoUi&) = delete;
  auto operator=(const AppInfoUi&) -> AppInfoUi& = delete;
  AppInfoUi(const AppInfoUi&&) = delete;
  auto operator=(const AppInfoUi &&) -> AppInfoUi& = delete;
  ~AppInfoUi() override;

  Gtk::Switch* enable = nullptr;

  Gtk::ToggleButton* mute = nullptr;
  Gtk::CheckButton* blocklist = nullptr;

  Gtk::Scale* volume = nullptr;

  Gtk::Image* app_icon = nullptr;
  Gtk::Image* mute_icon = nullptr;

  Gtk::Label* app_name = nullptr;
  Gtk::Label* media_name = nullptr;
  Gtk::Label* format = nullptr;
  Gtk::Label* rate = nullptr;
  Gtk::Label* channels = nullptr;
  Gtk::Label* resampler = nullptr;
  Gtk::Label* buffer = nullptr;
  Gtk::Label* latency = nullptr;
  Gtk::Label* state = nullptr;

  std::shared_ptr<AppInfo> app_info;

  void update(const std::shared_ptr<AppInfo>& info);

 private:
  std::string log_tag = "app_info_ui: ";

  bool running = true, is_enabled = true, is_blocklisted = true, pre_bl_state = true;

  sigc::connection enable_connection;
  sigc::connection volume_connection;
  sigc::connection mute_connection;
  sigc::connection blocklist_connection;
  sigc::connection timeout_connection;

  PulseManager* pm = nullptr;

  void init_widgets();

  void connect_signals();

  auto on_enable_app(bool state) -> bool;

  void on_volume_changed();

  void on_mute();
};

#endif
