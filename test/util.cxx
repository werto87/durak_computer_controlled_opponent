#include "src/cxx/util.hxx"
#include "src/cxx/database.hxx"
#include <catch2/catch.hpp>
#include <confu_soci/convenienceFunctionForSoci.hxx>
#include <cstddef>
#include <sstream>
#include <vector>

TEST_CASE ("database setup", "[abc]") { REQUIRE (gameStateAsString ({ { '1', '2', '3', 'a' }, { 'c', 'b' } }) == "1,2,3,a c,b"); }