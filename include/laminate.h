/**
 * The laminate struct contains the material information of each plys, 
 * the overall mechanical properties of the laminate, and the response of the
 * laminate subject to the external loads. 
 */



#include <Eigen/Dense>
#include <vector>
#include <array>
#include <string>
#include <cmath>

#include "ply.h"

struct laminate {
        //! A vector containing ply struct from bottom to top.
        std::vector<ply> ply_vector_;

        // The height of the laminate.
        double height_;
        
        // A_, B_, D_ is the submatrices of the stiffness matrix of the ply.
        Eigen::Matrix3d A_;
        Eigen::Matrix3d B_;
        Eigen::Matrix3d D_;
        
        //! Engineering strains(epsilon_x, epsilon_y, epsilon_xy) at the 
        //! mid-plane of the laminate
        Eigen::Vector3d mid_strain_;

        //! The curvature(kappa_x, kappa_y, kappa_xy) at the mid-plane
        //! of the laminate.
        Eigen::Vector3d mid_curvature_;

        //! Stresses (sigma_x, sigma_y, sigma_xy) at each sampling point of the
        //! laminate. First element of the vector corresponds to the bottom point.
        std::vector<Eigen::Vector3d> stresses_;

        //! Strains (epsilon_x, epsilon_y, epsilon_xy) at each sampling point 
        //! of the laminate. First element of the vector corresponds 
        //! to the bottom point.
        std::vector<Eigen::Vector3d> strains_;

        //! The coordinates of each sampling point.
        std::vector<double> profile_pt_;
        
        //! The in plane forces (Nx, Ny, Nxy) and moments(Mx, My, Mxy) 
        //! applied to the laminate.
        Eigen::Matrix<double, 6, 1> load_vector_;
        
        //! The laminate contructor. ply_vector and load_vector comes from material
        //! data. pt_spacing is the distance between sampling point of the laminate.
        laminate(std::vector<ply>& ply_vector, 
                Eigen::Matrix<double, 6, 1>& load_vector, double pt_spacing);
};