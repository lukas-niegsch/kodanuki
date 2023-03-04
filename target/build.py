import argparse
from target_graph import TargetGraph
from target_types import *

def parse_arguments():
	parser = argparse.ArgumentParser(description='The Kodanuki build script.')
	parser.add_argument('src_dir', help = 'The projects source directory.')
	parser.add_argument('out_dir', help = 'The projects output directory.')
	parser.add_argument('bin_dir', help = 'The projects build directory.')
	parser.add_argument('project', help = 'The current runnable project.')
	parser.add_argument('target', help = 'The build target for this script.')
	return parser.parse_args()

if __name__ == '__main__':
	targets = TargetGraph()
	targets.add_target('help', HelpTarget(targets))
	targets.add_target('list', ListTarget())
	targets.add_target('stats', StatsTarget())
	targets.add_target('clean', CleanTarget())
	targets.add_target('ninja', NinjaTarget())
	targets.add_target('compile', CompileTarget())
	targets.add_target('shaders', ShadersTarget())
	targets.add_target('run', RunTarget())
	targets.add_target('gdb', GdbTarget())
	targets.add_target('valgrind', ValgrindTarget())
	targets.add_target('splashdoc', SplashdocTarget())
	targets.run_target(parse_arguments())
