CXX = gcc -lstdc++
CFLAGS = -std=c++17 -Wall -Werror -Wextra
TEST_LIBS = -lgtest
ARCHIVE = archive-calculator2
BUILD = build
INSTALL_DIR = ~/Desktop/

all: clean install dist tests check

install: clean uninstall
	mkdir -p ${BUILD}
	cd ${BUILD} && 										  \
	cmake ../Calculator\ v2.0/CMakeLists.txt && 		  \
	${MAKE} -j6 && rm -rf ${INSTALL_DIR}/3DViewer.app &&  \
	mv -f Calculator.app Calculator\ 2.0.app && 		  \
	mv -f Calculator\ 2.0.app ${INSTALL_DIR}

open:
	open ~/Desktop/Calculator\ 2.0.app

uninstall:
	rm -rf ~/Desktop/Calculator\ 2.0.app

clean:
	rm -rf ${BUILD} ${ARCHIVE}*
	rm -rf ./report test.out

dvi:
	open dvi.tex -a Visual\ Studio\ Code

dist:
	mkdir -p ${ARCHIVE}
	cp -R Calculator\ v2.0/ ${ARCHIVE}/
	tar -cf ${ARCHIVE}.tar ${ARCHIVE}
	gzip -9f ${ARCHIVE}.tar
	rm -rf ${ARCHIVE}

tests:
	$(CXX) $(CFLAGS) $(TEST_LIBS) Unit-tests/*.cpp Calculator\ v2.0/Model/*.cpp Calculator\ v2.0/Controller/*.cpp -o test.out
	./test.out

gcov_report:
	$(CXX) $(CFLAGS) --coverage $(TEST_LIBS) Unit-tests/*.cpp Calculator\ v2.0/Model/*.cpp Calculator\ v2.0/Controller/*.cpp -o gcov_test
	chmod +x *
	./gcov_test
	lcov -t "gcov_test" -o gcov_test.info --no-external -c -d .
	genhtml -o report/ gcov_test.info
	open ./report/index.html
	rm -rf ./*.gcno ./*.gcda ./gcov*

check:
	clang-format --style=Google --dump-config > .clang-format
	cd Calculator\ v2.0/ && clang-format -n 		\
							../Unit-tests/*.cpp 	\
							Model/*.cpp 			\
							Model/*.h 				\
							Controller/*.cpp 		\
							Controller/*.h 			\
							View/Graphics/*.h 		\
							View/Graphics/*.cpp 	\
							View/ViewCalc/*.h 		\
							View/ViewCalc/*.cpp 	\
							View/ViewCredit/*.h 	\
							View/ViewCredit/*.cpp 	\
							View/ViewDeposit/*.h  	\
							View/ViewDeposit/*.cpp 	\
							main.cpp
	rm -f .clang-format
