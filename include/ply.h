/**
 * Ply object(a struct), which represents a single layer of the laminate.
 */

#ifndef PLY_H
#define PLY_H

#include <Eigen/Dense>
#include <cmath>
#include <iostream>
#include <utility>
#include <string>

//! Material properties of a composite ply.
struct Properties {
    
    //! Young's modulus in the first principle direction.
    double E1;

    //! Young's modulus in the second principle direction.
    double E2;

    //! Poisson's ration in the 12 direction.
    double nu12;

    //! Shear modulus in the 12 direction.
    double G12;

};

//! Ply struct contains information of a ply (unidirectional lamina for a given
//! layout orientation.)
struct ply {

    //! The material name of the ply
    const std::string material_label_;

    //! The material properties of the ply
    const Properties material_properties_;

    //! Ply fiber orientation
    const double theta_;

    //! Thickness of the ply.
    const double thickness_;

    //! The stiffness matrix in the laminate coordinates.
    //! Note: setting Eigen Matrix to const will induce more complicate behaviors,
    //! so here Eigen is not used.
    Eigen::Matrix3d Qbar_;
    
    // Ply constructor to read in material properties, ply orientations, 
    // thickness, and calculate the stiffness matrix in the laminate coordinates.
    ply(const std::string material_label, const Properties material_properties, 
        const double theta, const double thickness);
};

#endif