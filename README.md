# `xeus-octave`: A native Octave kernel for jupyter with advanced visualization

[![Binder](https://mybinder.org/badge_logo.svg)](https://mybinder.org/v2/gh/rapgenic/xeus-octave/master?filepath=examples)

`xeus-octave` is an Octave kernel for Jupyter built with Xeus, a C++ framework
for building Jupyter kernels, which means that it is completely native, and does
not run a virtual Octave session in a forked process.

Thanks to its being native, xeus-octave has access to the internal representation
of all Octave objects, and thus it is able to provide advanced visualisation of
many types with ease.

Available visualisations are:

* Image based plot rendering (using Octave native toolkit)
* Experimental plots using [plotly](https://github.com/plotly/plotly.js), with zoom, cursors, and hover tooltips
* Matrices shown as tables
* Structs shown as json
* Symbolic classes as latex
* Transfer functions in `control` package
* Many others coming...

## Installation

Jupyter lab or an alternative interface (e.g. nteract) is of course required.

To compile, first install [xeus](https://github.com/jupyter-xeus/xeus) and
octave. Then run in a terminal:

```
$ mkdir build && cd build
$ cmake ..
$ make
$ sudo make install
```

CMake will download and statically link xeus libraries within the final kernel binary if it cannot find them in the filesystem.

This behaviour can be overridden by setting the CMake `<package>_FETCH` variable to `TRUE`, which forces CMake to explicitly download and build all the following dependencies.

| Library | Variable            |
| ------- | ------------------- |
| cppzmq  | `CPPZMQ_FETCH=TRUE` |
| xtl     | `XTL_FETCH=TRUE`    |
| xeus    | `XEUS_FETCH=TRUE`   |
| glfw3   | `GLFW3_FETCH=TRUE`  |

If you use Jupyter lab you also need the plotly extension (nteract ships it by default):

```
$ jupyter labextension install jupyterlab-plotly
```

### Building for headless systems

Octave uses OpenGL for rendering, which means that it needs a display server to render figures. In order to work on headless systems (e.g. servers) `xeus-octave` supports linking against glfw with osmesa backend (a software based OpenGL implementation). For this set the `GLFW3_OSMESA_BACKEND` variable to `TRUE`.

Usually distributions do not provide glfw with the osmesa backend, so it's probably best to build glfw in-tree with the `GLFW3_FETCH` option.

This is an example invocation:

```
$ mkdir build && cd build
$ cmake -DGLFW3_OSMESA_BACKEND=TRUE -DGLFW3_FETCH=TRUE ..
$ make
$ sudo make install
```

### Arch Linux users

For arch linux users a PKGBUILD has been provided (you still need to install jupyterlab plotly extension)

