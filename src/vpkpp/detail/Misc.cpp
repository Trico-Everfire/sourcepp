#include <vpkpp/detail/Misc.h>

#include <filesystem>

#include <FileStream.h>

#include <sourcepp/math/Integer.h>
#include <sourcepp/string/String.h>

using namespace sourcepp;
using namespace vpkpp;

std::pair<std::string, std::string> detail::splitFilenameAndParentDir(const std::string& filename) {
	auto name = filename;
	string::normalizeSlashes(name);

	auto lastSeparator = name.rfind('/');
	auto dir = lastSeparator != std::string::npos ? name.substr(0, lastSeparator) : "";
	name = filename.substr(lastSeparator + 1);

	return {dir, name};
}

std::vector<std::byte> detail::readFileData(const std::string& filepath, std::size_t preloadBytesOffset) {
	FileStream stream{filepath};
	if (!stream) {
		return {};
	}
	stream.seek_in_u(preloadBytesOffset);
	return stream.read_bytes(std::filesystem::file_size(filepath) - preloadBytesOffset);
}

std::string detail::readFileText(const std::string& filepath, std::size_t preloadBytesOffset) {
	auto data = readFileData(filepath, preloadBytesOffset);
	std::string out;
	for (std::byte b : data) {
		out.push_back(static_cast<char>(b));
	}
	while (!out.empty() && out[out.size() - 1] == '\0') {
		out.pop_back();
	}
	return out;
}