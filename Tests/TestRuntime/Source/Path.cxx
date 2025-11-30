#include "AnemoneInterop/Headers.hxx"
#include "AnemoneStorage/FilePath.hxx"

ANEMONE_EXTERNAL_HEADERS_BEGIN

#include <catch_amalgamated.hpp>

ANEMONE_EXTERNAL_HEADERS_END

TEST_CASE("Path - Add Directory Separator")
{
    using namespace Anemone;

    SECTION("Empty path")
    {
        std::string result{};

        FilePath::AddDirectorySeparator(result);

        REQUIRE(result == "/");
    }

    SECTION("Path without separator")
    {
        std::string result = "/path/to/dir";

        FilePath::AddDirectorySeparator(result);

        REQUIRE(result == "/path/to/dir/");
    }

    SECTION("Path with separator")
    {
        std::string result = "/path/to/dir/";

        FilePath::AddDirectorySeparator(result);

        REQUIRE(result == "/path/to/dir/");
    }
}

TEST_CASE("Path - Remove Directory Separator")
{
    using namespace Anemone;

    SECTION("Empty path")
    {
        std::string result{};

        FilePath::RemoveDirectorySeparator(result);

        REQUIRE(result.empty());
    }

    SECTION("Path without separator")
    {
        std::string result = "/path/to/dir";

        FilePath::RemoveDirectorySeparator(result);

        REQUIRE(result == "/path/to/dir");
    }

    SECTION("Path with separator")
    {
        std::string result = "/path/to/dir/";

        FilePath::RemoveDirectorySeparator(result);

        REQUIRE(result == "/path/to/dir");
    }
}

TEST_CASE("Path - NormalizeDirectorySeparators")
{
    using namespace Anemone;

    SECTION("Normal case")
    {
        std::string path = "path/to/file.txt";

        FilePath::NormalizeDirectorySeparators(path);

        REQUIRE(path == "path/to/file.txt");
    }

#if defined(WIN32)
    SECTION("Windows-style path separators")
    {
        std::string path = "path\\to\\file.txt";

        FilePath::NormalizeDirectorySeparators(path);

        REQUIRE(path == "path/to/file.txt");
    }

    SECTION("Mixed separators")
    {
        std::string path = "path\\to/file.txt";

        FilePath::NormalizeDirectorySeparators(path);

        REQUIRE(path == "path/to/file.txt");
    }
#endif

    SECTION("Collapsing separators")
    {
        std::string path = "path//to/file.txt";

        FilePath::NormalizeDirectorySeparators(path);

        REQUIRE(path == "path/to/file.txt");
    }
}

TEST_CASE("Path - Push / Pop")
{
    using namespace Anemone;

    std::string path{};

    FilePath::PushFragment(path, "path");
    REQUIRE(path == "path");

    FilePath::PushFragment(path, "to");
    REQUIRE(path == "path/to");

    FilePath::PushFragment(path, "file.txt");
    REQUIRE(path == "path/to/file.txt");

    SECTION("Pop")
    {
        REQUIRE(FilePath::PopFragment(path));
        REQUIRE(path == "path/to");

        REQUIRE(FilePath::PopFragment(path));
        REQUIRE(path == "path");

        REQUIRE(FilePath::PopFragment(path));
        REQUIRE(path.empty());

        REQUIRE_FALSE(FilePath::PopFragment(path));
        REQUIRE(path.empty());
    }

    SECTION("Pop with tail")
    {
        std::string tail{};

        REQUIRE(FilePath::PopFragment(path, tail));
        REQUIRE(path == "path/to");
        REQUIRE(tail == "file.txt");

        REQUIRE(FilePath::PopFragment(path, tail));
        REQUIRE(path == "path");
        REQUIRE(tail == "to");

        REQUIRE(FilePath::PopFragment(path, tail));
        REQUIRE(path.empty());
        REQUIRE(tail == "path");

        REQUIRE_FALSE(FilePath::PopFragment(path, tail));
        REQUIRE(path.empty());
        REQUIRE(tail.empty());
    }
}

TEST_CASE("Path - SetExtension")
{
    using namespace Anemone;

    SECTION("Empty path")
    {
        std::string path{};

        FilePath::SetExtension(path, ".txt");

        REQUIRE(path == ".txt");
    }

    SECTION("Path without extension")
    {
        std::string path = "path/to/file";

        FilePath::SetExtension(path, ".txt");

        REQUIRE(path == "path/to/file.txt");
    }

    SECTION("Path with extension")
    {
        std::string path = "path/to/file.txt";

        FilePath::SetExtension(path, ".cfg");

        REQUIRE(path == "path/to/file.cfg");
    }

    SECTION("Extension with multiple dots")
    {
        std::string path = "path/to/file.txt";

        FilePath::SetExtension(path, "....cfg");

        REQUIRE(path == "path/to/file.cfg");
    }

    SECTION("Extension without dots")
    {
        std::string path = "path/to/file.txt";

        FilePath::SetExtension(path, "cfg");

        REQUIRE(path == "path/to/file.cfg");
    }

    SECTION("Invalid case - extension separator before directory separator")
    {
        std::string path = "path/to.dir/file";

        FilePath::SetExtension(path, ".txt");

        REQUIRE(path == "path/to.dir/file.txt");
    }
}

TEST_CASE("Path - Set Filename")
{
    using namespace Anemone;

    SECTION("Empty path")
    {
        std::string path{};

        FilePath::SetFileName(path, "changed.txt");

        REQUIRE(path == "changed.txt");
    }

    SECTION("Path without filename")
    {
        std::string path = "path/to/";

        FilePath::SetFileName(path, "changed.txt");

        REQUIRE(path == "path/to/changed.txt");
    }

    SECTION("Path with filename")
    {
        std::string path = "path/to/existing.cfg";

        FilePath::SetFileName(path, "changed.txt");

        REQUIRE(path == "path/to/changed.txt");
    }
}

TEST_CASE("Path - GetExtension")
{
    using namespace Anemone;

    SECTION("Empty path")
    {
        std::string const path{};

        REQUIRE(FilePath::GetExtension(path).empty());
    }

    SECTION("Path without trailing separator")
    {
        std::string const path = "path/to/dir";

        REQUIRE(FilePath::GetExtension(path).empty());
    }

    SECTION("Path with trailing separator")
    {
        std::string const path = "path/to/dir/";

        REQUIRE(FilePath::GetExtension(path).empty());
    }

    SECTION("Path with trailing separator and extension")
    {
        std::string const path = "path/to/dir/.txt";

        REQUIRE(FilePath::GetExtension(path) == ".txt");
    }

    SECTION("Path to filename with extension")
    {
        std::string const path = "path/to/dir/filename.txt";

        REQUIRE(FilePath::GetExtension(path) == ".txt");
    }

    SECTION("Extension from last path segment")
    {
        std::string const path = "path/to.some/file.txt";

        REQUIRE(FilePath::GetExtension(path) == ".txt");
    }

    SECTION("Extension from last path segment")
    {
        std::string const path = "path/to.some/file";

        REQUIRE(FilePath::GetExtension(path).empty());
    }
}

TEST_CASE("Path - GetFilename")
{
    using namespace Anemone;

    SECTION("Empty path")
    {
        std::string const path{};

        REQUIRE(FilePath::GetFileName(path).empty());
    }

    SECTION("Path without trailing separator")
    {
        std::string const path = "path/to/dir";

        REQUIRE(FilePath::GetFileName(path) == "dir");
    }

    SECTION("Path with trailing separator")
    {
        std::string const path = "path/to/dir/";

        REQUIRE(FilePath::GetFileName(path).empty());
    }

    SECTION("Path with trailing separator and extension")
    {
        std::string const path = "path/to/dir/.txt";

        REQUIRE(FilePath::GetFileName(path) == ".txt");
    }

    SECTION("Path to filename with extension")
    {
        std::string const path = "path/to/dir/filename.txt";

        REQUIRE(FilePath::GetFileName(path) == "filename.txt");
    }
}

TEST_CASE("Path - GetBaseName")
{
    using namespace Anemone;

    SECTION("Empty path")
    {
        std::string const path{};

        REQUIRE(FilePath::GetBaseName(path).empty());
    }

    SECTION("Path without trailing separator")
    {
        std::string const path = "path/to/dir";

        REQUIRE(FilePath::GetBaseName(path) == "dir");
    }

    SECTION("Path with trailing separator")
    {
        std::string const path = "path/to/dir/";

        REQUIRE(FilePath::GetBaseName(path).empty());
    }

    SECTION("Path with trailing separator and extension")
    {
        std::string const path = "path/to/dir/.txt";

        REQUIRE(FilePath::GetBaseName(path).empty());
    }

    SECTION("Path to filename with extension")
    {
        std::string const path = "path/to/dir/filename.txt";

        REQUIRE(FilePath::GetBaseName(path) == "filename");
    }
}

TEST_CASE("Path - SetBaseName")
{
    using namespace Anemone;

    SECTION("Empty path")
    {
        std::string path{};
        FilePath::SetBaseName(path, "changed");
        REQUIRE(path == "changed");
    }

    SECTION("File name with extension")
    {
        std::string path = "file.ext";
        FilePath::SetBaseName(path, "changed");
        REQUIRE(path == "changed.ext");
    }

    SECTION("Path without trailing separator")
    {
        std::string path = "path/to/dir";
        FilePath::SetBaseName(path, "changed");
        REQUIRE(path == "path/to/changed");
    }

    SECTION("Path with trailing separator")
    {
        std::string path = "path/to/dir/";
        FilePath::SetBaseName(path, "changed");
        REQUIRE(path == "path/to/dir/changed");
    }

    SECTION("Path to file with extension")
    {
        std::string path = "path/to/dir/file.ext";
        FilePath::SetBaseName(path, "changed");
        REQUIRE(path == "path/to/dir/changed.ext");
    }
}

TEST_CASE("Path - Normalize")
{
    using namespace Anemone;

    SECTION("Empty path")
    {
        std::string path{};
        FilePath::Normalize(path);
        REQUIRE(path.empty());
    }

    SECTION("Path with single dot")
    {
        std::string path = "path/to/dir/.";
        FilePath::Normalize(path);
        REQUIRE(path == "path/to/dir");
    }

    SECTION("Path with double dot")
    {
        std::string path = "path/to/dir/../file.txt";
        FilePath::Normalize(path);
        REQUIRE(path == "path/to/file.txt");
    }

    SECTION("Path with multiple dots")
    {
        std::string path = "./path/./to/dir/../file.txt";
        FilePath::Normalize(path);
        REQUIRE(path == "./path/to/file.txt");
    }

    SECTION("Path with leading double dots")
    {
        std::string path = "../../path/to/file.txt";
        FilePath::Normalize(path);
        REQUIRE(path == "../../path/to/file.txt");
    }

    SECTION("Path with consecutive separators")
    {
        std::string path = "path//to///file.txt";
        FilePath::Normalize(path);
        REQUIRE(path == "path/to/file.txt");
    }

    SECTION("Complex path")
    {
        std::string path = "/a/b/./c/../d//e/../f/./g/../../h.txt";
        FilePath::Normalize(path);
        REQUIRE(path == "/a/b/d/h.txt");
    }
}

TEST_CASE("Path - Relative")
{
    using namespace Anemone;

    SECTION("Same path")
    {
        std::string result{};
        REQUIRE(FilePath::Relative(result, "/a/b/c/", "/a/b/c"));
        REQUIRE(result == "../c");
    }

    SECTION("Same path")
    {
        std::string result{};
        REQUIRE(FilePath::Relative(result, "/a/b/c", "/a/b/c"));
        REQUIRE(result == "c");
    }

    SECTION("Direct subpath")
    {
        std::string result{};
        REQUIRE(FilePath::Relative(result, "/a/b/", "/a/b/c/d.txt"));
        REQUIRE(result == "c/d.txt");
    }

    SECTION("Different branches")
    {
        std::string result{};
        REQUIRE(FilePath::Relative(result, "/a/b/c/", "/a/d/e.txt"));
        REQUIRE(result == "../../d/e.txt");
    }

    SECTION("No common path")
    {
        std::string result{};
        REQUIRE(FilePath::Relative(result, "/a/b/c/", "/d/e/f.txt"));
        REQUIRE(result == "../../../d/e/f.txt");
    }

    SECTION("From relative to absolute path")
    {
        std::string result{};
        REQUIRE(FilePath::Relative(result, "a/b/c/", "/a/b/c/d/e.txt"));
        REQUIRE(result == "/a/b/c/d/e.txt");
    }

    SECTION("To relative path")
    {
        std::string result{};
        REQUIRE(FilePath::Relative(result, "/a/b/c/d/", "e/f.txt"));
        REQUIRE(result == "e/f.txt");
    }

    SECTION("Both relative paths")
    {
        std::string result{};
        REQUIRE(FilePath::Relative(result, "a/b/c/", "a/d/e.txt"));
        REQUIRE(result == "../../d/e.txt");
    }
}
