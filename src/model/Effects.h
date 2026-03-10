#ifndef EFFECT_H
#define EFFECT_H

#include <string>

enum class EffectType
{
	Damage,
	Heal,
	Buff,
	Debuff,
	Resist,
	Stun,
	Swap,
	Bleeding,
	Push,
	Pull,
	Other
};

enum class TargetType
{
	Self,
	Ally,
	Enemy,
	AllAllies,
	AllEnemies,
	All
};

class Character;

class Effect
{
	private:
		EffectType type;
		TargetType target;
		int value;
		int duration;

		Character* extraTarget1;
		Character* extraTarget2;

	public:
		Effect(EffectType t=EffectType::Damage, TargetType tgt=TargetType::Enemy, int val=0, int dur=0, Character* c1=nullptr, Character* c2=nullptr);
		~Effect();

		EffectType getType() const;
		TargetType getTarget() const;
		int getValue() const;
		int getDuration() const;
		Character* getExtraTarget1() const;
		Character* getExtraTarget2() const;

		void setValue(int val);
		void setDuration(int dur);
		void setExtraTarget1(Character* c);
		void setExtraTarget2(Character* c);
};
#endif