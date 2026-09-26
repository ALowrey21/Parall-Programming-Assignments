#ifndef PLOT_H
#define PLOT_H
//This code was written by ai. As c does not have any built in plotting, this file is instead used.
 
/*
 * Incremental SVG scatter (dot) plots.
 *
 * Usage:
 *     Plot *p = plotCreate(...);        // once, before your loop
 *     plotAddPoint(p, x, y, "label");   // any number of times
 *     plotFinish(p);                    // once, after your loop
 *
 * Axis ranges are fixed when the plot is created:
 *   - The y-axis always runs from 0 to yMax.
 *   - A linear x-axis runs from 0 to xMax (xMin is ignored).
 *   - A logarithmic x-axis (base 2) runs from xMin to xMax, so xMin must
 *     be greater than 0. Tick labels use binary suffixes: 1K = 1024,
 *     1M = 1024K, 1G = 1024M.
 *
 * Values are doubles; int and long long arguments convert automatically.
 */
 
typedef struct Plot Plot;
 
/*
 * Opens the file and draws the title, axes, tick marks, and axis labels.
 * title, xLabel, and yLabel may be NULL or "" to leave them out.
 * logX: 0 = linear x-axis, nonzero = logarithmic x-axis.
 * Returns NULL if the arguments are invalid or the file can't be opened.
 */
Plot *plotCreate(const char *filename, const char *title,
                 const char *xLabel, const char *yLabel,
                 double xMin, double xMax, double yMax, int logX);
 
/*
 * Draws one point, with label printed next to it (label may be NULL or "").
 * A point outside the plot's range is not drawn; a warning is printed to
 * stderr and the function returns -1. Returns 0 on success.
 */
int plotAddPoint(Plot *p, double x, double y, const char *label);
 
/*
 * Writes the end of the SVG, closes the file, and frees the plot.
 * The file is not a valid SVG until this is called.
 * Returns 0 on success, -1 on failure.
 */
int plotFinish(Plot *p);
 
#endif