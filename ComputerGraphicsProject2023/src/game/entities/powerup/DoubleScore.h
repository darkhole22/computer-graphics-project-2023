#pragma once

#include "vulture/core/Core.h"
#include "game/entities/Factory.h"
#include "game/terrain/Terrain.h"
#include "PowerUp.h"

namespace game {

	using namespace vulture;

	class DoubleExpData : public PowerUpData
	{
	public:
		virtual PowerUpType getType() const;

		virtual f32 getDuration() const;

		virtual ~DoubleExpData() = default;
	private:
		f32 m_Duration = 20.0f;
	};

	class DoubleExp
	{
	public:
		static const String s_ModelName;
		static const String s_TextureName;

		Ref<GameObject> m_GameObject;

		explicit DoubleExp(Ref<GameObject> gameObject);

		void setup(Ref<Terrain> terrain);

		EntityStatus update(f32 dt);

		~DoubleExp();
	private:
		Ref<Terrain> m_Terrain = nullptr;
		Ref<HitBox> m_Hitbox;
		EntityStatus m_Status = EntityStatus::ALIVE;
		DoubleExpData m_Data;

		f32 m_DeltaHeight = 0;
		f32 m_DeltaAngle = 0;
	};

} // namespace game
