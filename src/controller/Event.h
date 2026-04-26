/**
 * @file Event.h
 * @brief Événement transmis entre la vue et le contrôleur.
 */

#ifndef EVENT_H
#define EVENT_H

#include "../model/Utils.h"

/**
 * @class Event
 * @brief Encapsule un EventType émis par la vue à destination du contrôleur.
 */
class Event
{
    public:
        EventType type;   ///< Type de l'événement

        Event() : type(EventType::NONE) {}
        explicit Event(EventType t) : type(t) {}
};

#endif
