#pragma once

#include <concepts>
#include <string>
#include <string_view>
#include <vector>

#include <BufferStream.h>
#include <sourcepp/parser/Text.h>
#include <sourcepp/FS.h>
#include <sourcepp/String.h>

namespace kvpp {

template<typename V>
concept KV1ValueType = std::convertible_to<V, std::string_view>
                    || std::same_as<V, bool>
                    || std::same_as<V, int32_t>
                    || std::same_as<V, int64_t>
                    || std::same_as<V, float>;

template<typename S, typename K>
requires std::convertible_to<S, std::string_view>
class KV1ElementBase {
public:
	/// Get the key associated with the element
	[[nodiscard]] std::string_view getKey() const {
		return this->key;
	}

	/// Get the value associated with the element
	[[nodiscard]] std::string_view getValue() const {
		return this->value;
	}

	/// Get the value associated with the element as the given type
	template<KV1ValueType V>
	[[nodiscard]] V getValue() const {
		if constexpr (std::convertible_to<V, std::string_view>) {
			return this->value;
		} else if constexpr (std::same_as<V, bool>) {
			return static_cast<bool>(this->getValue<int32_t>());
		} else if constexpr (std::same_as<V, int32_t>) {
			if (this->value.length() == 10 && this->value.starts_with("0x") && sourcepp::parser::text::isNumber(this->value.substr(2))) {
				return std::stoi(std::string{this->value.substr(2)}, nullptr, 16);
			}
			return std::stoi(std::string{this->value});
		} else if constexpr (std::same_as<V, int64_t>) {
			if (this->value.length() == 18 && this->value.starts_with("0x") && sourcepp::parser::text::isNumber(this->value.substr(2))) {
				return std::stoll(std::string{this->value.substr(2)}, nullptr, 16);
			}
			return std::stoll(std::string{this->value});
		} else if constexpr (std::same_as<V, float>) {
			return std::stof(std::string{this->value});
		}
		return V{};
	}

	/// Get the conditional associated with the element
	[[nodiscard]] std::string_view getConditional() const {
		return this->conditional;
	}

	/// Check if the element has one or more children with the given name
	[[nodiscard]] bool hasChild(std::string_view childKey) const {
		return !this->operator[](childKey).isInvalid();
	}

	/// Get the number of child elements
	[[nodiscard]] uint64_t getChildCount() const {
		return this->children.size();
	}

	/// Get the number of child elements with the given key
	[[nodiscard]] uint64_t getChildCount(std::string_view childKey) const {
		uint64_t count = 0;
		for (const KV1ElementBase& element : this->children) {
			if (sourcepp::string::iequals(element.key, childKey)) {
				++count;
			}
		}
		return count;
	}

	/// Get the child elements of the element
	[[nodiscard]] const std::vector<K>& getChildren() const {
		return this->children;
	}

	using iterator = typename std::vector<K>::iterator;

	[[nodiscard]] constexpr iterator begin() {
		return this->children.begin();
	}

	[[nodiscard]] constexpr iterator end() {
		return this->children.end();
	}

	using const_iterator = typename std::vector<K>::const_iterator;

	[[nodiscard]] constexpr const_iterator begin() const {
		return this->children.begin();
	}

	[[nodiscard]] constexpr const_iterator end() const {
		return this->children.end();
	}

	[[nodiscard]] constexpr const_iterator cbegin() const {
		return this->children.cbegin();
	}

	[[nodiscard]] constexpr const_iterator cend() const {
		return this->children.cend();
	}

	/// Get the child element of the element at the given index
	[[nodiscard]] const KV1ElementBase& operator[](unsigned int n) const {
		return this->children.at(n);
	}

	/// Get the first child element of the element with the given key
	[[nodiscard]] const KV1ElementBase& operator[](std::string_view childKey) const {
		return this->operator()(childKey);
	}

	/// Get the first child element of the element with the given key
	[[nodiscard]] const KV1ElementBase& operator()(std::string_view childKey) const {
		for (const auto& element : this->children) {
			if (sourcepp::string::iequals(element.getKey(), childKey)) {
				return element;
			}
		}
		return getInvalid();
	}

	/// Get the nth child element of the element with the given key
	[[nodiscard]] const KV1ElementBase& operator()(std::string_view childKey, unsigned int n) const {
		unsigned int count = 0;
		for (const auto& element : this->children) {
			if (sourcepp::string::iequals(element.getKey(), childKey)) {
				if (count == n) {
					return element;
				}
				++count;
			}
		}
		return getInvalid();
	}

	/// Check if the given element is invalid
	[[nodiscard]] bool isInvalid() const {
		return this == &getInvalid();
	}

	static const KV1ElementBase& getInvalid() {
		static KV1ElementBase element;
		return element;
	}

protected:
	KV1ElementBase() = default;

	static void read(BufferStreamReadOnly& stream, BufferStream& backing, std::vector<K>& elements, const sourcepp::parser::text::EscapeSequenceMap& escapeSequences) {
		using namespace sourcepp;
		while (true) {
			// Check if the block is over
			parser::text::eatWhitespaceAndSingleLineComments(stream);
			if (stream.peek<char>() == '}') {
				stream.skip();
				break;
			}
			// Read key
			{
				auto childKey = parser::text::readStringToBuffer(stream, backing, parser::text::DEFAULT_STRING_START, parser::text::DEFAULT_STRING_END, escapeSequences);
				elements.push_back(K{});
				elements.back().key = childKey;
				parser::text::eatWhitespaceAndSingleLineComments(stream);
			}
			// Read value
			if (stream.peek<char>() != '{') {
				elements.back().value = parser::text::readStringToBuffer(stream, backing, parser::text::DEFAULT_STRING_START, parser::text::DEFAULT_STRING_END, escapeSequences);
				parser::text::eatWhitespaceAndSingleLineComments(stream);
			}
			// Read conditional
			if (stream.peek<char>() == '[') {
				elements.back().conditional = parser::text::readStringToBuffer(stream, backing, "[", "]", escapeSequences);
				parser::text::eatWhitespaceAndSingleLineComments(stream);
			}
			// Read block
			if (stream.peek<char>() == '{') {
				stream.skip();
				parser::text::eatWhitespaceAndSingleLineComments(stream);
				if (stream.peek<char>() != '}') {
					KV1ElementBase<S, K>::read(stream, backing, elements.back().children, escapeSequences);
				} else {
					stream.skip();
				}
			}
		}
	}

	S key = ""; // NOLINT(*-redundant-string-init)
	S value = ""; // NOLINT(*-redundant-string-init)
	S conditional = ""; // NOLINT(*-redundant-string-init)
	std::vector<K> children;
};

template<typename S = std::string_view>
requires std::convertible_to<S, std::string_view>
class KV1ElementReadable : public KV1ElementBase<S, KV1ElementReadable<S>> {
	friend class KV1ElementBase<S, KV1ElementReadable<S>>;

protected:
	KV1ElementReadable() = default;
};

template<typename S = std::string_view>
requires std::convertible_to<S, std::string_view>
class KV1 : public KV1ElementReadable<S> {
public:
	explicit KV1(std::string_view kv1Data, bool useEscapeSequences_ = false)
			: KV1ElementReadable<S>()
			, useEscapeSequences(useEscapeSequences_) {
		if (kv1Data.empty()) {
			return;
		}
		BufferStreamReadOnly stream{kv1Data.data(), kv1Data.size()};

		// Multiply by 2 to ensure buffer will have enough space (very generous)
		this->backingData.resize(kv1Data.size() * 2);
		BufferStream backing{this->backingData, false};
		try {
			KV1ElementBase<S, KV1ElementReadable<S>>::read(stream, backing, this->children, sourcepp::parser::text::getDefaultEscapeSequencesOrNone(this->useEscapeSequences));
		} catch (const std::overflow_error&) {}
	}

protected:
	using KV1ElementReadable<S>::getKey;
	using KV1ElementReadable<S>::getValue;
	using KV1ElementReadable<S>::getConditional;

	std::string backingData;
	bool useEscapeSequences;
};

template<typename S = std::string>
requires std::convertible_to<S, std::string_view>
class KV1ElementWritable : public KV1ElementBase<S, KV1ElementWritable<S>> {
	friend class KV1ElementBase<S, KV1ElementWritable<S>>;

public:
	/// Set the key associated with the element
	void setKey(std::string_view key_) {
		this->key = key_;
	}

	/// Set the value associated with the element
	template<KV1ValueType V>
	void setValue(V value_) {
		if constexpr (std::convertible_to<V, std::string_view>) {
			this->value = std::string_view{value_};
		} else {
			this->setValue(std::to_string(value_));
		}
	}

	/// Set the value associated with the element
	template<KV1ValueType V>
	KV1ElementWritable& operator=(V value_) {
		this->setValue(value_);
		return *this;
	}

	/// Set the conditional associated with the element
	void setConditional(std::string_view conditional_) {
		this->conditional = conditional_;
	}

	template<KV1ValueType V = std::string_view>
	KV1ElementWritable& addChild(std::string_view key_, V value_ = {}, std::string_view conditional_ = "") {
		KV1ElementWritable elem;
		elem.setKey(key_);
		elem.setValue(value_);
		elem.setConditional(conditional_);
		this->children.push_back(elem);
		return this->children.back();
	}

	/// Get the child element of the element at the given index
	[[nodiscard]] KV1ElementWritable& operator[](unsigned int n) {
		return this->children.at(n);
	}

	/// Get the first child element of the element with the given key, or create a new element if it doesn't exist
	[[nodiscard]] KV1ElementWritable& operator[](std::string_view childKey) {
		return this->operator()(childKey);
	}

	/// Get the first child element of the element with the given key, or create a new element if it doesn't exist
	[[nodiscard]] KV1ElementWritable& operator()(std::string_view childKey) {
		for (auto& element : this->children) {
			if (sourcepp::string::iequals(element.getKey(), childKey)) {
				return element;
			}
		}
		return this->addChild(childKey);
	}

	/// Get the nth child element of the element with the given key, or create a new element if it doesn't exist
	[[nodiscard]] KV1ElementWritable& operator()(std::string_view childKey, unsigned int n) {
		unsigned int count = 0;
		for (auto& element: this->children) {
			if (sourcepp::string::iequals(element.getKey(), childKey)) {
				if (count == n) {
					return element;
				}
				++count;
			}
		}
		return this->addChild(childKey);
	}

	/// Remove a child element from the element.
	void removeChild(unsigned int n) {
		if (this->children.size() > n) {
			this->children.erase(this->children.begin() + n);
		}
	}

	/// Remove a child element from the element. -1 means all children with the given key
	void removeChild(std::string_view childKey, int n = -1) {
		unsigned int count = 0;
		for (auto element = this->children.begin(); element != this->children.end(); ++element) {
			if (sourcepp::string::iequals(element->getKey(), childKey)) {
				if (n < 0 || count == n) {
					element = this->children.erase(element);
					if (count == n) {
						return;
					}
				}
				++count;
			}
		}
	}

protected:
	KV1ElementWritable() = default;

	static void write(BufferStream& stream, const std::vector<KV1ElementWritable>& elements, unsigned short indentLevel, const sourcepp::parser::text::EscapeSequenceMap& escapeSequences) {
		using namespace sourcepp;
		constexpr auto writeIndentation = [](BufferStream& stream_, unsigned short indentLevel_) {
			for (unsigned short i = 0; i < indentLevel_; i++) {
				stream_.write('\t');
			}
		};
		constexpr auto writeQuotedString = [](BufferStream& stream_, std::string_view str, const parser::text::EscapeSequenceMap& escapeSequences_, char quoteStart = '\"', char quoteEnd = '\"') {
			stream_.write(quoteStart);
			if (!str.empty()) {
				stream_.write(parser::text::convertSpecialCharsToEscapes(str, escapeSequences_), false);
			}
			stream_.write(quoteEnd);
		};

		for (auto& elem : elements) {
			writeIndentation(stream, indentLevel);
			writeQuotedString(stream, elem.key, escapeSequences);
			if (!elem.value.empty() || elem.children.empty()) {
				stream.write(' ');
				writeQuotedString(stream, elem.value, escapeSequences);
			}
			if (!elem.conditional.empty()) {
				stream.write(' ');
				writeQuotedString(stream, elem.conditional, escapeSequences, '[', ']');
			}
			stream.write('\n');
			if (!elem.children.empty()) {
				writeIndentation(stream, indentLevel);
				stream << '{' << '\n';
				write(stream, elem.children, indentLevel + 1, escapeSequences);
				writeIndentation(stream, indentLevel);
				stream << '}' << '\n';
			}
		}
	}
};

template<typename S = std::string>
requires std::convertible_to<S, std::string_view>
class KV1Writer : public KV1ElementWritable<S> {
public:
	explicit KV1Writer(std::string_view kv1Data = "", bool useEscapeSequences_ = false)
			: KV1ElementWritable<S>()
			, useEscapeSequences(useEscapeSequences_) {
		if (kv1Data.empty()) {
			return;
		}
		BufferStreamReadOnly stream{kv1Data.data(), kv1Data.size()};

		// Multiply by 2 to ensure buffer will have enough space (very generous)
		// Also it's ok that this backing data dies, it will perform copies here
		std::string backingData(kv1Data.size() * 2, '\0');
		BufferStream backing{backingData, false};
		try {
			KV1ElementBase<S, KV1ElementWritable<S>>::read(stream, backing, this->children, sourcepp::parser::text::getDefaultEscapeSequencesOrNone(this->useEscapeSequences));
		} catch (const std::overflow_error&) {}
	}

	[[nodiscard]] std::string bake() const {
		std::string buffer;
		BufferStream stream{buffer};
		KV1ElementWritable<S>::write(stream, this->children, 0, sourcepp::parser::text::getDefaultEscapeSequencesOrNone(this->useEscapeSequences));
		buffer.resize(stream.size());
		return buffer;
	}

	void bake(const std::string& kv1Path) const {
		sourcepp::fs::writeFileText(kv1Path, this->bake());
	}

protected:
	using KV1ElementWritable<S>::getKey;
	using KV1ElementWritable<S>::setKey;
	using KV1ElementWritable<S>::getValue;
	using KV1ElementWritable<S>::setValue;
	using KV1ElementWritable<S>::operator=;
	using KV1ElementWritable<S>::getConditional;
	using KV1ElementWritable<S>::setConditional;

	bool useEscapeSequences;
};

} // namespace kvpp
