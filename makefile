################################################################################
# Configuration                                                                #
################################################################################
SHELL := /bin/bash
FLAGS := -O2 -std=c++20 -pipe -Wall -Wextra -Werror -g
SRC_DIR := $(CURDIR)
OUT_DIR := /var/tmp/kodanuki
BIN_DIR := $(OUT_DIR)/build
RUNNABLE := tetris
PROJECTS := unittest kodanuki tetris
.EXPORT_ALL_VARIABLES:
LIBRARY_PATH := $(OUT_DIR)
LD_LIBRARY_PATH := $(OUT_DIR)
.SILENT:

################################################################################
# Make targets                                                                 #
################################################################################
all: compile run

clean:
	rm -rf $(BIN_DIR) $(OUT_DIR)

build:
	mkdir -p $(BIN_DIR) $(OUT_DIR)

ninja: build
	$(NINJA_GENERATOR)

compile: ninja
	ninja -C $(BIN_DIR) $(OUT_DIR)/$(RUNNABLE)

shader:
	glslc engine/graphics/shader/polygon.frag -o engine/graphics/shader/polygon.frag.spv
	glslc engine/graphics/shader/polygon.vert -o engine/graphics/shader/polygon.vert.spv

stats:
	pygount --format=summary

valgrind: compile
	valgrind --leak-check=full $(OUT_DIR)/$(RUNNABLE)

gdb:
	gdb $(OUT_DIR)/$(RUNNABLE)

run:
	$(OUT_DIR)/$(RUNNABLE)

################################################################################
# Python Scripts                                                               #
################################################################################
define NINJA_GENERATOR_SCRIPT
import dataclasses
import functools
import pathlib
import tomli

class Writer:
	def __init__(self):
		self.lines = []

	def write(self, filename):
		with open(filename, 'w') as ninja_file:
			ninja_file.write('\n'.join(self.lines))
			ninja_file.write('\n')

	def newline(self):
		self.lines.append('')

	def variable(self, key, value, indent = 0, use_equals = True):
		prefix = '  ' * indent
		equals = ' = ' if use_equals else ' '
		self.lines.append(f'{prefix}{key}{equals}{value}')

	def rule(self, name, command, variables = {}):
		self.variable('rule', name, use_equals = False)
		self.variable('command', command, indent = 1)
		for key, value in variables.items():
			self.variable(key, value, indent = 1)
		self.newline()

	def build(self, input, output, variables = {}):
		self.lines.append(f'build {output}: {input}')
		for key, value in variables.items():
			self.variable(key, value, indent = 1)

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
		paths = pathlib.Path(folder).rglob('build.toml')
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
	return f'$$src/{name}.cpp'

def output(name):
	return f'$$bin/{name}.o'

def program(name):
	return f'$$out/{name}'

def library(name, prefix = True):
	if prefix:
		return f'$$out/libkodanuki.{name}.so'
	else:
		return f'$$out/lib{name}.so'

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

if __name__ == '__main__':
	writer = Writer()
	writer.variable('src', '$(SRC_DIR)')
	writer.variable('bin', '$(BIN_DIR)')
	writer.variable('out', '$(OUT_DIR)')
	writer.variable('flags', '$(FLAGS)')
	writer.newline()
	writer.rule('cc', 'g++ -I$$src -MD -MF $$out.d -fPIC $$flags -c $$in -o $$out',
	{
		'description': 'compile $$out',
		'depfile': '$$out.d',
		'deps': 'gcc'
	})
	writer.rule('ld', 'g++ $$shared -fPIC $$libs $$in -o $$out',
	{
		'description': 'link $$out'
	})

	for module in Module.discover('$(SRC_DIR)'):
		outputs = []
		for name in module.files('cpp'):
			writer.build('cc ' + source(name), output(name))
			outputs.append(output(name))
		writer.build('ld ' + ' '.join(outputs) + ' | ' + linker_depends(module),
					linker_result(module), linker_params(module))
		writer.newline()

	projects = ' '.join(program(project) for project in "$(PROJECTS)".split())
	if projects:
		writer.variable('default ', projects, use_equals = False)
	writer.write('$(BIN_DIR)/build.ninja')

endef
export NINJA_GENERATOR_SCRIPT
NINJA_GENERATOR := python -c "$$NINJA_GENERATOR_SCRIPT"
