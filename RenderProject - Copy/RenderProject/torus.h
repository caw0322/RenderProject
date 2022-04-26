#include "common/staticMesh3D.h"
namespace static_meshes_3D {

	class StaticMeshIndexed3D : public StaticMesh3D
	{
	public:
		StaticMeshIndexed3D(bool withPositions, bool withTextureCoordinates, bool withNormals);
		virtual ~StaticMeshIndexed3D();


		VertexBufferObject _indicesVBO;

	};
	class Torus : public StaticMesh3D
	{
	public:
		Torus(int mainSegments, int tubeSegments, float mainRadius, float tubeRadius,
			bool withPositions = true, bool withTextureCoordinates = true, bool withNormals = true);

		void render() const override;
		void renderSpecial(int segments) const;

		float getMainRadius() const;
		float getTubeRadius() const;

	private:
		int _mainSegments;
		int _tubeSegments;
		float _mainRadius;
		float _tubeRadius;

		int _numIndices = 0;
		int _primitiveRestartIndex = 0;

		void initializeData() override;
	};
}