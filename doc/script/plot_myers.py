import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
import numpy as np

def compute_edit_path(A, B):
    """Compute the shortest edit path using dynamic programming."""
    N, M = len(A), len(B)
    dp = [[0] * (M + 1) for _ in range(N + 1)]
    for i in range(N + 1):
        dp[i][0] = i
    for j in range(M + 1):
        dp[0][j] = j
    for i in range(1, N + 1):
        for j in range(1, M + 1):
            if A[i-1] == B[j-1]:
                dp[i][j] = dp[i-1][j-1]
            else:
                dp[i][j] = min(dp[i-1][j], dp[i][j-1]) + 1
    path = []
    i, j = N, M
    while i > 0 or j > 0:
        path.append((i, j))
        if i > 0 and j > 0 and A[i-1] == B[j-1]:
            i -= 1
            j -= 1
        else:
            if i > 0 and dp[i][j] == dp[i-1][j] + 1:
                i -= 1
            else:
                j -= 1
    path.append((0, 0))
    path.reverse()
    return path

def draw_myers_graph(title, title_pad, A, B, transform_func, output_file):
    """
    Draw and save Myers graph with customizable parameters.
    """
    N, M = len(A), len(B)
    path = compute_edit_path(A, B)

    fig, ax = plt.subplots(figsize=(12, 12))
    ax.set_aspect('equal')
    ax.axis('off')
    ax.set_title(title, pad=title_pad)

    # Draw nodes and edges using transform_func
    for x in range(N + 1):
        for y in range(M + 1):
            tx, ty = transform_func(x, y)
            ax.plot(tx, ty, 'ko', markersize=5, zorder=3)
            
            if y < M:
                tx_next, ty_next = transform_func(x, y + 1)
                ax.plot([tx, tx_next], [ty, ty_next], 'b-', linewidth=1, alpha=0.5)
            
            if x < N:
                tx_next, ty_next = transform_func(x + 1, y)
                ax.plot([tx, tx_next], [ty, ty_next], 'b-', linewidth=1, alpha=0.5)
            
            if x < N and y < M and A[x] == B[y]:
                tx_diag, ty_diag = transform_func(x + 1, y + 1)
                ax.plot([tx, tx_diag], [ty, ty_diag], 'r-', linewidth=2, alpha=0.8)

    # Add labels using transform_func
    for i, char in enumerate(A):
        tx, ty = transform_func(i + 0.5, -0.25)
        ax.text(tx, ty, char, ha='center', va='center', fontsize=12, color='darkblue')
    for j, char in enumerate(B):
        tx, ty = transform_func(-0.25, j + 0.5)
        ax.text(tx, ty, char, ha='center', va='center', fontsize=12, color='darkred')

    # Draw k anti-diagonals
    max_k = N + M
    colors = plt.cm.tab20(np.linspace(0, 1, max_k + 1))
    
    for k in range(0, max_k + 1):
        color = colors[k]
        intersections = []
        if k <= M: intersections.append((0, k))
        if k <= N: intersections.append((k, 0))
        if k >= N: intersections.append((N, k - N))
        if k >= M: intersections.append((k - M, M))
        
        unique_points = list(dict.fromkeys(intersections))
        if len(unique_points) >= 2:
            start = unique_points[0]
            end = unique_points[-1]
            mid_x = (start[0] + end[0]) / 2
            mid_y = (start[1] + end[1]) / 2
            
            tx_start, ty_start = transform_func(*start)
            tx_mid, ty_mid = transform_func(mid_x, mid_y)
            tx_end, ty_end = transform_func(*end)
            
            ax.plot([tx_start, tx_mid], [ty_start, ty_mid], 
                    color=color, linestyle='--', alpha=0.4, linewidth=1)
            ax.plot([tx_mid, tx_end], [ty_mid, ty_end], 
                    color=color, linestyle='--', alpha=0.4, linewidth=1)
            
            ax.text(tx_mid, ty_mid, f'k={k}', color=color, fontsize=8,
                    ha='center', va='center', alpha=0.8)

    # Highlight path
    if path:
        path_transformed = [transform_func(x, y) for (x, y) in path]
        px, py = zip(*path_transformed)
        ax.plot(px, py, 'g-', linewidth=3, marker='o', markersize=8, 
                markeredgecolor='black', markerfacecolor='green', zorder=4)
        for step, (x, y) in enumerate(path):
            tx, ty = transform_func(x, y)
            ax.text(tx, ty, str(step), color='white', backgroundcolor='green', 
                    fontsize=8, ha='center', va='center', zorder=5)

    plt.savefig(output_file, dpi=300, bbox_inches='tight')
    plt.close()

def transform_classic(x, y):
    return x, -y

def transform_rhombous(x, y):
    b = -2 + np.sqrt(3)
    return x + b * y, -y - b * x

draw_myers_graph(
    title = "Myers Diff Graph",
    title_pad = 20,
    A="ABCABBA", 
    B="CBABAC",
    transform_func=transform_classic,
    output_file="doc/img/plot_classic.png")

draw_myers_graph(
    title = "Myers Diff Rhombous Graph",
    title_pad = 0,
    A="ABCABBA", 
    B="CBABAC",
    transform_func=transform_rhombous,
    output_file="doc/img/plot_classic_rhombous.png")
