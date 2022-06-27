#!/usr/local/bin/python3

import sys
import os
def run():
	# data = sys.stdin.buffer.read()
	data = open(0).read()
	print("Status: 200 OK", end="\r\n")
	print("Server: Webserv.cgi", end="\r\n")
	print("Content-Type: text/html", end="\r\n")
	print("\r\n")
	print("<html>")
	print("<head>")
	print("<meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\">")
	print("<title>Hello World</title>")
	print("</head>")
	print("<body>")
	
	print("<h3>")
	print("Hi! I am CGI script\n")
	print("</h3>")
	print("ENV VAR:", end="<br/>")
	print("ENV:")
	for k, v in sorted(os.environ.items()):
		print(k+':', v, end="<br/>")
	print("</body>")
	print("</html>")

if __name__ == "__main__":
	run()


