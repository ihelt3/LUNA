/*------------------------------------------------------------------------*\
**  
**  @file:      fields.cc
**
**  @author:    Isaiah Helt (ihelt@gatech.edu)
**
**  @brief:     Implementation of different fields classes
**
\*------------------------------------------------------------------------*/

#include <type_traits>

#include "fields.hh"

/*------------------------------------------------------------------------*\
**  Class field Implementation
\*------------------------------------------------------------------------*/

// * * * * * * * * * * * * * Get internal field * * * * * * * * * * * * * //
template<typename fieldType>
void UTILITIES::field<fieldType>::set_internal(const std::vector<fieldType>& internal)
{ 
    // Store the old internal field
    _old = _internal;
    // Store the new internal field
    _internal = internal; 

    if ( !_initialized ) { _initialized = true; }
}


MATH::Vector UTILITIES::field<MATH::Vector>::get_residual()
{
    if (!_initialized) {
        std::cerr << "ERROR: Old Field not initialized" << std::endl;
    }

    MATH::Vector residual(_internal[0].size());

    for (int i = 0; i < _internal.size(); i++) {
        for (int j = 0; j < _internal[i].size(); j++) {
            residual[j] = residual[j] + abs(_internal[i][j] - _old[i][j]);
        }
    }
    return residual;
}

double UTILITIES::field<double>::get_residual()
{
    if (!_initialized) {
        std::cerr << "ERROR: Old Field not initialized" << std::endl;
    }

    double residual = 0;

    for (int i = 0; i < _internal.size(); i++) {
        residual = residual + abs(_internal[i] - _old[i]);
    }
    return residual;
}

std::vector<double> UTILITIES::field<std::vector<double>>::get_residual()
{
    if (!_initialized) {
        std::cerr << "ERROR: Old Field not initialized" << std::endl;
    }
    
    std::vector<double> residual(_internal[0].size());

    for (int i = 0; i < _internal.size(); i++) {
        for (int j = 0; j < _internal[i].size(); j++) {
            residual[j] = residual[j] + abs(_internal[i][j] - _old[i][j]);
        }
    }
    return residual;
}


// Explicity Template Instantiation
template class UTILITIES::field<MATH::Vector>;
template class UTILITIES::field<std::vector<double>>;
template class UTILITIES::field<double>;





