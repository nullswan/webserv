NAME	:= webserv
CC		:= clang++

CFLAGS	:= -Wall -Wextra -Werror -std=c++98 -g3
DFLAGS	= -MMD -MF $(@:.o=.d)
# DEBUGFLAGS = -pedantic -Wunreachable-code -Wunused
# STRESSFLAGS = -O3

FILES	= main.cpp

OBJS_DIR	:= ./objs/
SRCS_DIR	:= ./srcs/

SRCS	:=	$(addprefix $(SRCS_DIR), $(FILES))
OBJS	:=	$(addprefix $(OBJS_DIR), $(FILES:.cpp=.o))
DEPS	:=	$(addprefix $(OBJS_DIR), $(FILES:.cpp=.d))

.PHONY	: all
all		: $(NAME)

-include $(DEPS)
$(NAME)	: $(OBJS)
	@	printf "Compiling $(NAME)\n"
	@	$(CC) $(CFLAGS) $^ -o $@

$(OBJS_DIR)%.o : $(SRCS_DIR)%.cpp
	@	mkdir -p $(dir $@)
	@	printf "Compiling: $<"
	@	$(CC) $(CFLAGS) -c $< -o $@ $(DFLAGS)
	@	printf " -> OK\n"

.PHONY	: clean
clean	:
	@	printf "Deleting $(OBJS_DIR)\n"
	@	rm -rf $(OBJS_DIR)

.PHONY	: fclean
fclean	: clean
	@	printf "Deleting $(NAME) binary\n"
	@	rm -f $(NAME)
ifneq ("$(wildcard .nginx)","")
	@	printf "Deleting nginx instance\n"	
	@	docker rm -f webserv_nginx
	@	rm -f .nginx
endif

.PHONY	: re
re		: fclean all

.PHONY	: run
run		: all
	@ ./webserv

.PHONY	: tests
tests	: re
	@	printf "Tests not implemented\n"
# @	./webserv

.PHONY	: valgrind
valgrind: all
	@	valgrind --leak-check=full --track-fds=yes ./webserv

.PHONY	: lint
lint	:
	@	cpplint --recursive --root=srcs \
		--filter=-whitespace/tab,-legal/copyright srcs/

.PHONY	: nginx
nginx	:
	@	printf "Starting nginx instance on port 9090\n"
	docker run --name webserv_nginx -p 9090:9090 -d nginx
	@ touch .nginx

.PHONY	: push
push	: lint tests