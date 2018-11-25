
CXXFLAGS += -Wall -Iinclude

NORMAL=\033[0m
YELLOW=\033[1;33m

all : doc fixprint examples

doc : doc/html/index.html

doc/html/index.html : doc/hffix.css include/hffix.hpp include/hffix_fields.hpp doc/Doxyfile README.md
	@echo -e "${YELLOW}*** Generating Doxygen in doc/html/ ...${NORMAL}"
	cd doc;rm -r html;doxygen Doxyfile
	@echo -e "${YELLOW}*** Generated Doxygen in doc/html/${NORMAL}"

clean : clean-bin

clean-all : clean-bin
	@echo -e "${YELLOW}*** Cleaning all artifacts ...${NORMAL}"
	-rm -r doc/html
	-rm include/hffix_fields.hpp
	-rm spec/fix.4.2/*.pdf
	-rm spec/fix.4.3/*.pdf
	-rm spec/fix.4.3/*.dtd
	-rm spec/fix.4.4/*.pdf
	-rm spec/fix.4.4/*.dtd
	-rm -r spec/fix.5.0.sp2/fixmlschema
	-rm ctags
	@echo -e "${YELLOW}*** Cleaned all artifacts${NORMAL}"

clean-bin :
	@echo -e "${YELLOW}*** Cleaning binaries ...${NORMAL}"
	-rm -r util/bin
	-rm -r test/bin
	-rm -r test/produced
	@echo -e "${YELLOW}*** Cleaned binaries${NORMAL}"

clean-spec :

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
	@echo -e "${YELLOW}*** Generating include/hffix_fields.hpp from FIX specs...${NORMAL}"
	cd spec/fix.4.2; unzip -u fix-42-with_errata_20010501_pdf.zip
	cd spec/fix.4.3; unzip -u FIX-43-with_errata_20020920_PDF.ZIP
	cd spec/fix.4.4; unzip -u fix-44_w_Errata_20030618_PDF.zip
	cd spec/fix.5.0.sp2; unzip -u -d fixmlschema fixmlschema_FIX.5.0SP2_Errata_20131209.zip
	cd spec;./codegen > ../include/hffix_fields.hpp
	@echo -e "${YELLOW}*** Generated include/hffix_fields.hpp from FIX specs${NORMAL}"

fixprint : util/bin/fixprint

util/bin/fixprint : util/src/fixprint.cpp include/hffix.hpp include/hffix_fields.hpp
	@echo -e "${YELLOW}*** Building fixprint utility util/bin/fixprint ...${NORMAL}"
	mkdir -p util/bin
	$(CXX) $(CXXFLAGS) -o util/bin/fixprint util/src/fixprint.cpp
	@echo -e "${YELLOW}*** Built fixprint utility util/bin/fixprint${NORMAL}"

test/bin/writer01 : test/src/writer01.cpp include/hffix.hpp include/hffix_fields.hpp
	@echo -e "${YELLOW}*** Building test/bin/writer01 ...${NORMAL}"
	mkdir -p test/bin
	$(CXX) $(CXXFLAGS) -o test/bin/writer01 test/src/writer01.cpp
	@echo -e "${YELLOW}*** Built test/bin/writer01${NORMAL}"

test/bin/reader01 : test/src/reader01.cpp include/hffix.hpp include/hffix_fields.hpp
	@echo -e "${YELLOW}*** Building test/bin/reader01 ...${NORMAL}"
	mkdir -p test/bin
	$(CXX) $(CXXFLAGS) -o test/bin/reader01 test/src/reader01.cpp
	@echo -e "${YELLOW}*** Built test/bin/reader01${NORMAL}"

examples : test/bin/writer01 test/bin/reader01

unit_tests :
	@echo -e "${YELLOW}*** Building test/bin/unit_tests ...${NORMAL}"
	$(CXX) $(CXXFLAGS) -std=c++11 -o test/bin/unit_tests test/src/unit_tests.cpp
	@echo -e "${YELLOW}*** Built test/bin/unit_tests ...${NORMAL}"
	@echo -e "${YELLOW}*** Running test/bin/unit_tests ...${NORMAL}"
	test/bin/unit_tests --color_output=true
	@echo -e "${YELLOW}*** Passed test/bin/unit_tests ...${NORMAL}"

ctags :
	ctags include/*

README.html : README.md
	pandoc --standalone --from markdown --to html < README.md > README.html

test : fixprint test01 test02 unit_tests

test01 : test/bin/writer01
	@echo -e "${YELLOW}*** Running $@ ...${NORMAL}"
	mkdir -p test/produced
	test/bin/writer01 > test/produced/writer01.fix
	diff test/expected/writer01.fix test/produced/writer01.fix || (echo -e "${YELLOW}*** $@ failed${NORMAL}" && exit 1)
	@echo -e "${YELLOW}*** Passed $@ ${NORMAL}"

test02 : test/bin/reader01 test/bin/writer01
	@echo -e "${YELLOW}*** Running $@ ...${NORMAL}"
	mkdir -p test/produced
	test/bin/writer01 | test/bin/reader01 > test/produced/reader01.txt
	diff test/expected/reader01.txt test/produced/reader01.txt || (echo -e "${YELLOW}*** $@ failed${NORMAL}" && exit 1)
	@echo -e "${YELLOW}*** Passed $@ ${NORMAL}"

.PHONY : help doc all clean clean-all clean-bin fixprint specs ctags examples test test01 test02 unit_tests
