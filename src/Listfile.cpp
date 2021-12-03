#include <Listfile.hpp>

using namespace BlizzardArchive::Listfile;

FileKey::FileKey(std::string const& filepath, std::uint32_t file_data_id)
: _file_data_id(file_data_id)
, _file_path(filepath)
{}


FileKey::FileKey(std::string const& filepath)
  : _file_path(filepath)
{}

FileKey::FileKey(std::uint32_t file_data_id)
  : _file_data_id(file_data_id)
{}