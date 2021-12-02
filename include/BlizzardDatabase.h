#pragma once
#include <string>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <BlizzardDatabaseTable.h>
#include <readers/BlizzardTableReaderFactory.h>
#include <DatabaseDefinition.h>
#include <extensions/StringExtensions.h>
#include <sstream>

namespace BlizzardDatabaseLib
{
	class BlizzardDatabase
	{
		friend class BlizzardDatabaseTable;
	private:
		const std::string _databaseFilesLocation;
		const std::string _databaseDefinitionFilesLocation;

		Reader::BlizzardTableReaderFactory _blizzardTableReaderFactory;

		std::map<std::string, std::shared_ptr<BlizzardDatabaseTable>> _loadedTables;
	public:
		BlizzardDatabase(const std::string& databaseCollectionDirectory, const std::string& databaseDefinitionDirectory);

		const BlizzardDatabaseTable& LoadTable(const std::string& tableName, const Structures::Build& build);
		void UnloadTable(const std::string& tableName);
	};
}