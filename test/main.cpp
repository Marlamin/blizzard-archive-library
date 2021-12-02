#include <BlizzardArchive.h>

int main(int argc, char* argv[])
{
    auto archive = BlizzardArchive();

    archive.CheckVersions();

    return 0;
}