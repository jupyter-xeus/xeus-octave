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

#include <iostream>
#include <memory>
#include <string_view>

#include <xeus-zmq/xserver_zmq.hpp>
#include <xeus/xeus_context.hpp>
#include <xeus/xhelper.hpp>
#include <xeus/xkernel.hpp>
#include <xeus/xkernel_configuration.hpp>
#include <xeus/xlogger.hpp>

#include "xeus-octave/config.hpp"
#include "xeus-octave/xinterpreter.hpp"

int main(int argc, char* argv[])
{

  if (xeus::should_print_version(argc, argv))
  {
    std::cout << "xoctave " << XEUS_OCTAVE_VERSION << '\n';
    return 0;
  }

#ifdef NDEBUG
  // If we are called from the Jupyter launcher, silence all logging. This
  // is important for a JupyterHub configured with cleanup_servers = False:
  // Upon restart, spawned single-user servers keep running but without the
  // std* streams. When a user then tries to start a new kernel, xoctave
  // will get a SIGPIPE and exit.
  if (std::getenv("JPY_PARENT_PID") != NULL)
  {
    std::clog.setstate(std::ios_base::failbit);
  }
#endif

  auto interpreter = xeus::xkernel::interpreter_ptr(new xeus_octave::xoctave_interpreter());
  xeus::register_interpreter(interpreter.get());
  auto config = xeus::load_configuration(xeus::extract_filename(argc, argv));
  std::cout << xeus::print_starting_message(config);

  auto kernel = xeus::xkernel(
    /* config= */ std::move(config),
    /* user_name= */ xeus::get_user_name(),
    /* context= */ xeus::make_context<zmq::context_t>(),
    /* interpreter= */ std::move(interpreter),
    /* sbuilder= */ xeus::make_xserver_zmq
  );
  kernel.start();

  return 0;
}
