#include <ClientData.hpp>
#include <Exception.hpp>
#include <MPQArchive.hpp>
#include <CascArchive.hpp>

#include <filesystem>

using namespace BlizzardArchive;
namespace fs = std::filesystem;

ClientData::ClientData(std::string const& path, ClientVersion version, Locale locale, std::string const& local_path)
  : _version(version)
  , _open_mode(OpenMode::LOCAL)
  , _storage_type(version > ClientVersion::WOTLK ? StorageType::CASC : StorageType::MPQ)
  , _locale_mode(locale)
  , _path(path)
  , _local_path(ClientData::normalizeFilenameUnix(local_path))
{
  validateLocale();

  switch (_storage_type)
  {
  case StorageType::MPQ:
    initializeMPQStorage();
    break;
  case StorageType::CASC:
    initializeCASCStorage();
    break;
  default:
    break;
  }
}

void ClientData::initializeMPQStorage()
{
  for (auto const& filename : ClientData::ArchiveNameTemplates)
  {
    std::string mpq_path = (fs::path(_path) / "Data" / filename).string();

    std::string::size_type location(std::string::npos);
    std::string_view const& locale = ClientData::Locales[static_cast<int>(_locale_mode) - 1];

    do
    {
      location = mpq_path.find("{locale}");
      if (location != std::string::npos)
      {
        mpq_path.replace(location, 8, locale);
      }
    } while (location != std::string::npos);

    if (mpq_path.find("{number}") != std::string::npos)
    {
      location = mpq_path.find("{number}");
      mpq_path.replace(location, 8, " ");
      for (char j = '2'; j <= '9'; j++)
      {
        mpq_path.replace(location, 1, std::string(&j, 1));
        if (fs::exists(mpq_path))
        {
          _archives.push_back(new Archive::MPQArchive(mpq_path, _locale_mode, &_listfile));
        }
         
      }
    }
    else if (mpq_path.find("{character}") != std::string::npos)
    {
      location = mpq_path.find("{character}");
      mpq_path.replace(location, 11, " ");
      for (char c = 'a'; c <= 'z'; c++)
      {
        mpq_path.replace(location, 1, std::string(&c, 1));
        if (fs::exists(mpq_path))
        {
          _archives.push_back(new Archive::MPQArchive(mpq_path, _locale_mode, &_listfile));
        }
      }
    }
    else
    {
      if (fs::exists(mpq_path))
      {
        _archives.push_back(new Archive::MPQArchive(mpq_path, _locale_mode, &_listfile));
      }
    }
  }

 
}

void ClientData::initializeCASCStorage()
{
  _archives.push_back(new Archive::CASCArchive(_path, _locale_mode, &_listfile));
}

void ClientData::validateLocale()
{
  if (_storage_type == StorageType::MPQ)
  {
    if (static_cast<int>(_locale_mode)) // manual locale
    [[unlikely]]
    {
      fs::path realmlist_path = fs::path(_path) / "Data" / ClientData::Locales[static_cast<int>(_locale_mode) - 1] / "realmlist.wtf";

      if (!fs::exists(realmlist_path))
      {
        throw Exceptions::Locale::LocaleNotFoundError("Requested locale \"" + std::string(ClientData::Locales[static_cast<int>(_locale_mode) - 1].data()) + "\" does not exist in the client directory."
          "Be sure, that there is one containing the file \"realmlist.wtf\".");
      }

    }
    else // auto locale
    [[likely]]
    {
      for (unsigned i = 0; i < ClientData::Locales.size(); ++i)
      {
        fs::path realmlist_path = fs::path(_path) / "Data" / ClientData::Locales[i] / "realmlist.wtf";

        if (fs::exists(realmlist_path))
        {
          _locale_mode = static_cast<Locale>(i + 1);
          return;
        }
      }

      throw Exceptions::Locale::LocaleNotFoundError("Automatic locale detection failed. The client directory does not contain any locale directory. Be sure, that there is one containing the file \"realmlist.wtf\".");
    }
  }
  else if (_storage_type == StorageType::CASC)
  {
    if (_locale_mode == Locale::AUTO)
    {
      throw Exceptions::Locale::IncorrectLocaleModeError("Automatic locale detection is not supported for CASC-based clients.");
    }
  }
  
  
}

bool ClientData::readFile(Listfile::FileKey const& file_key, std::vector<char>& buffer)
{
  HANDLE handle = nullptr;

  for (auto it = _archives.rbegin(); it != _archives.rend(); ++it)
  {
    if (!(*it)->openFile(file_key, _locale_mode, &handle))
      continue;

    std::uint64_t buf_size = (*it)->getFileSize(handle);
    buffer.resize(buf_size);

    (*it)->readFile(handle, buffer.data(), buf_size);
    (*it)->closeFile(handle);

    return true;
  }

  return false;
}

bool ClientData::existsOnDisk(Listfile::FileKey const& file_key)
{
  if (!file_key.hasFilepath())
    return false;

  return fs::exists(file_key.filepath());
}

bool ClientData::exists(Listfile::FileKey const& file_key)
{
  for (auto it = _archives.rbegin(); it != _archives.rend(); ++it)
  {
    if ((*it)->exists(file_key, _locale_mode))
      return true;
  }

  return false;
}

std::string ClientData::getDiskPath(Listfile::FileKey const& file_key)
{
  if (file_key.hasFilepath())
  {
    return (fs::path(_local_path) / ClientData::normalizeFilenameUnix(file_key.filepath())).string();
  }
  else
  {
    return (fs::path(_local_path) / "unknown_files/" / std::to_string(file_key.fileDataID())).string();
  }
   
}

std::string ClientData::normalizeFilenameUnix(std::string filename)
{
  //std::transform(filename.begin(), filename.end(), filename.begin(), ::tolower);
  std::transform(filename.begin(), filename.end(), filename.begin()
    , [](char c)
    {
      return c == '\\' ? '/' : c;
    }
  );
  return filename;
}

std::string ClientData::normalizeFilenameWoW(std::string filename)
{
  std::transform(filename.begin(), filename.end(), filename.begin(), ::toupper);
  std::transform(filename.begin(), filename.end(), filename.begin()
    , [](char c)
    {
      return c == '/' ? '\\' : c;
    }
  );
  return filename;
}