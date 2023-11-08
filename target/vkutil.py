import re
import string
from dataclasses import dataclass
from xml.etree import ElementTree


@dataclass
class VulkanMember:
	native_name: str
	custom_name: str
	native_type: str
	custom_type: str
	type_prefix: str
	type_middle: str
	type_suffix: str
	is_vector: bool
	is_counts: bool

	def native_string(self, type_size):
		return f'\t{self.native_type.ljust(type_size)} {self.native_name};'

	def custom_string(self, type_size, indent = '\t', suffix = ';'):
		return f'\t{self.custom_type.ljust(type_size)} {self.custom_name};'

	def init_custom_name(self, structs_mapping):
		if not self.custom_name:
			return
		prefix = self.type_prefix
		middle = self.type_middle
		suffix = self.type_suffix
		if middle in structs_mapping:
			middle = structs_mapping[middle]
		if self.is_vector and suffix == '* const*':
			self.custom_type = 'std::vector<const char *>'
			return
		elif self.is_vector:
			self.custom_type = f'std::vector<{middle}>'
			return
		if suffix == '*' and middle != 'char':
			suffix = '&'
		self.custom_type = ' '.join([prefix, middle, suffix]).strip()

	@staticmethod
	def parse_native_name(element):
		return element.find('./name').text

	@staticmethod
	def parse_custom_name(element):
		custom_name = VulkanMember.parse_native_name(element)
		if custom_name in ['sType', 'flags', 'pNext']:
			return None
		if custom_name.endswith('Count'):
			return None
		if custom_name.startswith('pp'):
			custom_name = custom_name.lstrip(string.ascii_lowercase)
		if custom_name.startswith('p') and custom_name[1].isupper():
			custom_name = custom_name.lstrip(string.ascii_lowercase)
		custom_name = list(custom_name)
		custom_name[0] = custom_name[0].upper()
		custom_name = ''.join(custom_name)
		return custom_name

	@staticmethod
	def parse_native_type(element):
		string  = ElementTree.tostring(element, encoding='utf-8').decode('utf-8')
		pattern = r'([^<>]*)<type>([^<>]*)</type>([^<>]*)'
		types3x = re.search(pattern, string).groups()
		return map(str.strip, types3x)

	@staticmethod
	def from_xml(element, is_vector, is_counts):
		prefix, middle, suffix = VulkanMember.parse_native_type(element)
		result = VulkanMember(
			native_name = VulkanMember.parse_native_name(element),
			custom_name = VulkanMember.parse_custom_name(element),
			native_type = f'{prefix} {middle} {suffix}'.strip(),
			custom_type = None, # changed later when all types are known
			type_prefix = prefix,
			type_middle = middle,
			type_suffix = suffix,
			is_vector = is_vector,
			is_counts = is_counts,
		)
		return result


@dataclass
class VulkanStruct:
	native_name: str
	custom_name: str
	sType_value: str
	members: list[VulkanMember]

	def native_string(self):
		lines = []
		lines.append(f'struct {self.native_name}')
		lines.append('{')
		type_len = max(len(member.native_type) for member in self.members)
		for member in self.members:
			lines.append(member.native_string(type_len))
		lines.append('};')
		return '\n'.join(lines)

	def custom_string(self):
		lines = []
		lines.append(f'struct {self.custom_name}')
		lines.append('{')
		type_len = max((len(member.custom_type) for member in self.members if member.custom_name), default = 0)
		for member in self.members:
			if not member.custom_name:
				continue
			lines.append(member.custom_string(type_len))
		lines.append('};')
		return '\n'.join(lines)

	@staticmethod
	def parse_native_name(element):
		return element.attrib['name']
	
	@staticmethod
	def parse_custom_name(element):
		custom_name = VulkanStruct.parse_native_name(element)
		custom_name = custom_name.rstrip(string.ascii_uppercase)
		custom_name = custom_name.removeprefix('Vk')
		custom_name = custom_name.removesuffix('CreateInfo')
		custom_name = f'Vulkan{custom_name}Builder'
		return custom_name

	@staticmethod
	def parse_stype_value(element):
		return element[0].attrib['values']

	@staticmethod
	def parse_members(element):
		members = []
		is_counts = False
		is_vector = False
		for member in element:
			if member.tag == 'comment':
				continue
			if 'api' in member.attrib and member.attrib['api'] == 'vulkansc':
				continue
			if is_vector:
				is_vector = False
			if is_counts:
				is_vector = True
				is_counts = False
			if member.find('./name').text.endswith('Count'):
				is_counts = True
			members.append(VulkanMember.from_xml(member, is_vector, is_counts))
		return members

	@staticmethod
	def from_xml(element):
		result = VulkanStruct(
			native_name = VulkanStruct.parse_native_name(element),
			custom_name = VulkanStruct.parse_custom_name(element),
			sType_value = VulkanStruct.parse_stype_value(element),
			members = VulkanStruct.parse_members(element),
		)
		return result
	
	@staticmethod
	def list_from_xml(vk_root):
		structs = []
		for element in vk_root.findall('.//type[@category="struct"]'):
			if 'alias' in element.attrib:
				continue
			if not 'CreateInfo' in element.attrib['name']:
				continue
			if not 'values' in element[0].attrib:
				continue
			structs.append(VulkanStruct.from_xml(element))

		mapping = {struct.native_name: struct.custom_name for struct in structs}
		for struct in structs:
			for member in struct.members:
				member.init_custom_name(mapping)
		return structs


if __name__ == '__main__':
	vk_root = ElementTree.parse('/usr/share/vulkan/registry/vk.xml')
	structs = VulkanStruct.list_from_xml(vk_root)

	for struct in structs:
		print(f'translation for {struct.native_name}:')
		print()
		print(struct.custom_string())
		print()
		print('-' * 30)
