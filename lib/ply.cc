#include <Eigen/Dense>
#include <cmath>
#include <iostream>
#include "../include/ply.h"




// contruct the stiffness matrix of the ply in the ply coordinates. P
Eigen::Matrix3d build_Q(const Properties& p);

double to_radian(double angle_degree);

using std::cin; using std::cout; using std::endl;
using Eigen::Matrix3d;

ply::ply(const std::string material_label, 
    const Properties material_properties, 
    const double theta, const double thickness):
    material_label_(material_label), 
    material_properties_(material_properties), theta_(theta), 
    thickness_(thickness) {
    
    double angle_radian = to_radian(theta);
    double s = sin(angle_radian);
    double c = cos(angle_radian);
    Matrix3d T_stress_inv;
    Matrix3d T_strain;

    T_stress_inv << pow(c,2), pow(s,2), -2*s*c,
                    pow(s,2), pow(c,2),  2*s*c,
                    s*c     , -s*c    ,  pow(c,2) - pow(s,2);

    T_strain << pow(c,2), pow(s,2),  s*c,
                pow(s,2), pow(c,2), -s*c,
                -2*s*c     , 2*s*c  ,  pow(c,2) - pow(s,2);    
    
    Matrix3d Q = build_Q(material_properties_);

    // Transformation from ply coordinates to laminate coordinates.
    Qbar_ = T_stress_inv * Q * T_strain;

}

Matrix3d build_Q(const Properties& p) {
    double D = 1 - p.E2 / p.E1 * pow(p.nu12, 2);
    Matrix3d Q;

    Q << p.E1/D       , p.nu12*p.E2/D, 0,
         p.nu12*p.E2/D, p.E2/D       , 0,
         0            , 0            , p.G12;

    return Q;
}

double to_radian(double angle_degree) {
    return angle_degree * M_PI/180;
}

