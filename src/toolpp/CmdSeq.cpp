#include <toolpp/CmdSeq.h>

#include <cstring>

#include <FileStream.h>
#include <kvpp/kvpp.h>
#include <sourcepp/String.h>

using namespace kvpp;
using namespace sourcepp;
using namespace toolpp;

namespace {

std::vector<std::byte> bakeBinary(const CmdSeq& cmdSeq) {
	std::vector<std::byte> out;
	BufferStream writer{out};

	writer
		.write("Worldcraft Command Sequences\r\n\x1a", 31)
		.write<float>(cmdSeq.getVersion())
		.write<uint32_t>(cmdSeq.getSequences().size());

	for (const auto& [seqName, seqCommands] : cmdSeq.getSequences()) {
		writer
			.write(seqName, true, 128)
			.write<uint32_t>(seqCommands.size());

		for (const auto& [enabled, special, executable, arguments, ensureFileExists, pathToTheoreticallyExistingFile, useProcessWindow, waitForKeypress] : seqCommands) {
			writer
				.write<uint32_t>(enabled)
				.write(special)
				.write(executable, true, 260)
				.write(arguments, true, 260)
				.write<uint32_t>(true)
				.write<uint32_t>(ensureFileExists)
				.write(pathToTheoreticallyExistingFile, true, 260)
				.write<uint32_t>(useProcessWindow);

			if (cmdSeq.getVersion() > 0.15f) {
				writer.write<uint32_t>(waitForKeypress);
			}
		}
	}

	out.resize(writer.size());
	return out;
}

std::vector<std::byte> bakeKeyValues(const CmdSeq& cmdSeq) {
	KV1Writer kv;
	auto& kvFile = kv.addChild("Command Sequences");
	for (const auto& [seqName, seqCommands] : cmdSeq.getSequences()) {
		auto& kvSequence = kvFile.addChild(seqName);
		for (int i = 1; i <= seqCommands.size(); i++) {
			const auto& [enabled, special, executable, arguments, ensureFileExists, pathToTheoreticallyExistingFile, useProcessWindow, waitForKeypress] = seqCommands[i - 1];
			auto& kvCommand = kvSequence.addChild(std::to_string(i));
			kvCommand["enabled"] = enabled;
			kvCommand["special_cmd"] = static_cast<int>(special);
			kvCommand["run"] = executable;
			kvCommand["params"] = arguments;
			kvCommand["ensure_check"] = ensureFileExists;
			kvCommand["ensure_fn"] = pathToTheoreticallyExistingFile;
			kvCommand["use_process_wnd"] = useProcessWindow;
			kvCommand["no_wait"] = waitForKeypress;
		}
	}

	const auto kvStr = kv.bake();
	std::vector<std::byte> out;
	out.resize(kvStr.length());
	std::memcpy(out.data(), kvStr.data(), kvStr.length());
	return out;
}

} // namespace

CmdSeq::CmdSeq(std::string path_)
		: version(0.f)
		, path(std::move(path_)) {
	{
		FileStream reader{path};
		if (!reader) {
			return;
		}
		if (auto binStr = reader.seek_in(0).read_string(10); binStr == "Worldcraft") {
			this->usingKeyValues = false;
		} else {
			auto kvStr = reader.seek_in(0).read_string(19);
			string::toLower(kvStr);
			if (kvStr == "\"command sequences\"") {
				this->usingKeyValues = true;
			} else {
				return;
			}
		}
	}
	if (this->usingKeyValues) {
		this->parseKeyValues(path);
	} else {
		this->parseBinary(path);
	}
}

float CmdSeq::getVersion() const {
	return this->version;
}

void CmdSeq::setVersion(bool isV02) {
	if (isV02) {
		this->version = 0.2f;
	} else {
		this->version = 0.1f;
	}
}

void CmdSeq::parseBinary(const std::string& path) {
	FileStream reader{path};
	if (!reader) {
		return;
	}

	reader.seek_in(31).read(this->version);

	const auto sequenceCount = reader.read<uint32_t>();
	for (uint32_t s = 0; s < sequenceCount; s++) {
		auto& [seqName, seqCommands] = this->sequences.emplace_back();
		seqName = reader.read_string(128);

		const auto commandCount = reader.read<uint32_t>();
		for (uint32_t c = 0; c < commandCount; c++) {
			auto& [enabled, special, executable, arguments, ensureFileExists, pathToTheoreticallyExistingFile, useProcessWindow, waitForKeypress] = seqCommands.emplace_back();
			enabled = reader.read<int32_t>() & 0xFF;
			special = reader.read<Command::Special>();
			executable = reader.read_string(260);
			arguments = reader.read_string(260);
			reader.skip_in<int32_t>();
			ensureFileExists = reader.read<int32_t>();
			pathToTheoreticallyExistingFile = reader.read_string(260);
			useProcessWindow = reader.read<int32_t>();
			if (version > 0.15f) {
				waitForKeypress = reader.read<int32_t>();
			}
		}
	}
}

void CmdSeq::parseKeyValues(const std::string& path) {
	this->version = 0.2f;

	const KV1 cmdSeq{fs::readFileText(path)};
	for (const auto& kvSequence : cmdSeq["Command Sequences"].getChildren()) {
		auto& [seqName, seqCommands] = this->sequences.emplace_back();
		seqName = kvSequence.getKey();

		for (const auto& kvCommand : kvSequence.getChildren()) {
			auto& [enabled, special, executable, arguments, ensureFileExists, pathToTheoreticallyExistingFile, useProcessWindow, waitForKeypress] = seqCommands.emplace_back();
			string::toBool(kvCommand["enabled"].getValue(), enabled);
			string::toInt(kvCommand["special_cmd"].getValue(), reinterpret_cast<std::underlying_type_t<Command::Special>&>(special));
			executable = kvCommand["run"].getValue();
			arguments = kvCommand["params"].getValue();
			string::toBool(kvCommand["ensure_check"].getValue(), ensureFileExists);
			pathToTheoreticallyExistingFile = kvCommand["ensure_fn"].getValue();
			string::toBool(kvCommand["use_process_wnd"].getValue(), useProcessWindow);
			string::toBool(kvCommand["no_wait"].getValue(), waitForKeypress);
		}
	}
}

std::vector<CmdSeq::Sequence>& CmdSeq::getSequences() {
	return this->sequences;
}

const std::vector<CmdSeq::Sequence>& CmdSeq::getSequences() const {
	return this->sequences;
}

std::vector<std::byte> CmdSeq::bake() const {
	return this->bake(this->usingKeyValues);
}

std::vector<std::byte> CmdSeq::bake(bool overrideUsingKeyValues) const {
	if (overrideUsingKeyValues) {
		return ::bakeKeyValues(*this);
	}
	return ::bakeBinary(*this);
}

bool CmdSeq::bake(const std::string& path_) {
	return this->bake(path_, this->usingKeyValues);
}

bool CmdSeq::bake(const std::string& path_, bool overrideUsingKeyValues) {
	FileStream writer{path_};
	if (!writer) {
		return false;
	}
	this->path = path_;

	writer.seek_out(0);
	if (overrideUsingKeyValues) {
		writer.write(::bakeKeyValues(*this));
	} else {
		writer.write(::bakeBinary(*this));
	}
	return true;
}
