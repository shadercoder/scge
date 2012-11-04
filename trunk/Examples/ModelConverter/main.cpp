#include <iostream>
#include <vector>
#include <fstream>
#include <filesystem>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <scge/Config.h>

inline bool isNaN(float f)
{
	return f != f;
}

bool isNaN(const aiVector3D &vector)
{
	return isNaN(vector.x) || isNaN(vector.y) || isNaN(vector.z);
}

bool isValidMesh(const aiMesh &mesh, bool showError = false, unsigned int errorInt = 0)
{
	if(!mesh.HasNormals() || !mesh.mNormals)
	{
		if(showError)
			std::cout << "Mesh " << errorInt << " does not have normals" << std::endl;
		return false;
	}
	if(!mesh.HasTangentsAndBitangents() || !mesh.mBitangents || !mesh.mTangents)
	{
		if(showError)
			std::cout << "Mesh " << errorInt << " does not have Tangents or BiNormals" << std::endl;
		return false;
	}
	if(mesh.GetNumUVChannels() < 1)
	{
		if(showError)
			std::cout << "Mesh " << errorInt << " does not have UVs" << std::endl;
		return false;
	}

	for(unsigned int vertexID = 0; vertexID < mesh.mNumVertices; ++vertexID)
	{
		if(isNaN(mesh.mVertices[vertexID]))
		{
			if(showError)
				std::cout << "Mesh " << errorInt << " has erroneous float in its vertex" << std::endl;
			return false;
		}
		if(isNaN(mesh.mTextureCoords[0][vertexID]))
		{
			if(showError)
				std::cout << "Mesh " << errorInt << " has erroneous float in its UVs" << std::endl;
			return false;
		}
		if(isNaN(mesh.mNormals[vertexID]))
		{
			if(showError)
				std::cout << "Mesh " << errorInt << " has erroneous float in its normals" << std::endl;
			return false;
		}
		if(isNaN(mesh.mBitangents[vertexID]))
		{
			if(showError)
				std::cout << "Mesh " << errorInt << " has erroneous float in its binormals" << std::endl;
			return false;
		}
		if(isNaN(mesh.mTangents[vertexID]))
		{
			if(showError)
				std::cout << "Mesh " << errorInt << " has erroneous float in its normals" << std::endl;
			return false;
		}
	}

	return true;
}

template <typename T>
inline int getValue(int minValue, int maxValue, T displayMessage)
{
	int inputValue = std::numeric_limits<int>::max();
	for(;;)
	{
		displayMessage();

		std::cin >> inputValue;
		std::cin.ignore();
		std::cout << "" << std::endl;

		if(inputValue >= minValue || inputValue <= maxValue)
			break;
	}

	return inputValue;
}

template <typename T>
inline void write(std::ofstream &file, const T &type)
{
	file.write((const char *)&type, sizeof(T));
}

bool RunOnModel(const aiScene *model, const std::string &outputFileName)
{
	std::vector<std::string> materialLibrary(model->mNumMaterials);
	if(model->HasMaterials())
	{
		bool ignoreRestOfMaterials = false;
		for(size_t i = 0; i < model->mNumMaterials; ++i)
		{
			const aiMaterial &material = *model->mMaterials[i];

			if(ignoreRestOfMaterials)
			{
				materialLibrary[i] = "skipme";
				continue;
			}

			std::cout << "Please input the material resource to use for material id : " << i << std::endl;
			if(material.mNumProperties > 0)
			{
				std::cout << "Properties:" << std::endl;
				for(size_t i = 0; i < material.mNumProperties; ++i)
				{
					std::string matProperty(material.mProperties[i]->mData, material.mProperties[i]->mDataLength);
					std::cout << matProperty << std::endl;
				}
			}
			std::cout << std::endl;

			int type = getValue(1, 4, []
			{
				std::cout << "Type 1 for a Material" << std::endl;
				std::cout << "Type 2 for an Alias" << std::endl;
				std::cout << "Type 3 to ignore material and meshes that use it" << std::endl;
				std::cout << "Type 4 to ignore material and meshes that use it, and all further materials and meshes" << std::endl;
			});

			if(type == 3 || type == 4)
			{
				if(type == 4) ignoreRestOfMaterials = true;

				materialLibrary[i] = "skipme";
				continue;
			}

			std::cout << "Enter the name for this " << (type == 1 ? "Material" : "Alias") << std::endl;

			std::string materialName;
			std::cin >> materialName;

			if(type == 1) materialName = "[Material] " + materialName + ".smtf";
			else if(type == 2) materialName = "[Alias] " + materialName;

			materialLibrary[i] = materialName;
		}
	}

	int ignoreNoMeshes = getValue(1, 2, []
	{
		std::cout << "Would you like to ignore any meshes?" << std::endl;
		std::cout << "Type 1 to choose, 2 to use all" << std::endl;
	});

	std::vector<bool> meshesToSkip(model->mNumMeshes);
	uint32 meshCount = 0;
	for(unsigned int meshID = 0; meshID < model->mNumMeshes; ++meshID)
	{
		if(model->HasMaterials() && materialLibrary[model->mMeshes[meshID]->mMaterialIndex] == "skipme")
		{
			meshesToSkip[meshID] = true;
			continue;
		}

		if(!isValidMesh(*model->mMeshes[meshID], true, meshID))
		{
			meshesToSkip[meshID] = true;
			continue;
		}

		if(ignoreNoMeshes == 1)
		{
			int toSkip = getValue(1, 2, [=]
			{
				std::cout << "Would you like to ignore mesh " << meshID << std::endl;
				if(model->HasMaterials())
					std::cout << L"With Material : " << materialLibrary[model->mMeshes[meshID]->mMaterialIndex] << std::endl;
				std::cout << "Type 1 to Skip, 2 to use" << std::endl;
			});

			if(toSkip == 1)
			{
				meshesToSkip[meshID] = true;
				continue;
			}
		}

		++meshCount;
	}

	if(meshCount == 0)
	{
		std::cout << "There are no meshes to use!" << std::endl;
		return false;
	}

	std::ofstream modelOutput(outputFileName, std::ios::binary);
	if(!modelOutput)
	{
		std::cout << "Could not open output file for writing!" << std::endl;
		return true;
	}

	uint32 fileFormat = 1;
	write(modelOutput, fileFormat);
	write(modelOutput, meshCount);

	for(unsigned int meshID = 0; meshID < model->mNumMeshes; ++meshID)
	{
		const aiMesh &mesh = *model->mMeshes[meshID];

		if(meshesToSkip[meshID]) continue;

		std::string materialName;
		if(model->HasMaterials())
			materialName = materialLibrary[mesh.mMaterialIndex];

		uint32 stringSize = materialName.length();
		write(modelOutput, stringSize);
		for(size_t curChar = 0; curChar < materialName.length(); ++curChar)
			write(modelOutput, materialName[curChar]);

		uint32 numVerticies = mesh.mNumVertices;
		write(modelOutput, numVerticies);

		for(unsigned int vertexID = 0; vertexID < mesh.mNumVertices; ++vertexID)
		{
			const aiVector3D &vertex = mesh.mVertices[vertexID];
			write(modelOutput, vertex.x);
			write(modelOutput, vertex.y);
			write(modelOutput, vertex.z);

			const aiVector3D &coord = mesh.mTextureCoords[0][vertexID];
			write(modelOutput, coord.x);
			write(modelOutput, coord.y);

			const aiVector3D &normal = mesh.mNormals[vertexID];
			write(modelOutput, normal.x);
			write(modelOutput, normal.y);
			write(modelOutput, normal.z);

			const aiVector3D &binormal = mesh.mBitangents[vertexID];
			write(modelOutput, binormal.x);
			write(modelOutput, binormal.y);
			write(modelOutput, binormal.z);

			const aiVector3D &tangent = mesh.mTangents[vertexID];
			write(modelOutput, tangent.x);
			write(modelOutput, tangent.y);
			write(modelOutput, tangent.z);
		}

		uint32 numIndices = 0;
		for(unsigned int faceID = 0; faceID < mesh.mNumFaces; ++faceID)
		{
			if(mesh.mFaces[faceID].mNumIndices != 3) continue;

			numIndices += 3;
		}

		write(modelOutput, numIndices);
		for(unsigned int faceID = 0; faceID < mesh.mNumFaces; ++faceID)
		{
			const aiFace &face = mesh.mFaces[faceID];
			if(face.mNumIndices != 3)
				continue;

			write(modelOutput, uint32(face.mIndices[0]));
			write(modelOutput, uint32(face.mIndices[1]));
			write(modelOutput, uint32(face.mIndices[2]));
		}
	}

	return false;
}

int main(int argc, char *argv[])
{
	std::vector<std::string> arguments(argv + 1, argv + argc);
	if(arguments.empty())
	{
		std::cout << "There are no arguments!" << std::endl;
		return 0;
	}

	std::string outputFileName = std::tr2::sys::path(arguments[0]).replace_extension(".smdf").string();

	Assimp::Importer importer;

	auto model = importer.ReadFile(arguments[0],
		aiProcess_CalcTangentSpace |
		//aiProcess_JoinIdenticalVertices |
		aiProcess_MakeLeftHanded |
		aiProcess_Triangulate |
		//aiProcess_GenNormals |
		//aiProcess_GenSmoothNormals |
		//aiProcess_SplitLargeMeshes |
		//aiProcess_PreTransformVertices |
		//aiProcess_ImproveCacheLocality |
		aiProcess_RemoveRedundantMaterials |
		//aiProcess_SortByPType |
		//aiProcess_FindDegenerates |
		aiProcess_FindInvalidData |
		aiProcess_GenUVCoords |
		aiProcess_TransformUVCoords |
		//aiProcess_OptimizeMeshes |
		//aiProcess_OptimizeGraph |
		aiProcess_FlipUVs |
		aiProcess_FlipWindingOrder);

	if(!model)
	{
		std::cout << "Failed to open model!" << std::endl;
		std::cout << importer.GetErrorString() << std::endl;
		return 0;
	}

	if(model->mNumMeshes == 0)
	{
		std::cout << "There are no meshes!" << std::endl;
		return 0;
	}

	for(;;)
	{
		std::cout << "Current File Name is " << outputFileName << std::endl;
		
		if(RunOnModel(model, outputFileName))
			break;

		std::cout << "Would you like to run again on the model?" << std::endl;
		std::cout << "1 for yes, 2 for no" << std::endl;

		int moreModel;
		std::cin >> moreModel;
		std::cin.ignore();

		if(moreModel == 2)
			break;
	}
	
	return 0;
}