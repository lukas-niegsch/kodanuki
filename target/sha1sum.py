import argparse
import io
import struct


def rotl(n: int, b: int, w: int = 32):
	'''
	Left rotates the given number by some bits.

	:param n: The number to bitwise rotate.
	:param b: The amount of bits to the left.
	:param w: The maximum bit width of n.
	'''
	return ((n << b) | (n >> w - b)) & (2 ** w - 1)


def sha1chunk(chunk, h0, h1, h2, h3, h4):
	'''
	Computes the sha1sum for the given 64 bit chunk.
	'''
	if not chunk:
		return h0, h1, h2, h3, h4
	w = [0] * 80
	for i in range(16):
		w[i] = struct.unpack(b'>I', chunk[i * 4:i * 4 + 4])[0]
	for i in range(16, 80):
		w[i] = rotl(w[i - 3] ^ w[i - 8] ^ w[i - 14] ^ w[i - 16], 1)
	a = h0
	b = h1
	c = h2
	d = h3
	e = h4
	for i in range(80):
		if 0 <= i <= 19:
			f = d ^ (b & (c ^ d))
			k = 0x5A827999
		elif 20 <= i <= 39:
			f = b ^ c ^ d
			k = 0x6ED9EBA1
		elif 40 <= i <= 59:
			f = (b & c) | (b & d) | (c & d)
			k = 0x8F1BBCDC
		elif 60 <= i <= 79:
			f = b ^ c ^ d
			k = 0xCA62C1D6
		a, b, c, d, e = (rotl(a, 5) + f + e + k + w[i]) & 0xFFFFFFFF, a, rotl(b, 30), c, d
	h0 = (h0 + a) & 0xFFFFFFFF
	h1 = (h1 + b) & 0xFFFFFFFF
	h2 = (h2 + c) & 0xFFFFFFFF
	h3 = (h3 + d) & 0xFFFFFFFF
	h4 = (h4 + e) & 0xFFFFFFFF
	return h0, h1, h2, h3, h4


def sha1sum(message: bytes, chunk_size: int = 64):
	'''
	Computes the sha1sum for the given message.

	:param message: The input data for the hash.
	:param chunk_size: The block size per chunk.
	'''
	h = (0x67452301, 0xEFCDAB89, 0x98BADCFE, 0x10325476, 0xC3D2E1F0)
	message_length = len(message)
	message = io.BytesIO(message)
	chunk = message.read(chunk_size)
	while len(chunk) == chunk_size:
		h = sha1chunk(chunk, *h)
		chunk = message.read(chunk_size)
	chunk += b'\x80'
	chunk += b'\x00' * (-(message_length + 9) % chunk_size)
	chunk += struct.pack('>Q', 8 * message_length)
	h = sha1chunk(chunk[:chunk_size], *h)
	h = sha1chunk(chunk[chunk_size:], *h)
	return '{:08x}{:08x}{:08x}{:08x}{:08x}'.format(*h)


if __name__ == '__main__':
	parser = argparse.ArgumentParser(description='Computes the Sha1 (160 bits) checksum!')
	parser.add_argument('file_path', nargs='*', help='The files to hash.')
	args = parser.parse_args()
	for file_path in args.file_path:
		with open(file_path, 'rb') as file:
			data = file.read()
		print(sha1sum(data) + '  ' + file_path)
