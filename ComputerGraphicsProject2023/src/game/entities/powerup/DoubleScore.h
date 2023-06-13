#pragma once

#include "vulture/core/Core.h"
#include "game/entities/Factory.h"
#include "game/terrain/Terrain.h"
#include "PowerUp.h"

namespace game {

	using namespace vulture;

	class DoubleScoreData : public PowerUpData
	{
	public:
		virtual PowerUpType getType() const;

		virtual f32 getDuration() const;

		virtual ~DoubleScoreData() = default;
	private:
		f32 m_Duration = 20.0f;
	};

	class DoubleScore
	{
	public:
		static const String s_ModelName;
		static const String s_TextureName;

		Ref<GameObject> m_GameObject;

		explicit DoubleScore(Ref<GameObject> gameObject);

		void setup(Ref<Terrain> terrain);

		EntityStatus update(f32 dt);

		~DoubleScore();
	private:
		Ref<Terrain> m_Terrain = nullptr;
		Ref<HitBox> m_Hitbox;
		EntityStatus m_Status = EntityStatus::ALIVE;
		DoubleScoreData m_Data;

		f32 m_DeltaHeight = 0;
		f32 m_DeltaAngle = 0;
	};

} // namespace game
