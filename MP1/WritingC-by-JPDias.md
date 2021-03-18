# Writing C code
	João Pedro Dias, Feb.2021

## Coding conventions and safety checks

Using `clang-tidy` we can assert the code for portability, readability and performance while following the [Google C/C++ coding conventions](https://google.github.io/styleguide/cppguide.html). Feel free to use any additional checks on your code.

1. `$ sudo apt install python3 python3-pip` : if you still don't have python
2. `$ pip3 install cpplint` [cpplint](https://github.com/cpplint/cpplint)
3. `$ cpplint --filter=-legal/copyright *.c`

The 3. runs checks for all the `.c` files in the current folder. To run to all files in all folders use the following script:

```sh
#!/bin/bash

THIS_PATH="$(realpath "$0")"
THIS_DIR="$(dirname "$THIS_PATH")"

# Find all files in THIS_DIR which end in .ino, .cpp, etc., as specified
# in the regular expression just below
FILE_LIST="$(find "$THIS_DIR" | grep -E ".*(\.c|\.h)$")"

echo -e "Files found to format = \n\"\"\"\n$FILE_LIST\n\"\"\""

# Run for all the files

cpplint --filter=-legal/copyright,-whitespace,-readability/casting,-readability/todo $FILE_LIST
```

[Visual Studio Code cpplint extension](https://marketplace.visualstudio.com/items?itemName=mine.cpplint)

## Static Analysis

Infer is a static analysis tool - if you give Infer some Java or C/C++/Objective-C code it produces a list of potential bugs. [Read more](https://fbinfer.com/).

Version to be used: VERSION=1.0.0

1. `$ wget https://github.com/facebook/infer/releases/download/v1.0.0/infer-linux64-v1.0.0.tar.xz`
2. `$ sudo tar -xf infer-linux64-v1.0.0.tar.xz -C /opt`
3. `$ ln -s "/opt/infer-linux64-v1.0.0/bin/infer" /usr/local/bin/infer`
4. `$ infer run -- make` : runs infer for all the files used in the makefile. A report is generated.

Possible issues:

- Missing `libtinfo.so`: `$ sudo apt install libncurses5`
