BUILD_DIR := build
TOOL_SOURCES := tool/pubspec.lock $(shell find tool -name '*.dart')
BUILD_SNAPSHOT := $(BUILD_DIR)/build.dart.snapshot
TEST_SNAPSHOT := $(BUILD_DIR)/test.dart.snapshot

default: clox jlox

# Run dart pub get on tool directory.
get:
	@ cd ./tool; dart pub get

# Remove all build outputs and intermediate files.
clean:
	@ rm -rf $(BUILD_DIR)
	@ rm -rf gen

$(BUILD_SNAPSHOT): $(TOOL_SOURCES)
	@ mkdir -p build
	@ echo "Compiling Dart snapshot..."
	@ dart --snapshot=$@ --snapshot-kind=app-jit tool/bin/build.dart >/dev/null

# Run the tests for the final versions of clox and jlox.
test: debug jlox $(TEST_SNAPSHOT)
	@- dart $(TEST_SNAPSHOT) clox
	@ dart $(TEST_SNAPSHOT) jlox

# Run the tests for the final version of clox.
test_clox: debug $(TEST_SNAPSHOT)
	dart $(TEST_SNAPSHOT) clox

# Run the tests for the final version of jlox.
test_jlox: jlox $(TEST_SNAPSHOT)
	@ dart $(TEST_SNAPSHOT) jlox

# Run the tests for every chapter's version of clox and jlox.
test_all: debug jlox compile_snippets $(TEST_SNAPSHOT)
	@ dart $(TEST_SNAPSHOT) all

$(TEST_SNAPSHOT): $(TOOL_SOURCES)
	@ mkdir -p build
	@ echo "Compiling Dart snapshot..."
	@ dart --snapshot=$@ --snapshot-kind=app-jit tool/bin/test.dart clox

# Compile a debug build of clox.
debug:
	@ $(MAKE) -f util/c.make NAME=cloxd MODE=debug SOURCE_DIR=c

# Compile the C interpreter.
clox:
	@ $(MAKE) -f util/c.make NAME=clox MODE=release SOURCE_DIR=c
	@ cp build/clox clox # For convenience, copy the interpreter to the top level.

# Compile and run the AST generator.
generate_ast:
	@ $(MAKE) -f util/java.make DIR=java PACKAGE=tool
	@ java -cp build/java com.craftinginterpreters.tool.GenerateAst \
			java/com/craftinginterpreters/lox

# Compile the Java interpreter .java files to .class files.
jlox: generate_ast
	@ $(MAKE) -f util/java.make DIR=java PACKAGE=lox

run_generate_ast = @ java -cp build/gen/$(1) \
			com.craftinginterpreters.tool.GenerateAst \
			gen/$(1)/com/craftinginterpreters/lox

compile_snippets:
	@ dart tool/bin/compile_snippets.dart

.PHONY: clean clox compile_snippets debug default get jlox test test_all test_c test_java
