#include <ClientData.hpp>
#include <ClientFile.hpp>

int main(int argc, char* argv[])
{
  auto directory_path = std::string("E:\\WoWModding\\World of Warcraft 3.3.5a");
  auto proj_path = std::string("C:\\Users\\Skarn\\Desktop\\test_patch");
  auto wow_fs = BlizzardArchive::ClientData(directory_path, BlizzardArchive::ClientVersion::WOTLK, BlizzardArchive::Locale::AUTO, proj_path);

  auto file = BlizzardArchive::ClientFile(BlizzardArchive::Listfile::FileKey("world/wmo/azeroth/human/buildings/human_farm/farm.wmo"), &wow_fs);
  file.save();

  return 0;
}