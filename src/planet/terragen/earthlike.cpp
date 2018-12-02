#include "planet/terragen/earthlike.h"
#include <cmath>
#include "utility/utility.h"
#include "utility/units.h"

namespace earthlike {
    const Vec3F GROUND = Vec3F(0.15, 0.35, 0.08) * 0.2;
    const Vec3F WATER = Vec3F(0.06, 0.08, 0.35) * 0.2;
    const Vec3F SNOW = Vec3F(0.5, 0.5, 0.8);
    const Vec3F ICE = Vec3F(0.4, 0.5, 0.8);

    const Vec3F SAND = "#EAEB7C"_col;

    const Vec3F height_palette[] = {
        "#FFFFFF"_col,
        "#FFFEFF"_col,
        "#FEFDFE"_col,
        "#F2F4EB"_col,
        "#DDE4CB"_col,
        "#C6D2A8"_col,
        "#AfC184"_col,
        "#97Af5f"_col,
        "#7f9C3B"_col,
        "#698A17"_col,
        "#658609"_col,
        "#779319"_col,
    };

    constexpr const double palette_size = sizeof(height_palette) / sizeof(height_palette[0]);
    constexpr const double TERRAIN_MAX_HEIGHT = 4.0_km;

    PointGenerator::PointGenerator(size_t seed) {
        auto& mountain_terrain_base = this->add_module<noise::module::RidgedMulti>();
        mountain_terrain_base.SetSeed(seed);
        mountain_terrain_base.SetOctaveCount(8);
        mountain_terrain_base.SetFrequency(200);

        auto& mountain_terrain = this->add_module<noise::module::ScaleBias>();
        mountain_terrain.SetSourceModule(0, mountain_terrain_base);
        mountain_terrain.SetScale(1.0);
        mountain_terrain.SetBias(1.0);

        auto& sea_bed = this->add_module<noise::module::Const>();
        sea_bed.SetConstValue(-1);

        auto& elevation_base = this->add_module<noise::module::Perlin>();
        elevation_base.SetSeed(++seed);
        elevation_base.SetFrequency(1);
        elevation_base.SetPersistence(0.5);

        auto& elevation = this->add_module<noise::module::ScaleBias>();
        elevation.SetSourceModule(0, elevation_base);
        elevation.SetScale(1);
        elevation.SetBias(-0.3);

        auto& mountains_multiplied = this->add_module<noise::module::Multiply>();
        mountains_multiplied.SetSourceModule(0, mountain_terrain);
        mountains_multiplied.SetSourceModule(1, elevation);

        auto& terrain_selector = this->add_module<noise::module::Select>();
        terrain_selector.SetSourceModule(0, sea_bed);
        terrain_selector.SetSourceModule(1, mountains_multiplied);
        terrain_selector.SetControlModule(elevation);
        terrain_selector.SetBounds(0.0, 1000.0);
        terrain_selector.SetEdgeFalloff(0.125);

        auto& waves = this->add_module<noise::module::Billow>();
        waves.SetSeed(++seed);
        waves.SetOctaveCount(8);
        waves.SetFrequency(400);
        waves.SetLacunarity(2.0);

        auto& solar_energy = this->add_module<noise::module::Function>();
        solar_energy.SetFunction([](double, double y, double) {
            return std::sqrt(1.0 - y * y) - 0.5;
        });

        auto& temperature_global_noise_base = this->add_module<noise::module::Perlin>();
        temperature_global_noise_base.SetPersistence(0.4);
        temperature_global_noise_base.SetFrequency(1);
        temperature_global_noise_base.SetOctaveCount(1);

        auto& temperature_global_noise = this->add_module<noise::module::ScaleBias>();
        temperature_global_noise.SetSourceModule(0, temperature_global_noise_base);
        temperature_global_noise.SetScale(0.2);
        temperature_global_noise.SetBias(0.0);

        auto& temperature_base = this->add_module<noise::module::Add>();
        temperature_base.SetSourceModule(0, temperature_global_noise);
        temperature_base.SetSourceModule(1, solar_energy);

        auto& terrain_temperature = this->add_module<noise::module::ScaleBias>();
        terrain_temperature.SetSourceModule(0, terrain_selector);
        terrain_temperature.SetScale(-0.1);
        terrain_temperature.SetBias(0.07);

        auto& temperature0 = this->add_module<noise::module::Add>();
        temperature0.SetSourceModule(0, temperature_base);
        temperature0.SetSourceModule(1, terrain_temperature);

        auto& local_temperature_bias = this->add_module<noise::module::Perlin>();
        local_temperature_bias.SetPersistence(0.4);
        local_temperature_bias.SetFrequency(5000);
        local_temperature_bias.SetOctaveCount(1);

        auto& local_temperature = this->add_module<noise::module::ScaleBias>();
        local_temperature.SetSourceModule(0, local_temperature_bias);
        local_temperature.SetScale(0.01);
        local_temperature.SetBias(0.0);

        auto& temperature = this->add_module<noise::module::Add>();
        temperature.SetSourceModule(0, temperature0);
        temperature.SetSourceModule(1, local_temperature);

        this->terrain = &terrain_selector;
        this->temperature = &temperature;
        this->waves = &waves;
    }

    PointProperties PointGenerator::operator()(const Vec3D& pos) {
        return {
            pos,
            this->terrain->GetValue(pos.x, pos.y, pos.z) * 3'000,
            this->waves->GetValue(pos.x, pos.y, pos.z) * 10,
            this->temperature->GetValue(pos.x, pos.y, pos.z)
        };
    }

    Vec3F TriangleGenerator::operator()(const TriangleProperties& prop, bool submerged) {
        auto avg_temp = (prop.a.temperature + prop.b.temperature + prop.c.temperature) / 3.0;

        if (submerged) {
            return avg_temp < 0 ? ICE : WATER;
        } else if (avg_temp < 0) {
            return SNOW;
        }

        auto clamp = [](auto x, auto min_val, auto max_val) {
            return std::min(std::max(x, min_val), max_val);
        };

        double height = std::min({prop.a.land_height, prop.b.land_height, prop.c.land_height});
        double scale_height = 1.0 - clamp(height / TERRAIN_MAX_HEIGHT, 0.001, 1.0);
        return height_palette[size_t(palette_size * scale_height)] * 0.3;
    }
}
