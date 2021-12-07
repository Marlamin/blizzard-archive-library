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
    CASCArchive(std::string const& path, Locale locale, Listfile::Listfile* listfile);
    ~CASCArchive();

    [[nodiscard]]
    virtual bool openFile(Listfile::FileKey const& file_key, Locale locale, HANDLE* file_handle) const override;

    virtual bool readFile(HANDLE file_handle, char* buffer, std::size_t buf_size) const override;
    virtual bool closeFile(HANDLE file_handle) const override;

    [[nodiscard]]
    virtual std::uint64_t getFileSize(HANDLE file_handle) const override;

    [[nodiscard]]
    virtual bool exists(Listfile::FileKey const& file_key, Locale locale) const override;

  private:
    HANDLE _handle = nullptr;
  };

}

#endif //BLIZZARDARCHIVE_CASCARCHIVE_HPP