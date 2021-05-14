# `xeus-octave`: A native Octave kernel for jupyter with advanced visualization

`xeus-octave` is an Octave kernel for Jupyter built with Xeus, a C++ framework
for building Jupyter kernels, which means that it is completely native, and does
not run a virtual Octave session in a forked process.

Thanks to its being native, xeus-octave has access to the internal representation
of all Octave objects, and thus it is able to provide advanced visualisation of
many types with ease.

Available visualisations are:

* Advanced plots using [plotly](https://github.com/plotly/plotly.js), with zoom, cursors, and hover tooltips
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

If you use Jupyter lab you also need the plotly extension (nteract ships it by default):

```
$ jupyter labextension install jupyterlab-plotly
```

### Arch Linux users

For arch linux users a PKGBUILD has been provided (you still need to install jupyterlab plotly extension)

