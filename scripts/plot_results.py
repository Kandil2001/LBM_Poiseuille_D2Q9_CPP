import pandas as pd
import matplotlib.pyplot as plt

profile = pd.read_csv("results/centerline_profile.csv")
conv = pd.read_csv("results/convergence.csv")

plt.figure()
plt.plot(profile["lbm_ux"], profile["y"], "o", label="LBM")
plt.plot(profile["analytical_ux"], profile["y"], "-", label="Analytical")
plt.xlabel("u velocity")
plt.ylabel("y")
plt.title("Poiseuille velocity profile")
plt.legend()
plt.grid(True)
plt.tight_layout()
plt.savefig("results/velocity_profile.png", dpi=200)

plt.figure()
plt.semilogy(conv["step"], conv["max_change"])
plt.xlabel("time step")
plt.ylabel("max change in ux")
plt.title("Convergence")
plt.grid(True)
plt.tight_layout()
plt.savefig("results/convergence.png", dpi=200)

print("Saved plots in results/")
