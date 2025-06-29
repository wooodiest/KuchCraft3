#pragma once

namespace KuchCraft {

	namespace Utils {

		std::string ReadFile(const std::filesystem::path& path);

		std::vector<uint8_t> ReadFileBinary(const std::filesystem::path& path);

		std::string TrimStart(const std::string& str);

		std::string Trim(const std::string& str);

		struct SourceRange
		{
			size_t Start = 0;
			size_t End = 0;

			std::string_view View(const std::string& source) const;
		};

	}
}