..  Copyright (c) 2020, Giulio Girardi

   Distributed under the terms of the BSD 3-Clause License.

   The full license is in the file LICENSE, distributed with this software.

.. raw:: html

   <style>
   .rst-content .section>img {
       width: 30px;
       margin-bottom: 0;
       margin-top: 0;
       margin-right: 15px;
       margin-left: 15px;
       float: left;
   }
   </style>

Installation
============
With mamba (or conda)
---------------------
*Xeus Octave* has been packaged for the mamba (or conda) package manager.

To ensure that the installation works, it is preferable to install ``xeus-octave`` in a fresh
environment.
It is also needed to use a miniforge_ or miniconda_ installation because with the full anaconda_
you may have a conflict with the `zeromq` library which is already installed in the anaconda
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
-----------
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

Installing the Kernel Spec
--------------------------

When installing xeus-octave in a given installation prefix, the corresponding Jupyter kernelspecs
are installed in the same environment and are automatically picked up by Jupyter if it is installed
in the same prefix.

However, if Jupyter is installed in a different location, it will not pick up the new kernel.
The xeus-octave can be registered with the following command:

.. code:: bash

   jupyter kernelspec install PREFIX/share/jupyter/kernels/xoctave --sys-prefix

For more information on the ``jupyter kernelspec`` command, please consult the
``jupyter_client`` documentation.

.. Sphink linkcheck fails on this anchor https://github.com/conda-forge/miniforge#mambaforge
.. _miniforge: https://github.com/conda-forge/miniforge
.. _miniconda: https://conda.io/miniconda.html
.. _anaconda: https://www.anaconda.com
.. _JupyterLab: https://jupyterlab.readthedocs.io
.. _xeus-octave-wheel: https://github.com/jupyter-xeus/xeus-octave-wheel
