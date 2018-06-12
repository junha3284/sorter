OUTFILE1 = sorter
OUTFILE2 = noworky
OUTDIR = $(HOME)/cmpt433/public/myApps
CROSS_COMPILE = arm-linux-gnueabihf-
CC_C = $(CROSS_COMPILE)gcc
CFLAGS = -Wall -g -std=c99 -D _POSIX_C_SOURCE=200809L -Werror -pthread

all: app done

app:
	$(CC_C) $(CFLAGS) sorter.c network.c userinterface.c main.c -o $(OUTDIR)/$(OUTFILE1)
	$(CC_C) $(CFLAGS) noworky.c -o $(OUTDIR)/$(OUTFILE2)

done:
	@echo "Finished building application."

test_network_target:
	$(CC_C) $(CFLAGS) network.c test_network.c -o $(OUTDIR)/test_network
	@echo "Finished building test_network for target"

test_UI:
	$(CC_C) $(CFLAGS) sorter.c network.c userinterface.c test_UI.c -o $(OUTDIR)/test_UI
	@echo "Finished building test_UI for target"

clean:
	rm $(OUTDIR)/$(OUTFILE1)
	rm $(OUTDIR)/$(OUTFILE2)
