# pyright: reportUnboundVariable=false

import cog # type: ignore
import traitlets
from ipywidgets import widgets
from pathlib import Path
from typing import Type
from ipywidgets.widgets.trait_types import TypedTuple, InstanceDict, NumberFormat, Color

widget_list = sorted(widgets.Widget._widget_types.items())
widget_name = Path(cog.outFile).stem

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
			if type(trait) in (
				traitlets.CaselessStrEnum,
				traitlets.Unicode,
				traitlets.CUnicode,
				Color,
				NumberFormat
			):
				cog.out(f" = \"{default}\"")
			elif type(trait) in (
				traitlets.CFloat, traitlets.Float,
				traitlets.CInt, traitlets.Int
			):
				cog.out(f" = {default}")
			elif type(trait) is traitlets.Bool:
				cog.out(f" = {str(default).lower()}")
			elif type(trait) is TypedTuple:
				if type(trait._trait) is traitlets.Unicode:
					elements = [f'"{val}"' for val in default]
					cog.out(f" = {{ {','.join(elements)} }}")
			elif type(trait) in (traitlets.Instance, InstanceDict) and issubclass(trait.klass, widgets.Widget): # type: ignore
				# To avoid warning
				pass
			else:
				cog.msg(f"Unhandled default of {trait_name}")

		cog.outl(";")

def constructor():
	cog.outl(f"function obj = {widget_name}()")

def initializers():
	for trait_name, trait in widget.traits(sync=True).items():
		if type(trait) in (traitlets.Instance, InstanceDict) and issubclass(trait.klass, widgets.Widget): # type: ignore
			if trait_name == '_view_count':
				# don't document this since it is totally experimental at this point
				continue

			for data, _klass in widget_list:
				if _klass == trait.klass:
					instance_name = data[2].removesuffix("Model")
					break

			cog.outl(f"obj.{trait_name} = xwidgets.{instance_name};")

def setters():
	for trait_name, trait in widget.traits(sync=True).items():
		if trait_name == '_view_count':
			# don't document this since it is totally experimental at this point
			continue

		if type(trait) is traitlets.CaselessStrEnum:
			members = [f'"{val}"' for val in trait.values]
			cog.out(f"""
				function set.{trait_name}(obj, value)
					mustBeMember (value, {{ {','.join(members)} }});

					obj.{trait_name} = value;
				endfunction
			""", dedent=True, trimblanklines=False)
		elif type(trait) in (traitlets.Unicode, NumberFormat, Color):
			cog.out(f"""
				function set.{trait_name}(obj, value)
					if !isstring(value)
						error ("input must be a string");
					end

					obj.{trait_name} = value;
				endfunction
			""", dedent=True, trimblanklines=False)
		elif type(trait) is traitlets.CUnicode:
			cog.out(f"""
				function set.{trait_name}(obj, value)
					if !isstring(value)
						obj.{trait_name} = num2str(value);
					else
						obj.{trait_name} = value;
					end
				endfunction
			""", dedent=True, trimblanklines=False)
		elif type(trait) in (traitlets.CFloat, traitlets.Float):
			cog.out(f"""
				function set.{trait_name}(obj, value)
					if !isreal(value) && !isscalar(value)
						error ("input must be a real scalar");
					end

					obj.{trait_name} = value;
				endfunction
			""", dedent=True, trimblanklines=False)
		elif type(trait) in (traitlets.CInt, traitlets.Int):
			cog.out(f"""
				function set.{trait_name}(obj, value)
					if !isreal(value) && !isscalar(value)
						error ("input must be a real scalar");
					end

					obj.{trait_name} = int64(value)
				endfunction
			""", dedent=True, trimblanklines=False)
		elif type(trait) is traitlets.Bool:
			cog.out(f"""
				function set.{trait_name}(obj, value)
					if !islogical(value)
						error ("input must be a logical value");
					end

					obj.{trait_name} = value;
				endfunction
			""", dedent=True, trimblanklines=False)
		elif type(trait) in (traitlets.Instance, InstanceDict) and issubclass(trait.klass, widgets.Widget): # type: ignore
			for data, _klass in widget_list:
				if _klass == trait.klass:
					instance_name = data[2].removesuffix("Model")
					break

			cog.out(f"""
				function set.{trait_name}(obj, value)
					if !isa(value, "xwidgets.{instance_name}")
						error ("input must be instance of xwidgets.{instance_name}");
					end

					obj.{trait_name} = value;
				endfunction
			""", dedent=True, trimblanklines=False)
		elif type(trait) is TypedTuple:
			if type(trait._trait) is traitlets.Unicode:
				cog.out(f"""
					function set.{trait_name}(obj, value)
						if !iscellstr(value)
							error ("input must be an array of strings");
						end

						obj.{trait_name} = value;
					endfunction
				""", dedent=True, trimblanklines=False)
			else:
				cog.msg(f"Unhandled setter of {trait_name} {type(trait)}")
		else:
			cog.msg(f"Unhandled setter of {trait_name} {type(trait)}")
