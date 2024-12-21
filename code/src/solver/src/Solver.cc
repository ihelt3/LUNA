/*------------------------------------------------------------------------*\
**  
**  @file:      Solver.cc
**
**  @author:    Isaiah Helt (ihelt@gatech.edu)
**
**  @brief:     Implementation for Solver base class
**
\*------------------------------------------------------------------------*/

#include "Solver.hh"
#include "BoundaryConditions.hh"

/*------------------------------------------------------------------------*\
**  Class Solver Implementation
\*------------------------------------------------------------------------*/

// * * * * * * * * * * * * * *  Constructor * * * * * * * * * * * * * * * //
SOLVER::Solver::Solver(std::shared_ptr<MESH::mesh> mesh)
:
    _mesh(mesh), 
    _cellVelocityField(mesh, MATH::Vector(mesh->get_dimension()), "cell", UTILITIES::fieldTypeEnum::VELOCITY),
    _faceVelocityField(mesh, MATH::Vector(mesh->get_dimension()), "face", UTILITIES::fieldTypeEnum::VELOCITY),
    _nodeVelocityField(mesh, MATH::Vector(mesh->get_dimension()), "node", UTILITIES::fieldTypeEnum::VELOCITY),
    _cellPressureField(mesh, 0.0, "cell", UTILITIES::fieldTypeEnum::PRESSURE),
    _facePressureField(mesh, 0.0, "face", UTILITIES::fieldTypeEnum::PRESSURE),
    _faceMassFluxField(mesh, 0.0, "face", UTILITIES::fieldTypeEnum::MASSFLUX),
    _massFluxDirection(mesh->get_elements().size(), mesh->get_faces().size())
{
    // Calculate any geometric data that remains constant
    std::cout << "Calculating geometric data..." << std::endl;
    calculateFaceNormalDeltas();
}



// * * * * * * * * * * * * * Calculate Face Normal Deltas * * * * * * * * * * * * * * //
void SOLVER::Solver::calculateFaceNormalDeltas() {
    std::cout << "  Calculating face normal deltas...";

    std::vector<MESH::element> elements = _mesh->get_elements();
    MATH::Vector delta(_mesh->get_dimension());
    MESH::element elem;
    MESH::element elem2;

    
    for (const MESH::face& f : _mesh->get_faces() ) {
        // Note: first element of face elements vector will always be interior element
        elem = elements[f.get_elements()[0]];

        // For a boundary face, the delta is just the distance to the face
        if (f.is_boundaryFace()) {
            delta = elem.get_centroid() - f.get_centroid();
        }
        // For interior faces, the delta is the distance between the two elements (normal to the face)
        else {
            elem2 = elements[f.get_elements()[1]];
            auto faceNormal = elem2.get_centroid() - elem.get_centroid();
            delta = elem2.get_centroid() - elem.get_centroid();
        }
        _faceNormalDeltas.push_back( abs(delta * elem.get_normals()[elem==f]) );
    }

    std::cout << " done!" << std::endl;
}


// * * * * * * * * * * * * * Compute Pressure Gradients * * * * * * * * * * * * * * //
std::vector<MATH::Vector> SOLVER::Solver::computeCellPressureGradient(std::vector<double> facePressure)
{

    assert(facePressure.size() == _mesh->get_faces().size() && "Invalid face pressure field size!");

    std::vector<MATH::Vector> pressureGradientField;
    
    // Loop over elements to calculte pressure gradient
    for (const MESH::element& elem : _mesh->get_elements() ) {
        MATH::Vector pgrad(_mesh->get_dimension());
        // Loop over faces to calculate pressure gradient
        for (const MESH::face& f : elem.get_faces() ) {
            pgrad = pgrad + ( facePressure[f.get_id()] * f.get_volume() * f.get_normal() );
        }
        // Normalize by cell volume
        pgrad = pgrad / elem.get_volume();
        // push back to pressure gradient field
        pressureGradientField.push_back(pgrad);
    }

    return pressureGradientField;
}


// * * * * * * * * * * * * * * Compute Face Pressure Gradient * * * * * * * * * * * * * * //
std::vector<MATH::Vector> SOLVER::Solver::computeFacePressureGradient(std::vector<double> cellPressure, std::vector<double> facePressure)
{
    assert(facePressure.size() == _mesh->get_faces().size() && "Invalid face pressure field size!");
    assert(cellPressure.size() == _mesh->get_elements().size() && "Invalid cell pressure field size!");

    std::vector<MATH::Vector> pressureGradientField;
    MATH::Vector pgrad(_mesh->get_dimension());
    double pdiff;
    
    // Loop over elements to calculte pressure gradient
    for (const MESH::face& f : _mesh->get_faces() ) {        

        // Boundary face
        if (f.is_boundaryFace()){
            // difference in cell pressure and face pressure
            pdiff = facePressure[f.get_id()] - cellPressure[f.get_elements()[0]];
            // Scale by face normal delta
            pdiff = pdiff / _faceNormalDeltas[f.get_id()];
        }
        // Internal face
        else {
            // Get difference in cell face pressures
            pdiff = cellPressure[f.get_elements()[1]] - cellPressure[f.get_elements()[0]];
            // Scale by face normal delta
            pdiff /= _faceNormalDeltas[f.get_id()];
        }
        
        // Note: Face normal corresponds to first element's outward facing normal
        // both pressure difference normal point from cell 0 to cell 1
        pgrad = pdiff * f.get_normal();
        pressureGradientField.push_back(pgrad);
    }

    return pressureGradientField;
}

// * * * * * * * * * * * * * * Implement nodal calculations * * * * * * * * * * * * * * // 
std::vector<MATH::Vector> SOLVER::Solver::computeNodalVector(UTILITIES::field<MATH::Vector> field)
{
    // Initialize nodal values
    std::vector<MATH::Vector> nodalValues(_mesh->get_nodes().size());

    // Loop over nodes
    for (int node=0 ; node<_mesh->get_nodes().size() ; node++)
    {
        nodalValues[node] = MATH::Vector(_mesh->get_dimension());

        // BOUNDARY NODE VALUE
        if (_mesh->get_nodes()[node].is_boundaryNode()) {
            // Check if the node is on a corner
            int nboundaries = 0;
            std::vector<int> faceIdxs = {};
            for (int f=0 ; f<_mesh->get_nodes()[node].get_faces().size() ; f++) {
                if (_mesh->get_faces()[_mesh->get_nodes()[node].get_faces()[f]].is_boundaryFace()) {
                    nboundaries++;
                    faceIdxs.push_back(_mesh->get_nodes()[node].get_faces()[f]);
                }
            }

            MATH::Vector val(std::vector<double>(_mesh->get_dimension(), 0.0));
            // Average the node value over the boundary face values
            for (const int& f : faceIdxs) {
                if (field.get_type() == UTILITIES::fieldTypeEnum::VELOCITY) {
                    val = val + _BCs[_mesh->get_faces()[f].get_elements()[1]]->get_velocity(f);
                }
                else {
                    std::cerr << "ERROR: Node calculation for vector field only implemented for velocity field" << std::endl;
                    std::abort();
                }
            }
            nodalValues[node] = val / double(nboundaries);
        }

        // INTERIOR NODE VALUE
        else {
            for (int e=0 ; e<_mesh->get_nodes()[node].get_elements().size() ; e++)
            {
                // Get nodal value
                nodalValues[node] = nodalValues[node] + _mesh->get_nodes()[node].get_distanceWeights()[e] * field.get_internal()[_mesh->get_nodes()[node].get_elements()[e]];
            }
        }
        
    }

    return nodalValues;
}

// * * * * * * * * * * * * * * Get nodal scalar field from cell field * * * * * * * * * * * * * * //
std::vector<double> SOLVER::Solver::computeNodalScalar(UTILITIES::field<double> field)
{
    assert(field.get_internal().size() == _mesh->get_elements().size()  && "Nodal values must be computed from CELL based field");

    // Initialize nodal values
    std::vector<double> nodalValues(_mesh->get_nodes().size());

    // Loop over nodes
    for (int node=0 ; node<_mesh->get_nodes().size() ; node++)
    {
        nodalValues[node] = 0.0;

        // BOUNDARY NODE VALUE
        if (_mesh->get_nodes()[node].is_boundaryNode()) {
            // Check if the node is on a corner
            int nboundaries = 0;
            std::vector<int> faceIdxs = {};
            for (int f=0 ; f<_mesh->get_nodes()[node].get_faces().size() ; f++) {
                if (_mesh->get_faces()[_mesh->get_nodes()[node].get_faces()[f]].is_boundaryFace()) {
                    nboundaries++;
                    faceIdxs.push_back(_mesh->get_nodes()[node].get_faces()[f]);
                }
            }

            double val = 0.0;
            // Average the node value over the boundary face values
            for (const int& f : faceIdxs) {
                if (field.get_type() == UTILITIES::fieldTypeEnum::PRESSURE) {
                    val += _BCs[_mesh->get_faces()[f].get_elements()[1]]->get_pressure(f);
                }
                else {
                    std::cerr << "ERROR: Node calculation for scalar field only implemented for pressure field" << std::endl;
                    std::abort();
                }
            }

            nodalValues[node] = val / nboundaries;
        }

        // INTERNAL NODE
        else {
            for (int e=0 ; e<_mesh->get_nodes()[node].get_elements().size() ; e++)
            {
                // Get nodal value
                nodalValues[node] = nodalValues[node] + _mesh->get_nodes()[node].get_distanceWeights()[e] * field.get_internal()[_mesh->get_nodes()[node].get_elements()[e]];
            }
        }
        
    }

    return nodalValues;
};

// * * * * * * * * * * * * * * Calculate pressure on cell faces * * * * * * * * * * * * * * //
std::vector<double> SOLVER::Solver::computeFacePressure(std::vector<double> cellPressure) {

    assert(cellPressure.size() == _mesh->get_elements().size() && "Invalid cell pressure field size!");
    
    // Get face pressure
    std::vector<double> pface(_mesh->get_faces().size());

    MESH::element cell1;
    MESH::element cell2;

    // Loop through faces
    for (const MESH::face& f : _mesh->get_faces()) {
        // Boundary Face Pressures
        if (f.is_boundaryFace()) {
            pface[f.get_id()] = _BCs[f.get_elements()[1]]->get_pressure(f.get_id());
        }
        // Use distance weighted average for internal face pressures
        else {
            cell1 = _mesh->get_elements()[f.get_elements()[0]];
            cell2 = _mesh->get_elements()[f.get_elements()[1]];
            pface[f.get_id()] = cellPressure[cell1.get_id()] * cell1.get_distanceWeights()[cell1==f] + cellPressure[cell2.get_id()] * cell2.get_distanceWeights()[cell2==f];
        }
    }

    return pface;
}


// * * * * * * * * * * * * * * Assign boundary conditions * * * * * * * * * * * * * * //
void SOLVER::Solver::setBoundaryCondition(std::shared_ptr<BOUNDARIES::BoundaryCondition> bc)
{
    // Make sure solver has not been completed
    assert(!_solved && "Cannot set boundary conditions after solver has already been solved!");

    // Make sure boundary condition is properly assigned
    
    if ( !bc->isComplete()) {
        std::ostringstream errorMsg1;
        errorMsg1 << "Boundary condition for " << bc->get_name() << " does not exist!";
        std::cerr << errorMsg1.str() << std::endl;
        std::abort();
    } 

    // Make sure boundary exists
    if (_mesh->get_boundaryID(bc->get_name()) == 0) {
        std::ostringstream errorMsg2;
        errorMsg2 << "Boundary " << bc->get_name() << " does not exist!";
        std::cerr << errorMsg2.str() << std::endl;
        std::abort();
    }
    
    // Set Boundary Condition
    _BCs[bc->get_id()] = bc;
}

// * * * * * * * * * * * * * * Check all boundary conditions are set * * * * * * * * * * * * * * //
bool SOLVER::Solver::checkBoundaryConditions()
{
    // Get all mesh boundaries
    auto boundaries = _mesh->get_boundaries();

    // Get all set boundary condition ids
    std::vector<int> boundaryIDs;
    for (const auto& pair : _BCs) {
        boundaryIDs.push_back(pair.first);
    }

    int id;
    for (const auto& boundary : boundaries) {
        // First make sure boundary condition is set
        id = boundary.get_id();
        if ( std::find(boundaryIDs.begin(), boundaryIDs.end(), id) == boundaryIDs.end() ) {
            std::ostringstream errorMsg1;
            errorMsg1 << "Boundary " << boundary.get_name() << " is not set!";
            std::cerr << errorMsg1.str() << std::endl;
            return false;
        };

        // Then make sure boundary condition is valid
        if ( !_BCs[id]->isComplete()) {
            std::ostringstream errorMsg2;
            errorMsg2 << "Boundary condition for " << boundary.get_name() << " is not set!";
            std::cerr << errorMsg2.str() << std::endl;
            return false;
        }

        // Make sure no boundary conditions are assigned more than once
    }

    return true;
}



