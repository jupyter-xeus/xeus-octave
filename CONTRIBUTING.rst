Contributing to Xeus-Octave
===========================

.. Used for including in Sphinx doc
.. [[CONTENT START]]

*Xeus* and *Xeus-Octave* are subprojects of Project Jupyter and subject to the
`Jupyter governance <https://github.com/jupyter/governance>`_ and
`Code of conduct <https://github.com/jupyter/governance/blob/master/conduct/code_of_conduct.md>`_.

General Guidelines
~~~~~~~~~~~~~~~~~~
For general documentation about contributing to Jupyter projects, see the
`Project Jupyter Contributor Documentation
<https://docs.jupyter.org/en/latest/contributing/content-contributor.html>`_.

Community
~~~~~~~~~
The Xeus team organizes public video meetings. The schedule for future meetings and
minutes of past meetings can be found on our
`team compass <https://jupyter-xeus.github.io/>`_.

Setting up a development environment
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
First, you need to fork the project.
Then setup your environment:

.. code:: bash

   # Create a new conda environment
   mamba create -n xeus-octave -f environment-dev.yml
   mamba activate -n xeus-octave

   # Download xeus-octave from your GitHub fork
   git clone https://github.com/<your-github-username>/xeus-octave.git

Development tools such as a C++ compiler, CMake, *etc.* are provided in
the ``environment-dev.yml`` dependency file.

Building and installing Xeus-Octave
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code:: bash

   # Generate the project configuration files with Cmake in "build" directory
   cmake -B build \
       -D CMAKE_BUILD_TYPE=Release  \
       -D CMAKE_PREFIX_PATH="${CONDA_PREFIX}" \
       -D CMAKE_INSTALL_PREFIX="${CONDA_PREFIX}"
   # Build xeus-octave
   cmake --build build --parallel 2
   # Install xeus-octave
   cmake --install build

Running the tests
~~~~~~~~~~~~~~~~~
The kernel tests are implemented through a Python test suite.

.. code:: bash

   python -m pytest test/

.. [[CONTENT END]]
