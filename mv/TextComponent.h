#pragma once
#include "Component.h"
#include "Event.h"


namespace mv
{
	class TextComponent : public Component
	{
	public:
		using base = Component;

	private:
		Event<std::string> _text_update_event;
		id_type _mesh_id;

	public:
		TextComponent(id_type font_id);
		TextComponent(const std::string& font_name);

		void init();

		void set_text(const std::string& text);
	};
}

MV_REGISTER_COMPONENT(mv::TextComponent)
