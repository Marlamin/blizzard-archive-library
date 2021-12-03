#include <Listfile.hpp>
#include <Exception.hpp>
#include <ClientData.hpp>
#include <fstream>
#include <sstream>
#include <cstdint>

using namespace BlizzardArchive::Listfile;

FileKey::FileKey(std::string const& filepath, std::uint32_t file_data_id)
: _file_data_id(file_data_id)
, _file_path(ClientData::normalizeFilenameWoW(filepath))
{}

FileKey::FileKey(std::string const& filepath)
  : _file_path(ClientData::normalizeFilenameWoW(filepath))
{}

FileKey::FileKey(std::uint32_t file_data_id)
  : _file_data_id(file_data_id)
{}

Listfile::Listfile()
{

}

void Listfile::initFromCSV(std::string const& listfile_path)
{
  std::ifstream fstream;
  fstream.open(listfile_path);

  if (!fstream.is_open())
  {
    throw Exceptions::Listfile::ListfileNotFoundError();
  }
  else
  {
    std::string line = "";
    while (std::getline(fstream, line))
    {
      std::string uid_str;
      std::string filename;
      std::uint32_t uid;

      std::stringstream ss(line);

      getline(ss, uid_str, ';');
      getline(ss, filename);

      uid = std::atoi(uid_str.c_str());

      _path_to_fdid[ClientData::normalizeFilenameWoW(filename)] = uid;
      _fdid_to_path[uid] = ClientData::normalizeFilenameWoW(filename);

    }

  }
}

void Listfile::initFromFileList(std::vector<char> const& file_list_blob)
{
  // TODO: feels very sketchy, copied it from original Noggit.
  // check if approach from initFromCSV() works any better (less reallocs maybe).

  std::string current;
  for (char c : file_list_blob)
  {
    if (c == '\r')
    {
      continue;
    }
    if (c == '\n')
    {
      _path_to_fdid[ClientData::normalizeFilenameWoW(current)] = 0;
      current.resize(0);
    }
    else
    {
      current += c;
    }
  }

  if (!current.empty())
  {
    _path_to_fdid[ClientData::normalizeFilenameWoW(current)] = 0;
  }
}

std::uint32_t Listfile::getFileDataID(std::string const& filename)
{
  auto it = _path_to_fdid.find(filename);

  if (it != _path_to_fdid.end())
  {
    return it->second;
  }
  else
  {
    return 0; // Not found
  }

}
std::string Listfile::getPath(std::uint32_t file_data_id)
{
  auto it = _fdid_to_path.find(file_data_id);

  if (it != _fdid_to_path.end())
  {
    return it->second;
  }
  else
  {
    return ""; // Not found
  }

}