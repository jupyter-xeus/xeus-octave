..  Copyright (c) 2020, Giulio Girardi

   Distributed under the terms of the GNU General Public License v3.

   The full license is in the file LICENSE, distributed with this software.

.. image:: xeus-octave-logo.svg
   :alt: Xeus Octave logo


Introduction
============
*Xeus Octave* is a Jupyter kernel for Octave based on the native implementation of
the C++ Jupyter protocol xeus_.
This means that it is completely native, and does not run a virtual Octave session
in a forked process.

Thanks to its being native, *Xeus Octave* has access to the internal representation
of all Octave objects, and thus it is able to provide advanced visualisation of
many types with ease.
See the :ref:`usage <usage>` section for available features.

Licensing
=========
Distributed under the terms of the GNU General Public License v3.
The full license is in the file LICENSE, distributed with this software.

.. toctree::
   :caption: Getting Started
   :maxdepth: 2
   :hidden:

   self
   installation.rst

.. toctree::
   :caption: Usage
   :maxdepth: 2
   :hidden:

   usage.rst

.. toctree::
   :caption: Developer Zone
   :maxdepth: 2
   :hidden:

   contributing.rst
   dev-build-options.rst

.. _xeus: https://github.com/jupyter-xeus/xeus-octave
