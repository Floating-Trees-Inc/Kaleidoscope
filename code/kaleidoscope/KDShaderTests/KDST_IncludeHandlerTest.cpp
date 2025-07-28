//
// > Notice: Floating Trees Inc. @ 2025
// > Create Time: 2025-07-16 20:47:56
//

#include <KDShader/KDS_IncludeHandler.h>
#include <catch2/catch_test_macros.hpp>

const char* testIncludeShader = R"(
#include ".gitkeep"
)";

TEST_CASE("include", "[KDS::IncludeHandler]") {
    KDS::ParseResult result;
    result.Includes.push_back(KDS::IncludeRecord{ ".gitkeep", 0 });

    KDS::IncludeHandler handler;
    handler.AddIncludePath("data");
    auto src = handler.ReplaceIncludes(result, testIncludeShader);

    auto lines = KC::StringUtil::ToLines(src);
    REQUIRE(lines[0] == "1");
}
