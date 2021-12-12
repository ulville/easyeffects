/*
 *  Copyright © 2017-2022 Wellington Wallace
 *
 *  This file is part of EasyEffects.
 *
 *  EasyEffects is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  EasyEffects is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with EasyEffects.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "bass_enhancer_ui.hpp"

namespace ui::bass_enhancer_box {

using namespace std::string_literals;

auto constexpr log_tag = "bass_enhancer_box: ";

struct Data {
 public:
  ~Data() { util::debug(log_tag + "data struct destroyed"s); }

  std::shared_ptr<BassEnhancer> bass_enhancer;

  std::vector<sigc::connection> connections;

  std::vector<gulong> gconnections;
};

struct _BassEnhancerBox {
  GtkBox parent_instance;

  GtkScale *input_gain, *output_gain;

  GtkLevelBar *input_level_left, *input_level_right, *output_level_left, *output_level_right;

  GtkLabel *input_level_left_label, *input_level_right_label, *output_level_left_label, *output_level_right_label;

  GtkToggleButton* bypass;

  GtkLevelBar* harmonics_levelbar;

  GtkLabel* harmonics_levelbar_label;

  GtkSpinButton *floor, *amount, *harmonics, *scope;

  GtkScale* blend;

  GtkToggleButton *floor_active, *listen;

  GSettings* settings;

  Data* data;
};

G_DEFINE_TYPE(BassEnhancerBox, bass_enhancer_box, GTK_TYPE_BOX)

void on_bypass(BassEnhancerBox* self, GtkToggleButton* btn) {
  self->data->bass_enhancer->bypass = gtk_toggle_button_get_active(btn);
}

void on_reset(BassEnhancerBox* self, GtkButton* btn) {
  gtk_toggle_button_set_active(self->bypass, 0);

  util::reset_all_keys(self->settings);
}

void setup(BassEnhancerBox* self, std::shared_ptr<BassEnhancer> bass_enhancer, const std::string& schema_path) {
  self->data->bass_enhancer = bass_enhancer;

  self->settings = g_settings_new_with_path("com.github.wwmm.easyeffects.bassenhancer", schema_path.c_str());

  bass_enhancer->post_messages = true;
  bass_enhancer->bypass = false;

  self->data->connections.push_back(bass_enhancer->input_level.connect([=](const float& left, const float& right) {
    update_level(self->input_level_left, self->input_level_left_label, self->input_level_right,
                 self->input_level_right_label, left, right);
  }));

  self->data->connections.push_back(bass_enhancer->output_level.connect([=](const float& left, const float& right) {
    update_level(self->output_level_left, self->output_level_left_label, self->output_level_right,
                 self->output_level_right_label, left, right);
  }));

  self->data->connections.push_back(bass_enhancer->harmonics.connect([=](const double& value) {
    gtk_level_bar_set_value(self->harmonics_levelbar, value);
    gtk_label_set_text(self->harmonics_levelbar_label, fmt::format("{0:.0f}", util::linear_to_db(value)).c_str());
  }));

  g_settings_bind(self->settings, "input-gain", gtk_range_get_adjustment(GTK_RANGE(self->input_gain)), "value",
                  G_SETTINGS_BIND_DEFAULT);
  g_settings_bind(self->settings, "output-gain", gtk_range_get_adjustment(GTK_RANGE(self->output_gain)), "value",
                  G_SETTINGS_BIND_DEFAULT);

  g_settings_bind(self->settings, "amount", gtk_spin_button_get_adjustment(self->amount), "value",
                  G_SETTINGS_BIND_DEFAULT);
  g_settings_bind(self->settings, "harmonics", gtk_spin_button_get_adjustment(self->harmonics), "value",
                  G_SETTINGS_BIND_DEFAULT);
  g_settings_bind(self->settings, "scope", gtk_spin_button_get_adjustment(self->scope), "value",
                  G_SETTINGS_BIND_DEFAULT);
  g_settings_bind(self->settings, "floor", gtk_spin_button_get_adjustment(self->floor), "value",
                  G_SETTINGS_BIND_DEFAULT);
  g_settings_bind(self->settings, "blend", gtk_range_get_adjustment(GTK_RANGE(self->blend)), "value",
                  G_SETTINGS_BIND_DEFAULT);

  g_settings_bind(self->settings, "listen", self->listen, "active", G_SETTINGS_BIND_DEFAULT);
  g_settings_bind(self->settings, "floor-active", self->floor_active, "active", G_SETTINGS_BIND_DEFAULT);
}

void dispose(GObject* object) {
  auto* self = EE_BASS_ENHANCER_BOX(object);

  self->data->bass_enhancer->bypass = false;

  for (auto& c : self->data->connections) {
    c.disconnect();
  }

  for (auto& handler_id : self->data->gconnections) {
    g_signal_handler_disconnect(self->settings, handler_id);
  }

  self->data->connections.clear();
  self->data->gconnections.clear();

  g_object_unref(self->settings);

  util::debug(log_tag + "disposed"s);

  G_OBJECT_CLASS(bass_enhancer_box_parent_class)->dispose(object);
}

void finalize(GObject* object) {
  auto* self = EE_BASS_ENHANCER_BOX(object);

  delete self->data;

  util::debug(log_tag + "finalized"s);

  G_OBJECT_CLASS(bass_enhancer_box_parent_class)->finalize(object);
}

void bass_enhancer_box_class_init(BassEnhancerBoxClass* klass) {
  auto* object_class = G_OBJECT_CLASS(klass);
  auto* widget_class = GTK_WIDGET_CLASS(klass);

  object_class->dispose = dispose;
  object_class->finalize = finalize;

  gtk_widget_class_set_template_from_resource(widget_class, "/com/github/wwmm/easyeffects/ui/bass_enhancer.ui");

  gtk_widget_class_bind_template_child(widget_class, BassEnhancerBox, input_gain);
  gtk_widget_class_bind_template_child(widget_class, BassEnhancerBox, output_gain);
  gtk_widget_class_bind_template_child(widget_class, BassEnhancerBox, input_level_left);
  gtk_widget_class_bind_template_child(widget_class, BassEnhancerBox, input_level_right);
  gtk_widget_class_bind_template_child(widget_class, BassEnhancerBox, output_level_left);
  gtk_widget_class_bind_template_child(widget_class, BassEnhancerBox, output_level_right);
  gtk_widget_class_bind_template_child(widget_class, BassEnhancerBox, input_level_left_label);
  gtk_widget_class_bind_template_child(widget_class, BassEnhancerBox, input_level_right_label);
  gtk_widget_class_bind_template_child(widget_class, BassEnhancerBox, output_level_left_label);
  gtk_widget_class_bind_template_child(widget_class, BassEnhancerBox, output_level_right_label);

  gtk_widget_class_bind_template_child(widget_class, BassEnhancerBox, bypass);

  gtk_widget_class_bind_template_child(widget_class, BassEnhancerBox, harmonics_levelbar_label);
  gtk_widget_class_bind_template_child(widget_class, BassEnhancerBox, harmonics_levelbar);
  gtk_widget_class_bind_template_child(widget_class, BassEnhancerBox, floor);
  gtk_widget_class_bind_template_child(widget_class, BassEnhancerBox, amount);
  gtk_widget_class_bind_template_child(widget_class, BassEnhancerBox, harmonics);
  gtk_widget_class_bind_template_child(widget_class, BassEnhancerBox, scope);
  gtk_widget_class_bind_template_child(widget_class, BassEnhancerBox, blend);
  gtk_widget_class_bind_template_child(widget_class, BassEnhancerBox, floor_active);
  gtk_widget_class_bind_template_child(widget_class, BassEnhancerBox, listen);

  gtk_widget_class_bind_template_callback(widget_class, on_bypass);
  gtk_widget_class_bind_template_callback(widget_class, on_reset);
}

void bass_enhancer_box_init(BassEnhancerBox* self) {
  gtk_widget_init_template(GTK_WIDGET(self));

  self->data = new Data();

  prepare_spinbutton<"dB">(self->amount);
  prepare_spinbutton<"Hz">(self->scope);
  prepare_spinbutton<"Hz">(self->floor);
  prepare_spinbutton<"">(self->harmonics);

  prepare_scale<"">(self->blend);
}

auto create() -> BassEnhancerBox* {
  return static_cast<BassEnhancerBox*>(g_object_new(EE_TYPE_BASS_ENHANCER_BOX, nullptr));
}

}  // namespace ui::bass_enhancer_box
