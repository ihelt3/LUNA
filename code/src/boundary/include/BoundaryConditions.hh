/*------------------------------------------------------------------------*\
**  
**  @file:      BoundaryConditions.hh
**
**  @author:    Isaiah Helt (ihelt@gatech.edu)
**
**  @brief:     Header file of different boundary condition classes
**
\*------------------------------------------------------------------------*/
#ifndef _BOUNDARYCONDITIONS_HH_
#define _BOUNDARYCONDITIONS_HH_

#include <string>
#include <unordered_map>

#include "MeshEntities.hh"
#include "Solver.hh"
#include "Vector.hh"

namespace BOUNDARIES {

// Enum for boundary condition types
enum class boundaryConditionType {
    INVALID,
    WALL,
    FREESTREAM,
    INLET,
    OUTLET,
    SYMMETRY
};

/*------------------------------------------------------------------------*\
**  Boundary Condition Base Class
\*------------------------------------------------------------------------*/
class BoundaryCondition
{
public:
    // Constructors
        // Construct with solver and name
        BoundaryCondition(std::weak_ptr<SOLVER::Solver> solver , std::string BCname);
        // Construct with solver and BC ID
        BoundaryCondition(std::weak_ptr<SOLVER::Solver> solver , int BCID);

    // Virtual Destructor
        virtual ~BoundaryCondition() {};

    // Virtual Methods to get different physical quantities (this allows polymorphisms for derived classes to call them using a pointer to the base class)
        // Method to check if BC is complete or not
        virtual bool isComplete() {return false;};
        // Methods to set velocity
        virtual void set_velocity(MATH::Vector) {};
        // Methods to get pressure
        virtual double get_pressure(int) = 0;
        // Methods to get Velocity
        virtual MATH::Vector get_velocity(int) = 0;
        // Method to get mass flux
        virtual double get_massFlux(int) = 0;

    // Common get methods
        // Get name
        std::string get_name() const { return _name; }
        // Get ID
        int get_id() const { return _bcID; }

protected:
    // Member Data
        // Pointer to solver for this BC
        std::weak_ptr<SOLVER::Solver> _solver;
        // Boundary Condition Name
        std::string _name;
        // Boundary Condition ID 
        int _bcID;
        // Boundary Condition Index (in mesh boundary vector)
        int _bcIdx;

    // Protected methods
        // get pointer to solver
        std::shared_ptr<SOLVER::Solver> get_solver();
};

}

#endif // _BOUNDARYCONDITIONS_HH_
