doc/html/index.html :
	cd doc;doxygen Doxyfile

doc : doc/html/index.html
	echo "*** Doxygen generated in doc/html/"

clean :
	rm -R doc/html

install :
	cp include/hffix.hpp /usr/include/hffix.hpp

uninstall :
	rm /usr/include/hffix.hpp

.PHONY : doc all clean install uninstall
