## Building Stuff

### Prerequisites

On OS X machine. Most of the work is orchestrated by make. The build scripts, test runner, and
other utilities are all written in [Dart][]:

```sh
$ make get
```

[dart]: https://dart.dev/
[install]: https://dart.dev/get-dart

This downloads all of the packages used by the build and test scripts.

In order to compile the two interpreters, you also need a C compiler on your
path as well as `javac`.

### Building

Once you've got that setup, try:

```sh
$ make
```

If everything is working, that will generate the site for the book as well as
compiling the two interpreters clox and jlox. You can run either interpreter
right from the root of the repo:

```sh
$ ./clox
$ ./jlox
```

### Building the interpreters

You can build each interpreter like so:

```sh
$ make clox
$ make jlox
```

This builds the final version of each interpreter as it appears at the end of
its part in the book.

### your build up

for the chapter by chapter accumulating build-up of clox, here it is in folder '/vm':

```sh
$ make clox "SNIPPET=true"
```

## Testing

I have a full Lox test suite that I use to ensure the interpreters in the book
do what they're supposed to do. The test cases live in `test/`. The Dart
program `tool/bin/test.dart` is a test runner that runs each of those test
files on a Lox interpreter, parses the result, and validates that that the test
does what it's expected to do.

There are various interpreters you can run the tests against:

```sh
$ make test       # The final versions of clox and jlox.
$ make test_clox  # The final version of clox.
$ make test_jlox  # The final version of jlox.
$ make test_c     # Every chapter's version of clox.
$ make test_java  # Every chapter's version of jlox.
$ make test_all   # All of the above.
```

### my way of testing
```sh
$ make test_clox --debug  # for c version
$ dart tool/bin/test.dart clox --interpreter ./clox

$ make test_jlox --debug  # for java version
```

### Testing your implementation

You are welcome to use the test suite and the test runner to test your own Lox
implementation. The test runner is at `tool/bin/test.dart` and can be given a
custom interpreter executable to run using `--interpreter`. For example, if you
had an interpreter executable at `my_code/boblox`, you could test it like:

```sh
$ dart tool/bin/test.dart clox --interpreter my_code/boblox
```

You still need to tell it which suite of tests to run because that determines
the test expectations. If your interpreter should behave like jlox, use "jlox"
as the suite name. If it behaves like clox, use "clox". If your interpreter is
only complete up to the end of one of the chapters in the book, you can use
that chapter as the suite, like "chap10_functions". See the Makefile for the
names of all of the chapters.

If your interpreter needs other command line arguments passed to use, pass them
to the test runner using `--arguments` and it will forward to your interpreter.

## Repository Layout

*   `asset/` – Sass files and jinja2 templates used to generate the site.
*   `book/` - Markdown files for the text of each chapter.
*   `build/` - Intermediate files and other build output (except for the site
    itself) go here. Not committed to Git.
*   `c/` – Source code of clox, the interpreter written in C. Also contains an
    XCode project, if that's your thing.
*   `gen/` – Java source files generated by GenerateAst.java go here. Not
    committed.
*   `java/` – Source code of jlox, the interpreter written in Java.
*   `note/` – Various research, notes, TODOs, and other miscellanea.
*   `note/answers` – Sample answers for the challenges. No cheating!
*   `site/` – The final generated site. The contents of this directory directly
    mirror craftinginterpreters.com. Most content here is generated by build.py,
    but fonts, images, and JS only live here. Everything is committed, even the
    generated content.
*   `test/` – Test cases for the Lox implementations.
*   `tool/` – Dart package containing the build, test, and other scripts.
