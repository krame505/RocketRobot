
all: doxygen.config myfooter.html
	doxygen doxygen.config
	$(MAKE) -C latex pdf

clean:
	rm -rf html latex LOGFILE
