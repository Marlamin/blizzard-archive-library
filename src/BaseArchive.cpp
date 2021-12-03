#include <BaseArchive.hpp>

using namespace BlizzardArchive::Archive;

BaseArchive::BaseArchive(std::string const& path, Locale locale)
  : _locale(locale)
  , _path(path)
{

}