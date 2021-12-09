#include <ClientData.hpp>
#include <ClientFile.hpp>

int main(int argc, char* argv[])
{
  auto proj_path = std::string("C:\\Users\\Skarn\\Desktop\\test_patch");

  // MPQ storage tests
  {  
    auto directory_path = std::string("E:\\WoWModding\\World of Warcraft 3.3.5a");
    auto wow_fs = BlizzardArchive::ClientData(directory_path, BlizzardArchive::ClientVersion::WOTLK, BlizzardArchive::Locale::AUTO, proj_path);

    auto file = BlizzardArchive::ClientFile(BlizzardArchive::Listfile::FileKey("world/wmo/azeroth/buildings/human_farm/farm.wmo"), &wow_fs);
    file.save();
  }

  // Local CASC storage tests
  {
    //auto directory_path = std::string("D:\\World of Warcraft");
    auto directory_path = std::string("C:\\Users\\Skarn\\Desktop\\test_patch");

    auto wow_fs = BlizzardArchive::ClientData(directory_path, BlizzardArchive::ClientVersion::SL, BlizzardArchive::Locale::enUS, proj_path);

    auto file = BlizzardArchive::ClientFile(BlizzardArchive::Listfile::FileKey("world/wmo/azeroth/buildings/human_farm/farm.wmo"), &wow_fs);
    file.save();

    auto file1 = BlizzardArchive::ClientFile(BlizzardArchive::Listfile::FileKey(130497), &wow_fs);
    file1.save();
  }



  return 0;
}