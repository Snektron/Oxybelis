// function.h
// This file was added by Robin Voetter

#ifndef NOISE_MODULE_FUNCTION_H
#define NOISE_MODULE_FUNCTION_H

#include <functional>
#include "modulebase.h"

namespace noise
{

  namespace module
  {

    /// @addtogroup libnoise
    /// @{

    /// @addtogroup modules
    /// @{

    /// @addtogroup modifiermodules
    /// @{

    /// Noise module that outputs the value from a custom function.
    class Function: public Module
    {

      public:

        /// Constructor.
        Function ():
          Module (GetSourceModuleCount ()),
          m_function ([](double, double, double){ return 0; })
        {
        }

        void SetFunction (std::function<double(double, double, double)> func) {
          m_function = func;
        }

        virtual int GetSourceModuleCount () const
        {
          return 0;
        }

        virtual double GetValue (double x, double y, double z) const
        {
          return m_function (x, y, z);
        }

      protected:

        std::function<double(double, double, double)> m_function;
    };

    /// @}

    /// @}

    /// @}

  }

}

#endif
