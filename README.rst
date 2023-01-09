.. image:: docs/source/img/xeus-octave-logo.svg
   :alt: Xeus Octave

.. image:: https://github.com/jupyter-xeus/xeus-octave/actions/workflows/main.yml/badge.svg
   :target: https://github.com/jupyter-xeus/xeus-octave/actions/workflows/main.yml
   :alt: Build status on Github Actions

.. image:: https://readthedocs.org/projects/xeus-python/badge/?version=latest
   :target: https://xeus-octave.readthedocs.io/en/latest/
   :alt: Documentation status on Readthedocs

.. image:: https://mybinder.org/badge_logo.svg
   :target: https://mybinder.org/v2/gh/jupyter-xeus/xeus-octave/main?urlpath=/lab/tree/notebooks/xeus-octave.ipynb
   :alt: Run on Binder

.. image:: https://badges.gitter.im/Join%20Chat.svg
   :target: https://gitter.im/QuantStack/Lobby?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge
   :alt: Join the Gitter Chat

.. Used for including in Sphinx doc
.. [[INTRODUCTION CONTENT START]]

*Xeus Octave* is a Jupyter kernel for Octave based on the native implementation of
the C++ Jupyter protocol `Xeus <https://github.com/jupyter-xeus/xeus-octave>`_.
This means that it is completely native, and does not run a virtual Octave session
in a forked process.

Thanks to its being native, *Xeus Octave* has access to the internal representation
of all Octave objects, and thus it is able to provide advanced visualisation of
many types with ease.

.. [[INTRODUCTION CONTENT END]]

Features
--------
Graphical Toolkits
~~~~~~~~~~~~~~~~~~
Native Octave graphical toolkit.

.. image:: docs/source/img/screenshots/plot.png
   :alt: A two dimensional function plot is displayed.

Plotly graphical toolkit.

.. image:: docs/source/img/screenshots/plotly.gif
   :alt: An interactive two dimensional function plot is displayed, the view and zoom can be changed

Completion and Outputs
~~~~~~~~~~~~~~~~~~~~~~
Error are properly reported.

.. image:: docs/source/img/screenshots/error.png
   :alt: A function that does not exist get called, raithing an error.

Code completion suggests possible matches.

.. image:: docs/source/img/screenshots/completion.png
   :alt: A dropdown window show completion matches for an incomplete function name.

Help can be queried using a ``?`` after a function name.

.. image:: docs/source/img/screenshots/help.png
   :alt: The documentaion for of a function is show as a result of executing the cell.

Rich Output
~~~~~~~~~~~
Matrices are displayed as tables.

.. image:: docs/source/img/screenshots/table.png
   :alt: A matrix is shown in HTML and Latex format.

Data structures are displayed as json.

.. image:: docs/source/img/screenshots/struct.png
   :alt: A data structure is can be explored with folds and intends.

Symbolic classes are displayed as latex.

.. image:: docs/source/img/screenshots/formula.png
   :alt: An explession is shown as a Latex formula.

Images can be displayed.

.. image:: docs/source/img/screenshots/image.png
   :alt: An grayscale image of uniform random number is displayed.

Installation
------------
.. Used for including in Sphinx doc
.. [[INSTALLATION CONTENT START]]
With mamba (or conda)
~~~~~~~~~~~~~~~~~~~~~
*Xeus Octave* has been packaged for the mamba (or conda) package manager.

To ensure that the installation works, it is preferable to install ``xeus-octave`` in a fresh
environment.
It is also needed to use a miniforge_ or miniconda_ installation because with the full anaconda_
you may have a conflict with the ``zeromq`` library which is already installed in the anaconda
distribution.

The safest usage is to create an environment named ``xeus-octave``

.. code::

    mamba create -n myenv -c conda-forge xeus-octave
    mamba activate -n myenv

Then you can install in this freshly created environment other dependencies, such as ``notebook``
or JupyterLab_

.. code::

    mamba install -c conda-forge jupyterlab

From Source
~~~~~~~~~~~
You can install ``xeus-octave`` from source with Cmake.
This requires that you have all the dependencies installed in the same prefix, for instance a
superset of these dependencies can be found in the file ``environment-dev.yml``.

.. code:: bash

   mamba install --file environment-dev.yml

Then you can install in the same Conda environment with

.. code:: bash

    cmake -B build -D CMAKE_PREFIX_PATH="${CONDA_PREFIX}" -D CMAKE_INSTALL_PREFIX="${CONDA_PREFIX}"
    cmake --build build --parallel 4
    cmake --install build

.. Sphink linkcheck fails on this anchor https://github.com/conda-forge/miniforge#mambaforge
.. _miniforge: https://github.com/conda-forge/miniforge
.. _miniconda: https://conda.io/miniconda.html
.. _anaconda: https://www.anaconda.com
.. _JupyterLab: https://jupyterlab.readthedocs.io
.. _xeus-octave-wheel: https://github.com/jupyter-xeus/xeus-octave-wheel

.. [[INSTALLATION CONTENT END]]

Trying it online
----------------
To try out ``xeus-octave`` interactively in your web browser, just click on the binder link:

.. image:: docs/source/img/binder-logo.svg
   :target: https://mybinder.org/v2/gh/jupyter-xeus/xeus-octave/main?urlpath=/lab/tree/notebooks/xeus-octave.ipynb
   :alt: Run on Binder

Documentation
-------------
To get started with using *Xeus Octave*, check out the full documentation
`xeus-octave.readthedocs.io <https://xeus-octave.readthedocs.io>`_

Contributing
------------
See `CONTRIBUTING.rst <./CONTRIBUTING.rst>`_ to know how to contribute and set up a
development environment.

License
-------
This software is licensed under the *GNU General Public License v3*.
See the `LICENSE <LICENSE>`_ file for details.
