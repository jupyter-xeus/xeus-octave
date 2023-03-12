/*
 * Copyright (C) 2022 Giulio Girardi.
 *
 * This file is part of xeus-octave.
 *
 * xeus-octave is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * xeus-octave is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with xeus-octave.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef XEUS_OCTAVE_XOBJECT_H
#define XEUS_OCTAVE_XOBJECT_H

#include "xeus-octave/xwidgets.hpp"

#include <xwidgets/xobject.hpp>

namespace xeus_octave::widgets
{

template <typename D> struct widget<xw::xobject<D>>
{
  using W = xw::xobject<D>;

  static constexpr auto name = "xobject";

  inline static void register_widget_members(octave::interpreter& interpreter, octave::cdef_class& cls)
  {
    static constexpr char const _model_module_name[] = "_ModelModule";
    static constexpr char const _model_module_version_name[] = "_ModelModuleVersion";
    static constexpr char const _model_name_name[] = "_ModelName";
    static constexpr char const _view_module_name[] = "_ViewModule";
    static constexpr char const _view_module_version_name[] = "_ViewModuleVersion";
    static constexpr char const _view_name_name[] = "_ViewName";

    xwidgets_add_property<&W ::_model_module, _model_module_name, true>(interpreter, cls);
    xwidgets_add_property<&W ::_model_module_version, _model_module_version_name, true>(interpreter, cls);
    xwidgets_add_property<&W ::_model_name, _model_name_name, true>(interpreter, cls);
    xwidgets_add_property<&W ::_view_module, _view_module_name, true>(interpreter, cls);
    xwidgets_add_property<&W ::_view_module_version, _view_module_version_name, true>(interpreter, cls);
    xwidgets_add_property<&W ::_view_name, _view_name_name, true>(interpreter, cls);
  };
};

}  // namespace xeus_octave::widgets

#endif
