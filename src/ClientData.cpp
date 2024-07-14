#include <ClientData.hpp>
#include <Exception.hpp>
#include <MPQArchive.hpp>
#include <DirectoryArchive.hpp>
#include <CASCArchive.hpp>

#include <StormLib.h>

#include <filesystem>
#include <cassert>
#include <regex>

using namespace BlizzardArchive;
namespace fs = std::filesystem;

ClientData::ClientData(std::string const& path, ClientVersion version, Locale locale, std::string const& local_path)
  : _version(version)
  , _open_mode(OpenMode::LOCAL)
  , _storage_type((version > ClientVersion::WOTLK) ? StorageType::CASC : StorageType::MPQ)
  , _locale_mode(locale)
  , _path(path)
  , _local_path(ClientData::normalizeFilenameUnix(local_path))
{

  validateLocale();

  switch (_storage_type)
  {
  case StorageType::MPQ:
    initializeMPQStorage();
    break;
  case StorageType::CASC:
    initializeCASCStorage();
    break;
  }
}

ClientData::ClientData(std::string const& path, std::string const& cdn_cache_path, ClientVersion version, Locale locale, std::string const& local_path)
    : _version(version)
    , _open_mode(OpenMode::REMOTE)
    , _storage_type((version > ClientVersion::WOTLK) ? StorageType::CASC : StorageType::MPQ)
    , _locale_mode(locale)
    , _path(path)
    , _local_path(ClientData::normalizeFilenameUnix(local_path))
    , _cdn_cache_path(cdn_cache_path)
{

  validateLocale();

  switch (_storage_type)
  {
    case StorageType::CASC:
      initializeCASCStorage();
      break;
    case StorageType::MPQ:
      throw Exceptions::Archive::ArchiveOpenError("MPQ storage does not support online loading.");
      break;
  }
}

ClientData::~ClientData()
{
  // clean up
  for (auto archive : _archives)
  {
    delete archive;
  }
}

bool BlizzardArchive::ClientData::mpqArchiveExistsOnDisk(std::string const& archive_name)
{
    std::string mpq_path = (fs::path(_path) / "Data" / archive_name).string();

    std::string::size_type location(std::string::npos);
    std::string_view const& locale = ClientData::Locales[static_cast<int>(_locale_mode) - 1];

    if (!fs::exists(mpq_path))
        return false;

    if (fs::is_directory(mpq_path))
    {
        return true; // directory patch. special return?
    }
    else
    {
        return true;
    }
}

// archive_name = patch-4.MPQ, must include extension
std::optional<Archive::MPQArchive*> BlizzardArchive::ClientData::getMPQArchive(std::string const& archive_name)
{
    if (_storage_type != StorageType::MPQ)
        return std::nullopt;

    const std::lock_guard _lock(_mutex);

    // case sensitive
    std::string mpq_path = (fs::path(_path) / "Data" / archive_name).string();

    for (auto* archive_ptr : _archives)
    {
        // full disk paths.
        if (archive_ptr->path() == mpq_path)
        {
            if (Archive::MPQArchive* mpqArchive_ptr = dynamic_cast<Archive::MPQArchive*>(archive_ptr))
            {
                return mpqArchive_ptr;
            }
        }
    }

    // if not in memory, try to get it on disk
    if (mpqArchiveExistsOnDisk(archive_name))
    {
        Archive::MPQArchive* new_archive = new Archive::MPQArchive(mpq_path, _locale_mode, &_listfile);
        if (new_archive)
        {
            _archives.push_back(new_archive);
            return new_archive;
        }
    }

    return std::nullopt;
}

std::optional<Archive::MPQArchive*> BlizzardArchive::ClientData::tryCreateMPQArchive(std::string const& archive_name)
{
    if (_storage_type != StorageType::MPQ)
        return std::nullopt;

    // check if there is already a file
    std::string mpq_path = (fs::path(_path) / "Data" / archive_name).string();

    std::string::size_type location(std::string::npos);
    std::string_view const& locale = ClientData::Locales[static_cast<int>(_locale_mode) - 1];

    if (!fs::exists(mpq_path))
    {
        // create archive here or in MPQ class ?
        HANDLE hMpq = NULL;
        unsigned long dwMaxFileCount = 0x2000;// 0x1000 seems to be the default // 0x4 is the minimum

        unsigned long dwCreateFlags = MPQ_CREATE_LISTFILE | MPQ_CREATE_ATTRIBUTES | MPQ_CREATE_ARCHIVE_V2; // v1 ?

        if (SFileCreateArchive(mpq_path.c_str(), dwCreateFlags, dwMaxFileCount, &hMpq))
        {
            if (!SFileCloseArchive(hMpq))
            {
                DWORD error = GetLastError();
                throw Exceptions::Archive::ArchiveCloseError("ClientData::tryCreateMPQArchive(): Error closing archive: " + mpq_path);
            }

            // now we can initialize the class and open it in read-only mode
            Archive::MPQArchive* new_archive = new Archive::MPQArchive(mpq_path, _locale_mode, &_listfile);
            if (new_archive)
            {
                _archives.push_back(new_archive);
                return new_archive;
            }
        }
        else
        {
            // failed to create archive
            DWORD error = GetLastError();
            throw Exceptions::Archive::ArchiveOpenError("ClientData::tryCreateMPQArchive(): Error creating archive: " + mpq_path, error);
        }
    }
    return std::nullopt;
}

bool BlizzardArchive::ClientData::isMPQNameValid(std::string const& archive_name, bool exclude_base_mpqs)
{
    // Make sure archive_name is lowercase!

    // if MPQ isn't in the list of allowed MPQs, return false
    // if exclude_base_mpqs, return false if the MPQ is present in the base 3.3.5 client. (eg patch-3 would be wrong, patch-4 would be allowed)

    int const clientPatchId = 3; // 3.[3].5

    for (auto const& filename : ClientData::ArchiveNameTemplates)
    {
        // auto lower_filename = "";
        // for (char ch : filename) {
        //     // Convert each character to lowercase
        //     lower_filename += std::tolower(ch);
        // }

        std::string mpq_path = (fs::path(_path) / "Data" / filename).string();

        std::string::size_type location(std::string::npos);
        std::string_view const& locale = ClientData::Locales[static_cast<int>(_locale_mode) - 1];

        do
        {
            location = mpq_path.find("{locale}");
            if (location != std::string::npos)
            {
                mpq_path.replace(location, 8, locale);
            }
        } while (location != std::string::npos);

        if (mpq_path.find("{number}") != std::string::npos)
        {
            location = mpq_path.find("{number}");
            mpq_path.replace(location, 8, " ");
            for (char j = '2'; j <= '9'; j++)
            {
                mpq_path.replace(location, 1, std::string(&j, 1));

                // hack to lowercase the extension
                auto mpq_filename = fs::path(mpq_path).filename().replace_extension(".mpq").string();
                if (archive_name == mpq_filename)
                {
                    // convert j from char to int
                    if (exclude_base_mpqs && (j - '0') <= 3)
                        return false;
                    else
                        return true;
                }
            }
        }
        else if (mpq_path.find("{character}") != std::string::npos)
        {
            location = mpq_path.find("{character}");
            mpq_path.replace(location, 11, " ");
            for (char c = 'a'; c <= 'z'; c++)
            {
                mpq_path.replace(location, 1, std::string(&c, 1));
                
                // all letter patches are valid.
                auto mpq_filename = fs::path(mpq_path).filename().replace_extension(".mpq").string();
                if (archive_name == mpq_filename)
                {
                    return true;
                }
            }
        }
        else
        {
            if (archive_name == fs::path(mpq_path).filename().replace_extension(".mpq").string())
                return exclude_base_mpqs ? false : true;
        }
    }
    // name was not in the list of allowed names.
    return false;
}

void ClientData::loadMPQArchive(std::string const& mpq_path)
{
  if (!fs::exists(mpq_path) || fs::equivalent(mpq_path, _local_path))
    return;

  if (fs::is_directory(mpq_path))
  {
    _archives.push_back(new Archive::DirectoryArchive(mpq_path, _locale_mode, &_listfile));
  }
  else
  {
    _archives.push_back(new Archive::MPQArchive(mpq_path, _locale_mode, &_listfile));
  }


}

void ClientData::initializeMPQStorage()
{
  for (auto const& filename : ClientData::ArchiveNameTemplates)
  {
    std::string mpq_path = (fs::path(_path) / "Data" / filename).string();

    std::string::size_type location(std::string::npos);
    std::string_view const& locale = ClientData::Locales[static_cast<int>(_locale_mode) - 1];

    do
    {
      location = mpq_path.find("{locale}");
      if (location != std::string::npos)
      {
        mpq_path.replace(location, 8, locale);
      }
    } while (location != std::string::npos);

    if (mpq_path.find("{number}") != std::string::npos)
    {
      location = mpq_path.find("{number}");
      mpq_path.replace(location, 8, " ");
      for (char j = '2'; j <= '9'; j++)
      {
        mpq_path.replace(location, 1, std::string(&j, 1));
        loadMPQArchive(mpq_path);
      }
    }
    else if (mpq_path.find("{character}") != std::string::npos)
    {
      location = mpq_path.find("{character}");
      mpq_path.replace(location, 11, " ");
      for (char c = 'a'; c <= 'z'; c++)
      {
        mpq_path.replace(location, 1, std::string(&c, 1));
        loadMPQArchive(mpq_path);
      }
    }
    else
    {
      loadMPQArchive(mpq_path);
    }
  }

 
}

void ClientData::initializeCASCStorage()
{
  _listfile.initFromCSV((fs::path(_local_path) / "listfile.csv").string());

  switch (_open_mode)
  {
    case OpenMode::LOCAL:
    {
      _archives.push_back(new Archive::CASCArchive(_path, "", _locale_mode, _open_mode, &_listfile));
      break;
    }
    case OpenMode::REMOTE:
    {
      assert(_cdn_cache_path.has_value());
      _archives.push_back(new Archive::CASCArchive(_path, _cdn_cache_path.value(), _locale_mode, _open_mode, &_listfile));
      break;
    }
  }

}

void ClientData::validateLocale()
{
  switch (_storage_type)
  {
    case StorageType::MPQ:
    {
      if (static_cast<int>(_locale_mode)) // manual locale
      [[unlikely]]
      {
        fs::path realmlist_path = fs::path(_path) / "Data"
            / ClientData::Locales[static_cast<int>(_locale_mode) - 1] / "realmlist.wtf";

        if (!fs::exists(realmlist_path))
        {
          throw Exceptions::Locale::LocaleNotFoundError("Requested locale \""
            + std::string(ClientData::Locales[static_cast<int>(_locale_mode) - 1].data()) +
            "\" does not exist in the client directory."
            "Be sure, that there is one containing the file \"realmlist.wtf\".");
        }

      }
      else // auto locale
      [[likely]]
      {
        for (unsigned i = 0; i < ClientData::Locales.size(); ++i)
        {
          fs::path realmlist_path = fs::path(_path) / "Data" / ClientData::Locales[i] / "realmlist.wtf";

          if (fs::exists(realmlist_path))
          {
            _locale_mode = static_cast<Locale>(i + 1);
            return;
          }
        }

        throw Exceptions::Locale::LocaleNotFoundError("Automatic locale detection failed. "
                                                      "The client directory does not contain any locale directory. Be "
                                                      "sure, that there is one containing the file \"realmlist.wtf\".");
      }
      break;
    }
    case StorageType::CASC:
    {
      if (_locale_mode == Locale::AUTO)
      {
        throw Exceptions::Locale::IncorrectLocaleModeError("Automatic locale detection is not"
                                                           " supported for CASC-based clients.");
      }
      break;
    }
  }
  
}

bool ClientData::readFile(Listfile::FileKey const& file_key, std::vector<char>& buffer)
{
  const std::lock_guard _lock(_mutex);

  HANDLE file_handle = nullptr;

  for (auto it = _archives.rbegin(); it != _archives.rend(); ++it)
  {
    if (!(*it)->openFile(file_key, _locale_mode, &file_handle))
      continue;

    std::uint64_t buf_size = (*it)->getFileSize(file_handle);

    // skip empty files, sometime there are empty duplicates in older MPQs.
    if (!buf_size)
        continue;

    buffer.resize(buf_size);

    if (!(*it)->readFile(file_handle, buffer.data(), buf_size))
    {
      assert(false);
    }

    if (!(*it)->closeFile(file_handle))
    {
      assert(false);
    }

    return true;
  }

  return false;
}

std::array<int, 2> BlizzardArchive::ClientData::saveLocalFilesToArchive(Archive::MPQArchive* archive, bool compress, bool compact)
{
    // Only supports MPQ currently.
    if (_storage_type != StorageType::MPQ)
        return {};

    const std::lock_guard _lock(_mutex);

    int file_count = 0;
    // size_t file_fail = 0;
    int file_success = 0;
    std::array<int, 2> result_array = {0,0};

    try {
        // Check if the given path is a directory
        if (!fs::is_directory(_local_path)) {
            return result_array;
        }

        if (!archive->openForWritting())
        {
            return result_array;
        }


        // Iterate through all files in the directory
        for (const auto& entry : fs::recursive_directory_iterator(_local_path)) {
            if (entry.is_regular_file()) {
                // std::cout << "Processing file: " << entry.path() << std::endl;

                std::string const extension = entry.path().extension().string();
                // filter noggit files
                if (extension == ".noggitproj" || extension == ".json" || extension == ".ini")
                    continue;

                // test limit files count
                // if (file_count >= 500)
                //     break;

                file_count++;

                // use SFileAddFileEx high level function which does everything, or read manually
                bool full_write_file = false;

                if (!full_write_file)
                {
                    // get internal Wow path
                    auto wow_path = fs::relative(entry.path(), fs::path(_local_path)).generic_string();

                    try
                    {
                        // normalize path with filekey or nah ?
                        bool success = archive->addFile(wow_path, entry.path().generic_string(), _locale_mode, 0, compress);
                        if (success)
                            file_success++;
                        else
                            continue;
                    }
                    catch (...)
                    {
                        continue;
                    }
                }
                else
                {
                    // Open the file
                    std::ifstream file_stream(entry.path(), std::ios_base::binary | std::ios_base::in);
                    if (file_stream.is_open())
                    {
                        // Get the file size
                        file_stream.seekg(0, std::ios::end);
                        std::size_t buf_size = file_stream.tellg();
                        file_stream.seekg(0, std::ios::beg);

                        // Allocate buffer
                        char* buffer = new char[buf_size];
                        // Read file into buffer
                        file_stream.read(buffer, buf_size);
                        file_stream.close();

                        // get internal Wow path
                        auto wow_path = fs::relative(entry.path(), fs::path(_local_path)).generic_string();
                        try
                        {
                            bool success = archive->writeFile(wow_path, buffer, buf_size, _locale_mode, 0, compress);
                            if (success)
                                file_success++;
                            else
                                continue;
                        }
                        catch (...)
                        {
                            continue;
                        }

                        delete[] buffer;
                    }
                    else {
                        continue; // std::cerr << "Unable to open file: " << entry.path() << std::endl;
                    }

                    if (file_count % 20 == 0) {
                        saveLocalFilesProgressCallback(file_count);
                    }
                }
            }
        }

        // compact if at least 1? file got added.
        if (compact && file_success >= 1)
        {
            archive->compactArchive();
        }

        // close archive back to read-only
        archive->closeToReadOnly();
    }
    catch (const fs::filesystem_error& ex) {
        // std::cerr << "Filesystem error: " << ex.what() << std::endl;
        return result_array;
    }
    catch (...)
    {
        archive->closeToReadOnly();
    }

    return result_array = {file_count, file_success};
}

bool ClientData::existsOnDisk(Listfile::FileKey const& file_key)
{
  if (!file_key.hasFilepath())
    return false;

  return fs::exists(getDiskPath(file_key));
}

bool ClientData::exists(Listfile::FileKey const& file_key)
{
  if (ClientData::existsOnDisk(file_key))
  {
    return true;
  }

  const std::lock_guard _lock(_mutex);

  for (auto it = _archives.rbegin(); it != _archives.rend(); ++it)
  {
    if ((*it)->exists(file_key, _locale_mode))
      return true;
  }

  return false;
}

std::string ClientData::getDiskPath(Listfile::FileKey const& file_key)
{
  const std::lock_guard _lock(_mutex);

  if (file_key.hasFilepath())
  {
    return (fs::path(_local_path) / ClientData::normalizeFilenameUnix(file_key.filepath())).string();
  }
  else
  {
    // try deducing filepath from listfile
    assert(file_key.hasFileDataID());
    std::string filepath = _listfile.getPath(file_key.fileDataID());

    if (!filepath.empty())
    {
      return (fs::path(_local_path) / ClientData::normalizeFilenameUnix(filepath)).string();
    }
    else
    {
      return (fs::path(_local_path) / "unknown_files/" / std::to_string(file_key.fileDataID())).string();
    }
  }
   
}

std::string ClientData::normalizeFilenameUnix(std::string filename)
{
  std::transform(filename.begin(), filename.end(), filename.begin()
    , [](char c)
    {
      return c == '\\' ? '/' : c;
    }
  );
  return filename;
}

std::string ClientData::normalizeFilenameInternal(std::string filename)
{
  std::transform(filename.begin(), filename.end(), filename.begin(), ::tolower);
  std::transform(filename.begin(), filename.end(), filename.begin()
      , [](char c)
                 {
                   return c == '\\' ? '/' : c;
                 }
  );

  if (filename.ends_with(".mdx"))
  {
    filename = std::regex_replace(filename, std::regex(".mdx"), ".m2");
  }
  else if(filename.ends_with(".mdl"))
  {
    filename = std::regex_replace(filename, std::regex(".mdl"), ".m2");
  }

  return filename;
}

std::string ClientData::normalizeFilenameWoW(std::string filename)
{
  std::transform(filename.begin(), filename.end(), filename.begin(), ::toupper);
  std::transform(filename.begin(), filename.end(), filename.begin()
    , [](char c)
    {
      return c == '/' ? '\\' : c;
    }
  );
  return filename;
}