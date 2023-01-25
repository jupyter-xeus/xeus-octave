from ipywidgets import widgets
from traitlets import CaselessStrEnum, Instance, Unicode, Bool, CFloat, Float, CInt, Int
from ipywidgets.widgets.trait_types import TypedTuple

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
			initializers = []

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
					setters.append(f"""
		function set.{name}(obj, value)
			mustBeMember(value, {{ {",".join([f'"{val}"' for val in trait.values])} }});
		endfunction\n""")
					if trait.default() is not None:
						cd.write(f" = \"{trait.default()}\"")
				elif isinstance(trait, Unicode):
					if trait.default() is not None:
						cd.write(f" = \"{trait.default()}\"")
				elif isinstance(trait, TypedTuple):
					if isinstance(trait._trait, Unicode):
						setters.append(f"""
		function set.{name}(obj, value)
			if ! iscellstr(value)
				error ("Error");
			end
		endfunction\n""")
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

					setters.append(f"""
		function set.{name}(obj, value)
			if ! isa(value, "xwidgets.{instance_name}")
				error ("Error");
			end
		endfunction\n""")
					initializers.append(f"{name} = xwidgets.{instance_name}")

					# if (trait.klass not in [i[1] for i in widget_list] and trait.klass is not widgets.Widget):
					# 	widget_list.append((trait.klass.__name__, trait.klass))
				else:
					print(f"Unhandled property {name} of {widget_name}")

				cd.write(f";\n")

			cd.write("\n\tend\n")

			if setters or initializers:
				cd.write("\n\tmethods (Sync = true)\n")

				# Properties to be initialized in constructor
				if initializers:
					cd.write(f"\n\t\tfunction obj = {klass.__name__}()\n")
					for initializer in initializers:
						cd.write(f"\t\t\tobj.{initializer};\n")
					cd.write("\t\tendfunction\n")

				for setter in setters:
					cd.write(setter)

				cd.write("\n\tend\n")

			cd.write("\nend\n")
