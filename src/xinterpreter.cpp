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

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstring>
#include <exception>
#include <iostream>
#include <ostream>
#include <regex>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

#include <nlohmann/json.hpp>
#include <octave/defun-dld.h>
#include <octave/error.h>
#include <octave/graphics-handle.h>
#include <octave/graphics-toolkit.h>
#include <octave/graphics.h>
#include <octave/interpreter.h>
#include <octave/lo-array-errwarn.h>
#include <octave/oct-stream.h>
#include <octave/ov-builtin.h>
#include <octave/ov.h>
#include <octave/ovl.h>
#include <octave/parse.h>
#include <octave/pt-stmt.h>
#include <octave/quit.h>
#include <octave/sighandlers.h>
#include <octave/utils.h>
#include <octave/version.h>
#include <xeus/xhelper.hpp>
#include <xeus/xinterpreter.hpp>

#include "xeus-octave/config.hpp"
#include "xeus-octave/input.hpp"
#include "xeus-octave/output.hpp"
#include "xeus-octave/utils.hpp"
#include "xeus-octave/xinterpreter.hpp"
#include "xeus-octave/utils.hpp"

#include "toolkits/notebook.hpp"
#include "toolkits/plotly.hpp"
#include "xoctave/display.hpp"

namespace nl = nlohmann;
namespace oc = octave;

namespace xeus_octave
{

namespace interpreter
{

namespace
{

/**
 * Native binding for getting the xeus-octave override path
 */
octave_value_list override_path(octave_value_list const& args, int /*nargout*/)
{
  if (args.length() != 0)
    print_usage();

  return ovl(XEUS_OCTAVE_OVERRIDE_PATH);
}

}  // namespace

void register_all(octave::interpreter& interpreter)
{
  utils::add_native_binding(interpreter, "XOCTAVE_OVERRIDE_PATH", override_path);
}

}  // namespace interpreter

xoctave_interpreter::xoctave_interpreter()
{
  xeus::register_interpreter(this);
}

void xoctave_interpreter::publish_stream(std::string const& name, std::string const& text)
{
  if (!m_silent)
    xinterpreter::publish_stream(name, text);
}

void xoctave_interpreter::display_data(nl::json data, nl::json metadata, nl::json transient)
{
  if (!m_silent)
    xinterpreter::display_data(data, metadata, transient);
}

void xoctave_interpreter::update_display_data(nl::json data, nl::json metadata, nl::json transient)
{
  if (!m_silent)
    xinterpreter::update_display_data(data, metadata, transient);
}

void xoctave_interpreter::publish_execution_result(int execution_count, nl::json data, nl::json metadata)
{
  if (!m_silent)
    xinterpreter::publish_execution_result(execution_count, data, metadata);
}

void xoctave_interpreter::publish_execution_error(
  std::string const& ename, std::string const& evalue, std::vector<std::string> const& trace_back
)
{
  if (!m_silent)
    xinterpreter::publish_execution_error(ename, evalue, trace_back);
}

namespace
{

/**
 * Extract the help for some symbol and return a json objects containing the
 * text in various mimetypes
 */
std::optional<nl::json> get_help_for_symbol(octave::interpreter& interpreter, std::string name)
{
  nl::json result;

  try
  {
    std::string text;
    std::string format;

    // Get the texinfo help text from the interpreter
    interpreter.get_help_system().get_help_text(name, text, format);

    // Octave gives the help in many formats according to the platform
    if (format == "texinfo")
    {
      // Generate help using the octave __makeinfo__ function requesting an html output
      octave_value_list help = interpreter.feval(
        "__makeinfo__",
        ovl(text, "html"),
        1  // For getting the return value
      );

      // Extract the HTML documentation
      std::string value = help(0).string_value();
      std::smatch match;
      // Remove the unused portion of the document (everything that's outside the body)
      std::regex_search(value, match, std::regex("<body.*>([^]*)</body>"));
      std::string html = match[1];

      // Jupyter style fixes. This is a little hacky, but jupyter messes up a bit
      // when rendering those tags
      html = std::regex_replace(
        html, std::regex("<dd(.*?)>"), "<dd $1 style='float:unset;width:unset;font-weight:unset;margin-left:40px'>"
      );
      html =
        std::regex_replace(html, std::regex("<dt(.*?)>"), "<dt $1 style='float:unset;width:unset;margin-left:0px;'>");

      // Return the help in both formats
      result["text/html"] = html;
      result["application/x-texinfo"] = text;
    }
    else if (format == "plain text")
    {
      // Return the help in plain text
      result["text/plain"] = text;
    }
    else if (format == "html")
    {
      // Return the help in plain text
      result["text/html"] = text;
    }
    else
    {
      return std::nullopt;
    }
  }
  catch (...)
  {
    std::clog << "Cannot get help for symbol " << name << std::endl;
    return std::nullopt;
  }

  return result;
}

/**
 * Concatenate strings.
 */
template <typename... Str> std::string concat(Str const&... strs)
{
  auto out = std::string();
  out.reserve((strs.size() + ...));
  ((out.append(strs)), ...);
  return out;
}

/**
 * Add @p ename and  @p evalue to trace_back.
 *
 * Simply setting ``"ename"`` and ``"evalue"`` in IOPub is not always enough to actually
 * print the error.
 * https://github.com/jupyter/jupyter_client/issues/363
 */
std::vector<std::string>
fix_traceback(std::string const& ename, std::string const& evalue, std::vector<std::string> trace_back = {})
{
  trace_back.push_back(concat(ename, std::string_view(": "), evalue));
  std::rotate(trace_back.rbegin(), trace_back.rbegin() + 1, trace_back.rend());
  return trace_back;
}

std::vector<std::string> fix_traceback(std::string const& ename, std::string const& evalue, std::string&& stack_trace)
{
  auto trace_back = std::vector<std::string>();
  trace_back.push_back(std::move(stack_trace));
  return fix_traceback(ename, evalue, std::move(trace_back));
}

}  // namespace

nl::json xoctave_interpreter::execute_request_impl(
  int /*execution_counter*/,
  std::string const& code,
  bool silent,
  bool /*store_history*/,
  nl::json /*user_expressions*/,
  bool allow_stdin
)
{
#ifndef NDEBUG
  std::clog << "Executing: " << code << std::endl;
#endif
  nl::json result;

  m_silent = silent;
  m_allow_stdin = allow_stdin;

  result = xeus::create_successful_reply();

  // Extract magic ?
  std::string trim = code;
  trim.erase(trim.find_last_not_of(" \n\r\t") + 1);
  if (trim.length() && trim[trim.length() - 1] == '?')
  {
    // User asked for function help
    // Remove ?
    trim.pop_back();
    auto data = get_help_for_symbol(interpreter, trim);

    if (!data)
    {
      auto ename = "Execution exception";
      auto evalue = concat(std::string_view("help: '"), trim, std::string_view("' not found\n"));
      result = xeus::create_error_reply(ename, evalue);
      publish_execution_error(ename, evalue, fix_traceback(ename, evalue));
    }
    else
    {
      auto payload = nl::json::array({{{"source", "page"}, {"start", 0}}});
      payload[0]["data"] = std::move(*data);
      result = xeus::create_successful_reply(std::move(payload));
    }
  }
  else
  {
    // Execute code
    auto str_parser = oc::parser(code, interpreter);

    // Clear current figure
    // This is useful for creating a figure in every cell, otherwise running code
    // in subsequent cells updates a previously displayed figure.
    // The current figure is stored in the properties of the root gh object (id 0)
    auto& root_figure =
      dynamic_cast<octave::root_figure::properties&>(interpreter.get_gh_manager().get_object(0).get_properties());
    root_figure.set_currentfigure(octave_value(NAN));

    // Code evaluation
    int exit_status = 0;
    do
    {
      try
      {
        str_parser.reset();
        exit_status = str_parser.run();

        if (exit_status == 0)
        {
          auto stmt_list = str_parser.statement_list();

          if (stmt_list)
          {
            interpreter.get_evaluator().eval(stmt_list, false);
          }
          else if (str_parser.at_end_of_input())
          {
            exit_status = EOF;
            break;
          }
        }
      }
      catch (oc::interrupt_exception const&)
      {
        auto const ename = "Interrupt exception";
        auto const evalue = "Kernel was interrupted";
        interpreter.recover_from_exception();
        publish_execution_error(ename, evalue, fix_traceback(ename, evalue));
        result = xeus::create_error_reply(ename, evalue);
      }
      catch (oc::index_exception const& e)
      {
        auto const ename = "Index exception";
        auto evalue = e.message();
        interpreter.recover_from_exception();
        publish_execution_error(ename, evalue, fix_traceback(ename, evalue, e.stack_trace()));
        result = xeus::create_error_reply(ename, evalue);
      }
      catch (oc::execution_exception const& e)
      {
        auto const ename = "Execution exception";
        auto evalue = e.message();
        interpreter.get_error_system().save_exception(e);
        interpreter.recover_from_exception();
        publish_execution_error(ename, evalue, fix_traceback(ename, evalue, e.stack_trace()));
        result = xeus::create_error_reply(ename, evalue);
      }
      catch (std::bad_alloc const&)
      {
        auto const ename = "Memory exception";
        auto const evalue = "Could not allocate the memory required for the computation";
        interpreter.recover_from_exception();
        publish_execution_error(ename, evalue, fix_traceback(ename, evalue));
        result = xeus::create_error_reply(ename, evalue);
      }
    } while (exit_status == 0);
  }

  // Update the figure if present
  interpreter.feval("drawnow");

  return result;
}

void xoctave_interpreter::configure_impl()
{
  // Override output system
  std::cout.rdbuf(&m_stdout);
  std::cerr.rdbuf(&m_stderr);

  // Install signal handlers to listen for CTRL+C
  octave::install_signal_handlers();

  // Set interpreter to read user/global configuration files
  interpreter.read_init_files(true);

  // Initialize interpreter
  interpreter.execute();

  // Fix disp function and clear display function
  interpreter.get_symbol_table().install_built_in_function("display", octave_value());

  // Prepend our override path to have precedence over default m-files
  interpreter.get_load_path().prepend(XEUS_OCTAVE_OVERRIDE_PATH);
  interpreter.get_load_path().set_add_hook(
    [prevhook = interpreter.get_load_path().get_add_hook(), this](std::string const& s)
    {
      interpreter.get_load_path().prepend(XEUS_OCTAVE_OVERRIDE_PATH);
      prevhook(s);
    }
  );

  interpreter.get_output_system().page_screen_output(true);

  // Register the graphics toolkits
  interpreter.get_gtk_manager().register_toolkit("notebook");
  interpreter.get_gtk_manager().load_toolkit(octave::graphics_toolkit(new xeus_octave::notebook_graphics_toolkit()));
  interpreter.get_gtk_manager().register_toolkit("plotly");
  interpreter.get_gtk_manager().load_toolkit(
    octave::graphics_toolkit(new xeus_octave::plotly_graphics_toolkit(interpreter))
  );

  // For unknown resons, setting a graphical toolkit does not work, unless
  // another "magic" toolkit such as gnuplot or fltk is loaded first. Since we
  // do not know which are magic and which are available at compile-time, we go
  // though them all.
  auto const& available_toolkits = interpreter.get_gtk_manager().available_toolkits_list().cellstr_value();
  for (auto i = octave_idx_type{0}; i < available_toolkits.numel(); ++i)
  {
    octave::feval("graphics_toolkit", ovl(available_toolkits.elem(i)));
  }

  octave::feval("graphics_toolkit", ovl("notebook"));

  // Register embedded functions
  xeus_octave::display::register_all(interpreter);
  xeus_octave::interpreter::register_all(interpreter);

  // Register the input system
  xeus_octave::io::register_input(m_stdin);

  // Install version variable
  interpreter.get_symbol_table().install_built_in_function(
    "XOCTAVE", new octave_builtin([](octave_value_list const&, int) { return ovl(XEUS_OCTAVE_VERSION); }, "XOCTAVE")
  );
}

namespace
{

/**
 * Given a chunk of text find the symbol under the cursor
 */
std::string get_symbol_from_cursor_pos(std::string const& code, size_t cursor_pos)
{
  if (cursor_pos == code.size())
    cursor_pos = code.size() - 1;

  while (cursor_pos > 0 &&
         (std::isalnum(code.at(cursor_pos)) || code.at(cursor_pos) == '_' || code.at(cursor_pos) == '.'))
  {
    cursor_pos--;
  }

  size_t end_pos = cursor_pos ? ++cursor_pos : 0;

  while (end_pos < code.size() && (std::isalnum(code.at(end_pos)) || code.at(end_pos) == '_' || code.at(end_pos) == '.')
  )
  {
    end_pos++;
  }

  return code.substr(cursor_pos, end_pos - cursor_pos);
}

}  // namespace

nl::json xoctave_interpreter::complete_request_impl(std::string const& code, int cursor_pos)
{
  // We are interested only in the code before the cursor
  assert(cursor_pos >= 0);

  std::string analysed_code = code.substr(0, static_cast<size_t>(cursor_pos));
  std::string symbol = get_symbol_from_cursor_pos(analysed_code, static_cast<size_t>(cursor_pos));
  auto matches = nl::json::array();

#ifndef NDEBUG
  std::clog << "Completing: " << symbol << std::endl;
#endif

  // Retrieve the completions from the interpreter
  auto const completions = interpreter.feval(
    "completion_matches",
    octave_value(symbol),
    1  // For getting the return value
  );

  if (completions.length())
  {
    for (auto completion : completions(0).string_vector_value().std_list())
    {
      std::string c = completion.substr(0, strlen(completion.c_str()));

      // Trim leading '\0'
#ifndef NDEBUG
      std::clog << c << std::endl;
#endif
      matches.push_back(c);
    }

#ifndef NDEBUG
    std::clog << matches << std::endl;
#endif
  }

  return xeus::create_complete_reply(
    /* matches= */ std::move(matches),
    /* cursor_start= */ cursor_pos - static_cast<int>(symbol.length()),
    /* cursor_end= */ cursor_pos
  );
}

nl::json xoctave_interpreter::inspect_request_impl(std::string const& code, int cursor_pos, int /*detail_level*/)
{
  assert(cursor_pos >= 0);

  // Get the inspected symbol
  std::string symbol = get_symbol_from_cursor_pos(code, static_cast<size_t>(cursor_pos));

#ifndef NDEBUG
  std::clog << "Inspect: " << symbol << std::endl;
#endif

  // Retrieve help for the symbol
  auto data = get_help_for_symbol(interpreter, symbol);

  if (data)
    return xeus::create_inspect_reply(true, *data);
  else
    return xeus::create_inspect_reply(false);
}

nl::json xoctave_interpreter::is_complete_request_impl(std::string const& /*code*/)
{
  // Not implemented
  return xeus::create_is_complete_reply("complete");
}

nl::json xoctave_interpreter::kernel_info_request_impl()
{
  return xeus::create_info_reply(
    "5.3",
    "xoctave",
    XEUS_OCTAVE_VERSION,
    "Octave",
    OCTAVE_VERSION,
    "text/x-octave",
    ".m",
    "octave",
    "octave",
    "",
    octave_startup_message(),
    false,
    nl::json::array()
  );
}

void xoctave_interpreter::shutdown_request_impl()
{
#ifndef NDEBUG
  std::clog << "Bye!!" << std::endl;
#endif
}

}  // namespace xeus_octave
