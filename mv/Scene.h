#pragma once
#include "setup.h"

#include <set>
#include <vector>

namespace mv
{
	class Scene
	{
	public:
		struct Bucket
		{
			id_type material_id;
			std::vector<id_type> scene_object_ids;
		};

	private:
		struct BucketCompare {
			inline bool operator() (const Bucket& x, const Bucket& y) const
			{ return x.material_id < y.material_id; }
		};

		std::set<Bucket, BucketCompare> _material_buckets;

	public:
		std::set<Bucket, BucketCompare>::const_iterator begin() const;
		std::set<Bucket, BucketCompare>::const_iterator end() const;
	};
}
