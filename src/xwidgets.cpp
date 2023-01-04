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

#include "xeus-octave/xwidgets.hpp"

#include <octave/interpreter.h>

#include "xeus-octave/xwidgets/xaccordion.hpp"
#include "xeus-octave/xwidgets/xaudio.hpp"
#include "xeus-octave/xwidgets/xbox.hpp"
#include "xeus-octave/xwidgets/xbutton.hpp"
#include "xeus-octave/xwidgets/xcommon.hpp"
#include "xeus-octave/xwidgets/xhtml.hpp"
#include "xeus-octave/xwidgets/ximage.hpp"
#include "xeus-octave/xwidgets/xlabel.hpp"
#include "xeus-octave/xwidgets/xmedia.hpp"
#include "xeus-octave/xwidgets/xnumber.hpp"
#include "xeus-octave/xwidgets/xnumeral.hpp"
#include "xeus-octave/xwidgets/xobject.hpp"
#include "xeus-octave/xwidgets/xoutput.hpp"
#include "xeus-octave/xwidgets/xpassword.hpp"
#include "xeus-octave/xwidgets/xplay.hpp"
#include "xeus-octave/xwidgets/xprogress.hpp"
#include "xeus-octave/xwidgets/xselection_container.hpp"
#include "xeus-octave/xwidgets/xslider.hpp"
#include "xeus-octave/xwidgets/xstring.hpp"
#include "xeus-octave/xwidgets/xtab.hpp"
#include "xeus-octave/xwidgets/xtext.hpp"
#include "xeus-octave/xwidgets/xtextarea.hpp"
#include "xeus-octave/xwidgets/xvideo.hpp"
#include "xeus-octave/xwidgets/xwidget.hpp"

namespace xeus_octave::widgets
{

void register_all(octave::interpreter& interpreter)
{
  widget<xw::accordion>::register_widget(interpreter);
  widget<xw::audio>::register_widget(interpreter);
  widget<xw::hbox>::register_widget(interpreter);
  widget<xw::vbox>::register_widget(interpreter);
  widget<xw::button>::register_widget(interpreter);
  widget<xw::html>::register_widget(interpreter);
  widget<xw::image>::register_widget(interpreter);
  widget<xw::label>::register_widget(interpreter);
  widget<xw::numeral<double>>::register_widget(interpreter);
  widget<xw::output>::register_widget(interpreter);
  widget<xw::password>::register_widget(interpreter);
  widget<xw::play>::register_widget(interpreter);
  widget<xw::progress<double>>::register_widget(interpreter);
  widget<xw::slider<double>>::register_widget(interpreter);
  widget<xw::tab>::register_widget(interpreter);
  widget<xw::text>::register_widget(interpreter);
  widget<xw::textarea>::register_widget(interpreter);
  widget<xw::video>::register_widget(interpreter);
}

}  // namespace xeus_octave::widgets
