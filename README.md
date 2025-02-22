# Comparex

Comparex is a new algorithm for finding the **Longest Common Subsequence (LCS)** between two sequences. It builds on the foundational insights of the classic **Myers algorithm**—well-known for its efficient diff computation—while introducing improvements that can make LCS computation faster and more intuitive under certain conditions.

This README will walk you step-by-step from the fundamentals of **Myers algorithm** to how **Comparex** works, along with examples to get you started quickly.

---

## Table of Contents

1. [Introduction](#introduction)  
2. [What is the Longest Common Subsequence?](#what-is-the-longest-common-subsequence)  
3. [Review of the Classic Myers Algorithm](#review-of-the-classic-myers-algorithm)  
   - [Key Concepts](#key-concepts)  
   - [Myers Algorithm Steps](#myers-algorithm-steps)  
4. [Inplace Twicks](#inplace-twicks)  
   - [Non-negative k loop](#non-negative-k-loop)  
   - [Parametrizable size](#parametrizable-size)
   - [Bitset for the Turns](#bitset-for-the-turns)
   - [Shrink the Turns Vector Size](#shrink-the-turns-vector-size)
   - [Limit the scanning](#limit-the-scanning)
5. [Comparex: New Longest Subsequence Algorithm](#comparex-new-longest-subsequence-algorithm)  
   - [Design Goals](#design-goals)  
   - [Algorithm Outline](#algorithm-outline)  
6. [Installation](#installation)  
7. [Quick Start Example](#quick-start-example)  
8. [Step-by-Step Guide to Comparex Implementation](#step-by-step-guide-to-comparex-implementation)  
   - [1. Initialization](#1-initialization)  
   - [2. Core Logic](#2-core-logic)  
   - [3. Traceback & Reconstructing the LCS](#3-traceback--reconstructing-the-lcs)  
9. [Performance Considerations](#performance-considerations)  
10. [Contributing](#contributing)  
11. [License](#license)  
12. [Further Reading](#further-reading)  

---

## Introduction

Diff algorithms are ubiquitous in software development (e.g., version control systems like Git). The **Myers algorithm** is often considered the gold standard for computing the difference between two sequences efficiently. 

However, for the specific task of **Longest Common Subsequence (LCS)**, we can sometimes streamline the process further. **Comparex** is an attempt to achieve a simpler, often more memory-efficient approach while retaining the essence of the Myers diff logic.

This guide is intended for:
- Students and researchers wanting a deeper look into sequence-comparison algorithms.
- Developers who need to integrate an LCS or diff algorithm into their applications and want an alternative to the classic approach.

---

## What is the Longest Common Subsequence?

A **subsequence** of a sequence is a new sequence derived from the original by removing some elements (possibly none) without changing the order of the remaining elements.

The **Longest Common Subsequence (LCS)** of two sequences is the subsequence of the greatest length common to both sequences.

For example, for sequences:
- A: `[A, B, C, D, G, H]`
- B: `[A, E, D, F, H, R]`

One LCS is `[A, D, H]`, having length 3.

---

## Review of the Classic Myers Algorithm

### Key Concepts

- **Edit Distance**: The minimum number of edits (insertions, deletions, substitutions) needed to transform one sequence into another.
- **Longest Common Subsequence**: Highly related to the edit distance problem. In fact, an LCS problem can be transformed into an edit distance problem or vice versa.

The **Myers algorithm** is generally known for:
- **Linear space** complexity in its optimized form.
- **Greedy extension** steps that expand a "furthest-reaching path."

### Myers Algorithm Steps

At a high level, Myers' algorithm can be broken down into:
1. **Track "Furthest Reaching" Positions**: For each possible difference `d` in the length of sequences, track the furthest index in the sequences that has been matched.
2. **Compute Paths**: Use these furthest-reaching positions to advance matching indices through both sequences.
3. **Traceback**: Reconstruct the actual edits or the common subsequence from the stored information.

While often used to compute "diffs," the algorithm can also yield an LCS by tracking the matched parts.

Myers himself drew a similar figure to illustrate how the algorithm works.

![Myers classic figure](doc/img/plot_classic.png)

While this is sufficient to illustrate the idea behind the algorithm, it is slightly incorrect. Myers suggested that the path adds a weight of 1 for a mismatch and 1 for a match, making diagonal movements exactly twice as cheap. To be more precise, a graph that represents this more accurately would look like this:

![Myers classic figure](doc/img/plot_classic_rhombous.png)

---

## Inplace twicks

In the in-place tweaks, I will list the changes made to the code of the classic algorithm. These adjustments do not result in any performance degradation and enable the easy implementation of CompareX features. Additionally, they ensure a fair comparison between the original algorithm and the new one.

### Non-negative k loop

In the original Myers algorithm, the `k` index loops from `-d` to `d`. In our modification, we adjust this to loop from `0` to `2d`. This change allows us to stay within the realm of unsigned numbers, effectively doubling the range of values the algorithm can handle. The minor calculation adjustments required for this modification do not result in any observable performance impact.

### Parametrizable size

Now that the algorithm operates entirely with unsigned numbers, it becomes straightforward to make the size parametrizable, allowing seamless switching between `uint32_t`, `uint64_t`, or other desired types.

### Bitset for the Turns

During forward tracing, it is essential to track the value of `x`, which inherently determines the value of `y`. However, during backtracing, this is unnecessary. The only requirement is to record the turns taken. This method reduces the memory footprint by approximately 32x or 64x, depending on the size of `size_type`. By managing significantly less memory, it also enhances performance.

**Note:** This is a significant idea. It separates the concept of forward traversal of data from the data needed for backtracking, which will become important later on.

### Shrink the Turns Vector Size

To further reduce the memory footprint, we make the following observation:  

We can divide the traversal graph into three sections:  

1. The section where moving forward introduces a new item from both sequences, **A** and **B**. Here, the size of the anti-diagonal increases by 1 with each turn.  
2. The section where one sequence continues introducing new items while the other has finished. In this area, the size of the diagonal remains constant.  
3. The section where neither sequence introduces new items, causing the size to decrease.  

With some mathematical adjustments, we can precisely maintain the size of the turns vector to track only the necessary data.

To illustrate this, let's make the sequences differ more in size.

![Myers A>B regions figure](doc/img/plot_regions1.png)

Also, note that region 2 may appear on either side, depending on whether **A** or **B** is longer.

![Myers A<B regions figure](doc/img/plot_regions2.png)

If the two strings are of equal length, this region 2 will be empty.

![Myers A=B regions figure](doc/img/plot_regions3.png)

### Limit the scanning

Using the same logic from the previous point, we can not only reduce the size of the turn vectors but also apply the same approach to scanning. This eliminates a large number of expansion attempts that we know will fail because they fall outside the boundaries of one sequence or the other.

## Comparex: New Longest Subsequence Algorithm

### Design Goals

### Algorithm Outline

---

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

---

## Further Reading

- **Original Paper**: [Eugene W. Myers, “An O(ND) Difference Algorithm and Its Variations.”](http://www.xmailserver.org/diff2.pdf)
- **Wikipedia**: [Longest Common Subsequence Problem](https://en.wikipedia.org/wiki/Longest_common_subsequence_problem)

---

**Happy Comparing!** If you have any questions or feedback, feel free to open an issue or start a discussion. We’re excited to see how Comparex helps you with sequence comparisons!