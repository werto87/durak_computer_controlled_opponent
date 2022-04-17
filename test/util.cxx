#include "src/cxx/util.hxx"
#include <catch2/catch.hpp>

TEST_CASE ("database setup", "[abc]") { REQUIRE (gameStateAsString ({ { '1', '2', '3', 'a' }, { 'c', 'b' } }) == "1,2,3,a c,b"); }