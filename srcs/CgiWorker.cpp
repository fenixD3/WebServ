
#include "CgiWorker.h"
#include <unistd.h>


CgiWorker::CgiWorker() {
	env["GATEWAY_INTERFACE"] = "CGI/1.1";
	env["SERVER_PROTOCOL"] = "HTTP/1.1";
}

char** convert_map_to_c_arr(std::map<std::string, std::string>& process_env) {
	char **result;
	result = new char*[process_env.size() + 1];
	int i = 0;
	for (std::map<std::string, std::string>::const_iterator elem = process_env.begin(); elem != process_env.begin(); ++elem) {
		std::string raw = elem->first + "=" + elem->second;
		result[i] = new char[raw.size() + 1];
		strcpy(result[i], raw.c_str());
		++i;
	}
	result[i] = NULL;
	return result;
}

char** CgiWorker::create_env(std::string cgi_script_path, std::string request_address, HttpRequest* request_pointer) {
	std::map<std::string, std::string> process_env(env);
	HttpRequest& request = *request_pointer;
	if (request["Auth-Scheme"].size()) {
		process_env["AUTH_TYPE"] = request["Authorization"];
	}
	process_env["SCRIPT_NAME"] = cgi_script_path;
	process_env["CONTENT_LENGTH"] = request.GetBody().size();
	process_env["CONTENT_TYPE"] = request["Content-Type"];

	if (request_address.find('?') != std::string::npos) {
		size_t term_pos = request_address.find('?');
		process_env["PATH_INFO"] = request_address.substr(0, term_pos);
		process_env["QUERY_STRING"] = request_address.substr(term_pos);
	} else {
		process_env["PATH_INFO"] = request_address;
	}
	process_env["REQUEST_METHOD"] = request.GetMethod();
	
	return convert_map_to_c_arr(process_env);
}

void free_env(char** env_array) {
	char** to_delete = env_array;
	while (*env_array) {
		delete[] *env_array;
		++env_array;
	}
	delete[] to_delete;
}

std::string CgiWorker::executeCgi(std::string cgi_script_path, std::string request_address, HttpRequest* request) {

	int pipe_in[2];
	int pipe_out[2];
	char **env_array;
	std::string cgi_responce;

	env_array = create_env(cgi_script_path, request_address, request);
	if (pipe(pipe_in) || pipe(pipe_out)) {
		throw CgiException();
	}
	int pid = fork();
	if (pid == -1) {
		throw CgiException();
	}
	if (pid == 0) {
		close(pipe_in[1]);
		close(pipe_out[0]);
		dup2(pipe_in[0], STDIN_FILENO);
		dup2(pipe_out[1], STDOUT_FILENO);
		execve(cgi_script_path.c_str(), NULL, env_array);
		exit(1);
	} else {
		char buffer[1024];

		close(pipe_in[0]);
		close(pipe_out[1]);
		// todo для очень больших реквестов можно создавать отдельный поток, чтобы pipe не блокировался
		write(pipe_in[1], std::string(request->GetBody().begin(), request->GetBody().end()).c_str(), request->GetBody().size());
		int len = -1;
		while (len) {
			int len = read(pipe_out[0], buffer, 1024);
			if (len) {
				std::string read_block(buffer, len);
				cgi_responce += read_block;
			} else if (len == -1) {
				throw CgiException();
			}
		}
		close(pipe_in[1]);
		close(pipe_out[0]);
	}
	free_env(env_array);
	return cgi_responce;
}
