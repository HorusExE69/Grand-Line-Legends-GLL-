/**
 * @file Settings.h
 * @brief Paramètres du jeu (fps, son, luminosité).
 */

#ifndef SETTINGS_H
#define SETTINGS_H

#include <string>
#include <fstream>

/**
 * @class Settings
 * @brief Stocke et persiste les paramètres de jeu.
 *
 * Les valeurs sont sauvegardées/chargées depuis un fichier texte.
 */
class Settings
{
    private:
        int fps;         ///< Cible de fps (30 ou 60)
        int volume;      ///< Volume sonore (0-100)
        int brightness;  ///< Luminosité (0-100)

    public:
        /// @brief Constructeur avec valeurs par défaut
        Settings();

        /// @brief Sauvegarde les paramètres dans un fichier
        void save(const std::string& path) const;
        /// @brief Charge les paramètres depuis un fichier (ne rien faire si absent)
        void load(const std::string& path);

        // Accesseurs
        int getFps()        const;
        int getVolume()     const;
        int getBrightness() const;

        // Mutateurs (avec bornes)
        void setFps(int f);
        void setVolume(int v);
        void setBrightness(int b);

        /// @brief Incrémente / décrémente fps (30 ↔ 60)
        void toggleFps();
        /// @brief Augmente le volume de 10 (max 100)
        void volumeUp();
        /// @brief Diminue le volume de 10 (min 0)
        void volumeDown();
        /// @brief Augmente la luminosité de 10 (max 100)
        void brightnessUp();
        /// @brief Diminue la luminosité de 10 (min 0)
        void brightnessDown();
};

#endif
