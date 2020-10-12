/**
 * Functions for parsing input file `laminate_input` and `material_data`.
 * Both input files should be simple text files with the following format:
 * 
 * `laminate_input`: The parser will only read lines with enclosing square 
 * brackets, and all information before "[" (but not "]") will be ignored.
 * The file should contain 4 lines in the given sequence--
 *      Laminate Code: The standard notation describe the stack sequence 
 *      of a composite laminate per Classical Laminate Theory (CLT), 
 *      e.g. `[0/45/-45/90]2s2`. The string after the right bracket served as 
 *      the subscript of the notation. The forward slash delimiter "/" can be 
 *      replaced by a space " " or a comma ",".
 *      
 *      Material Label: The material name that will be used for each ply in the
 *      composite laminate, corresponds to the element inside the square 
 *      bracket in the laminate code. e.g. [M1, M2, M1, M2], if corresponds to
 *      the example in the laminate code, means the material M1 is used for
 *      for 0 degree and -45 degree ply, and the material M2 is used for the 45
 *      degree and 90 degree ply. The delimiter ", " can be replace by space " "
 *      but not forward slash (because the material label might contain slashes).
 * 
 *      Ply Thickness: The thickness for each ply, corresponds to the element
 *      inside the square bracket in the laminate code.
 * 
 *      Load Vector: The load applied to the composite laminate, with the format
 *      as [Nx, Ny, Nxy, Mx, My, My]. Nx, Ny, Nxy denote in-plane forces of 
 *      the laminate, M denote moments apply to the laminate.
 * 
 * `material_data`: First line of the file will be ignored (served as the 
 * column labels). The remaining line should be datas for each materials.
 * each line should have the following properties in the given sequence, 
 * separated by spaces: 
 *      material label (material name)
 *      E1 (Young's modulus in the first principle direction)
 *      E2 (Young's modulus in the second principle direction)
 *      nu12 (Poisson's ratio in the 12 direction)
 *      G12 (Shear modulus in the 12 direction)
 */


#ifndef INPUT_PARSER_H
#define INPUT_PARSER_H

#include <string>
#include <vector>
#include "ply.h"

//! Read the laminate_input file, strip lines without matching square brackets 
//! and strings before left bracket, and return a vector containing each line
//! of the file.
std::vector<std::string> read_composite_input(const std::string& filename);

//! Read the laminate code string and the material_data, and return a vector
//! containing plys of the laminates, with the first element of the vector
//! represents the bottom ply, the last element represents the top ply.
std::vector<ply> get_ply_vector(
    std::vector<std::string>& laminate_strings,
    const std::string& material_data_filename);

//! Read the load vector string and returns the Eigen::vector object.
Eigen::Matrix<double, 6, 1> get_load_vector(std::string& load_vector_string);

//! Get minimum thickness of all plys. Used for calculating sampling point
//! spacing of the laminate.
double get_minimum_ply_thickness(std::string& thickness_strings_with_brackets);

#endif