#include <studiomodelpp/structs/MDL.h>

#include <BufferStream.h>
#include <sourcepp/detail/BufferUtils.h>

using namespace sourcepp;
using namespace sourcepp::detail;
using namespace studiomodelpp::MDL;

constexpr int MDL_ID = 'I' + ('D' << 8) + ('S' << 16) + ('T' << 24);

bool MDL::open(const std::byte* data, std::size_t size) {
	BufferStream stream{data, size};

	int id = stream.read<int>();
	if (id != MDL_ID) {
		return false;
	}

	stream.read(this->version);
	if (this->version < 44 || this->version > 49) {
		return false;
	}

	stream.read(this->checksum);
    stream.read(this->name, 64);

	// dataLength
	stream.skip<int>();

	stream.read(this->eyePosition);
	stream.read(this->illuminationPosition);
	stream.read(this->hullMin);
	stream.read(this->hullMax);
	stream.read(this->viewBBoxMin);
	stream.read(this->viewBBoxMax);
	this->flags = static_cast<Flags>(stream.read<int>());

	int boneCount = stream.read<int>();
	int boneOffset = stream.read<int>();

	int boneControllerCount = stream.read<int>();
	int boneControllerOffset = stream.read<int>();

	int hitboxSetCount = stream.read<int>();
	int hitboxSetOffset = stream.read<int>();

	//int animDescCount = stream.read<int>();
	//int animDescOffset = stream.read<int>();
	stream.skip<int>(2);

	//int sequenceDescCount = stream.read<int>();
	//int sequenceDescOffset = stream.read<int>();
	stream.skip<int>(2);

	stream.read(this->activityListVersion);
	stream.read(this->eventsIndexed);

	int materialCount = stream.read<int>();
	int materialOffset = stream.read<int>();

	int materialDirCount = stream.read<int>();
	int materialDirOffset = stream.read<int>();

    int skinReferenceCount = stream.read<int>();
    int skinReferenceFamilyCount = stream.read<int>();
    int skinReferenceOffset = stream.read<int>();

	int bodyPartCount = stream.read<int>();
	int bodyPartOffset = stream.read<int>();

	// Done reading sequentially, start seeking to offsets

	stream.seek(boneOffset);
	for (int i = 0; i < boneCount; i++) {
		auto& bone = this->bones.emplace_back();

		readStringAtOffset(stream, bone.name);
		stream.read(bone.parent);
		stream.read(bone.boneController);
		stream.read(bone.position);
		stream.read(bone.rotationQuat);
		stream.read(bone.rotationEuler);
		stream.read(bone.positionScale);
		stream.read(bone.rotationScale);
		stream.read(bone.poseToBose);
		stream.read(bone.alignment);
		stream.read(bone.flags);
		stream.read(bone.procType);
		stream.read(bone.procIndex);
		stream.read(bone.physicsBone);
		readStringAtOffset(stream, bone.surfacePropName, std::ios::cur, sizeof(int) * 12 + sizeof(Vector3) * 4 + sizeof(Quaternion) * 2 + sizeof(Matrix<3, 4>) + sizeof(Bone::Flags));
		stream.read(bone.contents);

		// _unused0
		stream.skip<int>(8);
	}

	stream.seek(boneControllerOffset);
	for (int i = 0; i < boneControllerCount; i++) {
		this->boneControllers.push_back(stream.read<BoneController>());

		// _unused0
		stream.skip<int>(8);
	}

	for (int i = 0; i < hitboxSetCount; i++) {
		auto hitboxSetPos = hitboxSetOffset + i * (sizeof(int) * 3);
		stream.seek(hitboxSetPos);

		auto& hitboxSet = this->hitboxSets.emplace_back();

		readStringAtOffset(stream, hitboxSet.name);
		int hitboxCount = stream.read<int>();
		int hitboxOffset = stream.read<int>();

		for (int j = 0; j < hitboxCount; j++) {
			auto hitboxPos = hitboxOffset + j * (sizeof(int) * 11 + sizeof(Vector3) * 2);
			stream.seek(hitboxSetPos + hitboxPos);

			auto& hitbox = hitboxSet.hitboxes.emplace_back();

			stream.read(hitbox.bone);
			stream.read(hitbox.group);
			stream.read(hitbox.bboxMin);
			stream.read(hitbox.bboxMax);

			// note: we don't know what model versions use absolute vs. relative offsets here
			//       and this is unimportant, so skip parsing the bbox name here
			//readStringAtOffset(stream, hitbox.name, std::ios::cur, sizeof(int) * 3 + sizeof(Vector3) * 2);
			stream.skip<int>();
			hitbox.name = "";

			// _unused0
			stream.skip<int>(8);
		}
	}

	/*
	stream.seek(animDescOffset);
	for (int i = 0; i < animDescCount; i++) {
		// todo(wrapper)
	}

	stream.seek(sequenceDescOffset);
	for (int i = 0; i < sequenceDescCount; i++) {
		// todo(wrapper)
	}
	*/

	stream.seek(materialOffset);
	for (int i = 0; i < materialCount; i++) {
		auto& material = this->materials.emplace_back();

		// Needs to be read from the base of the data structure
		readStringAtOffset(stream, material.name);

		stream.read(material.flags);

		// used
		stream.skip<int>();
		// _unused0
		stream.skip<int>(13);
	}

	stream.seek(materialDirOffset);
	for (int i = 0; i < materialDirCount; i++) {
		auto& materialDir = this->materialDirectories.emplace_back();
		readStringAtOffset(stream, materialDir, std::ios::beg, 0);
	}

    stream.seek(skinReferenceOffset);
    for (int i = 0; i < skinReferenceFamilyCount; i++) {
        std::vector<short> skinFamily;
        for (int j = 0; j < skinReferenceCount; j++) {
            skinFamily.push_back(stream.read<short>());
        }
        this->skins.push_back(std::move(skinFamily));
    }

	for (int i = 0; i < bodyPartCount; i++) {
		auto bodyPartPos = bodyPartOffset + i * (sizeof(int) * 4);
		stream.seek(bodyPartPos);

		auto& bodyPart = this->bodyParts.emplace_back();

		readStringAtOffset(stream, bodyPart.name);

		int modelsCount = stream.read<int>();
		// base
		stream.skip<int>();
		int modelsOffset = stream.read<int>();

		for (int j = 0; j < modelsCount; j++) {
			auto modelPos = modelsOffset + j * (64 + sizeof(float) + sizeof(int) * 20);
			stream.seek(bodyPartPos + modelPos);

			auto& model = bodyPart.models.emplace_back();

			stream.read(model.name, 64);
			stream.read(model.type);
			stream.read(model.boundingRadius);

			int meshesCount = stream.read<int>();
			int meshesOffset = stream.read<int>();

			stream.read(model.verticesCount);
			stream.read(model.verticesOffset);

			for (int k = 0; k < meshesCount; k++) {
				auto meshPos = meshesOffset + k * (sizeof(int) * (18 + MAX_LOD_COUNT) + sizeof(Vector3));
				stream.seek(bodyPartPos + modelPos + meshPos);

				auto& mesh = model.meshes.emplace_back();

				stream.read(mesh.material);
				stream.skip<int>();
				stream.read(mesh.verticesCount);
				stream.read(mesh.verticesOffset);
				stream.skip<int>(2);
				stream.read(mesh.materialType);
				stream.read(mesh.materialParam);
				stream.read(mesh.meshID);
				stream.read(mesh.center);
			}
		}
	}

	return true;
}
