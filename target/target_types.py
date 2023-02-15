import os
import subprocess
from module_parser import *
from ninja_writer import NinjaWriter
from target_graph import Target

class HelpTarget(Target):
	''' Display this help message. '''

	def __init__(self, targets):
		self.targets = targets

	def validate(self, args):
		super().validate(args)

	def execute(self, args):
		super().execute(args)
		print(f'Available targets for the {args.project} project:')
		for name, target in self.targets:
			print(f'\t{name:<15}{target.__doc__}'.expandtabs(4))


class ListTarget(Target):
	''' List all available projects. '''

	def validate(self, args):
		super().validate(args)

	def execute(self, args):
		super().execute(args)
		print(f'Available projects:')
		for module in Module.discover(args.src_dir):
			if module.library:
				continue
			print(f'\t{module.name:<15}{module.description}'.expandtabs(4))


class StatsTarget(Target):
	''' Show statistics about the source code. '''

	def validate(self, args):
		super().validate(args)

	def execute(self, args):
		super().execute(args)
		subprocess.call(f'pygount --format=summary engine source', shell = True)


class CleanTarget(Target):
	''' Clean the build directories. '''

	def validate(self, args):
		super().validate(args)

	def execute(self, args):
		super().execute(args)
		subprocess.call('git clean -qXdf', shell = True)
		subprocess.call(f'rm -rf {args.bin_dir} {args.out_dir}', shell = True)


class NinjaTarget(Target):
	''' Generate the ninja build script. '''

	def validate(self, args):
		super().validate(args)

	def execute(self, args):
		super().execute(args)
		subprocess.call(f'mkdir -p {args.bin_dir} {args.out_dir}', shell = True)
		writer = NinjaWriter()
		writer.variable('src', args.src_dir)
		writer.variable('bin', args.bin_dir)
		writer.variable('out', args.out_dir)
		writer.variable('flags', '-O2 -std=c++20 -Wall -Wextra -Werror -g')
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

		projects = [module.name for module in Module.discover(args.src_dir) if not module.library]
		projects = ' '.join(map(program, projects))
		if projects:
			writer.variable('default ', projects, use_equals = False)
		writer.write(f'{args.bin_dir}/build.ninja')


class CompileTarget(NinjaTarget):
	''' Compile project using ninja. '''

	def validate(self, args):
		super().validate(args)

	def execute(self, args):
		super().execute(args)
		os.environ['LIBRARY_PATH'] = args.out_dir
		os.environ['LD_LIBRARY_PATH'] = args.out_dir
		subprocess.call(f'ninja -C {args.bin_dir} {args.out_dir}/{args.project}', shell = True)


class ShadersTarget(Target):
	''' Compile all available shaders. '''

	def validate(self, args):
		super().validate(args)

	def execute(self, args):
		super().execute(args)
		command = f'find {args.src_dir}/assets/shaders/ -regextype posix-extended -regex ".*\.(comp|frag|geom|tesc|tese|vert)" -exec glslc {{}} -o {{}}.spv \;'
		subprocess.call(command, shell = True)


class RunTarget(CompileTarget):
	''' Run the project normally. '''

	def validate(self, args):
		super().validate(args)

	def execute(self, args):
		super().execute(args)
		subprocess.call(f'{args.out_dir}/{args.project}', shell = True)


class GdbTarget(CompileTarget):
	''' Run the project using gdb. '''

	def validate(self, args):
		super().validate(args)

	def execute(self, args):
		super().execute(args)
		subprocess.call(f'gdb {args.out_dir}/{args.project}', shell = True)


class ValgrindTarget(CompileTarget):
	''' Run the project using valgrind. '''

	def validate(self, args):
		super().validate(args)
	
	def execute(self, args):
		super().execute(args)
		command = f'valgrind --tool=massif {args.out_dir}/{args.project}'
		# command = f'valgrind --tool=callgrind {args.out_dir}/{args.project}'
		# command = f'valgrind --tool=cachegrind {args.out_dir}/{args.project}'
		# command = f'valgrind ---leak-check=full --show-leak-kinds=all --log-file="valgrind.out" {args.out_dir}/{args.project}'
		subprocess.call(command, shell = True)