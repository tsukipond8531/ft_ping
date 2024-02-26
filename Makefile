MSG = @echo "  \x1b[45m\x1b[38;2;16;19;33m\x1b[45m\x1b[01;37m    ft_ping \x1b[0m\x1b[35m\x1b[0m\x1b[0;90m $(1)\x1b[0m"

SRCS := utils.c \
				ip_header.c \
				icmp.c
MAIN := main.c
TEST := tests.c

OBJS = $(SRCS:.c=.o)
OBJM = $(MAIN:.c=.o)
OBJT = $(TEST:.c=.o)

NAME := ft_ping

%.o: %.c
					@printf "  \x1b[45m\x1b[38;2;16;19;33m\x1b[45m\x1b[01;37m    ft_ping \x1b[0m\x1b[35m\x1b[0m\x1b[0;90m Compiling \x1b[0;35m$@...         \x1b[0m\r"
					@clang -Wall -Werror -Wextra -c $< -o $@ -O3

$(NAME): $(OBJS) $(OBJM)
					@echo ""
					$(call MSG,"Linking objects into $(NAME)")
					@cc -o $(NAME) $(OBJS) $(OBJM)
					$(call MSG,"Finished linking")
					@echo ""

all: $(NAME)

test: $(OBJS) $(OBJT)
					@echo ""
					$(call MSG,"Finished linking")
					@cc -o $(NAME)_test $(OBJS) $(OBJT)
					$(call MSG,"Testing program...")
					@./$(NAME)_test
					$(call MSG,"Everything is good!")
					@rm -f $(NAME)_test
					@echo ""

bonus: $(NAME)

clean:
					@echo ""
					$(call MSG,"Deleting all objects...")
					@rm -f $(OBJS) $(OBJM) $(OBJT)

fclean: clean
					$(call MSG,"Deleting $(NAME)...","\n")
					@rm -f $(NAME)
					@echo ""

re: fclean $(NAME)

.PHONY: math all bonus clean fclean
