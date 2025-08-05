#ifndef BLIZZARDARCHIVE_CASCARCHIVE_HPP
#define BLIZZARDARCHIVE_CASCARCHIVE_HPP

#include <BaseArchive.hpp>

namespace BlizzardArchive::Listfile
{
  class Listfile;
}

namespace BlizzardArchive::Archive
{

  class CASCArchive : public BaseArchive
  {
  public:
    CASCArchive(std::string const& path, std::string const& cache_path, Locale locale, OpenMode open_mode, Listfile::Listfile* listfile);
    ~CASCArchive() override;

    [[nodiscard]]
    bool openFile(Listfile::FileKey const& file_key, Locale locale, HANDLE* file_handle) const override;

    bool readFile(HANDLE file_handle, char* buffer, std::size_t buf_size) const override;
    bool closeFile(HANDLE file_handle) const override;

    [[nodiscard]]
    std::uint64_t getFileSize(HANDLE file_handle) const override;

    [[nodiscard]]
    bool exists(Listfile::FileKey const& file_key, Locale locale) const override;

  private:
    HANDLE _handle = nullptr;
    typedef void (*SetConfigsFunc)(const char* buildConfig, const char* cdnConfig);
    typedef void (*SetBaseDirFunc)(const char* wowFolder);
    typedef void (*LoadFunc)();
    typedef const char* (*GetBuildStringFunc)();
    typedef const char* (*GetFileByIDFunc)(const uint32_t fileDataID);
    typedef const bool (*FileExistsByIDFunc)(const uint32_t fileDataID);
    typedef const uint64_t(*GetFileSizeByIDFunc)(const uint32_t fileDataID);

    SetBaseDirFunc setBaseDir;
    SetConfigsFunc setConfigs;
    LoadFunc load;
    GetBuildStringFunc getBuildString;
    GetFileByIDFunc getFileByID;
    GetFileSizeByIDFunc getFileSizeByID;
    FileExistsByIDFunc fileExistsByID;
  };

}

#endif //BLIZZARDARCHIVE_CASCARCHIVE_HPP