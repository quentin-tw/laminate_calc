#! /usr/bin/python3

""" A script to plot the stress and strain profile of a laminate."""

import numpy as np
import matplotlib.pyplot as plt

def plot_profile(input_filename):

    input_data = np.loadtxt(input_filename)

    fig, axs = plt.subplots(2, 3, figsize=(10,8))
    plt.subplots_adjust(wspace=0.3, hspace=0.25)
    fig.suptitle('Laminate Stress/Strain Profiles')
    axs = axs.flatten()
    profile_linewidth = 0.6

    for i in range(6):
        
        # Plot basic profile. Top ax row is stresses, Bottom ax row is strains.
        axs[i].plot(input_data[:, i+1], input_data[:, 0], 
            linewidth=profile_linewidth)

        # Add dotted lines at bottom to indicate the laminate boundaries.
        axs[i].plot([0, input_data[0, i+1]], 
                    [input_data[0, 0], input_data[0, 0]],
            'b--', linewidth=profile_linewidth)

        # Similar as above, add lines at top.
        axs[i].plot([0, input_data[-1, i+1]], 
                    [input_data[-1, 0], input_data[-1, 0]],
            'b--', linewidth=profile_linewidth)

        # Adjust xlim to display the stress/strain profile evenly
        column_abs_max = np.amax(np.absolute(input_data[:, i+1]))
        axs[i].set_xlim([-column_abs_max * 1.2 , column_abs_max* 1.2])

        # Add axes for every subplot as a reference
        axs[i].axhline(0, color='k', linewidth=profile_linewidth*1.5)
        axs[i].axvline(0, color='k', linewidth=profile_linewidth*1.5)
        
        # rotate tick labels of strains since they usually have long decimals.
        if (i > 2):
            axs[i].tick_params(axis='x', labelrotation=30)

    axs[0].set_xlabel(r'$\sigma_{x}$')
    axs[1].set_xlabel(r'$\sigma_{y}$')
    axs[2].set_xlabel(r'$\tau_{xy}$')
    axs[3].set_xlabel(r'$\epsilon_{x}$')
    axs[4].set_xlabel(r'$\epsilon_{y}$')
    axs[5].set_xlabel(r'$\gamma_{xy}$')

    fig.savefig("output_files/laminate_profile_plot.png", dpi=300)
    print("profile plot completed.")

if __name__ == '__main__':
    plot_profile("output_files/laminate_profile_data.txt")