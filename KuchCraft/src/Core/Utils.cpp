#include "kcpch.h"
#include "Core/Utils.h"

namespace KuchCraft {
	namespace Utils {

		std::string ReadFile(const std::filesystem::path& path)
		{
			if (!std::filesystem::exists(path))
			{
				KC_CORE_ERROR("File does not exist: {}", path.string());
				return std::string();
			}
			if (!std::filesystem::is_regular_file(path))
			{
				KC_CORE_ERROR("Path is not a regular file: {}", path.string());
				return std::string();
			}

			try
			{
				std::ifstream file(path);
				if (!file.is_open())
				{
					KC_CORE_ERROR("Failed to open file: {}", path.string());
					return std::string();
				}

				std::stringstream buffer;
				buffer << file.rdbuf();
				file.close();
				return buffer.str();
			}
			catch (const std::exception& e)
			{
				KC_CORE_ERROR("Error reading file {}: {}", path.string(), e.what());
			}

			return std::string();
		}

		std::vector<uint8_t> ReadFileBinary(const std::filesystem::path& path)
		{
			if (!std::filesystem::exists(path) || !std::filesystem::is_regular_file(path))
			{
				KC_CORE_ERROR("Invalid binary file path: {}", path.string());
				return {};
			}

			try 
			{
				std::ifstream file(path, std::ios::binary | std::ios::ate);
				if (!file.is_open())
				{
					KC_CORE_ERROR("Failed to open binary file: {}", path.string());
					return {};
				}

				std::streamsize size = file.tellg();
				file.seekg(0, std::ios::beg);

				std::vector<uint8_t> buffer(size);
				if (!file.read(reinterpret_cast<char*>(buffer.data()), size))
				{
					KC_CORE_ERROR("Failed to read binary file: {}", path.string());
					return {};
				}

				return buffer;
			}
			catch (const std::exception& e)
			{
				KC_CORE_ERROR("Error reading binary file {}: {}", path.string(), e.what());
				return {};
			}
		}

		std::string TrimStart(const std::string& str)
		{
			size_t start = str.find_first_not_of(" \t");
			return (start == std::string::npos) ? "" : str.substr(start);
		}

		std::string Trim(const std::string& str)
		{
			const auto begin = std::find_if_not(str.begin(), str.end(), [](unsigned char ch) {
				return std::isspace(ch);
			});

			const auto end = std::find_if_not(str.rbegin(), str.rend(), [](unsigned char ch) {
				return std::isspace(ch);
			}).base();

			return (begin < end) ? std::string(begin, end) : std::string();
		}

		std::string_view SourceRange::View(const std::string& source) const
		{
			return std::string_view(source.data() + Start, End - Start);
		}
	}
}