#ifndef INPUT_H
#define INPUT_H

#include "octave/cmd-edit.h"

namespace xoctave {

class input : public octave::command_editor {
public:
	static void set_command_editor(octave::command_editor &n);
	static void reset_command_editor();

	std::string do_readline(const std::string &prompt, bool &) override;
	void do_set_input_stream(FILE *) override {}
	FILE *do_get_input_stream(void) override { return nullptr; }
	void do_set_output_stream(FILE *) override {}
	FILE *do_get_output_stream(void) override { return nullptr; }
	string_vector do_generate_filename_completions(const std::string &) override { return {}; }
	std::string do_get_line_buffer(void) const override { return ""; }
	std::string do_get_current_line(void) const override { return ""; }
	char do_get_prev_char(int) const override { return '\0'; }
	void do_replace_line(const std::string &, bool) override {}
	void do_kill_full_line(void) override {}
	void do_insert_text(const std::string &) override {}
	void do_newline(void) override {}
	void do_accept_line(void) override {}
};

}  // namespace xoctave

#endif