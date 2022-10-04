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

Installing the Kernel
---------------------

.. include:: ../../README.rst
   :start-after: [[INSTALLATION CONTENT START]]
   :end-before:  [[INSTALLATION CONTENT END]]

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
