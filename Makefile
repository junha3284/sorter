OUTFILE = sorter
OUTDIR = $(HOME)/cmpt433/public/myApps
CROSS_COMPILE = 
CC_C = $(CROSS_COMPILE)gcc
CFLAGS = -Wall -g -std=c99 -D _POSIX_C_SOURCE=200809L -Werror -pthread

all: app done
app:
	$(CC_C) $(CFALGS) sorter.c main.c network.c -o $(OUTdIR)/$(OUTFILE)

done:
	@echo "Finished building application."
clean:
	rm $(OUTDIR)/$(OUTFILE)
