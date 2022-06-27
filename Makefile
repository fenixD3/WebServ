SHELL = /bin/sh

NAME_FILE = webserver

FLAGS = -Wall -Wextra -Werror -std=c++98

SRCS = Config.cpp \
       IConfig.cpp \
       CgiWorker.cpp \
       Cluster.cpp \
       HttpRequest.cpp \
       HttpResponce.cpp \
       HttpResponseBuilder.cpp \
       IOSocket.cpp \
       main.cpp \
       PhysicalServer.cpp \
       RequestBuilder.cpp \
       SocketContainer.cpp \
       VirtualServer.cpp \
       Worker.cpp \

OBJS = $(SRCS:.cpp=.o)

HEADER_PATH = ./includes/
HEADER_PATH_CFG = ./includes/config
HEADERS = Config.h \
          IOSocket.h \
          CgiWorker.h \
          Cluster.h \
          PhysicalServer.h \
          RequestBuilder.h \
          SocketContainer.h \
          HttpResponse.h \
          VirtualServer.h \
          IConfig.h \
          Worker.h \
          HttpRequest.h \
          raii_ptr.h \
          HttpResponseBuilder.h \

vpath %.cpp ./srcs/config ./srcs
vpath %.h ./includes/ ./includes/config

.PHONY: clean fclean re
all: $(NAME_FILE)

$(NAME_FILE): $(OBJS)
	@g++ -o $(NAME_FILE) $(OBJS)

$(OBJS): %.o:%.cpp $(HEADERS)
	@g++ -c $< -I$(HEADER_PATH) -I$(HEADER_PATH_CFG) $(FLAGS) -o $@

clean:
	@rm -f $(OBJS)

fclean: clean
	@rm -f $(NAME_FILE)

re: fclean all
