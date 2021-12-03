#ifndef BLIZZARD_ARCHIVE_CLIENT_DATA_HPP
#define BLIZZARD_ARCHIVE_CLIENT_DATA_HPP


#include <array>
#include <vector>
#include <string>
#include <string_view>
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
    * version - version of the game client. Currently only WotLK and Shadowlands are supported.
    * locale - prefered locale of the client. Wotlk supports automatic detection, for that use Locale::AUTO
    * local_path - project directory
    */
    explicit ClientData(std::string const& path, ClientVersion version, Locale locale, std::string const& local_path);

    [[nodiscard]]
    ClientVersion version() const { return _version; }

    [[nodiscard]]
    StorageType storageType() const { return _storage_type; }

    [[nodiscard]]
    OpenMode openMode() const { return _open_mode; }

    [[nodiscard]]
    std::string const& path() const { return _path; }

    [[nodiscard]]
    std::string getDiskPath(Listfile::FileKey const& file_key);

    /* Methods used to universally request client file data in an archive type agnostic way. */

    [[nodiscard]]
    bool readFile(Listfile::FileKey const& file_key, std::vector<char>& buffer);

    [[nodiscard]]
    bool exists(Listfile::FileKey const& file_key);

    [[nodiscard]]
    bool existsOnDisk(Listfile::FileKey const& file_key);

    /* Static helper methods */

    [[nodiscard]]
    static std::string normalizeFilenameUnix(std::string filename);

    [[nodiscard]]
    static std::string normalizeFilenameWoW(std::string filename);


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
    std::string _local_path;

    // A sorted list of loaded archives. The last one is the most up-to-date one.
    std::vector<Archive::BaseArchive*> _archives;
    Listfile::Listfile _listfile;


  };
}

#endif // BLIZZARD_ARCHIVE_CLIENT_DATA_HPP