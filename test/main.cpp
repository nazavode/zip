#define CATCH_CONFIG_MAIN
#include "catch.hpp"

TEST_CASE("runner is able to run a trivial test case", "[smoke]") {
    volatile auto True = true;
    REQUIRE(True);
}
