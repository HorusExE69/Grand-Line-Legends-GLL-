/**
 * @file Effects.h
 * @brief Effet appliqué par une capacité sur un personnage.
 */

#ifndef EFFECT_H
#define EFFECT_H

#include <string>
#include "Utils.h"

class Character;

/**
 * @class Effect
 * @brief Représente un effet de combat (dégâts, soin, buff, debuff, étourdissement…).
 *
 * Un effet est créé par une Capacity et appliqué à un Character via applyEffect().
 * Les cibles supplémentaires (extraTarget1, extraTarget2) sont utilisées
 * pour les effets Swap, Push et Pull.
 */
class Effect
{
    private:
        EffectType  type;          ///< Catégorie de l'effet
        TargetType  target;        ///< Portée de la cible
        int         value;         ///< Valeur numérique (dégâts, soin, durée…)
        int         duration;      ///< Durée en tours (0 = instantané)

        Character* extraTarget1;   ///< Première cible supplémentaire (Swap/Push/Pull)
        Character* extraTarget2;   ///< Deuxième cible supplémentaire (Swap)

    public:
        /**
         * @brief Constructeur
         * @param t    Type de l'effet
         * @param tgt  Portée de la cible
         * @param val  Valeur numérique
         * @param dur  Durée en tours
         * @param c1   Première cible supplémentaire (optionnel)
         * @param c2   Deuxième cible supplémentaire (optionnel)
         */
        Effect(EffectType t = EffectType::Damage, TargetType tgt = TargetType::Enemy,
               int val = 0, int dur = 0,
               Character* c1 = nullptr, Character* c2 = nullptr);
        /// @brief Destructeur
        ~Effect();

        /// @brief Retourne le type de l'effet
        EffectType getType()         const;
        /// @brief Retourne la portée cible
        TargetType getTarget()       const;
        /// @brief Retourne la valeur numérique
        int        getValue()        const;
        /// @brief Retourne la durée en tours
        int        getDuration()     const;
        /// @brief Retourne la première cible supplémentaire
        Character* getExtraTarget1() const;
        /// @brief Retourne la deuxième cible supplémentaire
        Character* getExtraTarget2() const;

        /// @brief Modifie la valeur numérique (utilisé pour appliquer les multiplicateurs)
        void setValue(int val);
        /// @brief Modifie la durée
        void setDuration(int dur);
        /// @brief Modifie la première cible supplémentaire
        void setExtraTarget1(Character* c);
        /// @brief Modifie la deuxième cible supplémentaire
        void setExtraTarget2(Character* c);
};

#endif
