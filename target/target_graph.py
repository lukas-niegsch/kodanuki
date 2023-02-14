from abc import ABC, abstractmethod

class Target(ABC):
	@abstractmethod
	def validate(self, args):
		assert(True)
	
	@abstractmethod
	def execute(self, args):
		pass

class TargetGraph:
	def __init__(self):
		self.targets = {}
	
	def __iter__(self):
		return iter(self.targets.items())
	
	def add_target(self, name, target):
		self.targets[name] = target
	
	def run_target(self, args):
		if not args.target in self.targets:
			print(f'unknown target: {args.target}\n')
			args.target = 'help'
		self.targets[args.target].validate(args)
		self.targets[args.target].execute(args)
