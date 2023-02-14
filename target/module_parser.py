import dataclasses
import pathlib
import tomli


@dataclasses.dataclass
class Module:
	name: str
	description: str
	path: str
	depends: list[str]
	library: bool

	def files(self, extension):
		paths = pathlib.Path(self.path).rglob(f'*.{extension}')
		normalize = lambda path: ''.join(str(path).split('.')[:-1])
		return set(map(normalize, paths))

	@staticmethod
	def build_files(folder):
		paths = pathlib.Path(folder).rglob('modules.toml')
		normalize = lambda path: path.relative_to(folder)
		yield from map(normalize, paths)

	@staticmethod
	def build_configs(build_file):
		with open(build_file, 'rb') as build_file:
			data = tomli.load(build_file)
		for name, config in data.items():
			config['name'] = name
			yield config

	@staticmethod
	def discover(folder):
		for build_file in Module.build_files(folder):
			for config in Module.build_configs(build_file):
				yield Module(**config)


def source(name):
	return f'$src/{name}.cpp'

def output(name):
	return f'$bin/{name}.o'

def program(name):
	return f'$out/{name}'

def library(name, prefix = True):
	if prefix:
		return f'$out/libkodanuki.{name}.so'
	else:
		return f'$out/lib{name}.so'

def linker_params(module):
	params = {
		'libs': ' '.join(f'-l{lib}' for lib in module.depends),
		'shared': '-shared' if module.library else ''
	}
	return params

def linker_depends(module):
	deps = [name for name in module.depends if name.startswith('kodanuki')]
	return ' '.join(library(name, prefix = False) for name in deps)

def linker_result(module):
	if module.library:
		return library(module.name, prefix = True)
	else:
		return program(module.name)
