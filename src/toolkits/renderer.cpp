#include "renderer.hpp"

#include <octave/gl-render.h>
#include <octave/graphics.h>

#include <iostream>
#include <memory>
#include <mutex>

#include "config.h"

#ifdef NATIVE_RENDERER_QT
#include <QGuiApplication>
#include <QOffscreenSurface>
#include <QOpenGLContext>
#include <QScreen>
#include <QOpenGLFramebufferObject>

#include "qopengl.hpp"
#endif

namespace xoctave {

#ifdef NATIVE_RENDERER_QT

class renderer_qt : public renderer {
public:
	renderer_qt()
		: thread(std::bind(&renderer_qt::run, this)),
		  m_renderer(m_glfncs) {
	}

	void draw(int width, int height, double dpr, const graphics_object& go) override {
		std::lock_guard<std::mutex> l(m);
		QOpenGLFramebufferObject fbo(width, height, QOpenGLFramebufferObject::Attachment::Depth);
		fbo.bind();
		m_renderer.set_viewport(width, height);
		m_renderer.set_device_pixel_ratio(dpr);
		m_renderer.draw(go);
		fbo.release();
	}

	double get_dpr() override {
		std::lock_guard<std::mutex> l(m);
		return p_surface->screen()->devicePixelRatio();
	}

	void get_pixels(int width, int height, void* pixels) override {
		std::lock_guard<std::mutex> l(m);
		m_glfncs.glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, pixels);
	}

private:
	void run() {
		int argc;
		char* argv[] = {};
		QGuiApplication a(argc, argv);

		p_context = new QOpenGLContext();
		p_context->create();

		if (!p_context->isValid())
			std::clog << "Cannot create opengl context" << std::endl;

		p_surface = new QOffscreenSurface();
		p_surface->create();
		if (!p_surface->isValid())
			std::clog << "Cannot create opengl context" << std::endl;

		p_context->makeCurrent(p_surface);

		std::clog << "Starting renderer thread" << std::endl;

		a.exec();
	}

	std::thread thread;
	std::mutex m;

	QOpenGLContext* p_context;
	QOffscreenSurface* p_surface;

	octave::qopengl_functions m_glfncs;
	octave::opengl_renderer m_renderer;
};

#endif

renderer& renderer::get() {
#ifdef NATIVE_RENDERER_QT
	static renderer_qt instance;
#endif

	return instance;
}

}  // namespace xoctave