import unittest
import requests

import utils as u

CONFIG = "tests/configs/full.conf"

class TestRedir(unittest.TestCase):
	pid, fd = 0, 0

	@classmethod
	def setUpClass(cls):
		cls.pid, cls.fd = u.start_server(CONFIG)

	@classmethod
	def tearDownClass(cls):
		if cls.fd and cls.pid:
			u.stop_server(cls.pid, cls.fd)

	# def test_redir_autoindex(self):
	# def test_redir_index(self):
	# def test_redir_httpbin(self):
	# def test_redir_google(self):
	# def test_redir_stackoverflow(self):
	# def test_redir_42(self):

class TestServerA(unittest.TestCase):
	pid, fd = 0, 0

	@classmethod
	def setUpClass(cls):
		cls.pid, cls.fd = u.start_server(CONFIG)

	@classmethod
	def tearDownClass(cls):
		if cls.fd and cls.pid:
			u.stop_server(cls.pid, cls.fd)

	# def test_server_index(self):

class TestServerB(unittest.TestCase):
	pid, fd = 0, 0

	@classmethod
	def setUpClass(cls):
		cls.pid, cls.fd = u.start_server(CONFIG)

	@classmethod
	def tearDownClass(cls):
		if cls.fd and cls.pid:
			u.stop_server(cls.pid, cls.fd)

	def test_server_index(self):
		pass

if __name__ == '__main__':
	unittest.main()