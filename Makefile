OUTFILE = sorter
OUTDIR = $(HOME)/cmpt433/public/myApps
CROSS_COMPILE = arm-linux-gnueabihf-
CC_C = $(CROSS_COMPILE)gcc
CFLAGS = -Wall -g -std=c99 -D _POSIX_C_SOURCE=200809L -Werror -pthread

all: app done

test_on_host: test_netowrk test_worter

test_on target: test_netowrk_target test_UI_target	

test_network_target:
	$(CC_C) $(CFLAGS) network.c test_network.c -o $(OUTDIR)/test_network
	@echo "Finished building test_network for target"

test_UI_target:
	$(CC_C) $(CFLAGS) sorter.c network.c userinterface.c test_UI.c -o $(OUTDIR)/test_UI 
	@echo "Finished building test_UI for host"

test_network:
	gcc $(CFLAGS) network.c test_network.c -o test_network
	@echo "Finished building test_network for host"

test_sorter:
	gcc $(CFLAGS) sorter.c test_sorter.c -o test_sorter
	@echo "Finished building test_sorter"

app:
	$(CC_C) $(CFLAGS) sorter.c network.c userinterface.c main.c -o $(OUTDIR)/$(OUTFILE)

done:
	@echo "Finished building application."

clean:
	@rm $(OUTDIR)/$(OUTFILE)
	@rm $(OUTDIR)/test_network
	@rm $(OUTDIR)/test_UI
