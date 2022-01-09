/***************************************************************************
 * Copyright (c) 2022, Giulio Girardi
 *
 * Distributed under the terms of the GNU General Public License v3.
 *
 * The full license is in the file LICENSE, distributed with this software.
 ****************************************************************************/

#ifndef XEUS_OCTAVE_INTERPRETER_HPP
#define XEUS_OCTAVE_INTERPRETER_HPP

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wattributes"
#endif

#include <octave/interpreter.h>

#include <memory>
#include <string>

#include "nlohmann/json.hpp"
#include "xeus-octave/input.hpp"
#include "xeus-octave/output.hpp"
#include "xeus/xinterpreter.hpp"
#include "xeus_octave_config.hpp"

namespace nl = nlohmann;

namespace xeus_octave {
class XEUS_OCTAVE_API xoctave_interpreter : public xeus::xinterpreter {
private:
	octave::interpreter interpreter;

public:
	xoctave_interpreter();
	virtual ~xoctave_interpreter() = default;

protected:
	void configure_impl() override;

	nl::json execute_request_impl(int execution_counter,
								  const std::string& code,
								  bool silent,
								  bool store_history,
								  nl::json user_expressions,
								  bool allow_stdin) override;

	nl::json complete_request_impl(const std::string& code, int cursor_pos) override;

	nl::json inspect_request_impl(const std::string& code,
								  int cursor_pos,
								  int detail_level) override;

	nl::json is_complete_request_impl(const std::string& code) override;

	nl::json kernel_info_request_impl() override;

	void shutdown_request_impl() override;

private:
	io::xoctave_output m_stdout{"stdout"};
	io::xoctave_output m_stderr{"stderr"};
	io::xoctave_input m_stdin;
};
}  // namespace xeus_octave

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

#endif
