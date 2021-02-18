#pragma once
#include "setup.h"

#include <set>
#include <vector>

#include "Matrix.h"


namespace mv
{
	class SceneObject;

	class Scene
	{
		friend class SceneManager;

	public:
		struct Bucket
		{
			id_type material_id;
			mutable std::vector<id_type> scene_object_ids;

			bool operator<(const Bucket& rhs) const;
			bool operator<(id_type rhs) const;
		};

		using bucket_list_type = std::set<Bucket, std::less<>>;

	private:
		bucket_list_type _material_buckets;
		mat4f _view;
		mat4f _proj;


		Scene();

		void add(const SceneObject& scene_object);
		void remove(const SceneObject& scene_object);

	public:
		bucket_list_type::const_iterator begin() const;
		bucket_list_type::const_iterator end() const;

		void set_proj_perspective(float view_angle, float aspect_ratio, float near, float far);
		void set_proj_ortho(float width, float height, float near, float far);

		const mat4f& view() const;
		const mat4f& proj() const;
	};

	bool operator<(id_type lhs, const Scene::Bucket& rhs);
}
