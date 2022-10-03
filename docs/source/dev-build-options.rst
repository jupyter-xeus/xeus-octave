.. Copyright (c) 2020, Giulio Girardi

   Distributed under the terms of the BSD 3-Clause License.

   The full license is in the file LICENSE, distributed with this software.

Build and configuration
=======================

General Build Options
---------------------

Building the xeus-octave library
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
``xeus-octave`` build supports the following options:

- ``XEUX_OCTAVE_BUILD_SHARED``: Build the xeus-octave shared library. **Enabled by default**.
- ``XEUS_OCTAVE_BUILD_STATIC``: Build the xeus-octave static library. **Enabled by default**.

Xeus-octave must link with xeus dynamically or statically.

- ``XEUS_OCTAVE_USE_SHARED_XEUS``: Link with the xeus shared library (instead of the static library).
  **Enabled by default**.

Running the Tests
~~~~~~~~~~~~~~~~~
The kernels tests can be run with

.. code-block:: bash

   python -m pytest test/
