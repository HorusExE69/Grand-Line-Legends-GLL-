#ifndef UTILS_H
#define UTILS_H

#include <string>
using namespace std;

enum class Rarity
{
	c,
	r,
	sr,
	e,
	l,
	m
};

enum class Type_
{
	snp_,
	int_,
	mag_,
	atk_,
	def_,
	sup_
};

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

enum class TypeC
{
	heal,
	proj,
	fire,
	light,
	sup,
	blade,
	punch,
	air,
	ice,
	kick,
	electricity,
	water,
	magic,
	self_buff,
	other
};


enum class EventType
{
	PLAY,
	BATTLE,
	SHOP,
	QUIT,
	NONE
};

struct TupleTC
{
	TypeC t1;
	TypeC t2;
};

Type_ stringToType_(const std::string& str);
Rarity stringToRarity(const std::string& str);
std::string Type_Tostring(const Type_& typ);
std::string RarityTostring(const Rarity& rar);

TypeC stringToTypeC(const std::string& s);
std::string typeCToString(TypeC& t);

TargetType stringToTargetType(const std::string& s);
std::string targetTypeToString(TargetType t);

EffectType stringToEffect(const std::string& s);
std::string EffectTostring(EffectType e);

std::string clean(string s);

int countLines(std::ifstream& file);


#endif