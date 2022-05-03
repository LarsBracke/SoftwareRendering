#pragma once
#include "EMath.h"
#include "ERGBColor.h"
#include "EVector.h"

#include "Ray.h"
#include "Material.h"
#include <vector>

enum class Cullmode
{
	back_face,
	front_face,
	no_culling
};

class Object
{
public:
	Object(Elite::FPoint3 origin, Elite::RGBColor color, Cullmode cullmode = Cullmode::no_culling);
	virtual ~Object();
	virtual void Update(float deltaT) = 0;
	virtual bool Hit(const Ray& ray, HitRecord& hitRecord, bool isShadowRay = false) const = 0;
	Elite::RGBColor GetColor() const;
	Material* GetMaterial() const;
	void SetMaterial(Material* pMaterial);
	Cullmode GetCullmode() const;
	virtual Elite::FVector3 GetNormal(Elite::FPoint3 surfacePoint) const = 0;
	void SetTranslation(const Elite::FVector3 translation);
	protected:
	Elite::FPoint3 m_Origin;
	Elite::RGBColor m_Color;
	Material* m_pMaterial;
	Cullmode m_Cullmode;

	Elite::FVector3 m_Translation;
	Elite::FMatrix3 m_Rotation;
};

class SceneGraph final
{
public:
	~SceneGraph();
	static SceneGraph& GetInstance();
	void UpdateObjects(float deltaT);
	void AddObjectToGraph(Object* object);
	std::vector<Object*>& GetObjects();
private:
	SceneGraph();
	std::vector<Object*> m_pObjects;
};

class Plane final : public Object
{
public:
	Plane(Elite::FPoint3 origin, Elite::RGBColor color, Elite::FVector3 normal);
	~Plane() override = default;
	virtual void Update(float deltaT);
	virtual bool Hit(const Ray& ray, HitRecord& hitRecord, bool isShadowRay = false) const;
	Elite::FVector3 GetNormal(Elite::FPoint3 surfacePoint) const override;

private:
	Elite::FVector3 m_Normal;
};

class Sphere final : public Object
{
public:
	Sphere(Elite::FPoint3 origin, Elite::RGBColor color, float radius);
	~Sphere() override = default;
	virtual void Update(float deltaT);
	virtual bool Hit(const Ray& ray, HitRecord& hitRecord, bool isShadowRay = false) const;
	Elite::FVector3 GetNormal(Elite::FPoint3 surfacePoint) const override;
private:
	float m_Radius;
};

class Triangle final : public Object
{
public:
	Triangle(Elite::FPoint3 v0, Elite::FPoint3 v1, Elite::FPoint3 v2,
		Elite::RGBColor color, Cullmode cullmode = Cullmode::no_culling);
	~Triangle() override = default;
	virtual void Update(float deltaT);
	virtual bool Hit(const Ray& ray, HitRecord& hitRecord, bool isShadowRay = false) const;
	Elite::FVector3 GetNormal(Elite::FPoint3 surfacePoint) const override;
private:
	std::vector<Elite::FVector3> m_Vertices;
	std::vector<Elite::FVector3> m_TransformedVertices;
	
	float m_RotationSpeed;
	float m_Angle;
};

class TriangleMesh final : public Object
{
public:
	TriangleMesh(Elite::FPoint3 origin, Elite::RGBColor color, Cullmode cullmode = Cullmode::no_culling);
	~TriangleMesh() override = default;
	virtual void Update(float deltaT);
	virtual bool Hit(const Ray& ray, HitRecord& hitRecord, bool isShadowRay = false) const ;
	Elite::FVector3 GetNormal(Elite::FPoint3 surfacePoint) const override;
	std::vector<Elite::FPoint3>& GetVertexBuffer();
	std::vector<unsigned int>& GetIndexBuffer();
private:
	std::vector<Elite::FPoint3> m_VertexBuffer;
	std::vector<unsigned int> m_IndexBuffer;
	Elite::FVector3 m_CurrentNormal;
};