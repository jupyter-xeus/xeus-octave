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

#ifdef __GNUC__
#include <execinfo.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#endif

#include "xeus-zmq/xzmq_context.hpp"
#include <xeus-zmq/xserver_zmq.hpp>
#include <xeus/xeus_context.hpp>
#include <xeus/xhelper.hpp>
#include <xeus/xkernel.hpp>
#include <xeus/xkernel_configuration.hpp>
#include <xeus/xlogger.hpp>

#include "xeus-octave/config.hpp"
#include "xeus-octave/xinterpreter.hpp"

#ifdef __GNUC__
void handler(int sig)
{
  void* array[10];
  // get void*'s for all entries on the stack
  int size = backtrace(array, 10);
  // print out all the frames to stderr
  fprintf(stderr, "Error: signal %d:\n", sig);
  backtrace_symbols_fd(array, size, STDERR_FILENO);
  exit(1);
}
#endif

std::unique_ptr<xeus::xlogger> make_file_logger(xeus::xlogger::level log_level)
{
  auto logfile = std::getenv("JUPYTER_LOGFILE");
  if (logfile == nullptr)
  {
    return nullptr;
  }
  return xeus::make_file_logger(log_level, logfile);
}

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

  // Registering SIGSEGV handler
#ifdef __GNUC__
  std::clog << "registering handler for SIGSEGV" << std::endl;
  signal(SIGSEGV, handler);
#endif

  std::unique_ptr<xeus::xcontext> context = xeus::make_zmq_context();
  auto interpreter = xeus::xkernel::interpreter_ptr(new xeus_octave::xoctave_interpreter());
  xeus::register_interpreter(interpreter.get());
  auto hist = xeus::make_in_memory_history_manager();
  auto logger = xeus::make_console_logger(xeus::xlogger::full, make_file_logger(xeus::xlogger::full));

  std::string connection_filename = xeus::extract_filename(argc, argv);

  if (!connection_filename.empty())
  {
    xeus::xconfiguration config = xeus::load_configuration(connection_filename);

    std::cout << "Instantiating kernel" << std::endl;
    auto kernel = xeus::xkernel(
      /* config= */ std::move(config),
      /* user_name= */ xeus::get_user_name(),
      /* context= */ std::move(context),
      /* interpreter= */ std::move(interpreter),
      /* sbuilder= */ xeus::make_xserver_default,
      /* history_manager= */ std::move(hist),
      /* logger= */ std::move(logger)
    );

    std::cout << "Starting xoctave kernel...\n\n"
                 "If you want to connect to this kernel from an other client, you can use"
                 " the " +
                   connection_filename + " file."
              << std::endl;
    kernel.start();
  }
  else
  {
    auto kernel = xeus::xkernel(
      /* user_name= */ xeus::get_user_name(),
      /* context= */ std::move(context),
      /* interpreter= */ std::move(interpreter),
      /* sbuilder= */ xeus::make_xserver_default
    );

    std::cout << "Getting config" << std::endl;
    auto const& config = kernel.get_config();
    std::cout << xeus::get_start_message(config);
    kernel.start();
  }

  return 0;
}
