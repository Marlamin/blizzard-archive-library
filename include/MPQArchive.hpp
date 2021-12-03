#include <BaseArchive.hpp>

namespace BlizzardArchive::Archive
{
  class MPQArchive : public BaseArchive
  {
  public:
    MPQArchive(std::string const& path, Locale locale);

    virtual bool readFile(char* buffer, std::size_t buf_size) override;
    virtual bool exists(Listfile::FileKey const& filekey) override;

  private:
    HANDLE _handle = nullptr;
  };
}