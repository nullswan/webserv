NAME	:= webserv
CC		:= clang++

BUILD_COMMIT 	:= $(shell git rev-parse --short HEAD)

OBJS_DIR	:= ./objs/
SRCS_DIR	:= ./srcs/

DBFLAGS = -pedantic -Wunreachable-code -Wunused
BFLAGS = -D WEBSERV_BUILD_COMMIT=\"@$(BUILD_COMMIT)\"
ESESSION = -D WEBSERV_SESSION=1
ifeq ($(SESSION), disable)
	ESESSION = 
endif
CFLAGS	:= -Wall -Wextra -Werror -std=c++98 -I $(SRCS_DIR) $(ESESSION)
OFLAGS  :=  -D WEBSERV_BENCHMARK=1 -O3
DFLAGS	= -MMD -MF $(@:.o=.d)
SHELL	:= /bin/bash

FILES	= main.cpp

SRCS	:=	$(addprefix $(SRCS_DIR), $(FILES))
OBJS	:=	$(addprefix $(OBJS_DIR), $(FILES:.cpp=.o))
DEPS	:=	$(addprefix $(OBJS_DIR), $(FILES:.cpp=.d))

.PHONY	: all
all		: $(NAME)

-include $(DEPS)
$(NAME)	: $(OBJS)
	@	printf "Compiling $(NAME)\n"
ifneq ($(MODE), benchmark)
	@	$(CC) $(CFLAGS) $(BFLAGS) $^ -o $@ -g3
else
	@	$(CC) $(CFLAGS) $(BFLAGS) $^ -o $@ $(OFLAGS)
endif

$(OBJS_DIR)%.o : $(SRCS_DIR)%.cpp
	@	mkdir -p $(dir $@)
	@	printf "Compiling: $<"
ifneq ($(MODE), benchmark)
	@	$(CC) $(CFLAGS) $(BFLAGS) -c $< -o $@ $(DFLAGS) -g3
else
	@	$(CC) $(CFLAGS) $(BFLAGS) -c $< -o $@ $(DFLAGS) $(OFLAGS)
endif
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
	@	./tests/unit/run.sh

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

.PHONY: confirm
confirm:
	@if [[ -z "$(CI)" ]]; then \
		REPLY="" ; \
		read -p "/!\ Are you sure? [y/n] > " -r ; \
		if [[ ! $$REPLY =~ ^[Yy]$$ ]]; then \
			printf "KO"; \
			exit 1 ; \
		else \
			printf "OK"; \
			exit 0; \
		fi \
	fi

.PHONY	: push
push	: lint tests
	@if $(MAKE) -s confirm ; then \
		git push ; \
	fi