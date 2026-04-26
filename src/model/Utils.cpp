#include "Utils.h"
#include <fstream>

using namespace std;

string clean(string s)
{
	while (!s.empty() && (s[s.size()-1] == '\r' || s[s.size()-1] == ' '))
		s.erase(s.size()-1);

	while (!s.empty() && (s[0] == ' '))
		s.erase(0,1);

	return s;
}

Type_ stringToType_(const string& str)
{
	if (str == "atk") return Type_::atk_;
	if (str == "def") return Type_::def_;
	if (str == "sup") return Type_::sup_;
	if (str == "int") return Type_::int_;
	if (str == "snp") return Type_::snp_;
	if (str == "mag") return Type_::mag_;
	return Type_::atk_; // valeur par défaut
}

Rarity stringToRarity(const string& str)
{
	if (str == "c") return Rarity::c;
	if (str == "r") return Rarity::r;
	if (str == "sr") return Rarity::sr;
	if (str == "e") return Rarity::e;
	if (str == "l") return Rarity::l;
	if (str == "m") return Rarity::m;
	return Rarity::c; // valeur par défaut
}

string Type_Tostring(const Type_& typ)
{
	if (typ == Type_::atk_) return "atk";
	if (typ == Type_::def_) return "def";
	if (typ == Type_::sup_) return "sup";
	if (typ == Type_::int_) return "int";
	if (typ == Type_::snp_) return "snp";
	if (typ == Type_::mag_) return "mag";
	return "atk"; // valeur par défaut
}

string Type_DisplayString(const Type_& typ)
{
	if (typ == Type_::atk_) return "Attaquant";
	if (typ == Type_::def_) return "Defenseur";
	if (typ == Type_::sup_) return "Support";
	if (typ == Type_::int_) return "Intelligent";
	if (typ == Type_::snp_) return "Sniper";
	if (typ == Type_::mag_) return "Magicien";
	return "Attaquant";
}

string RarityTostring(const Rarity& rar)
{
	if (rar == Rarity::c) return "c";
	if (rar == Rarity::r) return "r";
	if (rar == Rarity::sr) return "sr";
	if (rar == Rarity::e) return "e";
	if (rar == Rarity::l) return "l";
	if (rar == Rarity::m) return "m";
	return "c"; // valeur par défaut
}

string Rarity_DisplayString(const Rarity& rar)
{
	if (rar == Rarity::c)  return "Commun";
	if (rar == Rarity::r)  return "Rare";
	if (rar == Rarity::sr) return "Super Rare";
	if (rar == Rarity::e)  return "Epique";
	if (rar == Rarity::l)  return "Legendaire";
	if (rar == Rarity::m)  return "Mythique";
	return "Commun";
}

TypeC stringToTypeC(const string& s)
{
	if (s == "heal") return TypeC::heal;
	if (s == "proj") return TypeC::proj;
	if (s == "fire") return TypeC::fire;
	if (s == "light") return TypeC::light;
	if (s == "sup") return TypeC::sup;
	if (s == "blade") return TypeC::blade;
	if (s == "punch") return TypeC::punch;
	if (s == "air") return TypeC::air;
	if (s == "ice") return TypeC::ice;
	if (s == "kick") return TypeC::kick;
	if (s == "electricity") return TypeC::electricity;
	if (s == "water") return TypeC::water;
	if (s == "magic") return TypeC::magic;
	if (s == "self_buff") return TypeC::self_buff;
	return TypeC::other;
}

string typeCToString(TypeC& t)
{
	if (t == TypeC::heal) return "heal";
	if (t == TypeC::proj) return "proj";
	if (t == TypeC::fire) return "fire";
	if (t == TypeC::light) return "light";
	if (t == TypeC::sup) return "sup";
	if (t == TypeC::blade) return "blade";
	if (t == TypeC::punch) return "punch";
	if (t == TypeC::air) return "air";
	if (t == TypeC::ice) return "ice";
	if (t == TypeC::kick) return "kick";
	if (t == TypeC::electricity) return "electricity";
	if (t == TypeC::water) return "water";
	if (t == TypeC::magic) return "magic";
	if (t == TypeC::self_buff) return "self_buff";
	return "other";
}

TargetType stringToTargetType(const string& s)
{
	if (s == "Self") return TargetType::Self;
	if (s == "Ally") return TargetType::Ally;
	if (s == "Enemy") return TargetType::Enemy;
	if (s == "AllAllies") return TargetType::AllAllies;
	if (s == "AllEnemies") return TargetType::AllEnemies;
	if (s == "All") return TargetType::All;
	return TargetType::Enemy; // valeur par défaut si inconnu
}

string targetTypeToString(TargetType t)
{
	if (t == TargetType::Self) return "Self";
	if (t == TargetType::Ally) return "Ally";
	if (t == TargetType::Enemy) return "Enemy";
	if (t == TargetType::AllAllies) return "AllAllies";
	if (t == TargetType::AllEnemies) return "AllEnemies";
	if (t == TargetType::All) return "All";
	return "Enemy"; // valeur par défaut si inconnu
}

EffectType stringToEffect(const string& s)
{
	if (s == "Damage") return EffectType::Damage;
	if (s == "Heal") return EffectType::Heal;
	if (s == "Buff") return EffectType::Buff;
	if (s == "Debuff") return EffectType::Debuff;
	if (s == "Resist") return EffectType::Resist;
	if (s == "Stun") return EffectType::Stun;
	if (s == "Swap") return EffectType::Swap;
	if (s == "Bleeding") return EffectType::Bleeding;
	if (s == "Push") return EffectType::Push;
	if (s == "Pull") return EffectType::Pull;
	return EffectType::Damage; // valeur par défaut si inconnu
}

string EffectTostring(EffectType e)
{
	if (e == EffectType::Damage) return "Damage";
	if (e == EffectType::Heal) return "Heal";
	if (e == EffectType::Buff) return "Buff";
	if (e == EffectType::Debuff) return "Debuff";
	if (e == EffectType::Resist) return "Resist";
	if (e == EffectType::Stun) return "Stun";
	if (e == EffectType::Swap) return "Swap";
	if (e == EffectType::Bleeding) return "Bleeding";
	if (e == EffectType::Push) return "Push";
	if (e == EffectType::Pull) return "Pull";
	return "Other"; // valeur par défaut si inconnu
}

int countLines(ifstream& file)
{
    int count = 0;
    string line;

    while (getline(file, line))
        count++;

    file.clear();
    file.seekg(0);

    return count;
}