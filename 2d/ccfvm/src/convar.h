#ifndef __CONVAR_H__
#define __CONVAR_H__

#include "vec.h"
#include "flux.h"

//------------------------------------------------------------------------------
// Conserved variable
//------------------------------------------------------------------------------
class ConVar
{
   public:
      ConVar () {};
      ~ConVar () {};
      ConVar& operator=  (const ConVar& con_var);
      ConVar& operator=  (const double& scalar);
      ConVar& operator+= (const ConVar& con_var);
      ConVar  operator+  (const ConVar& con_var) const;
      ConVar  operator-  (const ConVar& con_var) const;
      ConVar  operator+  (const Flux&   flux) const;
      ConVar  operator-  (const Flux&   flux) const;
      ConVar  operator*  (const double  scalar) const;
      void min (const ConVar& p);
      void max (const ConVar& p);

      double density, energy;
      Vector momentum;

};

//------------------------------------------------------------------------------
// Assign conserved variable
//------------------------------------------------------------------------------
inline
ConVar& ConVar::operator= (const ConVar& con_var)
{
   density  = con_var.density;
   momentum = con_var.momentum;
   energy   = con_var.energy;

   return *this;
}

//------------------------------------------------------------------------------
// Set a scalar value
//------------------------------------------------------------------------------
inline
ConVar& ConVar::operator= (const double& scalar)
{
   density  = scalar;
   momentum = scalar;
   energy   = scalar;
   
   return *this;
}

//------------------------------------------------------------------------------
// Add conserved variable to given variable
//------------------------------------------------------------------------------
inline
ConVar& ConVar::operator+= (const ConVar& con_var)
{
   density  += con_var.density;
   momentum += con_var.momentum;
   energy   += con_var.energy;

   return *this;
}

//------------------------------------------------------------------------------
// Add two conserved variables and return result
//------------------------------------------------------------------------------
inline
ConVar ConVar::operator+ (const ConVar& con_var) const
{
   ConVar result;

   result.density  = density  + con_var.density;
   result.momentum = momentum + con_var.momentum;
   result.energy   = energy   + con_var.energy;

   return result;
}

//------------------------------------------------------------------------------
// Subtract two conserved variables and return result
//------------------------------------------------------------------------------
inline
ConVar ConVar::operator- (const ConVar& con_var) const
{
   ConVar result;

   result.density  = density  - con_var.density;
   result.momentum = momentum - con_var.momentum;
   result.energy   = energy   - con_var.energy;

   return result;
}

//------------------------------------------------------------------------------
// Add flux to conserved variable, used to update solution
//------------------------------------------------------------------------------
inline
ConVar ConVar::operator+ (const Flux& flux) const
{
   ConVar result;

   result.density  = density  + flux.mass_flux;
   result.momentum = momentum + flux.momentum_flux;
   result.energy   = energy   + flux.energy_flux;

   return result;
}

//------------------------------------------------------------------------------
// Subtract flux from conserved variable, used to update solution
//------------------------------------------------------------------------------
inline
ConVar ConVar::operator- (const Flux& flux) const
{
   ConVar result;

   result.density  = density  - flux.mass_flux;
   result.momentum = momentum - flux.momentum_flux;
   result.energy   = energy   - flux.energy_flux;

   return result;
}

//------------------------------------------------------------------------------
// Multiplied conserved variable by a scalar
//------------------------------------------------------------------------------
inline
ConVar ConVar::operator* (const double scalar) const
{
   ConVar result;

   result.density  = density  * scalar;
   result.momentum = momentum * scalar; 
   result.energy   = energy   * scalar;

   return result;
}

//------------------------------------------------------------------------------
// Update PrimVar = min(PrimVar, p)
//------------------------------------------------------------------------------
inline
void ConVar::min (const ConVar& p)
{
   density = std::min(density, p.density);
   momentum.x  = std::min(momentum.x,  p.momentum.x);
   momentum.y  = std::min(momentum.y,  p.momentum.y);
   momentum.z  = std::min(momentum.z,  p.momentum.z);
   energy    = std::min(energy,    p.energy);
}

//------------------------------------------------------------------------------
// Update PrimVar = max(PrimVar, p)
//------------------------------------------------------------------------------
inline
void ConVar::max (const ConVar& p)
{
   density = std::max(density, p.density);
   momentum.x  = std::max(momentum.x,  p.momentum.x);
   momentum.y  = std::max(momentum.y,  p.momentum.y);
   momentum.z  = std::max(momentum.z,  p.momentum.z);
   energy    = std::max(energy,    p.energy);
}

#endif
