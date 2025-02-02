#include "AnemoneRuntime/Platform/Base/BaseHeaders.hxx"
#include "AnemoneRuntime/System/Path.hxx"

ANEMONE_EXTERNAL_HEADERS_BEGIN

#include <catch_amalgamated.hpp>

ANEMONE_EXTERNAL_HEADERS_END

TEST_CASE("Path - Add Directory Separator")
{
    using namespace Anemone::System;

    SECTION("Empty path")
    {
        std::string result{};

        Path::AddDirectorySeparator(result);

        REQUIRE(result == "/");
    }

    SECTION("Path without separator")
    {
        std::string result = "/path/to/dir";

        Path::AddDirectorySeparator(result);

        REQUIRE(result == "/path/to/dir/");
    }

    SECTION("Path with separator")
    {
        std::string result = "/path/to/dir/";

        Path::AddDirectorySeparator(result);

        REQUIRE(result == "/path/to/dir/");
    }
}

TEST_CASE("Path - Remove Directory Separator")
{
    using namespace Anemone::System;

    SECTION("Empty path")
    {
        std::string result{};

        Path::RemoveDirectorySeparator(result);

        REQUIRE(result.empty());
    }

    SECTION("Path without separator")
    {
        std::string result = "/path/to/dir";

        Path::RemoveDirectorySeparator(result);

        REQUIRE(result == "/path/to/dir");
    }

    SECTION("Path with separator")
    {
        std::string result = "/path/to/dir/";

        Path::RemoveDirectorySeparator(result);

        REQUIRE(result == "/path/to/dir");
    }
}

TEST_CASE("Path - NormalizeDirectorySeparators")
{
    using namespace Anemone::System;

    SECTION("Normal case")
    {
        std::string path = "path/to/file.txt";

        Path::NormalizeDirectorySeparators(path);

        REQUIRE(path == "path/to/file.txt");
    }

#if defined(WIN32)
    SECTION("Windows-style path separators")
    {
        std::string path = "path\\to\\file.txt";

        Path::NormalizeDirectorySeparators(path);

        REQUIRE(path == "path/to/file.txt");
    }

    SECTION("Mixed separators")
    {
        std::string path = "path\\to/file.txt";

        Path::NormalizeDirectorySeparators(path);

        REQUIRE(path == "path/to/file.txt");
    }
#endif

    SECTION("Collapsing separators")
    {
        std::string path = "path//to/file.txt";

        Path::NormalizeDirectorySeparators(path);

        REQUIRE(path == "path/to/file.txt");
    }
}

TEST_CASE("Path - Push / Pop")
{
    using namespace Anemone::System;

    std::string path{};

    Path::Push(path, "path");
    REQUIRE(path == "path");

    Path::Push(path, "to");
    REQUIRE(path == "path/to");

    Path::Push(path, "file.txt");
    REQUIRE(path == "path/to/file.txt");

    SECTION("Pop")
    {
        REQUIRE(Path::Pop(path));
        REQUIRE(path == "path/to");

        REQUIRE(Path::Pop(path));
        REQUIRE(path == "path");

        REQUIRE(Path::Pop(path));
        REQUIRE(path.empty());

        REQUIRE_FALSE(Path::Pop(path));
        REQUIRE(path.empty());
    }

    SECTION("Pop with tail")
    {
        std::string tail{};

        REQUIRE(Path::Pop(path, tail));
        REQUIRE(path == "path/to");
        REQUIRE(tail == "file.txt");

        REQUIRE(Path::Pop(path, tail));
        REQUIRE(path == "path");
        REQUIRE(tail == "to");

        REQUIRE(Path::Pop(path, tail));
        REQUIRE(path.empty());
        REQUIRE(tail == "path");

        REQUIRE_FALSE(Path::Pop(path, tail));
        REQUIRE(path.empty());
        REQUIRE(tail.empty());
    }
}

TEST_CASE("Path - SetExtension")
{
    using namespace Anemone::System;

    SECTION("Empty path")
    {
        std::string path{};

        Path::SetExtension(path, ".txt");

        REQUIRE(path == ".txt");
    }

    SECTION("Path without extension")
    {
        std::string path = "path/to/file";

        Path::SetExtension(path, ".txt");

        REQUIRE(path == "path/to/file.txt");
    }

    SECTION("Path with extension")
    {
        std::string path = "path/to/file.txt";

        Path::SetExtension(path, ".cfg");

        REQUIRE(path == "path/to/file.cfg");
    }

    SECTION("Extension with multiple dots")
    {
        std::string path = "path/to/file.txt";

        Path::SetExtension(path, "....cfg");

        REQUIRE(path == "path/to/file.cfg");
    }

    SECTION("Extension without dots")
    {
        std::string path = "path/to/file.txt";

        Path::SetExtension(path, "cfg");

        REQUIRE(path == "path/to/file.cfg");
    }

    SECTION("Invalid case - extension separator before directory separator")
    {
        std::string path = "path/to.dir/file";

        Path::SetExtension(path, ".txt");

        REQUIRE(path == "path/to.dir/file.txt");
    }
}

TEST_CASE("Path - Set Filename")
{
    using namespace Anemone::System;

    SECTION("Empty path")
    {
        std::string path{};

        Path::SetFilename(path, "changed.txt");

        REQUIRE(path == "changed.txt");
    }

    SECTION("Path without filename")
    {
        std::string path = "path/to/";

        Path::SetFilename(path, "changed.txt");

        REQUIRE(path == "path/to/changed.txt");
    }

    SECTION("Path with filename")
    {
        std::string path = "path/to/existing.cfg";

        Path::SetFilename(path, "changed.txt");

        REQUIRE(path == "path/to/changed.txt");
    }
}

TEST_CASE("Path - GetExtension")
{
    using namespace Anemone::System;

    SECTION("Empty path")
    {
        std::string const path{};

        REQUIRE(Path::GetExtension(path).empty());
    }

    SECTION("Path without trailing separator")
    {
        std::string const path = "path/to/dir";

        REQUIRE(Path::GetExtension(path).empty());
    }

    SECTION("Path with trailing separator")
    {
        std::string const path = "path/to/dir/";

        REQUIRE(Path::GetExtension(path).empty());
    }

    SECTION("Path with trailing separator and extension")
    {
        std::string const path = "path/to/dir/.txt";

        REQUIRE(Path::GetExtension(path) == ".txt");
    }

    SECTION("Path to filename with extension")
    {
        std::string const path = "path/to/dir/filename.txt";

        REQUIRE(Path::GetExtension(path) == ".txt");
    }

    SECTION("Extension from last path segment")
    {
        std::string const path = "path/to.some/file.txt";

        REQUIRE(Path::GetExtension(path) == ".txt");
    }

    SECTION("Extension from last path segment")
    {
        std::string const path = "path/to.some/file";

        REQUIRE(Path::GetExtension(path).empty());
    }
}

TEST_CASE("Path - GetFilename")
{
    using namespace Anemone::System;

    SECTION("Empty path")
    {
        std::string const path{};

        REQUIRE(Path::GetFilename(path).empty());
    }

    SECTION("Path without trailing separator")
    {
        std::string const path = "path/to/dir";

        REQUIRE(Path::GetFilename(path) == "dir");
    }

    SECTION("Path with trailing separator")
    {
        std::string const path = "path/to/dir/";

        REQUIRE(Path::GetFilename(path).empty());
    }

    SECTION("Path with trailing separator and extension")
    {
        std::string const path = "path/to/dir/.txt";

        REQUIRE(Path::GetFilename(path) == ".txt");
    }

    SECTION("Path to filename with extension")
    {
        std::string const path = "path/to/dir/filename.txt";

        REQUIRE(Path::GetFilename(path) == "filename.txt");
    }
}

TEST_CASE("Path - GetFilenameWithoutExtension")
{
    using namespace Anemone::System;

    SECTION("Empty path")
    {
        std::string const path{};

        REQUIRE(Path::GetFilenameWithoutExtension(path).empty());
    }

    SECTION("Path without trailing separator")
    {
        std::string const path = "path/to/dir";

        REQUIRE(Path::GetFilenameWithoutExtension(path) == "dir");
    }

    SECTION("Path with trailing separator")
    {
        std::string const path = "path/to/dir/";

        REQUIRE(Path::GetFilenameWithoutExtension(path).empty());
    }

    SECTION("Path with trailing separator and extension")
    {
        std::string const path = "path/to/dir/.txt";

        REQUIRE(Path::GetFilenameWithoutExtension(path).empty());
    }

    SECTION("Path to filename with extension")
    {
        std::string const path = "path/to/dir/filename.txt";

        REQUIRE(Path::GetFilenameWithoutExtension(path) == "filename");
    }
}
