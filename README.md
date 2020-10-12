# Laminate_calc

Laminate_calc is a C++ code to read the conventional composite stack layup 
notation, and calculate the properties, stresses and strains of composite 
material laminate. A python script is used to generate stress and strain profile 
plots.

This code uses Eigen for matrix operation and linear algebra calculation. Related
files (Version eigen-3.3.7) are included in the `lib` folder for convenience in 
compilation. For licensing and detail information of Eigen please visit:
http://eigen.tuxfamily.org/ .

## Quick Start

Clone the repository in your local directory:

`git clone https://github.com/quentin-tw/laminate_calc.git`

To simply run the program and see the results, only `laminate_main`,
`input_files/laminate_input.lmc`, `input_files/material_data.lmc` are required.

`laminate_input.lmc` is a simple text file containing inputs for the 
`laminate_main`. The following is an example of `laminate_input.lmc`
```
Laminate Code  : [0/45/-45/90]8s4
Material Label : [M1, M2, M1, M2]
Ply Thickness  : [2e-4, 1.5e-4, 2e-4, 1.5e-4]
Load Vector    : [7e6, 0, 0, 0, 0, 0]
```
`material_data.lmc` is also a simple text file, which used as a source for 
material data. The material labels are used in `laminate_input.lmc` to identiy
the material properties of a laminate.

An example `material_data.lmc` is provided as below:

```
Label           E1          E2          nu12        G12
M1              1.38e11     1.00e10     0.34        7.00e9
M2              1.00e11     2.00e10     0.25        1.200e10
```

For detail information of the file format, see `include/input_parser.h`.

After setting up `laminate_input.lmc` and `material_data.lmc`, run the program
to obtain the calculation result:
```
./laminate_main
```

The output data `laminate_profile_data.txt` and `stiffness_submatrices_ABD.txt`
will be generated in the `output_file` folder. The `laminate_profile_date.txt`
contains the vertical coordinates, stress_x, stress_y, stress_xy, strain_x, 
strain_y, strain_xy in the respective order. The `stiffness_submatrices_ABD.txt`
contains the A, B, and D submatrices of the laminate stiffness matrix.

Once the data are obtained, the python script `profile_plot.py` can be used to 
generate stresses and strain profile plots (A python3 interpreter with numpy 
and matplotlib library is required): 

```
python3 ./profile_ploy.py
```

A plot `laminate_profile_plot.png` will be generated in the `output_files` folder.

![ProfilePlotResult](/output_files/laminate_profile_plot.png)

