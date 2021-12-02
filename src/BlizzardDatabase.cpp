#include "BlizzardDatabase.h"

namespace BlizzardDatabaseLib
{
    BlizzardDatabase::BlizzardDatabase(const std::string& databaseCollectionDirectory, const std::string& databaseDefinitionDirectory) :
        _databaseFilesLocation(databaseCollectionDirectory), _databaseDefinitionFilesLocation(databaseDefinitionDirectory)
    {
        _loadedTables = std::map<std::string, std::shared_ptr<BlizzardDatabaseTable>>();
        _blizzardTableReaderFactory = Reader::BlizzardTableReaderFactory();
    }

    const BlizzardDatabaseTable& BlizzardDatabase::LoadTable(const std::string& tableName, const Structures::Build& build)
    {
        if (_loadedTables.contains(tableName))
            std::cout << "Table Already Loaded" << std::endl;

        auto absoluteFilePathOfDatabaseTable = _databaseFilesLocation + "\\" + tableName + ".db2";
        auto absoluteFilePathOfDatabaseTableDefinition = _databaseDefinitionFilesLocation + "\\" + tableName + ".dbd";

        auto databaseDefinition = DatabaseDefinition(absoluteFilePathOfDatabaseTableDefinition);
        auto tableDefinition = Structures::VersionDefinition();
        auto tableFound = databaseDefinition.For(build, tableDefinition);

        if (tableFound == false)
            std::cout << "Verion Not found" << std::endl;

        auto fileStream = std::make_shared<std::ifstream>();
        fileStream->open(absoluteFilePathOfDatabaseTable, std::ifstream::binary);

        auto streamReader = std::make_shared<Stream::StreamReader>(fileStream);
        auto fileFormatIdentifier = streamReader->ReadString(4);

        auto tableReader = _blizzardTableReaderFactory.For(streamReader, tableDefinition,fileFormatIdentifier);

        auto constructedTable = std::make_shared<BlizzardDatabaseTable>(tableReader);
        constructedTable->LoadTableStructure();

        _loadedTables.emplace(tableName, constructedTable);

        return *constructedTable;
    }

    void BlizzardDatabase::UnloadTable(const std::string& tableName)
    {
        if (!_loadedTables.contains(tableName))
            std::cout << "Table Not Loaded" << std::endl;

        auto table = _loadedTables.at(tableName);

        table.reset();

        _loadedTables.erase(tableName);
    }
}
