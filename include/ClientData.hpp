#ifndef BLIZZARD_ARCHIVE_CLIENT_DATA_HPP
#define BLIZZARD_ARCHIVE_CLIENT_DATA_HPP


#include <array>
#include <vector>
#include <string>
#include <string_view>
#include <optional>
#include <Listfile.hpp>

typedef void* HANDLE;

namespace BlizzardArchive
{

  namespace Archive
  {
    class BaseArchive;
  }


  enum class ClientVersion
  {
    WOTLK,
    SL,
  };

  enum class StorageType
  {
    MPQ,
    CASC
  };

  enum class OpenMode
  {
    LOCAL,
    REMOTE
  };

  enum class Locale
  {
    AUTO, 
    enGB,
    enUS,
    deDE,
    koKR,
    frFR,
    zhCN,
    zhTW,
    esES,
    esMX,
    ruRU
  };

  class ClientData
  {
  public:
    /*
    * path - path to game directory for MPQ-based clients, path to storage directory (the one containing .build.info) for CASC-based clients.
    */
    explicit ClientData(std::string const& path, ClientVersion version, Locale locale);

    ClientVersion version() const { return _version; }
    StorageType storageType() const { return _storage_type; }
    OpenMode openMode() const { return _open_mode; }
    std::string const& path() const { return _path; }

  public:
    inline static constexpr std::array<std::string_view, 10> Locales { "enGB", "enUS", "deDE", "koKR", "frFR", "zhCN", "zhTW", "esES", "esMX", "ruRU" };

    // Templates in correct order for opening the wotlk client MPQs
    inline static constexpr std::array<std::string_view, 14> ArchiveNameTemplates { 
                                                                                    // common archives
                                                                                      "common.MPQ"
                                                                                    , "common-2.MPQ"
                                                                                    , "expansion.MPQ"
                                                                                    , "lichking.MPQ"
                                                                                    , "patch.MPQ"
                                                                                    , "patch-{number}.MPQ"
                                                                                    , "patch-{character}.MPQ"

                                                                                    // locale-specific archives
                                                                                    , "{locale}/locale-{locale}.MPQ"
                                                                                    , "{locale}/expansion-locale-{locale}.MPQ"
                                                                                    , "{locale}/lichking-locale-{locale}.MPQ"
                                                                                    , "{locale}/patch-{locale}.MPQ"
                                                                                    , "{locale}/patch-{locale}-{number}.MPQ" 
                                                                                    , "{locale}/patch-{locale}-{character}.MPQ"
                                                                                    , "development.MPQ"
                                                                                  };

  private:

    void initializeMPQStorage();
    void initializeCASCStorage();
    void validateLocale();

    OpenMode _open_mode;
    StorageType _storage_type;
    ClientVersion _version;
    Locale _locale_mode;
    std::string _path;

    // A sorted list of loaded archives. The last one is the most up-to-date one.
    std::vector<Archive::BaseArchive*> _archives;
    std::optional<Listfile::Listfile> _listfile;


  };
}

#endif // BLIZZARD_ARCHIVE_CLIENT_DATA_HPP