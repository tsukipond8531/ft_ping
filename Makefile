SRCS := utils.c \
				ft_ping.c \
				parser.c \
				main.c
OBJS = $(SRCS:.c=.o)

NAME := ft_ping
LIBS := -lm

%.o: %.c
					/usr/bin/cc -Wall -Werror -Wextra -c $< -o $@ -O3

$(NAME): $(OBJS) $(OBJM)
					@echo "Linking objects into $(NAME)"
					/usr/bin/cc -o $(NAME) $(OBJS) $(OBJM) $(LIBS)
					/usr/bin/strip --strip-all $(NAME)
					@echo "Finished linking"

all: $(NAME)

bonus: $(NAME)

clean:
					@echo "Deleting all objects..."
					@rm -f $(OBJS) $(OBJM) $(OBJT)

fclean: clean
					@echo "Deleting $(NAME)..."
					@rm -f $(NAME)

re: fclean $(NAME)

.PHONY: all bonus clean fclean re
