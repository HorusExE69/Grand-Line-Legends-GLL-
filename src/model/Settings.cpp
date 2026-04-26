/**
 * @file Settings.cpp
 * @brief Implémentation de Settings.
 */

#include "Settings.h"
#include <cassert>
#include <fstream>

using namespace std;

Settings::Settings()
{
    fps        = 60;
    volume     = 80;
    brightness = 80;
}

void Settings::save(const string& path) const
{
    ofstream f(path);
    if (!f.is_open()) return;
    f << fps << "\n" << volume << "\n" << brightness << "\n";
}

void Settings::load(const string& path)
{
    ifstream f(path);
    if (!f.is_open()) return;
    f >> fps >> volume >> brightness;
}

int Settings::getFps()        const { return fps;        }
int Settings::getVolume()     const { return volume;     }
int Settings::getBrightness() const { return brightness; }

void Settings::setFps(int f)
{
    assert(f == 30 || f == 60);
    fps = f;
}

void Settings::setVolume(int v)
{
    assert(v >= 0 && v <= 100);
    volume = v;
}

void Settings::setBrightness(int b)
{
    assert(b >= 0 && b <= 100);
    brightness = b;
}

void Settings::toggleFps()
{
    fps = (fps == 60) ? 30 : 60;
}

void Settings::volumeUp()
{
    if (volume <= 90) volume += 10;
    else volume = 100;
}

void Settings::volumeDown()
{
    if (volume >= 10) volume -= 10;
    else volume = 0;
}

void Settings::brightnessUp()
{
    if (brightness <= 90) brightness += 10;
    else brightness = 100;
}

void Settings::brightnessDown()
{
    if (brightness >= 10) brightness -= 10;
    else brightness = 0;
}
