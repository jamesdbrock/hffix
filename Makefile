CXXFLAGS += -Wall -Iinclude

NORMAL=\033[0m
YELLOW=\033[1;33m

all : spec include/hffix_fields.hpp doc fixprint examples

doc : doc/html/index.html

# Insert hffix.style.css into the head of only the index.html file, https://stackoverflow.com/questions/26141347/using-sed-to-insert-file-content-into-a-file-before-a-pattern
doc/html/index.html : doc/hffix.style.css include/hffix.hpp include/hffix_fields.hpp doc/Doxyfile README.md
	@echo -e "${YELLOW}*** Generating Doxygen in doc/html/ ...${NORMAL}"
	cd doc;rm -r html;doxygen Doxyfile
	cd doc;sed --in-place $$'/<\/head>/{e cat hffix.style.css\n}' html/index.html
	@echo -e "${YELLOW}*** Generated Doxygen in doc/html/${NORMAL}"

clean : clean-bin

clean-all : clean-bin clean-spec
	@echo -e "${YELLOW}*** Cleaning all artifacts ...${NORMAL}"
	-rm -r doc/html
	-rm -f ctags
	@echo -e "${YELLOW}*** Cleaned all artifacts${NORMAL}"

clean-bin :
	@echo -e "${YELLOW}*** Cleaning binaries ...${NORMAL}"
	-rm -r util/bin
	-rm -r test/bin
	-rm -r test/produced
	@echo -e "${YELLOW}*** Cleaned binaries${NORMAL}"

# Clean up all the specification documents except the ones needed to build hffix_fields.hpp
clean-spec:
	@echo -e "${YELLOW}*** Cleaning all fixspecs ...${NORMAL}"
	-rm fixspec/fix.4.2/*.pdf
	-rm fixspec/fix.4.3/*.pdf
	-rm fixspec/fix.4.4/*.pdf
	-GLOBIGNORE="fixspec/fix.5.0.sp2/fixmlschema/fixml-fields-base-5-0-SP2.xsd"; rm -r fixspec/fix.5.0.sp2/fixmlschema/*
	@echo -e "${YELLOW}*** Cleaned fixspecs${NORMAL}"

# Unzip all the specification documents
spec :
	cd fixspec/fix.4.2; unzip -u fix-42-with_errata_20010501_pdf.zip
	cd fixspec/fix.4.3; unzip -u FIX-43-with_errata_20020920_PDF.ZIP
	cd fixspec/fix.4.4; unzip -u fix-44_w_Errata_20030618_PDF.zip
	cd fixspec/fix.5.0.sp2; unzip -u -d fixmlschema fixmlschema_FIX.5.0SP2_Errata_20131209.zip

fixspec/fix.4.3/fixml4.3v20020920.dtd:
	cd fixspec/fix.4.3; unzip -u FIX-43-with_errata_20020920_PDF.ZIP fixml4.3v20020920.dtd

fixspec/fix.4.4/FIXML4.4v20030618.dtd:
	cd fixspec/fix.4.4; unzip -u fix-44_w_Errata_20030618_PDF.zip FIXML4.4v20030618.dtd

fixspec/fix.5.0.sp2/fixmlschema/fixml-fields-base-5-0-SP2.xsd:
	cd fixspec/fix.5.0.sp2; unzip -u -d fixmlschema fixmlschema_FIX.5.0SP2_Errata_20131209.zip fixml-fields-base-5-0-SP2.xsd

# This build step requires the Haskell Tool Stack
include/hffix_fields.hpp: \
    fixspec/spec-parse-fields/src/Main.hs \
    fixspec/fix.4.3/fixml4.3v20020920.dtd \
    fixspec/fix.4.4/FIXML4.4v20030618.dtd \
    fixspec/fix.5.0.sp2/fixmlschema/fixml-fields-base-5-0-SP2.xsd
	@echo -e "${YELLOW}*** Generating include/hffix_fields.hpp from FIX specs...${NORMAL}"
	-cd fixspec/spec-parse-fields && stack run --cwd .. > hffix_fields.hpp && mv hffix_fields.hpp ../../include/hffix_fields.hpp
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

test/bin/writer02 : test/src/writer02.cpp include/hffix.hpp include/hffix_fields.hpp
	@echo -e "${YELLOW}*** Building test/bin/writer02 ...${NORMAL}"
	mkdir -p test/bin
	$(CXX) $(CXXFLAGS) -std=c++11 -o test/bin/writer02 test/src/writer02.cpp
	@echo -e "${YELLOW}*** Built test/bin/writer02${NORMAL}"

test/bin/reader01 : test/src/reader01.cpp include/hffix.hpp include/hffix_fields.hpp
	@echo -e "${YELLOW}*** Building test/bin/reader01 ...${NORMAL}"
	mkdir -p test/bin
	$(CXX) $(CXXFLAGS) -o test/bin/reader01 test/src/reader01.cpp
	@echo -e "${YELLOW}*** Built test/bin/reader01${NORMAL}"

test/bin/reader02 : test/src/reader02.cpp include/hffix.hpp include/hffix_fields.hpp
	@echo -e "${YELLOW}*** Building test/bin/reader02 ...${NORMAL}"
	mkdir -p test/bin
	$(CXX) $(CXXFLAGS) -std=c++11 -o test/bin/reader02 test/src/reader02.cpp
	@echo -e "${YELLOW}*** Built test/bin/reader02${NORMAL}"

examples : test/bin/writer01 test/bin/writer02 test/bin/reader01 test/bin/reader02

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

test01 : test/bin/writer01 test/bin/writer02
	@echo -e "${YELLOW}*** Running $@ ...${NORMAL}"
	mkdir -p test/produced
	test/bin/writer01 > test/produced/writer01.fix
	diff test/expected/writer01.fix test/produced/writer01.fix || (echo -e "${YELLOW}*** $@ failed${NORMAL}" && exit 1)
	test/bin/writer02 > test/produced/writer02.fix
	diff test/expected/writer02.fix test/produced/writer02.fix || (echo -e "${YELLOW}*** $@ failed${NORMAL}" && exit 1)
	@echo -e "${YELLOW}*** Passed $@ ${NORMAL}"

test02 : test/bin/reader01 test/bin/reader02 test/bin/writer01
	@echo -e "${YELLOW}*** Running $@ ...${NORMAL}"
	mkdir -p test/produced
	test/bin/writer01 | test/bin/reader01 > test/produced/reader01.txt
	diff test/expected/reader01.txt test/produced/reader01.txt || (echo -e "${YELLOW}*** $@ failed${NORMAL}" && exit 1)
	test/bin/writer02 | test/bin/reader02 > test/produced/reader02.txt
	diff test/expected/reader02.txt test/produced/reader02.txt || (echo -e "${YELLOW}*** $@ failed${NORMAL}" && exit 1)
	@echo -e "${YELLOW}*** Passed $@ ${NORMAL}"

.PHONY : help doc all clean clean-all clean-bin fixprint spec ctags examples test test01 test02 unit_tests
