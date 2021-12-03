#include <ClientData.hpp>


namespace BlizzardArchive::Archive
{
  namespace Listfile
  {
    class FileKey;
  }

  class BaseArchive
  {
  public:
    BaseArchive(std::string const& path, Locale locale);
    std::string const& path() const { return _path; };

    virtual bool readFile(char* buffer, std::size_t buf_size) = 0;
    virtual bool exists(Listfile::FileKey const& filekey) = 0;

  private:
    std::string _path;
    Locale _locale;
  };
}

