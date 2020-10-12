//! Implementation of the input_parser.

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <utility>
#include <stack>
#include <algorithm>
#include <Eigen/Dense>
#include "../include/ply.h"
#include "../include/input_parser.h"


//! The subscript of the laminate code. e.g. [0/45/45/90]8s4. 8 is the pre_count,
//! s is the has_symmetry, and 4 is the post_count.
struct SubscriptInfo {
    int pre_count;
    bool has_symmetry;
    int post_count;
};

//! Read material_data file and return the format into a map.
std::map<std::string, Properties> 
    load_material_data(const std::string& filename);

//! Parse the laminate code and return a pair in which the first elemnt is the
//! vector containing ply angles, and the second element is the SubscriptInfo
//! of the given laminate. 
std::pair<std::vector<double>, SubscriptInfo> 
    laminate_code_parser(std::string laminate_code);

// Parse the trailing info of the laminate code and return the info into the
// SubscriptInfo struct.
SubscriptInfo subscript_parser(std::string subscript);

//! Convert the laminate information into a vector of ply objects.
std::vector<ply> build_laminate_vector(
    const std::pair<std::vector<double>, SubscriptInfo>& layout_info,
    std::vector<std::string>& ply_materials,
    const std::vector<double>& ply_thickness, 
    const std::map<std::string, Properties>& material_map);

//! Strip the left and right square bracket of a string.
std::string strip_bracket(std::string input_str);

//! Convert the string inside bracket into vector of doubles.
std::vector<double> strs_to_vector(std::string code);

//! Convert the string inside bracket into vector of strings. Used for the
//! material data parsing.
std::vector<std::string> mat_str_to_vector(std::string strings);

using std::string;
using std::cin; using std::cout; using std::endl;
using std::vector; using std::map; using std::stack; using std::pair;

vector<string> read_composite_input(const string& filename) {
    vector<string> result;
    string line;
    std::ifstream file(filename);
    if (file.is_open()) {
        while (getline(file, line)) {
            if(line.find("[") != string::npos && 
                line.find("]") != string::npos) {
                string::size_type l_bracket_pos = line.find("[");
                result.push_back(line.substr(l_bracket_pos));
            }
        }
    }
    return result;
}

vector<ply> get_ply_vector(vector<string>& input_strings, 
    const string& material_data_filename) {
        
        map<string, Properties> material_data = 
            load_material_data(material_data_filename);

        pair<vector<double>, SubscriptInfo> layout_info =
            laminate_code_parser(input_strings[0]);

        string material_strings = strip_bracket(input_strings[1]);
        vector<string> ply_materials = 
            mat_str_to_vector(material_strings);
        
        string thickness_strings = strip_bracket(input_strings[2]);

        vector<double> ply_thickness =
            strs_to_vector(thickness_strings);
            
        vector<ply> laminate = build_laminate_vector(layout_info, 
            ply_materials, ply_thickness, material_data);
    
    return laminate;
    }

Eigen::Matrix<double, 6, 1> get_load_vector(string& input_string) {
    string load_string = strip_bracket(input_string);
    vector<double> load_stl_vector = strs_to_vector(load_string);
    Eigen::Matrix<double, 6, 1> load_vector(load_stl_vector.data());
    return load_vector;

}

double get_minimum_ply_thickness(string& thickness_strings_with_brackets) {
    string thickness_strings = strip_bracket(thickness_strings_with_brackets);
    vector<double> ply_thickness =
        strs_to_vector(thickness_strings);
    auto min_it = std::min_element(ply_thickness.begin(), 
                                    ply_thickness.end());
    
    double minimum = *min_it;
    
    return minimum;
}

map<string, Properties> load_material_data(const string& filename) {
    map<string, Properties> data;
    string line;
    std::ifstream input_file(filename);
    if (input_file.is_open()) {
        string material_name;
        Properties p;
        input_file.ignore(500, '\n');  // Ignore the first line
        while (!input_file.eof()) {
            input_file >> material_name >> p.E1 >> p.E2 >> p.nu12 >> p.G12;
            if (!input_file) {
                cout << "Error: file corrupted. Lines After " 
                    "corrupted line are not read."<< endl;
                break;
            }
            data.insert({material_name, p});
        }
    } else {
        cout << "Error: Cannot open file." ;
    }
    return data;
}

string strip_bracket(string input_str) {
    std::size_t left_bracket_pos = input_str.find("[");
    std::size_t right_bracket_pos = input_str.find("]");
    return input_str.substr(left_bracket_pos+1, 
        right_bracket_pos-left_bracket_pos-1);

}

pair<vector<double>, SubscriptInfo> laminate_code_parser(string laminate_code) {
    std::size_t left_bracket_pos = laminate_code.find("[");
    std::size_t right_bracket_pos = laminate_code.find("]");
    pair<vector<double>, SubscriptInfo> result;
    if (left_bracket_pos == string::npos || right_bracket_pos == string::npos
        || left_bracket_pos > right_bracket_pos) {
            cout << "Invalid laminate code input." << endl;
        return result;
    }
    string theta_string = laminate_code.substr(left_bracket_pos+1, 
        right_bracket_pos-left_bracket_pos-1);
    string subscript_string = laminate_code.substr(right_bracket_pos+1, 
        laminate_code.size()-right_bracket_pos-1);
    result.first = strs_to_vector(theta_string);
    result.second = subscript_parser(subscript_string);
    return result;
}

vector<ply> build_laminate_vector(
    const pair<vector<double>, SubscriptInfo>& layout_info,
    vector<string>& ply_materials,
    const vector<double>& ply_thickness, 
    const std::map<string, Properties>& material_map) {
        
        vector<double> theta_vec = layout_info.first;
        SubscriptInfo info = layout_info.second;

        vector<ply> laminate_vec;
        stack<ply> ply_stack;
        
        // It's not optimized in that we don't need the stack operation if 
        // we don't have the symmetry subscript, but this arrangement 
        // makes the code less redundant.
        for (int i = 1; i <= info.pre_count; i++) {
            auto theta_it = theta_vec.begin();
            auto mat_it = ply_materials.begin();
            auto t_it = ply_thickness.begin();
            for (; theta_it != theta_vec.end(); theta_it++, mat_it++, t_it++) {
                ply local_ply(*mat_it, material_map.at(*mat_it), 
                    *theta_it, *t_it);
                laminate_vec.push_back(local_ply);
                ply_stack.push(local_ply); // for symmetric condition
            }
        }

        if (info.has_symmetry) {
            while (!ply_stack.empty()) {
                laminate_vec.push_back(ply_stack.top());
                ply_stack.pop();
            }
        }

        if (info.post_count > 1) {
            vector<ply> laminate_copy(laminate_vec);
            for (int i = 2; i <= info.post_count; i++) {
                for (auto it = laminate_copy.begin(); 
                    it != laminate_copy.end(); it++) {
                    laminate_vec.push_back(*it);
                }

            }
        }

    
    return laminate_vec;

}

std::vector<double> strs_to_vector(string theta_string) {
    std::replace(theta_string.begin(), theta_string.end(), '/', ' ');
    std::replace(theta_string.begin(), theta_string.end(), ',', ' ');
    std::istringstream theta_stream(theta_string);
    std::vector<double> theta_vec;
    double theta;
    while (theta_stream >> theta) {
        theta_vec.push_back(theta);
    }

    return theta_vec;
}

std::vector<string> mat_str_to_vector(string strings) {
    std::replace(strings.begin(), strings.end(), ',', ' ');
    std::istringstream stream(strings);
    std::vector<string> vec;
    string single_string;
    while (stream >> single_string) {
        vec.push_back(single_string);
    }
    return vec;
}

SubscriptInfo subscript_parser(string subscript) {
    SubscriptInfo info;
    std::size_t s_pos = subscript.find("s");
    if (s_pos != string::npos) {  // subscript contains an 's'
        info.has_symmetry = true;

        if (s_pos != 0) {  // subscript not at the beginning
            string pre_repetition_string = subscript.substr(0, s_pos);
            if (std::stoi(pre_repetition_string) > 0) {
                info.pre_count = std::stoi(pre_repetition_string);
            } else {
                info.pre_count = 1;
            }
        } else {
            info.pre_count = 1;
        }

        if (s_pos != subscript.size() - 1) {  // subscript not at the end
            string post_repetition_string = 
                subscript.substr(s_pos+1, subscript.size());
            info.post_count = std::stoi(post_repetition_string);
        } else {
            info.post_count = 1;
        }

    } else {
        string pre_repetition_string = subscript.substr(0, subscript.size());
        info.pre_count = std::stoi(pre_repetition_string);
        info.has_symmetry = false;
        info.post_count = 0;
    }
    return info;
}