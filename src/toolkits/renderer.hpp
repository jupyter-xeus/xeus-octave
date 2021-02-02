#ifndef RENDERER_H
#define RENDERER_H

#include <octave/graphics.h>

#include <functional>
#include <memory>
#include <thread>

namespace xoctave {

class renderer {
protected:
	renderer() {};

public:
    renderer(renderer const&) = delete;
    void operator=(renderer const&) = delete;

	static renderer &get();
	virtual void draw(int width, int height, double dpr, const graphics_object& go) = 0;
	virtual void get_pixels(int width, int height, void* pixels) = 0;
	virtual double get_dpr() = 0;
};

}  // namespace xoctave

#endif