#include "Effects.h"

Effect::Effect(EffectType t, TargetType tgt, int val, int dur, Character* c1, Character* c2)
{
	type = t;
	target = tgt;
	value = val;
	duration = dur;
	extraTarget1 = c1;
	extraTarget2 = c2;
}

Effect::~Effect() {}

EffectType Effect::getType() const { return type; }
TargetType Effect::getTarget() const { return target; }
int Effect::getValue() const { return value; }
int Effect::getDuration() const { return duration; }
Character* Effect::getExtraTarget1() const { return extraTarget1; }
Character* Effect::getExtraTarget2() const { return extraTarget2; }

void Effect::setValue(int val) { value = val; }
void Effect::setDuration(int dur) { duration = dur; }
void Effect::setExtraTarget1(Character* c) { extraTarget1 = c; }
void Effect::setExtraTarget2(Character* c) { extraTarget2 = c; }