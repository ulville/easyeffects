/*
 *  Copyright © 2017-2022 Wellington Wallace
 *
 *  This file is part of Easy Effects.
 *
 *  Easy Effects is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Easy Effects is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Easy Effects. If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

#include <adwaita.h>
#include "effects_base.hpp"
#include "multiband_gate_band_box.hpp"
#include "node_info_holder.hpp"
#include "tags_resources.hpp"
#include "ui_helpers.hpp"

namespace ui::multiband_gate_box {

G_BEGIN_DECLS

#define EE_TYPE_MULTIBAND_GATE_BOX (multiband_gate_box_get_type())

G_DECLARE_FINAL_TYPE(MultibandGateBox, multiband_gate_box, EE, MULTIBAND_GATE_BOX, GtkBox)

G_END_DECLS

auto create() -> MultibandGateBox*;

void setup(MultibandGateBox* self,
           std::shared_ptr<MultibandGate> multiband_gate,
           const std::string& schema_path,
           PipeManager* pm);

}  // namespace ui::multiband_gate_box
