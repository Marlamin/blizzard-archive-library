#include <iostream>
#include <StormLib.h>

enum class ArchiveVersion : char
{
	MPQ,
	CASC
};

class BlizzardArchive
{
public:
	BlizzardArchive() = default;

	void OpenArchive(ArchiveVersion version, std::string const& path);
};