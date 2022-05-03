#include "Object.h"
#include <iostream>

Object::Object(Elite::FPoint3 origin, Elite::RGBColor color, Cullmode cullmode)
	: m_Origin{ origin }
	, m_Color{ color }
	, m_pMaterial{ nullptr }
	, m_Cullmode{ cullmode }
	, m_Rotation{ Elite::FMatrix3::Identity() }
	, m_Translation{ Elite::FVector3::ZeroVector() }
{ }

Object::~Object()
{
	delete m_pMaterial;
	m_pMaterial = nullptr;
}

Elite::RGBColor Object::GetColor() const
{
	return m_Color;
}

Material* Object::GetMaterial() const
{
	return m_pMaterial;
}

void Object::SetMaterial(Material* pMaterial)
{
	m_pMaterial = pMaterial;
}

Cullmode Object::GetCullmode() const
{
	return m_Cullmode;
}

void Object::SetTranslation(const Elite::FVector3 translation)
{
	m_Translation = translation;
}

SceneGraph::~SceneGraph()
{
	std::cout << "SceneGraph destructor" << std::endl;
	for (Object* pObject : m_pObjects)
	{
		delete pObject;
		pObject = nullptr;
	}
}

SceneGraph& SceneGraph::GetInstance()
{
	static SceneGraph sceneGraph;
	return sceneGraph;
}

void SceneGraph::UpdateObjects(float deltaT)
{
	for (Object* pObject : m_pObjects)
	{
		pObject->Update(deltaT);
	}
}

void SceneGraph::AddObjectToGraph(Object* object)
{
	m_pObjects.push_back(object);
}

std::vector<Object*>& SceneGraph::GetObjects()
{
	return m_pObjects;
}

SceneGraph::SceneGraph()
{ }

Plane::Plane(Elite::FPoint3 origin, Elite::RGBColor color, Elite::FVector3 normal)
	: Object(origin, color)
	, m_Normal{ normal }
{ }

void Plane::Update(float deltaT)
{
	return;
}

bool Plane::Hit(const Ray& ray, HitRecord& hitRecord, bool isShadowRay) const
{
	// calculate parameter t
	float t;
	t =
	Elite::Dot({ m_Origin - ray.origin }, m_Normal) / Elite::Dot(ray.direction, m_Normal);

	// check t boundaries
	if (t >= ray.tMin && t <= ray.tMax)
	{
		hitRecord.tValue = t;
		hitRecord.hitPoint = ray.origin + t * ray.direction;
		hitRecord.normal = GetNormal(hitRecord.hitPoint);

		// check hit
		if (abs(Elite::Dot({hitRecord.hitPoint - m_Origin}, m_Normal)) <= 0.05f)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}

Elite::FVector3 Plane::GetNormal(Elite::FPoint3 surfacePoint) const
{
	return Elite::GetNormalized(m_Normal);
}

Sphere::Sphere(Elite::FPoint3 origin, Elite::RGBColor color, float radius)
	: Object(origin, color)
	, m_Radius{ radius }
{ }

void Sphere::Update(float deltaT)
{
	return;
}

bool Sphere::Hit(const Ray& ray, HitRecord& hitRecord, bool isShadowRay) const
{

	Elite::FVector3 L{ m_Origin - ray.origin };	// vector to sphere origin
	float tCa{ Elite::Dot(L, ray.direction) };	// project vector on ray direction
	float oD{ Elite::Magnitude(Elite::Reject(L, ray.direction)) };	// calculate perpendicular distance
																	// from sphere origin to ray
	if (oD > m_Radius) return false;	// if oD is bigger than the radius
										// the ray is not going through the sphere
	else
	{
		float tHc{ sqrt(m_Radius - oD) };	// solve triangle with radius
											// to find distance between first
											// intersection point and the projected point
		float t{ tCa - tHc };	// = t0
		if (t > ray.tMin && t < ray.tMax)
		{
			hitRecord.tValue = t;
			hitRecord.hitPoint = ray.origin + t * ray.direction;
			hitRecord.normal = GetNormal(hitRecord.hitPoint);
			return true;
		}
		return false;
		//else if (t < ray.tMin)
		//{
		//	t = tCa + tHc;	// = t1
		//	hitRecord.tValue = t;
		//	hitRecord.hitPoint = ray.origin + t * ray.direction;
		//	return true;
		//}
	}
}

Elite::FVector3 Sphere::GetNormal(Elite::FPoint3 surfacePoint) const
{
	Elite::FVector3 normal{ surfacePoint - m_Origin };
	return Elite::GetNormalized(normal);
}

Triangle::Triangle(Elite::FPoint3 v0, Elite::FPoint3 v1, Elite::FPoint3 v2,
	Elite::RGBColor color, Cullmode cullmode)
	: Object({ 0,0,0 }, color, cullmode)
	, m_RotationSpeed{ 1.f }
	, m_Angle{ 0.f }
{
	m_Vertices.push_back(Elite::FVector3(v0));
	m_Vertices.push_back(Elite::FVector3(v1));
	m_Vertices.push_back(Elite::FVector3(v2));

	m_TransformedVertices = m_Vertices;
}

void Triangle::Update(float deltaT)
{
	m_Angle += m_RotationSpeed * deltaT;
	m_Rotation = Elite::MakeRotationZYX(0.f, m_Angle, 0.f);
	for (int index = 0; index < m_Vertices.size(); ++index)
	{
		m_TransformedVertices[index] = m_Rotation * m_Vertices[index];
		m_TransformedVertices[index] = (m_TransformedVertices[index] + m_Translation);
	}
}

bool Triangle::Hit(const Ray& ray, HitRecord& hitRecord, bool isShadowRay) const
{
	// check if the ray hits the triangle plane
	Elite::FVector3 side0{ m_TransformedVertices[1] - m_TransformedVertices[0] };
	Elite::FVector3 side1{ m_TransformedVertices[2] - m_TransformedVertices[0] };
	Elite::FVector3 side2{ m_TransformedVertices[2] - m_TransformedVertices[1] };
	Elite::Normalize(side0);
	Elite::Normalize(side1);
	Elite::Normalize(side2);

	Elite::FVector3 normal{ Elite::Cross(side0, side1) };
	Elite::Normalize(normal);

	Cullmode cullmode{ m_Cullmode };
	if (isShadowRay && cullmode == Cullmode::front_face) cullmode = Cullmode::back_face;
	else if (isShadowRay && cullmode == Cullmode::back_face) cullmode = Cullmode::front_face;

	if (Elite::Dot(normal, ray.direction) == 0) return false;

	switch (cullmode)
	{
	case Cullmode::back_face:
		if (Elite::Dot(normal, ray.direction) > 0) return false;
		break;
	case Cullmode::front_face:
		if (Elite::Dot(normal, ray.direction) < 0) return false;
		break;
	}

	// calculate intersection point on the plane
	Elite::FPoint3 center{ (m_TransformedVertices[0] + m_TransformedVertices[1] + m_TransformedVertices[2]) / 3.f };
	float t{ Elite::Dot(center - ray.origin, normal) / Elite::Dot(ray.direction, normal) };
	
	if (t < ray.tMin || t > ray.tMax) return false;

	Elite::FPoint3 hitPoint{ ray.origin + t * ray.direction };

	// check if the point is in the triangle
	Elite::FVector3 pointToSide{ hitPoint - m_TransformedVertices[0] };
	Elite::Normalize(pointToSide);
	if (Elite::Dot(normal, Elite::Cross(side0, pointToSide)) < 0) return false;

	pointToSide = Elite::FVector3{ hitPoint - m_TransformedVertices[2] };
	Elite::Normalize(pointToSide);
	if (Elite::Dot(normal, Elite::Cross(side2, pointToSide)) < 0) return false;

	pointToSide = Elite::FVector3{ hitPoint - m_TransformedVertices[2] };
	Elite::Normalize(pointToSide);
	if (Elite::Dot(normal, Elite::Cross(side1, pointToSide)) > 0) return false;

	// fill in the hitrecord and return true
	hitRecord.hitPoint = hitPoint;
	hitRecord.normal = normal;
	hitRecord.tValue = t;
	return true;
}

Elite::FVector3 Triangle::GetNormal(Elite::FPoint3 surfacePoint) const
{
	Elite::FVector3 side0{ m_TransformedVertices[1] - m_TransformedVertices[0] };
	Elite::FVector3 side1{ m_TransformedVertices[2] - m_TransformedVertices[0] };
	Elite::FVector3 normal{ Elite::Cross(side0, side1) };
	return normal;
}

TriangleMesh::TriangleMesh(Elite::FPoint3 origin, Elite::RGBColor color, Cullmode cullmode)
	: Object(origin, color, cullmode)
	, m_CurrentNormal{ 0,0,0 }
{ }

void TriangleMesh::Update(float deltaT)
{
	return;
}

bool TriangleMesh::Hit(const Ray & ray, HitRecord & hitRecord, bool isShadowRay) const
{
	uint64_t currentIndex = 0;
	float t = FLT_MAX;

	while (currentIndex + 2 <= unsigned int(m_IndexBuffer.size()))
	{		
		unsigned int index0 = m_IndexBuffer[currentIndex];
		unsigned int index1 = m_IndexBuffer[currentIndex + 1];
		unsigned int index2 = m_IndexBuffer[currentIndex + 2];

		Cullmode cullmode{ Cullmode::no_culling };
		if (Elite::Dot(m_CurrentNormal, ray.direction) > 0.f) cullmode = Cullmode::no_culling;
		else cullmode = Cullmode::back_face;

		Triangle triangle = { m_VertexBuffer[index0], m_VertexBuffer[index1], m_VertexBuffer[index2], m_Color, cullmode};

		if (triangle.Hit(ray, hitRecord))
		{
			const_cast<Elite::FVector3&>(m_CurrentNormal) = triangle.GetNormal(hitRecord.hitPoint);
			if (hitRecord.tValue < t) t = hitRecord.tValue;
		}
		currentIndex += 3;
	}

	if (t < FLT_MAX) return true;
	return false;
}

Elite::FVector3 TriangleMesh::GetNormal(Elite::FPoint3 surfacePoint) const
{
	return m_CurrentNormal;
}

std::vector<Elite::FPoint3>& TriangleMesh::GetVertexBuffer()
{
	return m_VertexBuffer;
}

std::vector<unsigned int>& TriangleMesh::GetIndexBuffer()
{
	return m_IndexBuffer;
}
