from textwrap import indent

from ipywidgets import widgets
from traitlets import CaselessStrEnum, Instance, Unicode, Bool, CFloat, Float, CInt, Int
from ipywidgets.widgets.trait_types import TypedTuple

def setter(name, incode):
	code = [
		"\n",
		f"function set.{name}(obj, value)",
		indent(incode, "\t"),
		"endfunction"
	]
	return '\n'.join(code)

def enum_setter(name, values):
	members = [f'"{val}"' for val in values]
	return setter(name, f"mustBeMember(value, {{ {','.join(members)} }});")

def cellstr_setter(name):
	code = [
		"if ! iscellstr(value)",
		'	error ("input must be a cellstr");',
	 	"end"
	]
	return setter(name, '\n'.join(code))

def instance_setter(name, instance_name):
	code = [
		f'if ! isa(value, "xwidgets.{instance_name}")',
		f'	error ("input must be instance of {instance_name}");',
		"end"
	]
	return setter(name, '\n'.join(code))

if __name__ == '__main__':
	widget_list = sorted(widgets.Widget._widget_types.items())

	for widget_data, klass in widget_list:
		# Name
		widget_name = widget_data[2].removesuffix("Model")

		# Widget model and view data
		model = dict(zip(['module', 'version', 'name'], widget_data[:3]))
		view = dict(zip(['module', 'version', 'name'], widget_data[3:]))

		# Instanciate dummy widget
		if issubclass(klass, widgets.widget_link.Link):
			widget = klass((widgets.IntSlider(), 'value'),
								(widgets.IntSlider(), 'value'))
		elif issubclass(klass, (widgets.SelectionRangeSlider,
										widgets.SelectionSlider)):
			widget = klass(options=[1])
		else:
			widget = klass()

		with open(f"{widget_name}.m", "w") as cd:
			setters = []
			initializers = {}

			cd.write(f"classdef {widget_name} < __xwidget_internal__\n")
			cd.write("\n\tproperties (Sync = true)\n\n")

			for name, trait in widget.traits(sync=True).items():
				if name == '_view_count':
					# don't document this since it is totally experimental at this point
					continue

				if trait.help:
					cd.write(f"\t\t## {trait.help}\n")

				cd.write(f"\t\t{name}")

				if isinstance(trait, CaselessStrEnum):
					setters.append(indent(enum_setter(name, trait.values), '\t\t'))
					if trait.default() is not None:
						cd.write(f" = \"{trait.default()}\"")
				elif isinstance(trait, Unicode):
					if trait.default() is not None:
						cd.write(f" = \"{trait.default()}\"")
				elif isinstance(trait, TypedTuple):
					if isinstance(trait._trait, Unicode):
						setters.append(indent(cellstr_setter(name), '\t\t'))
				elif isinstance(trait, Bool):
					if trait.default() is not None:
						cd.write(f" = {str(trait.default()).lower()}")
				elif isinstance(trait, (CFloat, Float)):
					if trait.default() is not None:
						cd.write(f" = {trait.default()}")
				elif isinstance(trait, (CInt, Int)):
					if trait.default() is not None:
						cd.write(f" = {trait.default()}")
				elif isinstance(trait, Instance) and issubclass(trait.klass, widgets.Widget):
					for widget_data, klass in widget_list:
						if klass == trait.klass:
							instance_name = widget_data[2].removesuffix("Model")
							break

					setters.append(indent(instance_setter(name, instance_name), '\t\t'))
					initializers[name] = f"xwidgets.{instance_name}"
				else:
					print(f"Unhandled property {name} of {widget_name}")

				cd.write(f";\n")

			cd.write("\n\tend\n")

			# Methods section, setters and constructor.
			if setters or initializers:
				cd.write("\n\tmethods (Sync = true)\n")

				# Properties to be initialized in constructor
				if initializers:
					cd.write(f"\n\t\tfunction obj = {klass.__name__}()\n")
					for name, value in initializers.items():
						cd.write(f"\t\t\tobj.{name} = {value};\n")
					cd.write("\t\tendfunction")

				for s in setters:
					cd.write(s)

				cd.write("\n\tend\n")

			cd.write("\nend\n")
