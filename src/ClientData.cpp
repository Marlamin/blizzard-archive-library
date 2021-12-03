#include <ClientData.hpp>
#include <Exception.hpp>
#include <MPQArchive.hpp>

#include <filesystem>

using namespace BlizzardArchive;
namespace fs = std::filesystem;

ClientData::ClientData(std::string const& path, ClientVersion version, Locale locale)
  : _version(version)
  , _open_mode(OpenMode::LOCAL)
  , _storage_type(version > ClientVersion::WOTLK ? StorageType::CASC : StorageType::MPQ)
  , _locale_mode(locale)
  , _path(path)
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
    std::string_view const& locale = ClientData::Locales[static_cast<int>(_locale_mode)];

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
          _archives.push_back(new Archive::MPQArchive(mpq_path, _locale_mode));
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
          _archives.push_back(new Archive::MPQArchive(mpq_path, _locale_mode));
        }
      }
    }
    else
    {
      if (fs::exists(mpq_path))
      {
        _archives.push_back(new Archive::MPQArchive(mpq_path, _locale_mode));
      }
    }
  }

 
}

void ClientData::initializeCASCStorage()
{

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
        throw Exceptions::Locale::LocaleNotFoundException("Requested locale \"" + std::string(ClientData::Locales[static_cast<int>(_locale_mode) - 1].data()) + "\" does not exist in the client directory."
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

      throw Exceptions::Locale::LocaleNotFoundException("Automatic locale detection failed. The client directory does not contain any locale directory. Be sure, that there is one containing the file \"realmlist.wtf\".");
    }
  }
  else if (_storage_type == StorageType::CASC)
  {
    if (_locale_mode == Locale::AUTO)
    {
      throw Exceptions::Locale::IncorrectLocaleModeException("Automatic locale detection is not supported for CASC-based clients.");
    }
  }
  
  
}
