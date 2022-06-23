#!/usr/local/bin/python3

import sys
import os
def run():
	# data = sys.stdin.buffer.read()
	data = open(0).read()
	print("HTTP/1.1 200 OK")
	print("Server: Webserv.cgi")
	print("Content-Type: text/html", end="")
	print("\r\n")
	print("Hi! I am CGI script")
	print("You send me this request:")
	data.replace("\r\n", "\n-----------\n")
	print(str(data))
	print("-------")
	print("ENV:")
	for k, v in sorted(os.environ.items()):
		print(k+':', v)
	print("\r\n")
	print("\r\n")

if __name__ == "__main__":
	run()


