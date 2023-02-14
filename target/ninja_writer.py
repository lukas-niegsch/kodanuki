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
