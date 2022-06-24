#!/usr/local/bin/python3

import sys
import os
import random
import string
def run():
	# data = sys.stdin.buffer.read()
	data = open(0).read()
	visited = 0
	letters = string.ascii_lowercase
	session_info = ''.join(random.choice(letters) for i in range(10))
	debug = ""
	if os.environ.get('HTTP_Cookie'):
		for cookie in os.environ['HTTP_Cookie'].split(';'):
			key, value = cookie.split('=');
			key = key.strip()
			value = value.strip()
			if key == "Data":
				visited, session_info = value.split("-")
				try:
					visited = int(visited) + 1
				except:
					visited = 0
	print("Status: 200 OK", end="\r\n")
	print("Server: Webserv.cgi", end="\r\n")
	print("Content-Type: text/html", end="\r\n")
	print(f"Set-Cookie: Data={visited}-{session_info};", end="\r\n")
	
	
	print("\r\n")
	print("<html>")
	print("<head>")
	print("<meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\">")
	print("<title>Hello World</title>")
	print("</head>")
	print("<body>")
	# print(os.environ.get('HTTP_Cookie'))
	print(debug)
	print("<h3>")
	print("Hi! I am CGI Cookie script\n")
	print("</h3>")
	print(f"You visited our site {visited} times")
	print("</h3>")
	print(f"You session:  {session_info}")
	
	print("</body>")
	print("</html>")

if __name__ == "__main__":
	run()


