/**
 * @file Campaign.h
 * @brief Mode campagne contenant les 17 arcs de One Piece.
 */

#ifndef CAMPAIGN_H
#define CAMPAIGN_H

#include "Arc.h"

/// @brief Nombre d'arcs dans la campagne
static const int NB_ARCS = 18;

/**
 * @class Campaign
 * @brief Gère la progression dans les arcs et épisodes de la campagne.
 *
 * Les arcs sont définis statiquement selon le cahier des charges.
 * La progression (arc courant, épisode courant) est sauvegardée avec le Player.
 */
class Campaign
{
    private:
        Arc** arcs;         ///< Tableau des NB_ARCS arcs
        int   currentArc;   ///< Index de l'arc sélectionné (navigation)
        int   currentEp;    ///< Index de l'épisode sélectionné (navigation)

        /// @brief Construit et remplit les 17 arcs avec leurs épisodes et récompenses
        void buildArcs();

    public:
        Campaign();
        ~Campaign();

        // Navigation
        void selectArc(int idx);
        void selectEpisode(int idx);
        void arcUp();
        void arcDown();
        void episodeUp();
        void episodeDown();

        // Accesseurs
        Arc* getArc(int idx)                 const;
        Arc* getCurrentArc()                 const;
        int  getCurrentArcIdx()              const;
        int  getCurrentEpIdx()               const;
        int  getNbArcs()                     const;

        Episode* getCurrentEpisode()         const;

        /// @brief Marque l'épisode courant comme complété
        void completeCurrentEpisode();
        /// @brief Marque l'arc courant comme complété si tous ses épisodes le sont
        void checkArcCompletion();
};

#endif
