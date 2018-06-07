OUTFILE = sorter
OUTDIR = $(HOME)/cmpt433/public/myApps
CROSS_COMPILE = 
CC_C = $(CROSS_COMPILE)gcc
CFLAGS = -Wall -g -std=c99 -D _POSIX_C_SOURCE=200809L -Werror -pthread

test_sorter:
	gcc $(CFLAGS) sorter.c test_sorter.c -o test_sorter_c
	@echo "Finished building test_sorter"
all: app done
app:
	$(CC_C) $(CFLAGS) sorter.c main.c network.c -o $(OUTdIR)/$(OUTFILE)

done:
	@echo "Finished building application."
clean:
	rm $(OUTDIR)/$(OUTFILE)
