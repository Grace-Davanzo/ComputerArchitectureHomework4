import matplotlib.pyplot as plt
import numpy as np
import matplotlib.patches as mpatches

# Set style for professional look
plt.style.use('seaborn-v0_8-darkgrid')
plt.rcParams['figure.figsize'] = (14, 10)
plt.rcParams['font.size'] = 11
plt.rcParams['font.family'] = 'sans-serif'

# ============================================
# DATA FROM YOUR RESULTS
# ============================================

# Baseline (Unoptimized) - gcc -O0
baseline_1gb_time = 42.3322
baseline_2gb_time = 87.1689
baseline_4gb_time = 180.9564
baseline_1gb_cost = 0.00117589
baseline_2gb_cost = 0.00121068
baseline_4gb_cost = 0.00125664
baseline_1gb_throughput = 0.0236
baseline_2gb_throughput = 0.0229
baseline_4gb_throughput = 0.0221

# Optimized - 1 Thread
opt_1t_1gb_time = 23.5175
opt_1t_2gb_time = 49.2152
opt_1t_4gb_time = 103.0593
opt_1t_1gb_cost = 0.00065327
opt_1t_2gb_cost = 0.00068354
opt_1t_4gb_cost = 0.00071569
opt_1t_1gb_throughput = 0.0425
opt_1t_2gb_throughput = 0.0406
opt_1t_4gb_throughput = 0.0388

# Optimized - 2 Threads
opt_2t_1gb_time = 13.3410
opt_2t_2gb_time = 26.9396
opt_2t_4gb_time = 56.4735
opt_2t_1gb_cost = 0.00037058
opt_2t_2gb_cost = 0.00037416
opt_2t_4gb_cost = 0.00039218
opt_2t_1gb_throughput = 0.0750
opt_2t_2gb_throughput = 0.0742
opt_2t_4gb_throughput = 0.0708

# Optimized - 4 Threads
opt_4t_1gb_time = 7.6848
opt_4t_2gb_time = 19.3038
opt_4t_4gb_time = 40.1050
opt_4t_1gb_cost = 0.00021347
opt_4t_2gb_cost = 0.00026811
opt_4t_4gb_cost = 0.00027851
opt_4t_1gb_throughput = 0.1301
opt_4t_2gb_throughput = 0.1036
opt_4t_4gb_throughput = 0.0997

# Optimized - 8 Threads
opt_8t_1gb_time = 7.8883
opt_8t_2gb_time = 13.7052
opt_8t_4gb_time = 28.5950
opt_8t_1gb_cost = 0.00021912
opt_8t_2gb_cost = 0.00019035
opt_8t_4gb_cost = 0.00019858
opt_8t_1gb_throughput = 0.1268
opt_8t_2gb_throughput = 0.1459
opt_8t_4gb_throughput = 0.1399

# ============================================
# CREATE NEW FIGURE: BASELINE VS OPTIMIZED COMPARISON
# ============================================

fig_comparison = plt.figure(figsize=(18, 12))
fig_comparison.suptitle('Baseline vs Optimized Merge Sort - Comprehensive Comparison\nOriginal Code (gcc -O0) vs Optimized Code (gcc -O3 -fopenmp)', 
                       fontsize=16, fontweight='bold', y=0.995)

# ============================================
# GRAPH 1: Time Taken Comparison
# ============================================
ax1 = plt.subplot(2, 3, 1)

datasets = ['1 GB', '2 GB', '4 GB']
baseline_times = [baseline_1gb_time, baseline_2gb_time, baseline_4gb_time]
opt_4t_times = [opt_4t_1gb_time, opt_4t_2gb_time, opt_4t_4gb_time]
opt_8t_times = [opt_8t_1gb_time, opt_8t_2gb_time, opt_8t_4gb_time]

x = np.arange(len(datasets))
width = 0.25

bars1 = ax1.bar(x - width, baseline_times, width, label='Baseline (Original)', 
                color='#d62728', edgecolor='black', linewidth=1.2, alpha=0.8)
bars2 = ax1.bar(x, opt_4t_times, width, label='Optimized (4 Threads)', 
                color='#2ca02c', edgecolor='black', linewidth=1.2, alpha=0.8)
bars3 = ax1.bar(x + width, opt_8t_times, width, label='Optimized (8 Threads)', 
                color='#1f77b4', edgecolor='black', linewidth=1.2, alpha=0.8)

# Add value labels
for i, (b, o4, o8) in enumerate(zip(baseline_times, opt_4t_times, opt_8t_times)):
    ax1.text(x[i] - width, b + 3, f'{b:.1f}s', ha='center', va='bottom', 
             fontsize=9, fontweight='bold')
    ax1.text(x[i], o4 + 3, f'{o4:.1f}s', ha='center', va='bottom', 
             fontsize=9, fontweight='bold')
    ax1.text(x[i] + width, o8 + 3, f'{o8:.1f}s', ha='center', va='bottom', 
             fontsize=9, fontweight='bold')

ax1.set_ylabel('Time (seconds)', fontweight='bold', fontsize=12)
ax1.set_title('⏱️ Execution Time Comparison', fontweight='bold', fontsize=13, pad=15)
ax1.set_xticks(x)
ax1.set_xticklabels(datasets, fontsize=11)
ax1.legend(fontsize=10, loc='upper left')
ax1.grid(axis='y', alpha=0.4, linestyle='--')

# ============================================
# GRAPH 2: Throughput Comparison
# ============================================
ax2 = plt.subplot(2, 3, 2)

baseline_throughputs = [baseline_1gb_throughput, baseline_2gb_throughput, baseline_4gb_throughput]
opt_4t_throughputs = [opt_4t_1gb_throughput, opt_4t_2gb_throughput, opt_4t_4gb_throughput]
opt_8t_throughputs = [opt_8t_1gb_throughput, opt_8t_2gb_throughput, opt_8t_4gb_throughput]

bars1 = ax2.bar(x - width, baseline_throughputs, width, label='Baseline (Original)', 
                color='#d62728', edgecolor='black', linewidth=1.2, alpha=0.8)
bars2 = ax2.bar(x, opt_4t_throughputs, width, label='Optimized (4 Threads)', 
                color='#2ca02c', edgecolor='black', linewidth=1.2, alpha=0.8)
bars3 = ax2.bar(x + width, opt_8t_throughputs, width, label='Optimized (8 Threads)', 
                color='#1f77b4', edgecolor='black', linewidth=1.2, alpha=0.8)

# Add value labels
for i, (b, o4, o8) in enumerate(zip(baseline_throughputs, opt_4t_throughputs, opt_8t_throughputs)):
    ax2.text(x[i] - width, b + 0.003, f'{b:.3f}', ha='center', va='bottom', 
             fontsize=9, fontweight='bold')
    ax2.text(x[i], o4 + 0.003, f'{o4:.3f}', ha='center', va='bottom', 
             fontsize=9, fontweight='bold')
    ax2.text(x[i] + width, o8 + 0.003, f'{o8:.3f}', ha='center', va='bottom', 
             fontsize=9, fontweight='bold')

ax2.set_ylabel('Throughput (GB/s)', fontweight='bold', fontsize=12)
ax2.set_title('🚀 Throughput Comparison', fontweight='bold', fontsize=13, pad=15)
ax2.set_xticks(x)
ax2.set_xticklabels(datasets, fontsize=11)
ax2.legend(fontsize=10, loc='upper left')
ax2.grid(axis='y', alpha=0.4, linestyle='--')

# ============================================
# GRAPH 3: Speedup Comparison
# ============================================
ax3 = plt.subplot(2, 3, 3)

speedup_4t = [baseline_1gb_time / opt_4t_1gb_time, 
              baseline_2gb_time / opt_4t_2gb_time,
              baseline_4gb_time / opt_4t_4gb_time]
speedup_8t = [baseline_1gb_time / opt_8t_1gb_time,
              baseline_2gb_time / opt_8t_2gb_time,
              baseline_4gb_time / opt_8t_4gb_time]

bars1 = ax3.bar(x - width/2, speedup_4t, width, label='Optimized (4 Threads)', 
                color='#2ca02c', edgecolor='black', linewidth=1.2, alpha=0.8)
bars2 = ax3.bar(x + width/2, speedup_8t, width, label='Optimized (8 Threads)', 
                color='#1f77b4', edgecolor='black', linewidth=1.2, alpha=0.8)

# Add value labels with speedup
for i, (s4, s8) in enumerate(zip(speedup_4t, speedup_8t)):
    ax3.text(x[i] - width/2, s4 + 0.15, f'{s4:.2f}x', ha='center', va='bottom', 
             fontsize=10, fontweight='bold')
    ax3.text(x[i] + width/2, s8 + 0.15, f'{s8:.2f}x', ha='center', va='bottom', 
             fontsize=10, fontweight='bold')

# Add horizontal line at y=1 (baseline)
ax3.axhline(y=1, color='red', linestyle='--', linewidth=2, alpha=0.5, label='Baseline (1.0x)')

ax3.set_ylabel('Speedup vs Baseline', fontweight='bold', fontsize=12)
ax3.set_title('⚡ Speedup Factor', fontweight='bold', fontsize=13, pad=15)
ax3.set_xticks(x)
ax3.set_xticklabels(datasets, fontsize=11)
ax3.legend(fontsize=10, loc='upper left')
ax3.grid(axis='y', alpha=0.4, linestyle='--')
ax3.set_ylim(0, max(speedup_4t + speedup_8t) * 1.15)

# ============================================
# GRAPH 4: Cost per GB Comparison
# ============================================
ax4 = plt.subplot(2, 3, 4)

baseline_costs = [baseline_1gb_cost, baseline_2gb_cost, baseline_4gb_cost]
opt_4t_costs = [opt_4t_1gb_cost, opt_4t_2gb_cost, opt_4t_4gb_cost]
opt_8t_costs = [opt_8t_1gb_cost, opt_8t_2gb_cost, opt_8t_4gb_cost]

bars1 = ax4.bar(x - width, baseline_costs, width, label='Baseline (Original)', 
                color='#d62728', edgecolor='black', linewidth=1.2, alpha=0.8)
bars2 = ax4.bar(x, opt_4t_costs, width, label='Optimized (4 Threads)', 
                color='#2ca02c', edgecolor='black', linewidth=1.2, alpha=0.8)
bars3 = ax4.bar(x + width, opt_8t_costs, width, label='Optimized (8 Threads)', 
                color='#1f77b4', edgecolor='black', linewidth=1.2, alpha=0.8)

# Add value labels
for i, (b, o4, o8) in enumerate(zip(baseline_costs, opt_4t_costs, opt_8t_costs)):
    ax4.text(x[i] - width, b + 0.00005, f'${b:.5f}', ha='center', va='bottom', 
             fontsize=8, fontweight='bold')
    ax4.text(x[i], o4 + 0.00005, f'${o4:.5f}', ha='center', va='bottom', 
             fontsize=8, fontweight='bold')
    ax4.text(x[i] + width, o8 + 0.00005, f'${o8:.5f}', ha='center', va='bottom', 
             fontsize=8, fontweight='bold')

ax4.set_ylabel('Cost per GB ($)', fontweight='bold', fontsize=12)
ax4.set_title('💰 Cost Efficiency Comparison', fontweight='bold', fontsize=13, pad=15)
ax4.set_xticks(x)
ax4.set_xticklabels(datasets, fontsize=11)
ax4.legend(fontsize=10, loc='upper left')
ax4.grid(axis='y', alpha=0.4, linestyle='--')

# ============================================
# GRAPH 5: Cost Savings Percentage
# ============================================
ax5 = plt.subplot(2, 3, 5)

savings_4t = [(baseline_1gb_cost - opt_4t_1gb_cost) / baseline_1gb_cost * 100,
              (baseline_2gb_cost - opt_4t_2gb_cost) / baseline_2gb_cost * 100,
              (baseline_4gb_cost - opt_4t_4gb_cost) / baseline_4gb_cost * 100]
savings_8t = [(baseline_1gb_cost - opt_8t_1gb_cost) / baseline_1gb_cost * 100,
              (baseline_2gb_cost - opt_8t_2gb_cost) / baseline_2gb_cost * 100,
              (baseline_4gb_cost - opt_8t_4gb_cost) / baseline_4gb_cost * 100]

bars1 = ax5.bar(x - width/2, savings_4t, width, label='Optimized (4 Threads)', 
                color='#2ca02c', edgecolor='black', linewidth=1.2, alpha=0.8)
bars2 = ax5.bar(x + width/2, savings_8t, width, label='Optimized (8 Threads)', 
                color='#1f77b4', edgecolor='black', linewidth=1.2, alpha=0.8)

# Add value labels
for i, (s4, s8) in enumerate(zip(savings_4t, savings_8t)):
    ax5.text(x[i] - width/2, s4 + 1.5, f'{s4:.1f}%', ha='center', va='bottom', 
             fontsize=10, fontweight='bold')
    ax5.text(x[i] + width/2, s8 + 1.5, f'{s8:.1f}%', ha='center', va='bottom', 
             fontsize=10, fontweight='bold')

ax5.set_ylabel('Cost Savings (%)', fontweight='bold', fontsize=12)
ax5.set_title('📉 Cost Reduction vs Baseline', fontweight='bold', fontsize=13, pad=15)
ax5.set_xticks(x)
ax5.set_xticklabels(datasets, fontsize=11)
ax5.legend(fontsize=10, loc='lower right')
ax5.grid(axis='y', alpha=0.4, linestyle='--')
ax5.set_ylim(0, 90)

# ============================================
# GRAPH 6: Summary Comparison Table
# ============================================
ax6 = plt.subplot(2, 3, 6)
ax6.axis('off')

# Create comparison table
comparison_data = [
    ['Metric', 'Baseline', 'Opt (4T)', 'Opt (8T)', 'Best Gain'],
    ['Time (1GB)', f'{baseline_1gb_time:.2f}s', f'{opt_4t_1gb_time:.2f}s', 
     f'{opt_8t_1gb_time:.2f}s', f'{speedup_4t[0]:.2f}x'],
    ['Time (4GB)', f'{baseline_4gb_time:.2f}s', f'{opt_4t_4gb_time:.2f}s', 
     f'{opt_8t_4gb_time:.2f}s', f'{speedup_8t[2]:.2f}x'],
    ['Cost/GB (1GB)', f'${baseline_1gb_cost:.6f}', f'${opt_4t_1gb_cost:.6f}', 
     f'${opt_8t_1gb_cost:.6f}', f'{savings_4t[0]:.1f}%'],
    ['Throughput (1GB)', f'{baseline_1gb_throughput:.4f}', f'{opt_4t_1gb_throughput:.4f}', 
     f'{opt_8t_1gb_throughput:.4f}', f'{opt_4t_1gb_throughput/baseline_1gb_throughput:.2f}x'],
]

table = ax6.table(cellText=comparison_data, cellLoc='center', loc='center',
                  colWidths=[0.22, 0.18, 0.18, 0.18, 0.18])
table.auto_set_font_size(False)
table.set_fontsize(9)
table.scale(1, 2.8)

# Style header row
for i in range(5):
    table[(0, i)].set_facecolor('#4CAF50')
    table[(0, i)].set_text_props(weight='bold', color='white')

# Highlight rows
for i in range(1, 5):
    table[(i, 0)].set_facecolor('#E3F2FD')
    table[(i, 0)].set_text_props(weight='bold')
    # Highlight best performance column
    table[(i, 4)].set_facecolor('#FFE082')
    table[(i, 4)].set_text_props(weight='bold')

ax6.set_title('📊 Performance Comparison Summary', fontweight='bold', pad=20, fontsize=13)

plt.tight_layout(rect=[0, 0, 1, 0.99])
plt.savefig('/mnt/user-data/outputs/baseline_vs_optimized_comparison.png', 
            dpi=300, bbox_inches='tight')
print("✅ NEW Graph saved: baseline_vs_optimized_comparison.png")

# ============================================
# CREATE ADDITIONAL DETAILED LINE GRAPHS
# ============================================

fig_detailed = plt.figure(figsize=(16, 10))
fig_detailed.suptitle('Detailed Performance Metrics: Baseline vs All Optimized Configurations', 
                     fontsize=16, fontweight='bold')

# ============================================
# GRAPH 1: Time Progression Across Datasets
# ============================================
ax1 = plt.subplot(2, 2, 1)

dataset_sizes_numeric = [1, 2, 4]
ax1.plot(dataset_sizes_numeric, baseline_times, 'o-', linewidth=2.5, markersize=10,
         color='#d62728', label='Baseline (Original)', marker='s')
ax1.plot(dataset_sizes_numeric, [opt_1t_1gb_time, opt_1t_2gb_time, opt_1t_4gb_time], 
         'o-', linewidth=2.5, markersize=10, color='#ff7f0e', label='Optimized (1T)')
ax1.plot(dataset_sizes_numeric, [opt_2t_1gb_time, opt_2t_2gb_time, opt_2t_4gb_time], 
         'o-', linewidth=2.5, markersize=10, color='#9467bd', label='Optimized (2T)')
ax1.plot(dataset_sizes_numeric, opt_4t_times, 'o-', linewidth=2.5, markersize=10,
         color='#2ca02c', label='Optimized (4T)', marker='D')
ax1.plot(dataset_sizes_numeric, opt_8t_times, 'o-', linewidth=2.5, markersize=10,
         color='#1f77b4', label='Optimized (8T)', marker='^')

ax1.set_xlabel('Dataset Size (GB)', fontweight='bold', fontsize=12)
ax1.set_ylabel('Time (seconds)', fontweight='bold', fontsize=12)
ax1.set_title('Execution Time Scaling', fontweight='bold', fontsize=13, pad=10)
ax1.legend(fontsize=10, loc='upper left')
ax1.grid(True, alpha=0.4, linestyle='--')
ax1.set_xticks(dataset_sizes_numeric)
ax1.set_xticklabels(['1 GB', '2 GB', '4 GB'])

# ============================================
# GRAPH 2: Throughput Progression
# ============================================
ax2 = plt.subplot(2, 2, 2)

ax2.plot(dataset_sizes_numeric, baseline_throughputs, 'o-', linewidth=2.5, markersize=10,
         color='#d62728', label='Baseline (Original)', marker='s')
ax2.plot(dataset_sizes_numeric, [opt_1t_1gb_throughput, opt_1t_2gb_throughput, opt_1t_4gb_throughput], 
         'o-', linewidth=2.5, markersize=10, color='#ff7f0e', label='Optimized (1T)')
ax2.plot(dataset_sizes_numeric, [opt_2t_1gb_throughput, opt_2t_2gb_throughput, opt_2t_4gb_throughput], 
         'o-', linewidth=2.5, markersize=10, color='#9467bd', label='Optimized (2T)')
ax2.plot(dataset_sizes_numeric, opt_4t_throughputs, 'o-', linewidth=2.5, markersize=10,
         color='#2ca02c', label='Optimized (4T)', marker='D')
ax2.plot(dataset_sizes_numeric, opt_8t_throughputs, 'o-', linewidth=2.5, markersize=10,
         color='#1f77b4', label='Optimized (8T)', marker='^')

ax2.set_xlabel('Dataset Size (GB)', fontweight='bold', fontsize=12)
ax2.set_ylabel('Throughput (GB/s)', fontweight='bold', fontsize=12)
ax2.set_title('Throughput Scaling', fontweight='bold', fontsize=13, pad=10)
ax2.legend(fontsize=10, loc='upper right')
ax2.grid(True, alpha=0.4, linestyle='--')
ax2.set_xticks(dataset_sizes_numeric)
ax2.set_xticklabels(['1 GB', '2 GB', '4 GB'])

# ============================================
# GRAPH 3: Speedup vs Dataset Size
# ============================================
ax3 = plt.subplot(2, 2, 3)

speedup_1t = [baseline_1gb_time / opt_1t_1gb_time,
              baseline_2gb_time / opt_1t_2gb_time,
              baseline_4gb_time / opt_1t_4gb_time]
speedup_2t = [baseline_1gb_time / opt_2t_1gb_time,
              baseline_2gb_time / opt_2t_2gb_time,
              baseline_4gb_time / opt_2t_4gb_time]

ax3.plot(dataset_sizes_numeric, speedup_1t, 'o-', linewidth=2.5, markersize=10,
         color='#ff7f0e', label='Optimized (1T)')
ax3.plot(dataset_sizes_numeric, speedup_2t, 'o-', linewidth=2.5, markersize=10,
         color='#9467bd', label='Optimized (2T)')
ax3.plot(dataset_sizes_numeric, speedup_4t, 'o-', linewidth=2.5, markersize=10,
         color='#2ca02c', label='Optimized (4T)', marker='D')
ax3.plot(dataset_sizes_numeric, speedup_8t, 'o-', linewidth=2.5, markersize=10,
         color='#1f77b4', label='Optimized (8T)', marker='^')

# Add value labels
for i, (s1, s2, s4, s8) in enumerate(zip(speedup_1t, speedup_2t, speedup_4t, speedup_8t)):
    if i == 0:  # Only label 1GB for clarity
        ax3.text(dataset_sizes_numeric[i], s4 + 0.2, f'{s4:.2f}x', 
                ha='center', va='bottom', fontsize=9, fontweight='bold', color='#2ca02c')

ax3.axhline(y=1, color='red', linestyle='--', linewidth=2, alpha=0.4, label='Baseline (1.0x)')

ax3.set_xlabel('Dataset Size (GB)', fontweight='bold', fontsize=12)
ax3.set_ylabel('Speedup vs Baseline', fontweight='bold', fontsize=12)
ax3.set_title('Speedup Factor Across Dataset Sizes', fontweight='bold', fontsize=13, pad=10)
ax3.legend(fontsize=10, loc='upper right')
ax3.grid(True, alpha=0.4, linestyle='--')
ax3.set_xticks(dataset_sizes_numeric)
ax3.set_xticklabels(['1 GB', '2 GB', '4 GB'])

# ============================================
# GRAPH 4: Cost/GB vs Dataset Size
# ============================================
ax4 = plt.subplot(2, 2, 4)

cost_1t = [opt_1t_1gb_cost, opt_1t_2gb_cost, opt_1t_4gb_cost]
cost_2t = [opt_2t_1gb_cost, opt_2t_2gb_cost, opt_2t_4gb_cost]

ax4.plot(dataset_sizes_numeric, baseline_costs, 'o-', linewidth=2.5, markersize=10,
         color='#d62728', label='Baseline (Original)', marker='s')
ax4.plot(dataset_sizes_numeric, cost_1t, 'o-', linewidth=2.5, markersize=10,
         color='#ff7f0e', label='Optimized (1T)')
ax4.plot(dataset_sizes_numeric, cost_2t, 'o-', linewidth=2.5, markersize=10,
         color='#9467bd', label='Optimized (2T)')
ax4.plot(dataset_sizes_numeric, opt_4t_costs, 'o-', linewidth=2.5, markersize=10,
         color='#2ca02c', label='Optimized (4T)', marker='D')
ax4.plot(dataset_sizes_numeric, opt_8t_costs, 'o-', linewidth=2.5, markersize=10,
         color='#1f77b4', label='Optimized (8T)', marker='^')

ax4.set_xlabel('Dataset Size (GB)', fontweight='bold', fontsize=12)
ax4.set_ylabel('Cost per GB ($)', fontweight='bold', fontsize=12)
ax4.set_title('Cost Efficiency Across Dataset Sizes', fontweight='bold', fontsize=13, pad=10)
ax4.legend(fontsize=10, loc='upper right')
ax4.grid(True, alpha=0.4, linestyle='--')
ax4.set_xticks(dataset_sizes_numeric)
ax4.set_xticklabels(['1 GB', '2 GB', '4 GB'])

plt.tight_layout()
plt.savefig('/mnt/user-data/outputs/detailed_performance_comparison.png', 
            dpi=300, bbox_inches='tight')
print("✅ NEW Graph saved: detailed_performance_comparison.png")

print("\n" + "="*70)
print("✅ ALL COMPARISON GRAPHS GENERATED SUCCESSFULLY!")
print("="*70)
print("\nGenerated NEW comparison files:")
print("1. baseline_vs_optimized_comparison.png   - Main 6-panel comparison")
print("2. detailed_performance_comparison.png    - Line graphs showing trends")
print("\nAll graphs saved to: /mnt/user-data/outputs/")
print("="*70)