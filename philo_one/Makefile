# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: fhenrion <fhenrion@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2019/12/07 11:40:22 by fhenrion          #+#    #+#              #
#    Updated: 2020/10/23 18:19:04 by fhenrion         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = philo_one
HDR_NAME = philo_one.h

SRC_NAME = philo_one time utils init print threads forks
SRC = $(addsuffix .c,$(SRC_NAME))
SRC_O = $(addsuffix .o,$(SRC_NAME))

CC = gcc
CFLAGS = -Werror -Wall -Wextra

.PHONY: clean fclean all re bonus

all: $(NAME)

$(NAME): $(SRC_O)
	$(CC) $(CFLAGS) -I $(HDR_NAME) $^ -o $@
	@echo "\033[0;32mCompilation OK\033[0m"

%.o : %.c ${HDR_NAME}
	$(CC) -c $(CFLAGS) -I $(HDR_NAME) $< -o $@

commit:
	git add $(SRC) Makefile
	git commit -m 'makefile_commit'
	git push

clean:
	@/bin/rm -f $(SRC_O)
	@echo "\033[0;32mObjects files cleaning OK\033[0m"

fclean: clean
	@/bin/rm -f $(NAME)
	@echo "\033[0;32mLibrary cleaning OK\033[0m"

re: fclean $(NAME)