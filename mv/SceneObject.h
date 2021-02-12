#pragma once
#include "setup.h"
#include "Matrix.h"
#include "Quaternion.h"

namespace mv
{
	class SceneObject
	{
		friend class SceneManager;

	private:
		id_type _scene_id;
		id_type _material_id; // changing this will also need to update the object's bucket, let SceneManager set this value
		mat4f _model_transform;

	public:
		// update position only, scale and rotation remain the same
		void set_position(const vec3f& position);
		// update position, rotation and scale
		void set_transform(const vec3f& position, const quatf& rotation, const vec3f& scale);
		// update position and rotation only, scale is reset
		void set_transform(const vec3f& position, const quatf& rotation);
		// update position and scale only, rotation is reset
		void set_transform(const vec3f& position, const vec3f& scale);
		// update position only, scale and rotation are reset
		void set_transform(const vec3f& position);

		const mat4f& get_transform() const;
	};
}
