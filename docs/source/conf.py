#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import os
import subprocess

import sphinx_rtd_theme

on_rtd = os.environ.get("READTHEDOCS", None) == "True"

if on_rtd:
    subprocess.call("cd ..; doxygen", shell=True)

html_theme = "sphinx_rtd_theme"

html_theme_path = [sphinx_rtd_theme.get_html_theme_path()]


def setup(app):
    app.add_css_file("main_stylesheet.css")


extensions = ["breathe", "sphinx_rtd_theme"]
breathe_projects = {"xeus-octave": "../xml"}
templates_path = ["_templates"]
html_static_path = ["_static"]
source_suffix = ".rst"
master_doc = "index"
project = "xeus-octave"
copyright = "2020, Giulio Girardi"
author = "Giulio Girardi"
html_logo = "quantstack-white.svg"
exclude_patterns = []
highlight_language = "c++"
pygments_style = "sphinx"
todo_include_todos = False
htmlhelp_basename = "xeus-octavedoc"
