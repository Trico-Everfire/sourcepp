#pragma once

#include <stdexcept>
#include <string_view>
#include <unordered_map>

class BufferStream;

namespace sourcepp::parser::text {

constexpr std::string_view DEFAULT_SINGLE_LINE_COMMENT_START = "//";
constexpr std::string_view DEFAULT_MULTI_LINE_COMMENT_START  = "/*";
constexpr std::string_view DEFAULT_MULTI_LINE_COMMENT_END    = "*/";
constexpr std::string_view DEFAULT_STRING_START              = "\"";
constexpr std::string_view DEFAULT_STRING_END                = "\"";

using EscapeSequenceMap = std::unordered_map<char, char>;

[[nodiscard]] const EscapeSequenceMap& getDefaultEscapeSequences();

[[nodiscard]] const EscapeSequenceMap& getDefaultEscapeSequencesOrNone(bool useEscapes);

/**
 * If a char is a newline character.
 * @param c The char.
 * @return The char is a newline character.
 */
[[nodiscard]] bool isNewLine(char c);

/**
 * If a string is entirely composed of newline characters.
 * @param str The string.
 * @return The string is entirely composed of newline characters.
 */
[[nodiscard]] bool isNewLine(std::string_view str);

/**
 * If a char is a whitespace character.
 * @param c The char.
 * @return The char is a whitespace character.
 */
[[nodiscard]] bool isWhitespace(char c);

/**
 * If a string is entirely composed of whitespace characters.
 * @param str The string.
 * @return The string is entirely composed of whitespace characters.
 */
[[nodiscard]] bool isWhitespace(std::string_view str);

/**
 * If a char is a numerical character (0-9).
 * @param c The char.
 * @return The char is a numerical character.
 */
[[nodiscard]] bool isNumber(char c);

/**
 * If a string is entirely composed of numerical characters (0-9).
 * @param str The string.
 * @return The string is entirely composed of numerical characters.
 */
[[nodiscard]] bool isNumber(std::string_view str);

/**
 * Convert special characters like \\n to escaped special characters like \\\\n
 * @param str The string to convert
 * @param escapeSequences The escapes to convert
 * @return The string without any special characters
 */
[[nodiscard]] std::string convertSpecialCharsToEscapes(std::string_view str, const EscapeSequenceMap& escapeSequences);

/**
 * Convert escaped special characters like \\\\n to special characters like \\n
 * @param str The string to convert
 * @param escapeSequences The escapes to convert
 * @return The string with special characters instead of escapes
 */
[[nodiscard]] std::string convertEscapesToSpecialChars(std::string_view str, const EscapeSequenceMap& escapeSequences);

/**
 * Eat all whitespace after the current stream position.
 * @param stream The BufferStream to modify.
 */
void eatWhitespace(BufferStream& stream);

/**
 * If a single line comment is detected, eat its contents.
 * This function does not handle the detection of single line comments!
 * @param stream The BufferStream to modify.
 */
void eatSingleLineComment(BufferStream& stream);

/**
 * If a multi line comment is detected, eat its contents.
 * This function does not handle the detection of multi line comments!
 * @param stream The BufferStream to modify.
 * @param multiLineCommentEnd The multi line comment ending to search for.
 */
void eatMultiLineComment(BufferStream& stream, std::string_view multiLineCommentEnd = DEFAULT_MULTI_LINE_COMMENT_END);

/**
 * Eat all whitespace and single line comments after the current stream position.
 * @param stream The BufferStream to modify.
 * @param singleLineCommentStart The single line comment start sequence. Leave empty to skip checking for single line comments.
 */
void eatWhitespaceAndSingleLineComments(BufferStream& stream, std::string_view singleLineCommentStart = DEFAULT_SINGLE_LINE_COMMENT_START);

/**
 * Eat all whitespace and multi line comments after the current stream position.
 * @param stream The BufferStream to modify.
 * @param multiLineCommentStart The multi line comment start sequence. Leave empty to skip checking for multi line comments.
 */
void eatWhitespaceAndMultiLineComments(BufferStream& stream, std::string_view multiLineCommentStart = DEFAULT_MULTI_LINE_COMMENT_START);

/**
 * Eat all whitespace and comments after the current stream position.
 * @param stream The BufferStream to modify.
 * @param singleLineCommentStart The single line comment start sequence. Leave empty to skip checking for single line comments.
 * @param multiLineCommentStart The multi line comment start sequence. Leave empty to skip checking for multi line comments.
 */
void eatWhitespaceAndComments(BufferStream& stream, std::string_view singleLineCommentStart = DEFAULT_SINGLE_LINE_COMMENT_START, std::string_view multiLineCommentStart = DEFAULT_MULTI_LINE_COMMENT_START);

/**
 * If the given char exists at the current position, skip over it
 * @param stream The BufferStream to modify
 * @param c The char to compare
 * @return If the char exists at the current position
 */
[[nodiscard]] bool tryToEatChar(BufferStream& stream, char c);

/**
 * Read a string starting at the current stream position.
 * @param stream The BufferStream to modify.
 * @param backing The BufferStream to store the string data in.
 * @param start The starting string chars. If any processed char is in this string, the string will be treated as "quoted" and will include spaces.
 * @param end The ending string chars. If any processed char is in this string, and a starting char is detected, the string will terminate.
 * @param escapeSequences Characters that will be escaped if a backslash is present before them. To disable escapes, pass an empty map.
 * @return A view over the string written to the backing stream.
 */
std::string_view readStringToBuffer(BufferStream& stream, BufferStream& backing, std::string_view start = DEFAULT_STRING_START, std::string_view end = DEFAULT_STRING_END, const EscapeSequenceMap& escapeSequences = getDefaultEscapeSequences());

/**
 * Read a string starting at the current stream position.
 * @param stream The BufferStream to modify.
 * @param backing The BufferStream to store the string data in.
 * @param escapeSequences Characters that will be escaped if a backslash is present before them. To disable escapes, pass an empty map.
 * @return A view over the string written to the backing stream.
 */
std::string_view readUnquotedStringToBuffer(BufferStream& stream, BufferStream& backing, const EscapeSequenceMap& escapeSequences = getDefaultEscapeSequences());

/**
 * Read a string starting at the current stream position.
 * @param stream The BufferStream to modify.
 * @param backing The BufferStream to store the string data in.
 * @param end The ending string chars. If any processed char is in this string, the string will terminate.
 * @param escapeSequences Characters that will be escaped if a backslash is present before them. To disable escapes, pass an empty map.
 * @return A view over the string written to the backing stream.
 */
std::string_view readUnquotedStringToBuffer(BufferStream& stream, BufferStream& backing, std::string_view end, const EscapeSequenceMap& escapeSequences = getDefaultEscapeSequences());

class syntax_error : public std::runtime_error {
public:
	using std::runtime_error::runtime_error;
};

} // namespace sourcepp::parser::text
