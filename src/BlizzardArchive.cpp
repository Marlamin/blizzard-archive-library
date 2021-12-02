#include <BlizzardArchive.h>
#include <string>

void BlizzardArchive::OpenArchive(ArchiveVersion version, std::string path)
{
	std::cout << "Selected Path : "<< path << std::endl;


	if (version == ArchiveVersion::MPQ)
	{
		std::cout << "MPQ Version" << std::endl;

		HANDLE mpqArchive;
	    if(!SFileOpenArchive(path.c_str(), 0, MPQ_OPEN_NO_LISTFILE | STREAM_FLAG_READ_ONLY, &mpqArchive))
	    {
			std::cout << "FAILED TO OPEN!" << std::endl;
	    }
		else
		{
			std::cout << "IT IS OPEN!" << std::endl;
		}

		return;
	}

	if(version == ArchiveVersion::CASC)
	{
		std::cout << "Casc Version" << std::endl;
		return;
	}
}
