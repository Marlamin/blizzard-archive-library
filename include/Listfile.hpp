#ifndef BIZZZARDARCHIVE_LISTFILE_HPP
#define BIZZZARDARCHIVE_LISTFILE_HPP
#include <string>
#include <cstdint>
#include <optional>

namespace BlizzardArchive::Listfile
{
  class Listfile
  {
  public:
    Listfile(std::string const& filepath);
    ~Listfile() = default;
  };

  class FileKey
  {
  public:
    FileKey(std::string const& filepath);
    FileKey(std::uint32_t file_data_id);
    FileKey(std::string const& filepath, std::uint32_t file_data_id);

    bool hasFilepath() { return _file_path.has_value(); };
    bool hasFileDataID() { return static_cast<bool>(_file_data_id); };

  private:
    std::uint32_t _file_data_id = 0;
    std::optional<std::string> _file_path;
  };
}

#endif // BIZZZARDARCHIVE_LISTFILE_HPP