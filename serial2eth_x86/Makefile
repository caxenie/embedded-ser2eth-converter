SUBDIRS= s2e-converter s2e-tester

all:
	for x in $(SUBDIRS); do $(MAKE) -C $$x ; done

clean:
	for x in $(SUBDIRS); do $(MAKE) -C $$x clean ; done
