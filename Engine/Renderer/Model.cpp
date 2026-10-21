#include "pch.h"
#include "Model.h"
#include "Renderer.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace nu
{
	bool Model::Load(const std::string& filename, const Renderer& renderer)
	{
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(filename,
			aiProcess_Triangulate |
			aiProcess_GenSmoothNormals |
			aiProcess_CalcTangentSpace);

		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		{
			std::cerr << "Could not load assimp file " << filename << ": " << importer.GetErrorString() << std::endl;

			return false;
		}

		// todo: start processing the scene from the root node with ProcessNode(node, scene, renderer)
		// the root node is scene->mRootNode

		return true;
	}

	void Model::Draw(Renderer& renderer) const
	{
		for (const auto& mesh : m_meshes)
		{
			// todo: set the mesh vertex buffer on the renderer with renderer.SetVertexBuffer(vertexBuffer)
			// the buffers are stored as unique_ptr, so dereference them with *

			// todo: set the mesh index buffer on the renderer with renderer.SetIndexBuffer(indexBuffer)

			// todo: draw the mesh with renderer.DrawIndex(indexBuffer)
		}
	}

	void Model::ProcessNode(aiNode* node, const aiScene* scene, const Renderer& renderer)
	{
		// process the current node meshes
		// a node only stores mesh indices, the actual meshes are stored in scene->mMeshes
		for (unsigned int i = 0; i < node->mNumMeshes; i++)
		{
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];

			// todo: process the mesh with ProcessMesh(mesh, scene, renderer)
		}

		// process the current node children
		for (unsigned int i = 0; i < node->mNumChildren; i++)
		{
			// todo: process the child node with ProcessNode(child, scene, renderer)
			// the children are stored in node->mChildren, this is recursive so every node in the tree gets processed
		}
	}

	void Model::ProcessMesh(aiMesh* mesh, const aiScene* scene, const Renderer& renderer)
	{
		// get model vertex attributes
		std::vector<Vertex> vertices;
		vertices.reserve(mesh->mNumVertices);

		for (unsigned int i = 0; i < mesh->mNumVertices; i++)
		{
			Vertex vertex;

			// todo: set vertex.position from mesh->mVertices[i] (x, y, z)
			// a mesh always has positions, so no null check is needed

			// texcoords are optional, use (0, 0) if the mesh doesn't have them
			vertex.texcoord = (mesh->mTextureCoords[0])
				? glm::vec2{ mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y }
				: glm::vec2{ 0, 0 };

			// todo: set vertex.normal from mesh->mNormals[i] (x, y, z)
			// normals are optional, use (0, 0, 0) if mesh->mNormals is null (see texcoord above)

			// todo: set vertex.tangent from mesh->mTangents[i] (x, y, z)
			// tangents are optional, use (0, 0, 0) if mesh->mTangents is null (see texcoord above)

			vertices.push_back(vertex);
		}

		// get model indices
		// each face is a triangle (aiProcess_Triangulate), each face stores the indices of its vertices
		std::vector<uint32_t> indices;
		indices.reserve(mesh->mNumFaces * 3);

		for (unsigned int i = 0; i < mesh->mNumFaces; i++)
		{
			const aiFace& face = mesh->mFaces[i];
			for (unsigned int j = 0; j < face.mNumIndices; j++)
			{
				// todo: add the face index face.mIndices[j] to indices
			}
		}

		// create the vertex buffer (uploads to the gpu)
		Mesh newMesh;
		newMesh.vertexBuffer = std::make_unique<VertexBuffer>();
		if (!newMesh.vertexBuffer->Create(vertices, renderer))
		{
			std::cerr << "Could not create vertex buffer for mesh " << mesh->mName.C_Str() << std::endl;

			return;
		}

		// create the index buffer (uploads to the gpu)
		newMesh.indexBuffer = std::make_unique<IndexBuffer>();
		if (!newMesh.indexBuffer->Create(indices, renderer))
		{
			std::cerr << "Could not create index buffer for mesh " << mesh->mName.C_Str() << std::endl;

			return;
		}

		// todo: add newMesh to m_meshes
		// Mesh holds unique_ptrs so it can't be copied, use std::move(newMesh)
	}
}
