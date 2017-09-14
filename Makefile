
CXXFLAGS += -Wall -Iinclude

all : doc fixprint examples

doc : doc/html/index.html

doc/html/index.html : doc/hffix.css include/hffix.hpp include/hffix_fields.hpp doc/Doxyfile README.md
	@echo "*** Generating Doxygen in doc/html/ ..."
	cd doc;doxygen Doxyfile
	@echo "*** Generated Doxygen in doc/html/"

clean :
	@echo "*** Cleaning ..."
	-rm -R doc/html
	-rm -R util/bin
	-rm -R test/bin
	-rm include/hffix_fields.hpp
	-rm spec/fix.4.2/*.pdf
	-rm spec/fix.4.3/*.pdf
	-rm spec/fix.4.3/*.dtd
	-rm spec/fix.4.4/*.pdf
	-rm spec/fix.4.4/*.dtd
	-rm -r spec/fix.5.0.sp2/fixmlschema
	@echo "*** Cleaned"

# Unzip all the specification documents
specs : spec/fix.4.3/*.dtd spec/fix.4.4/*.dtd spec/fix.5.0.sp2/fixmlschema
	cd spec/fix.4.2; unzip -u fix-42-with_errata_20010501_pdf.zip

spec/fix.4.3/*.dtd:
	cd spec/fix.4.3; unzip -u FIX-43-with_errata_20020920_PDF.ZIP

spec/fix.4.4/*.dtd:
	cd spec/fix.4.4; unzip -u fix-44_w_Errata_20030618_PDF.zip

spec/fix.5.0.sp2/fixmlschema:
	cd spec/fix.5.0.sp2; unzip -u -d fixmlschema fixmlschema_FIX.5.0SP2_Errata_20131209.zip

include/hffix_fields.hpp: spec/codegen spec/fix.4.3/*.dtd spec/fix.4.4/*.dtd spec/fix.5.0.sp2/fixmlschema
	@echo "*** Generating include/hffix_fields.hpp from FIX specs..."
	cd spec/fix.4.2; unzip -u fix-42-with_errata_20010501_pdf.zip
	cd spec/fix.4.3; unzip -u FIX-43-with_errata_20020920_PDF.ZIP
	cd spec/fix.4.4; unzip -u fix-44_w_Errata_20030618_PDF.zip
	cd spec/fix.5.0.sp2; unzip -u -d fixmlschema fixmlschema_FIX.5.0SP2_Errata_20131209.zip
	cd spec;./codegen > ../include/hffix_fields.hpp
	@echo "*** Generated include/hffix_fields.hpp from FIX specs"

fixprint : util/bin/fixprint

util/bin/fixprint : util/src/fixprint.cpp include/hffix.hpp include/hffix_fields.hpp
	@echo "*** Building fixprint utility util/bin/fixprint ..."
	mkdir -p util/bin
	$(CXX) $(CXXFLAGS) -o util/bin/fixprint util/src/fixprint.cpp
	@echo "*** Built fixprint utility util/bin/fixprint"

test/bin/writer01 : test/src/writer01.cpp include/hffix.hpp include/hffix_fields.hpp
	@echo "*** Building test/bin/writer01 ..."
	mkdir -p test/bin
	$(CXX) $(CXXFLAGS) -o test/bin/writer01 test/src/writer01.cpp
	@echo "*** Built test/bin/writer01"

test/bin/reader01 : test/src/reader01.cpp include/hffix.hpp include/hffix_fields.hpp
	@echo "*** Building test/bin/reader01 ..."
	mkdir -p test/bin
	$(CXX) $(CXXFLAGS) -o test/bin/reader01 test/src/reader01.cpp
	@echo "*** Built test/bin/reader01"

examples : test/bin/writer01 test/bin/reader01

ctags :
	ctags include/*

README.html : README.md
	pandoc --from markdown --to html < README.md > README.html

.PHONY : help doc all clean install uninstall fixprint specs ctags examples
