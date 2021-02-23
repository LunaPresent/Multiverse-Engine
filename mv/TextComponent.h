#pragma once
#include "Component.h"
#include "Event.h"


namespace mv
{
	class TextComponent : public Component
	{
		MV_COMPONENT_HEADER(Component)

	private:
		Event<std::string> _text_update_event;
		id_type _mesh_id;

	protected:
		TextComponent(id_type font_id);
		TextComponent(const std::string& font_name);

	private:
		void on_create() override;

	public:
		void set_text(const std::string& text);
	};
}

MV_REGISTER_COMPONENT(mv::TextComponent)
