#include <iostream>
#include <Eigen/Dense>
#include <vector>
#include <array>
#include <string>
#include <cmath>

#include "../include/input_parser.h"
#include "../include/ply.h"
#include "../include/laminate.h"

using std::cin; using std::cout; using std::endl;
using std::string;
using std::vector;
using Eigen::Matrix; using Eigen::Matrix3d; using Eigen::Vector3d;

// Get the mid-plane strain of the laminate.
void solve_mid_strain(laminate& lam, Matrix<double, 6, 1>& load_vector);

// Get the stresses and strains for each sampling point of the laminate.
void solve_stress_strain_profile(laminate& lam, double pt_spacing);

// Construct laminate from a vector of ply, the input load, and the spacing
// between sampling points.
laminate::laminate(vector<ply>& ply_vector, Matrix<double, 6, 1>& load_vector,
                    double pt_spacing): 
        ply_vector_(ply_vector), load_vector_(load_vector) {
    height_ = 0.;
    for (auto it = ply_vector_.begin(); it != ply_vector_.end(); it++) {
        height_ += it->thickness_;
    }
    
    A_ = Matrix3d::Zero();
    B_ = Matrix3d::Zero();
    D_ = Matrix3d::Zero();
    double bottom_coordinate = -height_/2;
    for (auto it = ply_vector_.begin(); it != ply_vector_.end(); it++) {
        double top_coordinate = bottom_coordinate + it->thickness_;
        A_ = A_ + it->Qbar_ * (top_coordinate - bottom_coordinate);
        B_ = B_ + 1./2 * it->Qbar_ 
            * (pow(top_coordinate, 2) - pow(bottom_coordinate, 2));
        D_ = D_ + 1./3 * it->Qbar_
            * (pow(top_coordinate, 3) - pow(bottom_coordinate, 3));
        
        bottom_coordinate = top_coordinate;
    }
    solve_mid_strain(*this, load_vector_);
    solve_stress_strain_profile(*this, pt_spacing);

}

void solve_mid_strain(laminate& lam, Matrix<double, 6, 1>& load_vector) {
    Matrix<double, 6, 6> stiffness = Matrix<double, 6, 6>::Zero();
    stiffness.block<3, 3>(0, 0) = lam.A_;
    stiffness.block<3, 3>(0, 3) = lam.B_;
    stiffness.block<3, 3>(3, 0) = lam.B_;
    stiffness.block<3, 3>(3, 3) = lam.D_;
    Matrix<double, 6, 1> strain_vector = 
        stiffness.colPivHouseholderQr().solve(load_vector);
    lam.mid_strain_ = strain_vector.head<3>();
    lam.mid_curvature_ = strain_vector.tail<3>();
}

void solve_stress_strain_profile(laminate& lam, double pt_spacing) {
       
    lam.profile_pt_.push_back(-lam.height_/2);
    lam.strains_.push_back(
        lam.mid_strain_ + lam.profile_pt_.back() * lam.mid_curvature_);
    vector<ply>::size_type current_layer = 0;  
    lam.stresses_.push_back(
        lam.ply_vector_[current_layer].Qbar_ * lam.strains_.back());

    double current_bottom_pt = -lam.height_/2;
    double current_top_pt = current_bottom_pt
                          + lam.ply_vector_[current_layer].thickness_;
    while (lam.profile_pt_.back() <= lam.height_/2) {
        double next_profile_pt = lam.profile_pt_.back() + pt_spacing;
        if (next_profile_pt > current_top_pt) {
            current_bottom_pt = current_top_pt;
            current_layer++;
            current_top_pt = current_bottom_pt
                           + lam.ply_vector_[current_layer].thickness_;
        }
        lam.profile_pt_.push_back(lam.profile_pt_.back() + pt_spacing);
        lam.strains_.push_back(
            lam.mid_strain_ + lam.profile_pt_.back() * lam.mid_curvature_);
        lam.stresses_.push_back(
            lam.ply_vector_[current_layer].Qbar_ * lam.strains_.back());
    }
}


