#ifndef _OXYBELIS_PLANET_RENDER_ATMOSPHERE_CONSTANTS_H
#define _OXYBELIS_PLANET_RENDER_ATMOSPHERE_CONSTANTS_H

#include <cmath>
#include <vector>
#include <memory>
#include "atmosphere/model.h"

// Constants taken from
// https://github.com/ebruneton/precomputed_atmospheric_scattering/blob/master/atmosphere/demo/demo.cc

namespace atmosphere_constants {
    constexpr double to_radians(double x) {
        return x * M_PI / 180.0;
    }

    constexpr const double SUN_ANGULAR_RADIUS = 0.00935 / 2.0;

    constexpr const int LAMBDA_MIN = 360;
    constexpr const int LAMBDA_MAX = 830;

    constexpr const double SOLAR_IRRADIANCE[48] = {
        1.11776, 1.14259, 1.01249, 1.14716, 1.72765, 1.73054, 1.6887, 1.61253,
        1.91198, 2.03474, 2.02042, 2.02212, 1.93377, 1.95809, 1.91686, 1.8298,
        1.8685, 1.8931, 1.85149, 1.8504, 1.8341, 1.8345, 1.8147, 1.78158, 1.7533,
        1.6965, 1.68194, 1.64654, 1.6048, 1.52143, 1.55622, 1.5113, 1.474, 1.4482,
        1.41018, 1.36775, 1.34188, 1.31429, 1.28303, 1.26758, 1.2367, 1.2082,
        1.18737, 1.14683, 1.12362, 1.1058, 1.07124, 1.04992
    };

    constexpr const double OZONE_CROSS_SECTION[48] = {
        1.18e-27, 2.182e-28, 2.818e-28, 6.636e-28, 1.527e-27, 2.763e-27, 5.52e-27,
        8.451e-27, 1.582e-26, 2.316e-26, 3.669e-26, 4.924e-26, 7.752e-26, 9.016e-26,
        1.48e-25, 1.602e-25, 2.139e-25, 2.755e-25, 3.091e-25, 3.5e-25, 4.266e-25,
        4.672e-25, 4.398e-25, 4.701e-25, 5.019e-25, 4.305e-25, 3.74e-25, 3.215e-25,
        2.662e-25, 2.238e-25, 1.852e-25, 1.473e-25, 1.209e-25, 9.423e-26, 7.455e-26,
        6.566e-26, 5.105e-26, 4.15e-26, 4.228e-26, 3.237e-26, 2.451e-26, 2.801e-26,
        2.534e-26, 1.624e-26, 1.465e-26, 2.078e-26, 1.383e-26, 7.105e-27
    };

    constexpr const double DOBSON_UNIT = 2.687e20;

    constexpr const double MAX_OZONE_NUMBER_DENSITY = 300.0 * DOBSON_UNIT / 15000.0;

    constexpr double CONSTANT_SOLAR_IRRADIANCE = 1.5;
    constexpr double RAYLEIGH = 1.24062e-6;
    constexpr double RAYLEIGH_SCALE_HEIGHT = 8000.0;
    constexpr double MIE_SCALE_HEIGHT = 1200.0;
    constexpr double MIE_ANGSTROM_ALPHA = 0.0;
    constexpr double MIE_ANGSTROM_BETA = 5.328e-3;
    constexpr double MIE_SINGLE_SCATTERING_ALBEDO = 0.9;
    constexpr double MIE_PHASE_FUNCTION_G = 0.8;
    constexpr double GROUND_ALBEDO = 0.1;

    constexpr const double MAX_SUN_ZENITH_ANGLE = to_radians(120.0);

    const atmosphere::DensityProfileLayer RAYLEIGH_LAYER(0.0, 1.0, -1.0 / RAYLEIGH_SCALE_HEIGHT, 0.0, 0.0);
    const atmosphere::DensityProfileLayer MIE_LAYER(0.0, 1.0, -1.0 / MIE_SCALE_HEIGHT, 0.0, 0.0);

    const std::vector<atmosphere::DensityProfileLayer> OZONE_DENSITY = {
        atmosphere::DensityProfileLayer(25000.0, 0.0, 0.0, 1.0 / 15000.0, -2.0 / 3.0),
        atmosphere::DensityProfileLayer(0.0, 0.0, 0.0, -1.0 / 15000.0, 8.0 / 3.0)
    };

    struct AtmosphereData {
        std::vector<double> wavelengths;
        std::vector<double> solar_irradiance;
        std::vector<double> rayleigh_scattering;
        std::vector<double> mie_scattering;
        std::vector<double> mie_extinction;
        std::vector<double> absorption_extinction;
        std::vector<double> ground_albedo;

        AtmosphereData() {
            for (int l = LAMBDA_MIN; l <= LAMBDA_MAX; l += 10) {
                double lambda = static_cast<double>(l) * 1e-3;  // micro-meters
                double mie = MIE_ANGSTROM_BETA / MIE_SCALE_HEIGHT * pow(lambda, -MIE_ANGSTROM_ALPHA);
                this->wavelengths.push_back(l);

                this->solar_irradiance.push_back(SOLAR_IRRADIANCE[(l - LAMBDA_MIN) / 10]);

                this->rayleigh_scattering.push_back(RAYLEIGH * pow(lambda, -4));
                this->mie_scattering.push_back(mie * MIE_SINGLE_SCATTERING_ALBEDO);
                this->mie_extinction.push_back(mie);
                this->absorption_extinction.push_back(MAX_OZONE_NUMBER_DENSITY * OZONE_CROSS_SECTION[(l - LAMBDA_MIN) / 10]);
                this->ground_albedo.push_back(GROUND_ALBEDO);
            }
        }
    };

    const AtmosphereData ATMOSPHERE_DATA;

    std::unique_ptr<atmosphere::Model> load_earthlike_model(double radius, double outer_radius, const AtmosphereData& data = ATMOSPHERE_DATA) {
        auto model = std::unique_ptr<atmosphere::Model>(
            new atmosphere::Model(
                data.wavelengths,
                data.solar_irradiance,
                SUN_ANGULAR_RADIUS,
                radius, outer_radius,
                {RAYLEIGH_LAYER},
                data.rayleigh_scattering,
                {MIE_LAYER},
                data.mie_scattering,
                data.mie_extinction,
                MIE_PHASE_FUNCTION_G,
                OZONE_DENSITY,
                data.absorption_extinction,
                data.ground_albedo,
                MAX_SUN_ZENITH_ANGLE,
                1, // length unit in meters
                3, // use_luminance_ != PRECOMPUTED
                false,
                false
            )
        );

        model->Init();
        return model;
    }
};

#endif
