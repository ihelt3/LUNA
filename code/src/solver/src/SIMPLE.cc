/*------------------------------------------------------------------------*\
**  
**  @file:      SIMPLE.cc
**
**  @author:    Isaiah Helt (ihelt@gatech.edu)
**
**  @brief:     Implementation for SIMPLE class
**
\*------------------------------------------------------------------------*/

// TEMPORARY, DELETE LATER
#include <iostream>
#include <fstream>

#include <chrono>

#include "SIMPLE.hh"


/*------------------------------------------------------------------------*\
**  Class SIMPLE Implementation
\*------------------------------------------------------------------------*/

// * * * * * * * * * * * * * *  Constructor * * * * * * * * * * * * * * * //
SOLVER::SIMPLE::SIMPLE(std::shared_ptr<MESH::mesh> mesh)
:
    SOLVER::Solver(mesh),
    _momentumSystemA(_mesh->get_elements().size(),_mesh->get_elements().size()),
    _momentumSystemb_x(_mesh->get_elements().size()),
    _momentumSystemb_y(_mesh->get_elements().size()),
    _momentumSystemb_z(_mesh->get_elements().size())
{
    // Initialize face mass flux field
    std::cout << "Initializing face mass flux field...";
    _faceMassFluxField = UTILITIES::field(_mesh, 0.0, "face",UTILITIES::fieldTypeEnum::MASSFLUX);
    std::cout << " done!" << std::endl;

    // Initialize momentum matrix
    std::cout << "Initializing momentum matrix..." << std::endl;
    updateMomentumMatrix();
    std::cout << " done!" << std::endl;

};

// * * * * * * * * * * * * * *  Solve Method * * * * * * * * * * * * * * * //
void SOLVER::SIMPLE::solve()
{
    std::cout << "Running SIMPLE solver..." << std::endl;

    // Check that boundary conditions have been set
    std::cout << "Checking boundary conditions...";
    checkBoundaryConditions();
    std::cout << "done!" << std::endl;

    _solved = true;

    // Timer
    auto start = std::chrono::steady_clock::now();

    // Begin loop
    for ( int i=0 ; i<iter ; i++ )
    {
        std::cout << "==========================" << std::endl;
        std::cout << "Iteration: " << i+1 << std::endl;
        
        // Solving momentum system
        std::cout << "Solving Momentum System..." << std::endl;
        std::cout << "  Updating Momentum matrix..." << std::endl;
        updateMomentumMatrix();
        std::cout << "  Updating Momentum RHS..." << std::endl;
        updateMomentumRHS();
        std::cout << "  Solving..." << std::endl;
        solveMomentumSystem();

        
        std::cout << "  Calculating face mass flux..." << std::endl;
        computeFaceMassFlux();

        // TEMPORARY CODE: Checking pressure correction
        if ( i <= 1000 ) {
        // Solving pressure correction
        std::cout << "Solving Pressure Correction..." << std::endl;
        SolvePressureCorrection();

        // Update cell velocities and face mass flux
        std::cout << "Updating cell velocities and face mass flux..." << std::endl;
        correctCellVelocity();
        correctFaceMassFlux();
        }

        // Check for convergence
        if ( i > 0 )
        {
            if ( checkConvergence() )
            {
                std::cout << "Converged!" << std::endl;
                break;
            }
        }
    }

    auto end = std::chrono::steady_clock::now();
    std::cout << "SIMPLE Solver completed " << iter << " iterations in " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()/1000.0 << "s" << std::endl;

}

// * * * * * * * * * * * * * * Compute Face Velocities * * * * * * * * * * * * * * //
void SOLVER::SIMPLE::computeFaceVelocities()
{
    // Initialize
    std::vector<MATH::Vector> faceVelocities;
    MATH::Vector faceVelocity(_mesh->get_dimension());

    // Compute cell and face pressure gradients
    std::vector<MATH::Vector> cellPressureGradients = computeCellPressureGradient(_facePressureField.get_internal());
    std::vector<MATH::Vector> facePressureGradients = computeFacePressureGradient(_cellPressureField.get_internal(), _facePressureField.get_internal());
    
    // Initialize variables
    double w1;
    double A0_1;
    double A0_2;
    int cellid1;
    int cellid2;
    MESH::element cell1;
    MESH::element cell2;

    MATH::Vector ucell1;
    MATH::Vector ucell2;
    MATH::Vector udp;

    // Loop over faces
    for (const MESH::face& f : _mesh->get_faces() ) {
        // Check if face is on boundary
        // std::cout << "Face: " << f.get_id() << std::endl;
        if (f.is_boundaryFace()) {
            // Second element should be boundary ID (for faces in mesh faces)
            faceVelocity = _BCs[f.get_elements()[1]]->get_velocity(f.get_id());
        }
        else {
            cellid1 = f.get_elements()[0];
            cellid2 = f.get_elements()[1];
            cell1 = _mesh->get_elements()[cellid1];
            cell2 = _mesh->get_elements()[cellid2];

            // Get distance weight of first cell
            w1 = cell1.get_distanceWeights()[cell1==f];

            // Cell 1 contribution
            A0_1 = _momentumSystemA.get_value(cellid1,cellid1);
            ucell1 = w1 * ( _cellVelocityField.get_internal()[cellid1] + (1.0/A0_1 )*cell1.get_volume()*cellPressureGradients[cellid1] );
            // Cell 2 contribution
            A0_2 = _momentumSystemA.get_value(cellid2,cellid2);
            ucell2 = (1.0 - w1) * ( _cellVelocityField.get_internal()[cellid2] + (1.0/A0_2 )*cell2.get_volume()*cellPressureGradients[cellid2] );
            // Pressure contribution
            udp = (w1*(cell1.get_volume()/A0_1) + (1.0-w1)*(cell2.get_volume()/A0_2)) * facePressureGradients[f.get_id()];

            // Total Cell Velocity
            // std::cout << "    cellVelocity1: " << _cellVelocityField.get_internal()[cellid1] << "      cellVelocity2: " << _cellVelocityField.get_internal()[cellid2] << std::endl;
            // std::cout << "    ucell1: " << ucell1 << "      ucell2: " << ucell2 << "      udp: " << udp << std::endl;
            faceVelocity = ucell1 + ucell2 + udp;
            
        }
        faceVelocities.push_back(faceVelocity);
    }

    _faceVelocityField.set_internal(faceVelocities);
    const auto& faceVelocities2 = _faceVelocityField.get_internal();
}


// * * * * * * * * * * * * * * Compute Face Mass Flux Using Face Velocities * * * * * * * * * * * * * * //
void SOLVER::SIMPLE::computeFaceMassFlux()
{
    computeFaceVelocities();
    std::vector<MATH::Vector> faceVelocities = _faceVelocityField.get_internal();
    std::vector<double> mdotf = _faceMassFluxField.get_internal();

    // Initializing variables
    MESH::element cell1;
    MESH::element cell2;
    MATH::Vector faceNormal;
    

    // Loop over faces
    for (const MESH::face& f : _mesh->get_faces() ) {

        cell1 = _mesh->get_elements()[f.get_elements()[0]];

        // outward pointing normal
        faceNormal = cell1.get_normals()[cell1==f];

        // Boundary Face
        if (f.is_boundaryFace()) {
            

            // Returns mass flux going INTO the cell
            mdotf[f.get_id()] = _BCs[f.get_elements()[1]]->get_massFlux(f.get_id());

            // Update mass flux direction matrix
            if (mdotf[f.get_id()] > 0.0) {
                _massFluxDirection.set_value(f.get_elements()[0],f.get_id(),1.0);
            }
            else {
                _massFluxDirection.set_value(f.get_elements()[0],f.get_id(),-1.0);
            }

            // Only track absolute value of mass flux here
            mdotf[f.get_id()] = abs(mdotf[f.get_id()]);
        }
        // Internal Face
        else {
            // Get face cells
            cell2 = _mesh->get_elements()[f.get_elements()[1]];

            // Calculate mass flux INTO cell1
            mdotf[f.get_id()] = - (faceVelocities[f.get_id()] * rho * faceNormal * f.get_volume());

            // Update mass flux direction matrix
            if (mdotf[f.get_id()] >= 0.0) {
                // Mass flux INTO first face
                _massFluxDirection.set_value(f.get_elements()[0],f.get_id(),1.0);
                _massFluxDirection.set_value(f.get_elements()[1],f.get_id(),-1.0);
            }
            else {
                // Mass flux OUT OF first face
                _massFluxDirection.set_value(f.get_elements()[0],f.get_id(),-1.0);
                _massFluxDirection.set_value(f.get_elements()[1],f.get_id(),1.0);
            }

            // Only track absolute value of mass flux here
            mdotf[f.get_id()] = abs(mdotf[f.get_id()]);
        }
        
    }

    // Update face mass flux field
    _faceMassFluxField.set_internal(mdotf);
}


// * * * * * * * * * * * * * Initialize Momentum System * * * * * * * * * * * * * * //
// Intialize momentum system
void SOLVER::SIMPLE::updateMomentumMatrix()
{
    double A0;          // diagonal elements
    double Anb;         // off-diagonal (from neighbors)
    double mdotf;       // mass flux through face
    double dface;        // delta_face value: distance between nodes for non-boundary, distance to face for boundary
    int faceidx;        // face index
    int nbidx;          // neighbor index 

    MESH::element cell;     // cell
    MESH::element cellnb;   // cell neighbor

    // reinitialize momentum matrix
    _momentumSystemA = MATH::matrixCSR(_mesh->get_elements().size(),_mesh->get_elements().size());

    for (int c=0 ; c<_mesh->get_elements().size() ; c++) {
        // Initialize diagonal element
        A0 = 0;
        cell = _mesh->get_elements()[c];
        // Loop through neighbor elements
        for (int nb=0 ; nb<cell.get_faces().size() ; nb++ ) {

            // Get mass flux INTO face
            faceidx = cell.get_faces()[nb].get_id(); 
            // boundary or not doesn't matter, that should be accounted for in calculation of mass flux field (Mass flux INTO cell)
            mdotf = _faceMassFluxField.get_internal()[faceidx] * _massFluxDirection.get_value(c,faceidx);

            // Calculate cell neighbor coefficient if the neighbor is not a boundary
            nbidx = cell.get_faces()[nb].get_elements()[0];
            
            if (nbidx >= 0) 
            // Internal Element
            {

                // Neighbor cell
                cellnb = _mesh->get_elements()[nbidx];

                // Difference between cell and neighbor centroid, NORMAL TO THE FACE
                dface = _faceNormalDeltas[faceidx];

                // Neighbor (off diagonal) coefficients
                Anb = -(abs(mdotf)-mdotf)/2.0 - mu*cell.get_faces()[nb].get_volume()/dface;
                // Update neighbor coefficient
                _momentumSystemA.set_value(c,nbidx,Anb);

                // Incremement cell coefficient (FIRST ORDER UPWIND DIFFERENCING USED HERE)
                A0 += (abs(mdotf)+mdotf)/2.0 + mu*cell.get_faces()[nb].get_volume()/dface;
            }
            else
            // Boundary Face
            {
                // Use distance to face for delta_face measurement 
                dface = _faceNormalDeltas[faceidx];
                // Update diagonal term, but no change to source term
                A0 += (abs(mdotf)+mdotf)/2.0 + mu*cell.get_faces()[nb].get_volume()/dface;
            }
        }
        // Update Cell Coefficient
        _momentumSystemA.set_value(c,c,A0);
    }
}


// * * * * * * * * * * * * * Update RHS of Momentum System * * * * * * * * * * * * * * //
// update RHS of momentum equation
void SOLVER::SIMPLE::updateMomentumRHS()
{
    // Total Source Terms
    MATH::Vector Sx(_mesh->get_elements().size());
    MATH::Vector Sy(_mesh->get_elements().size());
    MATH::Vector Sz(_mesh->get_elements().size());

    // Calculate Source terms due to velocity skew and pressure sources
    std::vector<MATH::Vector> nodeVelocities = computeNodalVector(_cellVelocityField);

    // Initializing variables
    MATH::Vector f_tangent;
    MATH::Vector skewVelocity;
    double faceSkew;
    MESH::face f;
    double mdotf;
    MATH::Vector faceVelocity;

    // Sources due to Pressure
    MATH::Vector S_p;
    // Sources due to face skew
    MATH::Vector S_skew;
    // Sources due to boundary conditions
    MATH::Vector S_bc;

    for (const MESH::element& cell : _mesh->get_elements()) {
        // std::cout << "Cell Coordinates: " << cell.get_centroid() << std::endl;
        for (int fi=0 ; fi<cell.get_faces().size() ; fi++) {
            
            f = cell.get_faces()[fi];

            // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
            // SKEW SOURCES

            // Difference in nodes normalized by distance normal to face between cells
            skewVelocity = nodeVelocities[f.get_nodeIDs()[0]] - nodeVelocities[f.get_nodeIDs()[1]];
            skewVelocity = skewVelocity ;

            // face tangent
            f_tangent = _mesh->get_nodes()[f.get_nodeIDs()[0]].get_coordinates() - _mesh->get_nodes()[f.get_nodeIDs()[1]].get_coordinates();
            f_tangent = f_tangent / f_tangent.getL2Norm();

            // Skewness: dot product of face tangent and cell centroid vectors
            if (f.is_boundaryFace()) {
                faceSkew = f_tangent * (cell.get_centroid() - f.get_centroid());
            }
            else {
                faceSkew = f_tangent * (cell.get_centroid() - _mesh->get_elements()[f.get_elements()[0]].get_centroid());
            }

            // Face Skew Source
            S_skew = -1.0 *(skewVelocity/_faceNormalDeltas[f.get_id()]) * faceSkew * mu;

            // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
            // BOUNDARY SOURCES
            if (f.is_boundaryFace()) {
                // Get face mass flux INTO the cell
                mdotf = _faceMassFluxField.get_internal()[f.get_id()] * _massFluxDirection.get_value(cell.get_id(),f.get_id());
                // Get face velocity
                //    NOTE: For faces contained in elements, only the second element ID is stored (hence .get_elements()[0])
                faceVelocity = _BCs[f.get_elements()[0]]->get_velocity(f.get_id());

                S_bc = faceVelocity * ( (abs(mdotf)-mdotf)/2.0 + mu*cell.get_faces()[fi].get_volume()/_faceNormalDeltas[f.get_id()] );
            }
            else {
                S_bc = MATH::Vector(_mesh->get_dimension(),0.0);
            }
            

            // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
            // PRESSURE SOURCES
            S_p = -1.0 * _facePressureField.get_internal()[f.get_id()] * cell.get_normals()[fi] * f.get_volume();
            // std::cout << "     Face Normal: " <<cell.get_normals()[fi] << "     Face Coordinates: " << f.get_centroid() << "     Face Volume: " << 
            //         f.get_volume() << "        Face Pressure: " << _facePressureField.get_internal()[f.get_id()] << std::endl;
            // S_p = MATH::Vector(_mesh->get_dimension(),0.0);

            // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
            // Total Sources
            // std::cout << "     Sx: " << Sx[cell.get_id()] << std::endl;
            // std::cout << "           " << S_p[0] << " + " << S_skew[0] << " + " << S_bc[0] << std::endl;
            Sx[cell.get_id()] += S_p[0] + S_skew[0] + S_bc[0];
            Sy[cell.get_id()] += S_p[1] + S_skew[1] + S_bc[1];
            if (_mesh->get_dimension() == 3) Sz[cell.get_id()] += S_p[2] + S_skew[2] + S_bc[2];

        }
    }

    // Update RHS
    _momentumSystemb_x = Sx;
    _momentumSystemb_y = Sy;
    if (_mesh->get_dimension() == 3) _momentumSystemb_z = Sz;
}


// * * * * * * * * * * * * * Solve Momentum Equation * * * * * * * * * * * * * * //
void SOLVER::SIMPLE::solveMomentumSystem()
{
    double tol = 1.0e-6;
    int iter = 1e5;

    // Set guesses to previous iteration values
    MATH::Vector x_guess(_mesh->get_elements().size());
    MATH::Vector y_guess(_mesh->get_elements().size());
    MATH::Vector z_guess(_mesh->get_elements().size());
    for (int i=0 ; i<_mesh->get_elements().size() ; i++) {
        x_guess[i] = _cellVelocityField.get_internal()[i][0];
        y_guess[i] = _cellVelocityField.get_internal()[i][1];
        if (_mesh->get_dimension() == 3) z_guess[i] = _cellVelocityField.get_internal()[i][2];
    }

    // Solve x momentum system
    MATH::gauss_seidel<MATH::matrixCSR> solverx;
    solverx.set_matrix(_momentumSystemA);
    solverx.set_rhs(_momentumSystemb_x);
    solverx.set_guess(x_guess);
    MATH::Vector x = solverx.solve(iter,tol);
    std::cout << "x-momentum solver residual: " << solverx.get_residual() << " in " << solverx.get_iterations() << " iterations" << std::endl;

    // std::cout << "x momentume A: " << std::endl;
    // std::cout << _momentumSystemA << std::endl << std::endl;

    // std::cout << "x momentume b: " << std::endl;
    // std::cout << _momentumSystemb_x << std::endl << std::endl;

        // std::cout << "outputting A matrix and b vector" << std::endl;
        // // print A matrix
        // std::ofstream outfile("./output/A_matrix.txt");
        // outfile << _momentumSystemA;
        // outfile.close();

        // // print b vector
        // std::ofstream outfile2("./output/b_vector.txt");
        // outfile2 << _momentumSystemb_x;
        // outfile2.close();

    // Solve y momentum system
    MATH::gauss_seidel<MATH::matrixCSR> solvery;
    solvery.set_matrix(_momentumSystemA);
    solvery.set_rhs(_momentumSystemb_y);
    solvery.set_guess(y_guess);
    MATH::Vector y = solvery.solve(iter,tol);
    std::cout << "y-momentum solver residual: " << solvery.get_residual() << " in " << solvery.get_iterations() << " iterations" << std::endl;

    // std::cout << "y momentume A: " << std::endl;
    // std::cout << _momentumSystemA << std::endl << std::endl;

    // std::cout << "y momentume b: " << std::endl;
    // std::cout << _momentumSystemb_y << std::endl << std::endl;

    // *********************************************************************
    // DEBUGGING

    // std::cout << "y-momentum matrix:" << std::endl;
    // std::cout << _momentumSystemA << std::endl;
    // std::cout << "node velocities:" << std::endl;
    // std::vector<MATH::Vector> nodeVelocities = computeNodalVector(_cellVelocityField);
    // for (int i=0 ; i<_mesh->get_nodes().size() ; i++) {
    //     std::cout << nodeVelocities[i] << std::endl;
    // }
    // std::cout << "x-momentum RHS:" << std::endl;
    // std::cout << _momentumSystemb_x << std::endl;
    // std::cout << "x-momentum solution:" << std::endl;
    // std::cout << x << std::endl;
    // std::cout << "y-momentum RHS:" << std::endl;
    // std::cout << _momentumSystemb_y << std::endl;


    // **********************************************************************


    // Solve z momentum system
    MATH::Vector z(_mesh->get_elements().size());
    if (_mesh->get_dimension() == 3) {
        MATH::gauss_seidel<MATH::matrixCSR> solverz;
        solverz.set_matrix(_momentumSystemA);
        solverz.set_rhs(_momentumSystemb_z);
        solverz.set_guess(z_guess);
        MATH::Vector z = solverz.solve(iter,tol);
        std::cout << "z-momentum solver residual: " << solverz.get_residual() << " in " << solverz.get_iterations() << " iterations" << std::endl;
    }

    // Relax Momentum Equation
    x = x*_urelax + x_guess*(1.0-_urelax);
    y = y*_urelax + y_guess*(1.0-_urelax);
    if (_mesh->get_dimension() == 3) z = z*_urelax + z_guess*(1.0-_urelax);


    // Assign values back to velocity field
    std::vector<MATH::Vector> cellVelocities(_mesh->get_elements().size());
    MATH::Vector v(_mesh->get_dimension());
    for (const MESH::element& cell : _mesh->get_elements()) {
        v[0] = x[cell.get_id()];
        v[1] = y[cell.get_id()];
        if (_mesh->get_dimension() == 3) v[2] = z[cell.get_id()];
        cellVelocities[cell.get_id()] = v;
    }

    _cellVelocityField.set_internal(cellVelocities);

}


// * * * * * * * * * * * * * Solve Pressure Correction Equation * * * * * * * * * * * * * * //
void SOLVER::SIMPLE::SolvePressureCorrection()
{   
    // First Initialize RHS
    MATH::Vector mdot_imb(_mesh->get_elements().size());

    // pressure correction equation RHS has mass imbalance into cell
    for (const MESH::element& cell : _mesh->get_elements()) {
        for (const MESH::face& f : cell.get_faces()) {
            mdot_imb[cell.get_id()] += _faceMassFluxField.get_internal()[f.get_id()] * _massFluxDirection.get_value(cell.get_id(),f.get_id());
        }
    }

    // std::cout << "Mass flux imbalance: " << std::endl;
    // std::cout << mdot_imb << std::endl;

    // Initialize pressure correction matrix
    double diag;
    double offdiag;
    double w1;
    double mdotbc;
    MESH::element cell2;
    MATH::matrixCSR pc_matrix(_mesh->get_elements().size(),_mesh->get_elements().size());
    for (const MESH::element& cell : _mesh->get_elements()) {
        diag = 0.0;
        for (const MESH::face& f : cell.get_faces()) {
            if (f.is_boundaryFace()) {
                // Pressure correction is solving for mass flux into cell
                mdotbc = rho * (_faceVelocityField.get_internal()[f.get_id()] * cell.get_normals()[cell==f]);
                mdot_imb[cell.get_id()] -= mdotbc;
                continue; // (fine to skip for walls for now, need to fix later...)
            }
            else {
                w1 = cell.get_distanceWeights()[cell==f];
                // Note: faces in cell only contain opposing element
                cell2 = _mesh->get_elements()[f.get_elements()[0]];

                // Assign off diagonal
                offdiag = - (        w1  * cell.get_volume()  / _momentumSystemA.get_value(cell.get_id() ,cell.get_id()  ) 
                              + (1.0-w1) * cell2.get_volume() / _momentumSystemA.get_value(cell2.get_id(),cell2.get_id() ) 
                            ) * rho * f.get_volume() / _faceNormalDeltas[f.get_id()];
                pc_matrix.set_value(cell.get_id(),cell2.get_id(),offdiag);

                // Increment diagonal
                diag += -offdiag;
            }
        }
        pc_matrix.set_value(cell.get_id(),cell.get_id(),diag);
    }


    // Solve the system for the pressure correction
    double iter = 500;
    double tol = 1.0e-6;
    MATH::conjugate_gradient<MATH::matrixCSR> solverpc;
    solverpc.set_matrix(pc_matrix);
    solverpc.set_rhs(mdot_imb);
    solverpc.set_guess(std::vector(_mesh->get_elements().size(),0.0)); // Pressure correction needs to be initialized to 0
    _pressureCorrection = solverpc.solve(iter,tol);
    std::cout << "Pressure Correction solver residual: " << solverpc.get_residual() << " in " << solverpc.get_iterations() << " iterations" << std::endl;

    // RELAX PRESSURE CORRECTION
    _pressureCorrection = _prelax * _pressureCorrection;

    // ****************************************************************************
    // DEBUGGING

    // std::cout << "pressure correction RHS: " << std::endl;
    // std::cout << mdot_imb << std::endl;
    // std::cout << "pressure correction solution: " << std::endl;
    // std::cout << _pressureCorrection << std::endl;

    // ****************************************************************************

    // Correct Pressures
    std::vector<double> new_pressure(_mesh->get_elements().size());
    for (const MESH::element& cell : _mesh->get_elements()) {
        new_pressure[cell.get_id()] = _cellPressureField.get_internal()[cell.get_id()] + _pressureCorrection[cell.get_id()];
    }
    _cellPressureField.set_internal(new_pressure);

    // Update Face pressures
    std::vector<double> pface = computeFacePressure(_cellPressureField.get_internal());
    _facePressureField.set_internal(pface);
            
}


// * * * * * * * * * * * * * Velocity Correction * * * * * * * * * * * * * * //
void SOLVER::SIMPLE::correctCellVelocity()
{
    // Get gradient of pressure correction in the cells
    std::vector<double> pcface = computeFacePressure(_pressureCorrection.get_vector());;

    std::vector<MATH::Vector> vnew = _cellVelocityField.get_internal();

    // Loop through each cell and correct
    for (const MESH::element& cell : _mesh->get_elements()) 
    {
        MATH::Vector vc(_mesh->get_dimension());
        // std::cout << "centroid: " << cell.get_centroid() << std::endl;
        for (int fi=0 ; fi<cell.get_faces().size() ; fi++) {
            // std::cout << "    face normal: " << cell.get_normals()[fi] << "     pc: " << pcface[cell.get_faces()[fi].get_id()] << std::endl;
            //          (face pressure correction)              (face area)                         (OUTWARD pointing face normal)
            vc = vc + ( pcface[cell.get_faces()[fi].get_id()] * cell.get_faces()[fi].get_volume() * (cell.get_normals()[fi]) );
        }
        //          Divide by A0
        vc = vc * (-1.0/_momentumSystemA.get_value(cell.get_id(),cell.get_id()));
        
        // Correct velocities (relaxation done to pressure correction)
        vnew[cell.get_id()] = vnew[cell.get_id()] + vc;
    }

    // Set new velocities
    _cellVelocityField.set_internal(vnew);
}


// * * * * * * * * * * * * * Face Mass Flux Correction * * * * * * * * * * * * * * //
void SOLVER::SIMPLE::correctFaceMassFlux()
{
    // std::vector<double> mdotf_old = _faceMassFluxField.get_internal();
    std::vector<double> mdotf = _faceMassFluxField.get_internal();
    std::vector<double> mdotf_cor(_mesh->get_faces().size(),0.0);
    MESH::element cell1;
    MESH::element cell2;
    double w1;



    for (const MESH::face& f : _mesh->get_faces()) {

        // Re-directionalize mass flux
        //          mass flux relative to cell 1: i.e. mdotf > 0 -> mass flux INTO cell 1
        mdotf[f.get_id()] *= _massFluxDirection.get_value(f.get_elements()[0],f.get_id());

        if (f.is_boundaryFace()) {
            // Mass flux at the boundary, going INTO the cell
            mdotf[f.get_id()] = _BCs[f.get_elements()[1]]->get_massFlux(f.get_id());

            if (mdotf[f.get_id()] > 0.0) {
                _massFluxDirection.set_value(f.get_elements()[0],f.get_id(),1.0);
            }
            else {
                _massFluxDirection.set_value(f.get_elements()[0],f.get_id(),-1.0);
            }
        }
        else
        {
            cell1 = _mesh->get_elements()[f.get_elements()[0]];
            cell2 = _mesh->get_elements()[f.get_elements()[1]];

            w1 = cell1.get_distanceWeights()[cell1==f];

            // Calculate mass flux correction going INTO cell 1
            mdotf_cor[f.get_id()] = -1.0 * rho*f.get_volume() 
                                        * (w1*cell1.get_volume()/_momentumSystemA.get_value(cell1.get_id(),cell1.get_id()) 
                                                    + (1-w1)*cell2.get_volume()/_momentumSystemA.get_value(cell2.get_id(),cell2.get_id()))
                                        * ( _pressureCorrection[cell2.get_id()] - _pressureCorrection[cell1.get_id()]) / _faceNormalDeltas[f.get_id()] ;

            // Correct face
            mdotf[f.get_id()] += mdotf_cor[f.get_id()];

            // std::cout << cell1.get_centroid() << "   :   " << f.get_normal() << "   :   " << mdotf[f.get_id()] << std::endl;

            // Update mass flux direction
            if (mdotf[f.get_id()] > 0.0) {
                // Mass flux going INTO the first cell
                _massFluxDirection.set_value(f.get_elements()[0],f.get_id(),1.0);
                _massFluxDirection.set_value(f.get_elements()[1],f.get_id(),-1.0);
            }
            else {
                // Mass flux going OUT OF the first cell
                _massFluxDirection.set_value(f.get_elements()[0],f.get_id(),-1.0);
                _massFluxDirection.set_value(f.get_elements()[1],f.get_id(),1.0);
            }

        }
    }

    // absolute value of mass flux
    std::transform( mdotf.begin(), mdotf.end(), mdotf.begin(), [](double x){ return std::fabs(x); } );

    // Set new face mass flux
    _faceMassFluxField.set_internal(mdotf);
}


// * * * * * * * * * * * * * check for solution convergence * * * * * * * * * * * * * * //
bool SOLVER::SIMPLE::checkConvergence()
{
    double presid = _pressureCorrection.getL2Norm();
    MATH::Vector v_resid = _cellVelocityField.get_residual();

    // Output convergences
    std::cout << "Pressure correction convergence: " << _pressureCorrection.getL2Norm() << std::endl;
    for (int i=0 ; i<_mesh->get_dimension() ; i++ )
    {
        std::cout << "Velocity u" << i+1 << " convergence: " << v_resid[i] << std::endl;
    }

    // First check pressure convergence
    if ( presid < ptol )
    {
        // Check velocity residuals
        
        for (int i=0 ; i<_mesh->get_dimension() ; i++ )
        {
            if ( v_resid[i] > utol )
            {
                return false;
            }
        }
    }
    else
    {
        return false;
    }

    return true;
}
