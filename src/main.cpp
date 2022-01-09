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

#include <memory>
#include <xeus/xkernel.hpp>
#include <xeus/xkernel_configuration.hpp>
#include <xeus/xserver_zmq_split.hpp>

#include "xoctave_interpreter.hpp"

int main(int argc, char* argv[]) {
	// Load configuration file
	std::string file_name = (argc == 1) ? "connection.json" : argv[2];
	xeus::xconfiguration config = xeus::load_configuration(file_name);

	auto context = xeus::make_context<zmq::context_t>();

	// Create interpreter instance
	using interpreter_ptr = std::unique_ptr<xoctave::xoctave_interpreter>;
	interpreter_ptr interpreter = interpreter_ptr(new xoctave::xoctave_interpreter());
	xeus::register_interpreter(interpreter.get());

	// Create kernel instance and start it
	xeus::xkernel kernel(config,
						 xeus::get_user_name(),
						 std::move(context),
						 std::move(interpreter),
						 xeus::make_xserver_zmq);
	kernel.start();

	return 0;
}
