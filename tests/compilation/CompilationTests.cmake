# This file defines all the compilation tests

##########
# Tests for testing whether the compilation tests system works as expected.
# If at least one of those tests fails, something is wrong with the cmake scripts.

cxx_compilation_test(compilation_test_system_test_1 SHOULD_COMPILE "
int main() { return 0; }
")

cxx_compilation_test(compilation_test_system_test_2 SHOULD_FAIL "
int main(float i) { return foobar; }
")
#########
