#pragma once
#include "setup.h"
#include "IDList.h"
#include "Scene.h"
#include "SceneObject.h"

namespace mv
{
	class SceneManager final
	{
		friend class Multiverse;

	private:
		static SceneManager* _instance;

		IDList<Scene> _scenes;
		IDList<SceneObject> _scene_objects;
		std::vector<id_type> _active_scene_ids;

		SceneManager();
		SceneManager(const SceneManager&) = delete;

		~SceneManager();

		SceneManager& operator=(const SceneManager&) = delete;

	public:
		static SceneManager& instance();

		id_type create_scene();
		void destroy_scene(id_type id);
		id_type create_scene_object(id_type scene_id, id_type material_id, id_type mesh_id);
		void destroy_scene_object(id_type id);

		void transfer_scene_object(id_type object_id, id_type destination_scene_id);
		void set_scene_object_material(id_type object_id, id_type material_id);

		Scene& scene(id_type id);
		SceneObject& scene_object(id_type id);

		const std::vector<id_type>& active_scene_ids() const;
	};
}
