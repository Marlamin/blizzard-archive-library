#include <CASCArchive.hpp>

#include <Exception.hpp>
#include <CascLib.h>


using namespace BlizzardArchive::Archive;

CASCArchive::CASCArchive(std::string const& path, Locale locale, Listfile::Listfile* listfile)
  : BaseArchive(path, locale, listfile)
{
  if (!SFileOpenArchive(path.c_str(), 0, MPQ_OPEN_NO_LISTFILE | STREAM_FLAG_READ_ONLY, &_handle))
  {
    throw Exceptions::Archive::ArchiveOpenError("Error opening archive: " + path);
  }
}

bool CASCArchive::openFile(Listfile::FileKey const& file_key, Locale locale, HANDLE* file_handle) const
{
  assert(file_key.hasFilepath());
  return SFileOpenFileEx(_handle, file_key.filepath().c_str(), 0, file_handle);
}

bool CASCArchive::readFile(HANDLE file_handle, char* buffer, std::size_t buf_size) const
{
  assert(file_handle);
  return SFileReadFile(file_handle, buffer, buf_size, nullptr, nullptr);
}

bool CASCArchive::closeFile(HANDLE file_handle) const
{
  assert(file_handle);
  return SFileCloseFile(file_handle);
}

std::uint64_t CASCArchive::getFileSize(HANDLE file_handle) const
{
  assert(file_handle);
  return SFileGetFileSize(file_handle, nullptr);
}

bool CASCArchive::exists(Listfile::FileKey const& file_key, Locale locale) const
{
  assert(file_key.hasFilepath());
  return SFileHasFile(_handle, file_key.filepath().c_str());
}

CASCArchive::~CASCArchive()
{
  if (_handle)
    SFileCloseArchive(_handle);
}