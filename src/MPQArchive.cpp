#include <MPQArchive.hpp>
#include <Exception.hpp>
#include <StormLib.h>


using namespace BlizzardArchive::Archive;

MPQArchive::MPQArchive(std::string const& path, Locale locale)
: BaseArchive(path, locale)
{
  if (!SFileOpenArchive(path.c_str(), 0, MPQ_OPEN_NO_LISTFILE | STREAM_FLAG_READ_ONLY, &_handle))
  {
    throw Exceptions::Archive::ArchiveOpenError("Error opening archive: " + path);
  }
}

bool MPQArchive::readFile(char* buffer, std::size_t buf_size)
{

}

bool MPQArchive::exists(Listfile::FileKey const& filekey)
{

}