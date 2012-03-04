#ifndef MeshResource_h__
#define MeshResource_h__

#include "scge\Graphics\Resources\MaterialResource.h"

class MeshResource
{
public:
	MeshResource() {}
	virtual ~MeshResource() {}

	virtual ResourceReference<MaterialResource> getMaterial() const = 0;

private:
	MeshResource(const MeshResource &);
	MeshResource &operator=(const MeshResource &);
};

#endif // MeshResource_h__