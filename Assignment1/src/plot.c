#include "plot.h"
 #include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//This code was written by ai. As c does not have any built in plotting, this file is instead used.
/* Overall image size and the margins around the plotting area (pixels). */
#define SVG_W        800
#define SVG_H        500
#define MARGIN_LEFT  90
#define MARGIN_RIGHT 50
#define MARGIN_TOP   50
#define MARGIN_BOT   70
 
#define PLOT_X0 MARGIN_LEFT
#define PLOT_X1 (SVG_W - MARGIN_RIGHT)
#define PLOT_Y0 MARGIN_TOP
#define PLOT_Y1 (SVG_H - MARGIN_BOT)
 
/* Keeps points at the ends of the x range off the vertical axis line. */
#define X_PAD 20
 
/* Allowed rounding slop when checking whether a point is in range. */
#define RANGE_EPS 1e-9
 
struct Plot {
    FILE  *file;
    char   name[256];     /* filename, for warning messages */
    double xLo, xHi;      /* x range, in log2 units when logX is set */
    double yMax;
    int    logX;
};
 
/* Writes text with the characters SVG treats specially escaped. */
static void writeEscaped(FILE *f, const char *s) {
    if (s == NULL) return;
    for (; *s; s++) {
        switch (*s) {
            case '&': fputs("&amp;", f);  break;
            case '<': fputs("&lt;", f);   break;
            case '>': fputs("&gt;", f);   break;
            case '"': fputs("&quot;", f); break;
            default:  fputc(*s, f);
        }
    }
}
 
/* Picks a "nice" tick spacing (1, 2, or 5 times a power of ten). */
static double niceStep(double range, int targetTicks) {
    double raw = range / targetTicks;
    double mag = pow(10.0, floor(log10(raw)));
    double frac = raw / mag;
 
    if (frac <= 1.0) return 1.0 * mag;
    if (frac <= 2.0) return 2.0 * mag;
    if (frac <= 5.0) return 5.0 * mag;
    return 10.0 * mag;
}
 
/* Formats a power of two with a binary suffix, e.g. 2048 -> "2K". */
static void formatBinary(char *buf, size_t size, double value) {
    if (value >= 1073741824.0)
        snprintf(buf, size, "%gG", value / 1073741824.0);
    else if (value >= 1048576.0)
        snprintf(buf, size, "%gM", value / 1048576.0);
    else if (value >= 1024.0)
        snprintf(buf, size, "%gK", value / 1024.0);
    else
        snprintf(buf, size, "%g", value);
}
 
/* Maps a value in [lo, hi] onto the pixel range [p0, p1]. */
static double mapRange(double v, double lo, double hi, double p0, double p1) {
    return p0 + (v - lo) / (hi - lo) * (p1 - p0);
}
 
static double pixelX(const Plot *p, double xv) {
    return mapRange(xv, p->xLo, p->xHi, PLOT_X0 + X_PAD, PLOT_X1 - X_PAD);
}
 
static double pixelY(const Plot *p, double yv) {
    return mapRange(yv, 0.0, p->yMax, PLOT_Y1, PLOT_Y0);   /* y is flipped */
}
 
Plot *plotCreate(const char *filename, const char *title,
                 const char *xLabel, const char *yLabel,
                 double xMin, double xMax, double yMax, int logX) {
    if (filename == NULL) return NULL;
    if (!isfinite(xMax) || !isfinite(yMax) || yMax <= 0.0) return NULL;
    if (logX) {
        if (!isfinite(xMin) || xMin <= 0.0 || xMax <= xMin) return NULL;
    } else if (xMax <= 0.0) {
        return NULL;
    }
 
    Plot *p = malloc(sizeof *p);
    if (p == NULL) return NULL;
 
    p->file = fopen(filename, "w");
    if (p->file == NULL) {
        free(p);
        return NULL;
    }
    snprintf(p->name, sizeof p->name, "%s", filename);
    p->logX = logX ? 1 : 0;
    p->xLo  = logX ? log2(xMin) : 0.0;
    p->xHi  = logX ? log2(xMax) : xMax;
    p->yMax = yMax;
 
    FILE *f = p->file;
 
    /* ---- Header and background ---- */
    fprintf(f, "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"%d\" height=\"%d\" "
               "viewBox=\"0 0 %d %d\" font-family=\"Arial, Helvetica, sans-serif\">\n",
            SVG_W, SVG_H, SVG_W, SVG_H);
    fprintf(f, "<rect width=\"%d\" height=\"%d\" fill=\"white\"/>\n", SVG_W, SVG_H);
 
    /* ---- Title ---- */
    if (title != NULL && title[0] != '\0') {
        fprintf(f, "<text x=\"%d\" y=\"30\" text-anchor=\"middle\" font-size=\"18\">",
                SVG_W / 2);
        writeEscaped(f, title);
        fputs("</text>\n", f);
    }
 
    /* ---- Y gridlines and tick labels (0 up to yMax) ---- */
    double yStep = niceStep(yMax, 5);
    for (int i = 0; i * yStep <= yMax * (1.0 + RANGE_EPS); i++) {
        double v = i * yStep;
        double py = pixelY(p, v);
        fprintf(f, "<line x1=\"%d\" y1=\"%.1f\" x2=\"%d\" y2=\"%.1f\" "
                   "stroke=\"#e0e0e0\"/>\n", PLOT_X0, py, PLOT_X1, py);
        fprintf(f, "<text x=\"%d\" y=\"%.1f\" text-anchor=\"end\" font-size=\"12\" "
                   "dominant-baseline=\"middle\">%g</text>\n", PLOT_X0 - 8, py, v);
    }
 
    /* ---- X gridlines and tick labels ---- */
    if (p->logX) {
        /* Ticks on whole powers of two inside the range, at most ~18 of them. */
        int first = (int)ceil(p->xLo - RANGE_EPS);
        int last  = (int)floor(p->xHi + RANGE_EPS);
        int step  = (last - first) / 18 + 1;
        for (int e = last; e >= first; e -= step) {   /* keep the top tick */
            double px = pixelX(p, e);
            char label[32];
            formatBinary(label, sizeof label, pow(2.0, e));
            fprintf(f, "<line x1=\"%.1f\" y1=\"%d\" x2=\"%.1f\" y2=\"%d\" "
                       "stroke=\"#e0e0e0\"/>\n", px, PLOT_Y0, px, PLOT_Y1);
            fprintf(f, "<text x=\"%.1f\" y=\"%d\" text-anchor=\"middle\" "
                       "font-size=\"12\">%s</text>\n", px, PLOT_Y1 + 20, label);
        }
    } else {
        double xStep = niceStep(xMax, 6);
        for (int i = 0; i * xStep <= xMax * (1.0 + RANGE_EPS); i++) {
            double v = i * xStep;
            double px = pixelX(p, v);
            fprintf(f, "<line x1=\"%.1f\" y1=\"%d\" x2=\"%.1f\" y2=\"%d\" "
                       "stroke=\"#e0e0e0\"/>\n", px, PLOT_Y0, px, PLOT_Y1);
            fprintf(f, "<text x=\"%.1f\" y=\"%d\" text-anchor=\"middle\" "
                       "font-size=\"12\">%g</text>\n", px, PLOT_Y1 + 20, v);
        }
    }
 
    /* ---- Axes ---- */
    fprintf(f, "<line x1=\"%d\" y1=\"%d\" x2=\"%d\" y2=\"%d\" stroke=\"black\"/>\n",
            PLOT_X0, PLOT_Y1, PLOT_X1, PLOT_Y1);
    fprintf(f, "<line x1=\"%d\" y1=\"%d\" x2=\"%d\" y2=\"%d\" stroke=\"black\"/>\n",
            PLOT_X0, PLOT_Y1, PLOT_X0, PLOT_Y0);
 
    /* ---- Axis labels ---- */
    if (xLabel != NULL && xLabel[0] != '\0') {
        fprintf(f, "<text x=\"%d\" y=\"%d\" text-anchor=\"middle\" font-size=\"14\">",
                (PLOT_X0 + PLOT_X1) / 2, SVG_H - 20);
        writeEscaped(f, xLabel);
        fputs("</text>\n", f);
    }
    if (yLabel != NULL && yLabel[0] != '\0') {
        int cx = 22, cy = (PLOT_Y0 + PLOT_Y1) / 2;
        fprintf(f, "<text x=\"%d\" y=\"%d\" text-anchor=\"middle\" font-size=\"14\" "
                   "transform=\"rotate(-90 %d %d)\">", cx, cy, cx, cy);
        writeEscaped(f, yLabel);
        fputs("</text>\n", f);
    }
 
    fflush(f);
    return p;
}
 
int plotAddPoint(Plot *p, double x, double y, const char *label) {
    if (p == NULL || p->file == NULL) return -1;
 
    int ok = isfinite(x) && isfinite(y) && y >= 0.0 &&
             y <= p->yMax * (1.0 + RANGE_EPS) && (!p->logX || x > 0.0);
    double xv = 0.0;
    if (ok) {
        xv = p->logX ? log2(x) : x;
        double span = p->xHi - p->xLo;
        ok = xv >= p->xLo - span * RANGE_EPS && xv <= p->xHi + span * RANGE_EPS;
    }
    if (!ok) {
        fprintf(stderr, "plot warning (%s): point (%g, %g)%s%s%s is outside "
                        "the plot's range and was not drawn\n",
                p->name, x, y,
                label ? " \"" : "", label ? label : "", label ? "\"" : "");
        return -1;
    }
 
    double px = pixelX(p, xv);
    double py = pixelY(p, y);
    FILE *f = p->file;
 
    fprintf(f, "<circle cx=\"%.1f\" cy=\"%.1f\" r=\"4\" fill=\"steelblue\"/>\n", px, py);
 
    if (label != NULL && label[0] != '\0') {
        /* Put the label to the right of the dot, unless it would run off the
           image; 6.5 px per character is a rough width for 11 px text. */
        int nearRight = px + 7 + 6.5 * (double)strlen(label) > SVG_W - 5;
        fprintf(f, "<text x=\"%.1f\" y=\"%.1f\" text-anchor=\"%s\" font-size=\"11\" "
                   "fill=\"#333333\">",
                nearRight ? px - 7 : px + 7, py - 6, nearRight ? "end" : "start");
        writeEscaped(f, label);
        fputs("</text>\n", f);
    }
 
    fflush(f);   /* keep the file current in case the program stops early */
    return 0;
}
 
int plotFinish(Plot *p) {
    if (p == NULL) return -1;
 
    int result = 0;
    if (p->file != NULL) {
        fputs("</svg>\n", p->file);
        if (fclose(p->file) != 0) result = -1;
    }
    free(p);
    return result;
}