# ProImg

ProImg is a tool designed for analyzing and manipulating images with ease.

* [Description](#description)
* [How to Use](#how-to-use)
   * [Keyboard Inputs](#keyboard-inputs)
   * [Unit Tests](#unit-tests)
* [Further Information](#further-information)
   * [Tools and Frameworks Used](#tools-and-frameworks-used)
   * [Project Structure](#project-structure)
* [License](#license)
* [Additional Resources](#additional-resources)

## Description

ProImg v1.0.0 is the first stable release of a bigger project which is under development. ProImg v1.0.0 supports only 8-bit per channel ppm images. ProImg is designed for UNIX-like operating systems (e.g. macOS, Linux), so it does not support Windows. ProImg uses Bazel as its build system, and is written entirely in C++23 standard.

## How to Use

[Install Bazel](https://bazel.build/install)

``` bash
git clone https://github.com/Ahmetofc/ProImg.git
cd ProImg

# Build the project
bazel build //app:ProImg

# Run the binary
bazel run //app:ProImg -- test.ppm
```

Note that test.ppm stands for any image filename that you have put in the data/ directory **prior** to compilation of the binary. The saved images will appear in the out/ directory, along with the session log in the logs/ directory, if quitted expectedly (see [Keyboard Inputs](#keyboard-inputs) for more information).

### Keyboard Inputs

- B: Gaussian blur
- C: Contrast
- E: Exposure
- S: Saturation
- R: Red channel extraction
- G: Green channel extraction
- M: Blue channel extraction
- Y: Luma channel extraction
- U: Chroma blue channel extraction
- I: Chroma red channel extraction
- L: Logging the actions (up to 10)
- 0-9, ., -: Used for changing the value of the intensity
- CTRL+Z: Undo
- CTRL+SHIFT+Z: Redo
- CTRL+Q: Quitting ProImg (also creates a log file of the session in the logs/ directory, logging will not happen if the program is exitted prematurely)
- CTRL+S: Activating save mode
- Alphanumeric Characters, ., -, _ (save mode): Changing the name of the output file
- ENTER: Reapplying the previous effect (not supported for extraction actions)
- ENTER (save mode): Saving the file (ends the save mode)
- BACKSPACE: Removing characters from the buffers (both intensity and name)

### Unit Tests

Unit tests for the math library are defined under the tests/ directory. They can be run via the following command:

``` bash
# Unit Tests
bazel test //tests:testfile
```

Note that testfile stands for any C++ test defined in the tests/BUILD.bazel file. The current ones are:

- complex_test
- matrix_test
- range_test
- vector_test

Users are encouraged to write their own test files. To be able to run them, you would need the following:
1. Create a test.cc file under tests/ directory.
2. In tests/BUILD.bazel file have a cc_test rule, with you test.cc file declared in sources.
3. Run the command above, replacing the testfile placeholder with you test name.

## Further Information

ProImg also comes with math and image frameworks for independent usage. It provides libraries to write and extract data to/from ppm images, general linear algebra operations with the namespace math, and visualization/extraction functions.

### Tools and Frameworks Used

- GoogleTest Framework for unit tests
- SDL2 Library for CPU-side real-time rendering
- Bzlmod for building the project and organizing the dependencies

### Project Structure

``` text
ProImg
│
├── app/
│   ├── BUILD.bazel
│   ├── log-inl.h
│   └── proimg.cc
│
├── data/
│   └── BUILD.bazel
│
├── image/
│   ├── BUILD.bazel
│   ├── effects.h
│   ├── effects-inl.h
│   ├── ppm.h
│   └── ppm-inl.h
│
├── logs/
│
├── math/
│   ├── BUILD.bazel
│   ├── complex.h
│   ├── complex-inl.h
│   ├── convolution.h
│   ├── convolution-inl.h
│   ├── macros.h
│   ├── matrix.h
│   ├── matrix-inl.h
│   ├── BUILD.bazel
│   ├── random.h
│   ├── range.h
│   ├── types.h
│   ├── vector.h
│   └── vector-inl.h
│
├── out/
│
├── tests/
│   ├── BUILD.bazel
│   ├── complex_test.cc
│   ├── matrix_test.cc
│   ├── range_test.cc
│   └── vector_test.cc
│
├── .bazelrc
├── .gitignore
├── LICENSE.txt
├── MODULE.bazel
├── MODULE.bazel.lock
└── README.md
```

## License

MIT License

Copyright (c) 2026 Ahmet Rakap

## Additional Resources

Since finding raw ppm image files on the internet is a challenge in and of itself, here are some links to websites that provide ppm images:

* [Rhodes University - Image Processing](https://www.cs.rhodes.edu/~kirlinp/courses/cs1/f19/projects/proj8/)
* [Hlevkin - Test Images Collection](https://www.hlevkin.com/hlevkin/06testimages.htm)
* [Cornell University - CS 664](https://www.cs.cornell.edu/courses/cs664/2003fa/images/)
