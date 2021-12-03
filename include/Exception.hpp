#ifndef BLIZZARD_ARCHIVE_EXCEPTION_HPP
#define BLIZZARD_ARCHIVE_EXCEPTION_HPP

#include <stdexcept>

namespace BlizzardArchive::Exceptions
{
  namespace Locale
  {
    class LocaleNotFoundException : public std::runtime_error
    {
    public:
      LocaleNotFoundException(const std::string& what = "") : std::runtime_error(what) {}
    };

    class IncorrectLocaleModeException : public std::runtime_error
    {
    public:
      IncorrectLocaleModeException(const std::string& what = "") : std::runtime_error(what) {}
    };
  }

  namespace Archive
  {
    class ArchiveOpenError : public std::runtime_error
    {
    public:
      ArchiveOpenError(const std::string& what = "") : std::runtime_error(what) {}
    };
  }

}

#endif