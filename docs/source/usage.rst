.. Copyright (c) 2020, Giulio Girardi

   Distributed under the terms of the BSD 3-Clause License.

   The full license is in the file LICENSE, distributed with this software.

.. _usage:

Usage
=====

Launch the Jupyter notebook with ``jupyter notebook`` or Jupyter lab with ``jupyter lab`` and launch
a new Python notebook by selecting the **xoctave** kernel.

Xeus-Octave can execute any piece of Octave code.

.. image:: img/screenshots/code.png
   :alt: A variable is assigned and it value is shown.

Error are properly reported.

.. image:: img/screenshots/error.png
   :alt: A function that does not exist get called, raithing an error.

Output streams can also be used directly.

.. image:: img/screenshots/output.png
   :alt: A message is printed to stdout and stdin.

Code completion suggests possible matches.

.. image:: img/screenshots/completion.png
   :alt: A dropdown window show completion matches for an incomplete function name.

Help can be queried using a ``?`` after a function name.

.. image:: img/screenshots/help.png
   :alt: The documentaion for of a function is show as a result of executing the cell.

Visualisation
=============
Graphical toolkit
-----------------
*Xeus Octave* comes with two Ocatve graphical toolkit for rendering plots.

Notebook toolkit
~~~~~~~~~~~~~~~~
The ``notebook`` toolkit renders plots as inline images in the notebook.
The render is done natively using OpenGL on the kernel side.
Select this toolkit using ``graphics_toolkit notebook`` in the notebook (this is the default).

.. image:: img/screenshots/plot.png
   :alt: A two dimensional function plot is displayed.

Plotly toolkit
~~~~~~~~~~~~~~
The experimental ``plotly`` toolkit calls `Plotly <https://github.com/plotly/plotly.js>`_
web API to render plot as interactive HTML elements, with zoom, cursors, and hover tooltips.
Select this toolkit using ``graphics_toolkit plotly`` in the notebook.

.. image:: img/screenshots/plotly.gif
   :alt: An interactive two dimensional function plot is displayed, the view and zoom can be changed

With Jupyter lab, the plotly extension is also needed.
It can be installed with ``conda``/``mamba`` (or ``pip``)

.. code:: bash

   mamba install -c conda-forge jupyterlab plotly ipywidgets jupyter-dash

The extension is already provided with Nteract.

See `Plotly documentation <https://plotly.com/python/getting-started/>`_
for detailed instructions and troubleshooting.

Other
-----
Matrices are displayed as tables.

.. image:: img/screenshots/table.png
   :alt: A matrix is shown in HTML and Latex format.

Data structures are displayed as json.

.. image:: img/screenshots/struct.png
   :alt: A data structure is can be explored with folds and intends.

Symbolic classes are displayed as latex.

.. image:: img/screenshots/formula.png
   :alt: An explession is shown as a Latex formula.

Images can be displayed.

.. image:: img/screenshots/image.png
   :alt: An grayscale image of uniform random number is displayed.
