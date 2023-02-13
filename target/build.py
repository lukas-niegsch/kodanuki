import argparse
import dataclasses
import functools
import pathlib
import tomli
import subprocess
import os

FLAGS = "-O2 -std=c++2b -Wall -Wextra -Werror -g"
PROJECTS = "unittest kodanuki tetris splash"


def parse_arguments():
	parser = argparse.ArgumentParser(description='The Kodanuki build script.')
	parser.add_argument('src_dir', help = 'The projects source directory.')
	parser.add_argument('out_dir', help = 'The projects output directory.')
	parser.add_argument('bin_dir', help = 'The projects build directory.')
	parser.add_argument('project', help = 'The current runnable project.')
	parser.add_argument('target', help = 'The build target for this script.')
	return parser.parse_args()


class NinjaWriter:
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

def target_ninja(args):
	writer = NinjaWriter()
	writer.variable('src', args.src_dir)
	writer.variable('bin', args.bin_dir)
	writer.variable('out', args.out_dir)
	writer.variable('flags', FLAGS)
	writer.newline()
	writer.rule('cc', 'g++ -I$src/extern/imgui -I$src -MD -MF $out.d -fPIC $flags -c $in -o $out',
	{
		'description': 'compile $out',
		'depfile': '$out.d',
		'deps': 'gcc'
	})
	writer.rule('ld', 'g++ $shared -fPIC $libs $in -o $out',
	{
		'description': 'link $out'
	})

	for module in Module.discover(args.src_dir):
		outputs = []
		for name in module.files('cpp'):
			writer.build('cc ' + source(name), output(name))
			outputs.append(output(name))
		writer.build('ld ' + ' '.join(outputs) + ' | ' + linker_depends(module),
					linker_result(module), linker_params(module))
		writer.newline()

	projects = ' '.join(program(project) for project in PROJECTS.split())
	if projects:
		writer.variable('default ', projects, use_equals = False)
	writer.write(f'{args.bin_dir}/build.ninja')

def execute_target(args, target):
	if target == 'help':
		print(args)
	elif target == 'ninja':
		execute_target(args, 'build')
		target_ninja(args)
	elif target == 'clean':
		subprocess.call(f'rm -rf {args.bin_dir} {args.out_dir}', shell = True)
	elif target == 'build':
		subprocess.call(f'mkdir -p {args.bin_dir} {args.out_dir}', shell = True)
	elif target == 'compile':
		execute_target(args, 'ninja')
		subprocess.call(f'ninja -C {args.bin_dir} {args.out_dir}/{args.project}', shell = True)
	elif target == 'stats':
		subprocess.call(f'pygount --format=summary engine source', shell = True)
	elif target == 'valgrind':
		execute_target(args, 'compile')
		subprocess.call(f'valgrind --tool=massif {args.out_dir}/{args.project}', shell = True)
		# valgrind --tool=callgrind $(OUT_DIR)/$(RUNNABLE)
		# valgrind --tool=cachegrind $(OUT_DIR)/$(RUNNABLE)
		# valgrind --leak-check=full --show-leak-kinds=all --log-file="valgrind.out" $(OUT_DIR)/$(RUNNABLE)
	elif target == 'gdb':
		execute_target(args, 'compile')
		subprocess.call(f'gdb {args.out_dir}/{args.project}', shell = True)
	elif target == 'run':
		execute_target(args, 'compile')
		subprocess.call(f'{args.out_dir}/{args.project}', shell = True)
	elif target == 'shaders':
		command = f'find {args.src_dir}/assets/shaders/ -regextype posix-extended -regex ".*\.(comp|frag|geom|tesc|tese|vert)" -exec glslc {{}} -o {{}}.spv \;'
		subprocess.call(command, shell = True)
	elif target == 'list':
		for module in Module.discover(args.src_dir):
			if module.library:
				continue
			print(module.name)
	else:
		print(f"Unknown target: {target}")

if __name__ == '__main__':
	args = parse_arguments()
	os.environ['LIBRARY_PATH'] = args.out_dir
	os.environ['LD_LIBRARY_PATH'] = args.out_dir
	execute_target(args, args.target)
