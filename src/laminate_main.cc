/**
 * laminate_main reads the input file `laminate_input` and `material_data`,
 * construct a laminate model containing material properties and response to
 * the load, and save the stress and strain profile into 
 * `laminate_profile_data.txt`, and save the A,B, and D submatrices into
 * `stifness_submatrices ABD.txt`.
 * 
 * `laminate_profile_data.txt` contains the following columns (in order):
 * Coordinates, stress_x, stress_y, stress_xy, strain_x, strain_y, strain_xy.
 * 
 * `stiffness_submatrices_ABD.txt` contains the A, B, and D submatrices of the
 * composite laminate, in the given order separate by new lines.
 * 
 */

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <Eigen/Dense>
#include "../include/input_parser.h"
#include "../include/ply.h"
#include "../include/laminate.h"

void save_laminate_profile(laminate& lam);

int main() {
    std::vector<std::string> input_strings = 
        read_composite_input("input_files/laminate_input.lmc");
    std::vector<ply> ply_vector = 
        get_ply_vector(input_strings, "input_files/material_data.lmc");
    Eigen::Matrix<double, 6, 1> load_vector = get_load_vector(input_strings[3]);
    double min_thickness = get_minimum_ply_thickness(input_strings[2]);
    double pt_spacing = min_thickness/20.;
    laminate lam(ply_vector, load_vector, pt_spacing);
    save_laminate_profile(lam);
    std::cout << "Laminate_main -- Data saved." << std::endl;
    return 0;
}

void save_laminate_profile(laminate& lam) {
    std::ofstream profile_file;
    profile_file.open("output_files/laminate_profile_data.txt");
    auto i_sigma = lam.stresses_.begin();
    auto i_eps = lam.strains_.begin();
    auto i_pt = lam.profile_pt_.begin();
    for (; i_sigma != lam.stresses_.end(); i_sigma++, i_eps++, i_pt++) {
        Eigen::Matrix<double, 7, 1> combined_data;
        combined_data << *i_pt, *i_sigma, *i_eps;
        
        Eigen::Map<Eigen::Matrix<double, 1, 7>> 
            row(combined_data.data(), combined_data.size());
        profile_file << row << std::endl;
    }
    std::ofstream stiffness_file;
    stiffness_file.open("output_files/stiffness_submatrices ABD.txt");
    stiffness_file << lam.A_;
    stiffness_file << std::endl<< std::endl;
    stiffness_file << lam.B_;
    stiffness_file << std::endl << std::endl;
    stiffness_file << lam.D_;
    stiffness_file << std::endl << std::endl;
}