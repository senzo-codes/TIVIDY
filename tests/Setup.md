# Catch2 Setup

## This documentation otlines steps required to setup catch2 v2.

---

| Content | Redirection |
|---------|-------------|
|Purpose||

---

## **Purpose**

1. By the end of this document you should have catch2 v2 installed, and running tests defined in the project.

## **Prerequisites**

1. Operating system: No specific OS, It is a c++ library.
2. Language: It accepts c++11 or newer
3. Compiler: GCC, CLang, or MSVC
4. CMake: If using CMake, it must be version 3.5 or higher


## **Folder stucture**

``` text

|- MAKEFILE
|- docs/
|- Diagrams/
|- tests/
    |- test_main.cpp
    |- test_testname.cpp
```

## **Setup and Installation**

1. **Clone the Repository**

``` bash
git clone https://github.com/senzo-codes/TIVIDY
cd TIVIDY
```

2. **Installing catch2 v2**

# Linux and macOS
``` bash
curl -L -o catch.hpp https://github.com/catchorg/Catch2/releases/download/v2.13.10/catch.hpp
```

# Windows (PowerShell)
``` bash
Invoke-WebRequest -Uri https://github.com/catchorg/Catch2/releases/download/v2.13.10/catch.hpp -OutFile catch.hpp
```

## **Running**

## There are two ways thatt you can run the tests:
	1. Using the MAKEFILE
	2. Using the Terminal commands


## For using the MAKEFILE

``` bash
make clean
make
make test
```


## For using the Terminal commands

# Linux

``` bash
cd tests
g++ -std=c++11 test_testname.cpp -o test
./test
```

# macOS

``` bash
cd tests
clang++ -std=c++11 test_testname.cpp -o test
./tests
```

# Windows

``` bash
cd tests
cl /EHsc test_testname.cpp /Fe:test.exe
test.exe 
```
