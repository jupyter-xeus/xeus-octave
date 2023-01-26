# pyright: reportUnboundVariable=false

import cog # type: ignore
import traitlets
from ipywidgets import widgets
from pathlib import Path
from typing import Type

widget_list = sorted(widgets.Widget._widget_types.items())

widget_name = Path(cog.inFile).stem

for data, _klass in widget_list:
	if data[2] == (widget_name + "Model"):
		klass: Type[widgets.Widget] = _klass
		break

# Instanciate dummy widget
if issubclass(klass, widgets.widget_link.Link):
	widget = klass((widgets.IntSlider(), 'value'), (widgets.IntSlider(), 'value'))
elif issubclass(klass, (widgets.SelectionRangeSlider, widgets.SelectionSlider)):
	widget = klass(options=[1])
else:
	widget = klass()

def classdef():
	cog.outl(f"classdef {widget_name} < __xwidget_internal__")

def properties():
	for trait_name, trait in widget.traits(sync=True).items():
		if trait_name == '_view_count':
			# don't document this since it is totally experimental at this point
			continue

		if trait.help:
			cog.outl(f"## {trait.help}")

		cog.out(trait_name)

		default = trait.default()
		if default is not None:
			if isinstance(trait, (
				traitlets.CaselessStrEnum,
				traitlets.Unicode,
				traitlets.CFloat, traitlets.Float,
				traitlets.CInt, traitlets.Int
			)):
				cog.out(f" = {default}")
			elif isinstance(trait, traitlets.Bool):
				cog.out(f" = {str(default).lower()}")
			elif isinstance(trait, traitlets.Instance):
				# To avoid warning
				pass
			else:
				cog.msg(f"Unhandled default of {trait_name}")

		cog.outl(";")

def constructor():
	cog.outl(f"function obj = {widget_name}()")

def initializers():
	for trait_name, trait in widget.traits(sync=True).items():
		if isinstance(trait, traitlets.Instance) and issubclass(trait.klass, widgets.Widget):
			for data, _klass in widget_list:
				if _klass == trait.klass:
					instance_name = data[2].removesuffix("Model")
					break

			cog.outl(f"obj.{trait_name} = xwidgets.{instance_name};")