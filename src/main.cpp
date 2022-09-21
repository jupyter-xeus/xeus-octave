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
#include <xeus/xserver_zmq.hpp>

#include "xeus-octave/xinterpreter.hpp"

int main(int argc, char* argv[]) {
	// Configuration file
	auto const* const file_name = (argc == 1) ? "connection.json" : argv[2];
	auto interpreter = xeus::xkernel::interpreter_ptr(new xeus_octave::xoctave_interpreter());
	xeus::register_interpreter(interpreter.get());

	auto kernel = xeus::xkernel(
		xeus::load_configuration(file_name),
		xeus::get_user_name(),
		xeus::make_context<zmq::context_t>(),
		std::move(interpreter),
		xeus::make_xserver_zmq
	);
	kernel.start();

	return 0;
}
