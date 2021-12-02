#include <BlizzardArchive.h>

int main(int argc, char* argv[])
{
    auto archive = BlizzardArchive();

    auto directoryPath = std::string("D:\World Of Wacraft\World of Warcraft Classic\Data\patch.mpq");
    archive.OpenArchive(ArchiveVersion::MPQ, directoryPath);

    return 0;
}