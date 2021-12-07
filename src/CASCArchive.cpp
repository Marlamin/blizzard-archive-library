#include <CASCArchive.hpp>

#include <Exception.hpp>
#include <CascLib.h>


using namespace BlizzardArchive::Archive;

CASCArchive::CASCArchive(std::string const& path, Locale locale, Listfile::Listfile* listfile)
  : BaseArchive(path, locale, listfile)
{
  CASC_OPEN_STORAGE_ARGS args;
  args.Size = sizeof(CASC_OPEN_STORAGE_ARGS);
  args.szLocalPath = path.c_str();
  args.szCodeName = "wow";
  args.szRegion = nullptr;
  args.PfnProgressCallback = nullptr;
  args.PtrProgressParam = nullptr;
  args.PfnProductCallback = nullptr;
  args.PtrProductParam = nullptr;
  args.dwLocaleMask = 0;  // TODO: pass locale


  if (!CascOpenStorageEx(nullptr, &args, false, &_handle))
  {
    throw Exceptions::Archive::ArchiveOpenError("Error opening CASC archive: " + path);
  }
}

bool CASCArchive::openFile(Listfile::FileKey const& file_key, Locale locale, HANDLE* file_handle) const
{
  std::uint32_t file_data_id;

  assert(file_key.hasFileDataID() || file_key.hasFilepath());

  if (file_key.hasFileDataID())
  {
    assert(file_key.fileDataID());

    file_data_id = file_key.fileDataID();
  }
  else
  {
    file_data_id = _listfile->getFileDataID(file_key.filepath());
  }


  return CascOpenFile(_handle, &file_data_id, 0, 3, file_handle);
}

bool CASCArchive::readFile(HANDLE file_handle, char* buffer, std::size_t buf_size) const
{
  assert(file_handle);
  return CascReadFile(file_handle, buffer, buf_size, nullptr);
}

bool CASCArchive::closeFile(HANDLE file_handle) const
{
  assert(file_handle);
  return CascCloseFile(file_handle);
}

std::uint64_t CASCArchive::getFileSize(HANDLE file_handle) const
{
  assert(file_handle);
  std::uint64_t size;

  CascGetFileSize64(file_handle, &size);

  return size;
}

bool CASCArchive::exists(Listfile::FileKey const& file_key, Locale locale) const
{
  HANDLE* file_handle = nullptr;
  std::uint32_t file_data_id;

  assert(file_key.hasFileDataID() || file_key.hasFilepath());

  if (file_key.hasFileDataID())
  {
    assert(file_key.fileDataID());

    file_data_id = file_key.fileDataID();
  }
  else
  {
    file_data_id = _listfile->getFileDataID(file_key.filepath());
  }


  bool status = CascOpenFile(_handle, &file_data_id, 0, 3, file_handle);
  CascCloseFile(file_handle);
  return status;

}

CASCArchive::~CASCArchive()
{
  if (_handle)
    CascCloseStorage(_handle);
}