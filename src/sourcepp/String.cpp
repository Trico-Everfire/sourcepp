#include <sourcepp/String.h>

#include <algorithm>
#include <cctype>
#include <random>
#include <sstream>

namespace {

std::mt19937& getRandomGenerator() {
	static std::random_device random_device{};
	static std::mt19937 generator{random_device()};
	return generator;
}

} // namespace

using namespace sourcepp;

bool string::contains(std::string_view s, char c) {
	return std::find(s.begin(), s.end(), c) != s.end();
}

bool string::matches(std::string_view in, std::string_view search) {
	int inPos = 0, searchPos = 0;
	for ( ; inPos < in.length() && searchPos < search.length(); inPos++, searchPos++) {
		if (search[searchPos] == '%') {
			if (++searchPos == search.length()) {
				return false;
			}
			switch (search[searchPos]) {
				default:
				case '?': // wildcard
					break;
				case 'w': // whitespace
					if (!std::isspace(in[inPos])) return false;
					break;
				case 'a': // letter
					if (!(in[inPos] >= 'a' && in[inPos] <= 'z' || in[inPos] >= 'A' && in[inPos] <= 'Z')) return false;
					break;
				case 'u': // uppercase letter
					if (!(in[inPos] >= 'A' && in[inPos] <= 'Z')) return false;
					break;
				case 'l': // lowercase letter
					if (!(in[inPos] >= 'a' && in[inPos] <= 'z')) return false;
					break;
				case 'd': // digit
					if (!std::isdigit(in[inPos])) return false;
					break;
				case '%': // escaped percent
					if (in[inPos] != '%') return false;
					break;
			}
		} else if (in[inPos] != search[searchPos]) {
			return false;
		}
	}
	return inPos == in.length() && searchPos == search.length();
}

bool string::iequals(std::string_view s1, std::string_view s2) {
	return std::ranges::equal(s1, s2, [](char a, char b) { return std::tolower(a) == std::tolower(b); });
}

// https://stackoverflow.com/a/217605

void string::ltrim(std::string& s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](char c) { return !std::isspace(c); }));
}

std::string_view string::ltrim(std::string_view s) {
	while (!s.empty() && std::isspace(s[0])) {
		s.remove_prefix(1);
	}
	return s;
}

void string::rtrim(std::string& s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](char c) { return !std::isspace(c); }).base(), s.end());
}

std::string_view string::rtrim(std::string_view s) {
	while (!s.empty() && std::isspace(s[s.size() - 1])) {
		s.remove_suffix(1);
	}
	return s;
}

void string::trim(std::string& s) {
    rtrim(s);
    ltrim(s);
}

std::string_view string::trim(std::string_view s) {
	return ltrim(rtrim(s));
}

void string::trimInternal(std::string& s) {
	s.erase(std::ranges::unique(s, [](char lhs, char rhs) { return lhs == rhs && std::isspace(lhs); }).begin(), s.end());
}

std::string string::trimInternal(std::string_view s) {
	std::string out{s};
	trimInternal(out);
	return out;
}

void string::ltrim(std::string& s, std::string_view chars) {
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), [chars](char c) {
		return !contains(chars, c);
	}));
}

std::string_view string::ltrim(std::string_view s, std::string_view chars) {
	while (!s.empty() && contains(chars, s[0])) {
		s.remove_prefix(1);
	}
	return s;
}

void string::rtrim(std::string& s, std::string_view chars) {
	s.erase(std::find_if(s.rbegin(), s.rend(), [chars](char c) {
		return !contains(chars, c);
	}).base(), s.end());
}

std::string_view string::rtrim(std::string_view s, std::string_view chars) {
	while (!s.empty() && contains(chars, s[s.size() - 1])) {
		s.remove_suffix(1);
	}
	return s;
}

void string::trim(std::string& s, std::string_view chars) {
    rtrim(s, chars);
    ltrim(s, chars);
}

std::string_view string::trim(std::string_view s, std::string_view chars) {
	return ltrim(rtrim(s, chars), chars);
}

void string::trimInternal(std::string& s, std::string_view chars) {
	s.erase(std::ranges::unique(s, [chars](char lhs, char rhs) { return lhs == rhs && std::ranges::find(chars, lhs) != chars.end(); }).begin(), s.end());
}

std::string string::trimInternal(std::string_view s, std::string_view chars) {
	std::string out{s};
	trimInternal(out, chars);
	return out;
}

// https://stackoverflow.com/a/46931770

std::vector<std::string> string::split(std::string_view s, char delim) {
    std::vector<std::string> result;
    std::stringstream ss(std::string{s});
    std::string item;
    while (std::getline(ss, item, delim)) {
        result.push_back(item);
    }
    return result;
}

void string::toLower(std::string& input) {
	std::transform(input.begin(), input.end(), input.begin(), [](unsigned char c){ return std::tolower(c); });
}

std::string string::toLower(std::string_view input) {
	std::string out{input};
	toLower(out);
	return out;
}

void string::toUpper(std::string& input) {
	std::transform(input.begin(), input.end(), input.begin(), [](unsigned char c){ return std::toupper(c); });
}

std::string string::toUpper(std::string_view input) {
	std::string out{input};
	toUpper(out);
	return out;
}

std::string string::createRandom(uint16_t length, std::string_view chars) {
	auto& generator = ::getRandomGenerator();
	std::uniform_int_distribution distribution{0, static_cast<int>(chars.length() - 1)};

	std::string out;
	for (uint16_t i = 0; i < length; i++) {
		out += chars[distribution(generator)];
	}

	return out;
}

std::string string::generateUUIDv4() {
	static constexpr std::string_view chars = "0123456789abcdef";

	auto& generator = ::getRandomGenerator();
	std::uniform_int_distribution distribution{0, static_cast<int>(chars.length() - 1)};

	std::string out;
	for (uint16_t i = 0; i < 8; i++) {
		out += chars[distribution(generator)];
	}
	out += '-';
	for (uint16_t i = 0; i < 3; i++) {
		for (uint16_t j = 0; j < 4; j++) {
			out += chars[distribution(generator)];
		}
		out += '-';
	}
	for (uint16_t i = 0; i < 12; i++) {
		out += chars[distribution(generator)];
	}

	return out;
}

std::string string::padNumber(int64_t number, int width, char pad) {
	const auto numStr = std::to_string(number);
	return std::string(width - std::min<std::string::size_type>(width, numStr.length()), pad) + numStr;
}

void string::normalizeSlashes(std::string& path, bool stripSlashPrefix, bool stripSlashSuffix) {
	std::replace(path.begin(), path.end(), '\\', '/');
	if (stripSlashPrefix && path.starts_with('/')) {
		path = path.substr(1);
	}
	if (stripSlashSuffix && path.ends_with('/')) {
		path.pop_back();
	}
}

void string::denormalizeSlashes(std::string& path, bool stripSlashPrefix, bool stripSlashSuffix) {
	std::replace(path.begin(), path.end(), '/', '\\');
	if (stripSlashPrefix && path.starts_with('\\')) {
		path = path.substr(1);
	}
	if (stripSlashSuffix && path.ends_with('\\')) {
		path.pop_back();
	}
}

std::from_chars_result string::toBool(std::string_view number, bool& out, int base) {
	uint8_t tmp;
	const auto result = std::from_chars(number.data(), number.data() + number.size(), tmp, base);
	out = tmp;
	return result;
}
