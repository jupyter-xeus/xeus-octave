/*
 * Copyright (C) 2020 Giulio Girardi.
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

#ifndef XEUS_OCTAVE_TEX2HTML_H
#define XEUS_OCTAVE_TEX2HTML_H

#include <octave/octave-config.h>
#include <octave/text-engine.h>

#include <iostream>
#include <sstream>
#include <stack>
#include <string>

using namespace octave;

namespace xeus_octave
{

class tex_to_html : public text_processor
{
public:

  tex_to_html() {}

  operator std::string() const { return html.str(); }

  void visit(text_element_string& e) override
  {
#ifndef NDEBUG
    std::clog << "string: " << e.string_value() << std::endl;
#endif

    html << e.string_value();
  }

  void visit(text_element_subscript& e) override
  {
#ifndef NDEBUG
    std::clog << "subscript" << std::endl;
#endif

    html << "<sub>";
    text_processor::visit(e);
    html << "</sub>";
  }

  void visit(text_element_superscript& e) override
  {
#ifndef NDEBUG
    std::clog << "superscript" << std::endl;
#endif

    html << "<sup>";
    text_processor::visit(e);
    html << "</sup>";
  }

  void visit(text_element_color& e) override
  {
#ifndef NDEBUG
    std::clog << "color: " << e.get_color() << std::endl;
#else
    octave_unused_parameter(e);
#endif
  }

  void visit(text_element_fontsize& e) override
  {
#ifndef NDEBUG
    std::clog << "fontsize: " << e.get_fontsize() << std::endl;
#else
    octave_unused_parameter(e);
#endif
  }

  void visit(text_element_fontname& e) override
  {
#ifndef NDEBUG
    std::clog << "fontname: " << e.get_fontname() << std::endl;
#else
    octave_unused_parameter(e);
#endif
  }

  void visit(text_element_fontstyle& e) override
  {
#ifndef NDEBUG
    std::clog << "fontstyle: " << e.get_fontstyle() << std::endl;
#endif

    switch (e.get_fontstyle())
    {
    case text_element_fontstyle::normal:
      if (status.bold)
        html << "</b>";
      if (status.italic)
        html << "</i>";

      status.bold = false;
      status.italic = false;
      break;
    case text_element_fontstyle::bold:
      html << "<b>";
      status.bold = true;
      break;
    case text_element_fontstyle::italic:
      html << "<i>";
      status.italic = true;
      break;
    case text_element_fontstyle::oblique:
      html << "<i>";
      status.italic = true;
      break;
    }
  }

  void visit(text_element_symbol& e) override
  {
    uint32_t code = e.get_symbol_code();

#ifndef NDEBUG
    std::clog << "symbol: " << code << std::endl;
#endif

    if (code != text_element_symbol::invalid_code)
    {
      html << "&#" << code << ";";
    }
  }

  void visit(text_element_combined& /*e*/) override
  {
#ifndef NDEBUG
    std::clog << "combination" << std::endl;
#endif
  }

  void visit(text_element_list& e) override
  {
#ifndef NDEBUG
    std::clog << "list" << std::endl;
#endif
    status_t save = status;

    text_processor::visit(e);

    if (status.bold && !save.bold)
      html << "</b>";
    if (status.italic && !save.italic)
      html << "</i>";

    status = save;
  }

private:

  typedef struct status_t
  {
    bool bold = false;
    bool italic = false;
  } status_t;

  status_t status;
  std::stringstream html;
};

}  // namespace xeus_octave

#endif  // XEUS_OCTAVE_TEX2HTML_H
