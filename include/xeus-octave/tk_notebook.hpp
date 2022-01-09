/***************************************************************************
* Copyright (c) 2022, Giulio Girardi
*
* Distributed under the terms of the GNU General Public License v3.
*
* The full license is in the file LICENSE, distributed with this software.
****************************************************************************/


#ifndef XEUS_OCTAVE_TK_NOTEBOOK_HPP
#define XEUS_OCTAVE_TK_NOTEBOOK_HPP

#include <GLFW/glfw3.h>
#include <octave/graphics-toolkit.h>
#include <octave/interpreter.h>

#include <memory>
#include <mutex>
#include <nlohmann/json.hpp>

#include "xeus-octave/opengl.hpp"

namespace xeus_octave::tk::notebook {
	class notebook_graphics_toolkit : public octave::base_graphics_toolkit {
		public:
			notebook_graphics_toolkit(octave::interpreter &);
			~notebook_graphics_toolkit();

			bool is_valid() const override { return true; }

			bool initialize(const graphics_object &) override;
			void redraw_figure(const graphics_object &) const override;
			void show_figure(const graphics_object &) const override;
			void update(const graphics_object &, int) override;

			void finalize(const graphics_object &) override;

		private:
			octave::interpreter &m_interpreter;

			GLFWwindow *window = nullptr;
	};

	void register_all(octave::interpreter &interpreter);
}

#endif