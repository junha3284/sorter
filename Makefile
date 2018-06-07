OUTFILE = sorter
OUTDIR = $(HOME)/cmpt433/public/myApps
CROSS_COMPILE = arm-linux-gnueabihf-
CC_C = $(CROSS_COMPILE)gcc
CFLAGS = -Wall -g -std=c99 -D _POSIX_C_SOURCE=200809L -Werror -pthread

test_sorter:
	gcc $(CFLAGS) sorter.c test_sorter.c -o test_sorter
	@echo "Finished building test_sorter"

test_network:
	gcc $(CFLAGS) network.c test_network.c -o test_network

test_network_target:
	$(CC_C) $(CFLAGS) network.c test_network.c -o $(OUTDIR)/test_network

all: app done

app:
	$(CC_C) $(CFLAGS) sorter.c main.c network.c -o $(OUTDIR)/$(OUTFILE)

done:
	@echo "Finished building application."
clean:
	rm $(OUTDIR)/$(OUTFILE)
