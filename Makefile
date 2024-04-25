SRCS := utils.c \
				ip_header.c \
				icmp.c \
				ft_ping.c
MAIN := main.c
TEST := tests.c

OBJS = $(SRCS:.c=.o)
OBJM = $(MAIN:.c=.o)
OBJT = $(TEST:.c=.o)

NAME := ft_ping

%.o: %.c
					/usr/bin/cc -Wall -Werror -Wextra -c $< -o $@ -O3

$(NAME): $(OBJS) $(OBJM)
					@echo "Linking objects into $(NAME)"
					/usr/bin/cc -o $(NAME) $(OBJS) $(OBJM)
					/usr/bin/strip --strip-all $(NAME)
					@echo "Finished linking"

all: $(NAME)

test: $(OBJS) $(OBJT)
					@echo "Finished linking"
					/usr/bin/cc -o $(NAME)_test $(OBJS) $(OBJT)
					echo "Testing program..."
					@./$(NAME)_test
					@echo "Everything is good!"
					@rm -f $(NAME)_test

bonus: $(NAME)

clean:
					@echo "Deleting all objects..."
					@rm -f $(OBJS) $(OBJM) $(OBJT)

fclean: clean
					@echo "Deleting $(NAME)..."
					@rm -f $(NAME)

re: fclean $(NAME)

.PHONY: math all bonus clean fclean
