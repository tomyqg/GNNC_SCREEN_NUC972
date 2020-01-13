#include "resource.h"

static const unsigned short raw_data[] = {
    65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 
    65535, 0, 0, 0, 0, 0, 0, 0, 65535, 65535, 0, 0, 0, 0, 0, 0, 0, 0, 65535, 65535, 65535, 0, 0, 0, 0, 0, 0, 0, 0, 42292, 35921, 29614, 27469, 25388, 29582, 31727, 35953, 46486, 0, 0, 0, 0, 0, 0, 0, 65535, 65535, 0, 0, 0, 0, 0, 0, 0, 0, 0, 65535, 0, 0, 0, 0, 0, 0, 65535, 
    65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 63422, 46486, 27469, 10565, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2113, 16904, 33808, 52825, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 
    65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 50744, 23275, 2145, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8484, 33808, 59164, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 0, 65535, 
    65535, 0, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 46486, 12710, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 32, 25356, 57051, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 0, 65535, 
    65535, 0, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 52825, 14791, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2241, 4450, 6659, 6692, 6692, 6659, 6595, 4418, 96, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 27469, 61309, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 0, 65535, 
    65535, 0, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 63422, 29582, 0, 0, 0, 0, 0, 0, 0, 0, 4418, 8868, 13287, 17673, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 17673, 13255, 8804, 2241, 0, 0, 0, 0, 0, 0, 0, 4258, 42292, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 0, 65535, 
    65535, 0, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 54970, 10597, 0, 0, 0, 0, 0, 0, 129, 8836, 17576, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 15431, 6692, 32, 0, 0, 0, 0, 0, 0, 25356, 63422, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 0, 65535, 
    65535, 0, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 46486, 2113, 0, 0, 0, 0, 0, 129, 11110, 19817, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 17673, 8836, 64, 0, 0, 0, 0, 0, 10597, 59164, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 0, 65535, 
    65535, 0, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 38066, 0, 0, 0, 0, 0, 32, 8836, 19753, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 17608, 6563, 0, 0, 0, 0, 0, 6339, 54938, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 0, 65535, 
    65535, 0, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 38034, 0, 0, 0, 0, 0, 2241, 15496, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 13190, 64, 0, 0, 0, 0, 4226, 52857, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 
    65535, 0, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 42260, 0, 0, 0, 0, 0, 4515, 19753, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 17576, 2273, 0, 0, 0, 0, 4258, 57051, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 
    65535, 0, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 50712, 32, 0, 0, 0, 0, 8836, 19817, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 17673, 4418, 0, 0, 0, 0, 8452, 61277, 65535, 65535, 65535, 65535, 0, 65535, 
    65535, 0, 65535, 65535, 65535, 65535, 65535, 65535, 59196, 6339, 0, 0, 0, 0, 8804, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19785, 4450, 0, 0, 0, 0, 19017, 65535, 65535, 65535, 65535, 0, 65535, 
    65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 16936, 0, 0, 0, 0, 6659, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19753, 2306, 0, 0, 0, 0, 35921, 65535, 65535, 65535, 0, 65535, 
    65535, 65535, 65535, 65535, 65535, 65535, 65535, 40179, 0, 0, 0, 0, 4482, 19817, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 17641, 129, 0, 0, 0, 32, 57051, 65535, 65535, 0, 65535, 
    65535, 0, 65535, 65535, 65535, 65535, 61277, 4226, 0, 0, 0, 2177, 17673, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 15367, 0, 0, 0, 0, 16936, 65535, 65535, 0, 65535, 
    65535, 0, 65535, 65535, 65535, 65535, 29582, 0, 0, 0, 0, 13287, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 8836, 0, 0, 0, 0, 44405, 65535, 0, 65535, 
    65535, 0, 65535, 65535, 65535, 59164, 2113, 0, 0, 0, 6627, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19753, 15496, 19817, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19753, 2209, 0, 0, 0, 12678, 65535, 65535, 65535, 
    65535, 0, 65535, 65535, 65535, 31695, 0, 0, 0, 32, 17608, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 15399, 64, 0, 4386, 17673, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 13190, 0, 0, 0, 0, 46486, 65535, 65535, 
    65535, 0, 65535, 65535, 63422, 4226, 0, 0, 0, 6692, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 4483, 0, 0, 0, 2305, 17673, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19817, 2273, 0, 0, 0, 16936, 0, 65535, 
    65535, 0, 65535, 65535, 52857, 0, 0, 0, 0, 17576, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 6659, 0, 0, 0, 0, 2305, 17673, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 13158, 0, 0, 0, 0, 0, 65535, 
    65535, 0, 65535, 65535, 63390, 4258, 0, 0, 6595, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 17640, 2209, 0, 0, 0, 0, 2305, 17673, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19785, 64, 0, 0, 0, 0, 65535, 
    65535, 65535, 65535, 65535, 65535, 54970, 10885, 8901, 19753, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 17608, 2209, 0, 0, 0, 0, 2305, 17673, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 6692, 0, 0, 0, 0, 65535, 
    65535, 65535, 65535, 65535, 65535, 61373, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 17608, 2209, 0, 0, 0, 0, 2305, 17673, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 13287, 0, 0, 0, 0, 63390, 
    65535, 0, 65535, 65535, 65535, 48919, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 17608, 2209, 0, 0, 0, 0, 2305, 17673, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19785, 32, 0, 0, 0, 46518, 
    65535, 0, 65535, 65535, 65535, 38547, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 17608, 2209, 0, 0, 0, 0, 2305, 17673, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 4418, 0, 0, 0, 33840, 
    65535, 0, 65535, 65535, 65535, 32271, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 17608, 2209, 0, 0, 0, 0, 2305, 17673, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 6724, 0, 0, 0, 23275, 
    65535, 0, 65535, 65535, 65535, 23980, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 17608, 2209, 0, 0, 0, 0, 2305, 17673, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 11013, 0, 0, 0, 14791, 
    65535, 0, 61309, 61309, 61309, 19721, 17673, 17673, 17673, 17673, 17673, 17673, 17673, 17673, 17673, 17673, 17673, 17673, 17673, 17673, 17673, 17673, 17673, 17673, 17673, 17673, 17673, 17673, 17673, 17673, 17673, 17673, 17673, 17673, 17673, 17673, 15496, 2209, 0, 0, 0, 0, 2273, 17673, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 13255, 0, 0, 0, 8452, 
    44405, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 64, 17673, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 15367, 0, 0, 0, 4258, 
    6339, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6627, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 15464, 0, 0, 0, 32, 
    6371, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6627, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 15464, 0, 0, 0, 32, 
    44405, 4226, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 64, 17673, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 15367, 0, 0, 0, 4258, 
    65535, 65535, 61309, 61309, 61309, 19721, 17673, 17673, 17673, 17673, 17673, 17673, 17673, 17673, 17673, 17673, 17673, 17673, 17673, 17673, 17673, 17673, 17673, 17673, 17673, 17673, 17673, 17673, 17673, 17673, 17673, 17673, 17673, 17673, 17673, 17673, 15464, 2209, 0, 0, 0, 0, 2273, 17673, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 13255, 0, 0, 0, 8452, 
    65535, 65535, 65535, 65535, 65535, 23980, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 17608, 2209, 0, 0, 0, 0, 2305, 17673, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 11013, 0, 0, 0, 14791, 
    65535, 0, 65535, 65535, 65535, 32271, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 17608, 2209, 0, 0, 0, 0, 2305, 17673, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 6692, 0, 0, 0, 23275, 
    65535, 0, 65535, 65535, 65535, 38547, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 17608, 2209, 0, 0, 0, 0, 2305, 17673, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 4418, 0, 0, 0, 33840, 
    65535, 0, 65535, 65535, 65535, 48920, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 17608, 2209, 0, 0, 0, 0, 2305, 17673, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19753, 32, 0, 0, 0, 46518, 
    65535, 0, 65535, 65535, 65535, 61373, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 17608, 2209, 0, 0, 0, 0, 2305, 17673, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 13287, 0, 0, 0, 0, 63390, 
    65535, 0, 65535, 65535, 65535, 54970, 10853, 8901, 19753, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 17608, 2209, 0, 0, 0, 0, 2305, 17673, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 6691, 0, 0, 0, 0, 65535, 
    65535, 0, 65535, 65535, 63390, 4258, 0, 0, 6595, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 17608, 2209, 0, 0, 0, 0, 2305, 17673, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19785, 64, 0, 0, 0, 0, 65535, 
    65535, 65535, 65535, 65535, 52857, 0, 0, 0, 0, 17576, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 6659, 0, 0, 0, 0, 2305, 17673, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 13158, 0, 0, 0, 0, 0, 65535, 
    65535, 0, 65535, 65535, 63422, 4226, 0, 0, 0, 6692, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 4515, 0, 0, 0, 2305, 17673, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19817, 2241, 0, 0, 0, 16936, 65535, 65535, 
    65535, 0, 65535, 65535, 65535, 31695, 0, 0, 0, 0, 17576, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 15399, 64, 0, 4418, 17673, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 13190, 0, 0, 0, 0, 46518, 65535, 65535, 
    65535, 0, 65535, 65535, 65535, 59196, 2113, 0, 0, 0, 6595, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19753, 17544, 19817, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19753, 2177, 0, 0, 0, 12678, 65535, 65535, 65535, 
    65535, 0, 65535, 65535, 65535, 65535, 29582, 0, 0, 0, 0, 13287, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 8804, 0, 0, 0, 0, 46486, 65535, 65535, 65535, 
    65535, 0, 65535, 65535, 65535, 65535, 61309, 4226, 0, 0, 0, 2177, 17673, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 15367, 0, 0, 0, 0, 16936, 65535, 65535, 0, 65535, 
    65535, 65535, 65535, 65535, 65535, 65535, 65535, 40179, 0, 0, 0, 0, 4482, 19817, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 17641, 128, 0, 0, 0, 32, 57051, 65535, 65535, 0, 65535, 
    65535, 0, 65535, 65535, 65535, 65535, 65535, 65535, 16936, 0, 0, 0, 0, 6659, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19753, 2305, 0, 0, 0, 0, 35921, 65535, 65535, 65535, 0, 65535, 
    65535, 0, 65535, 65535, 65535, 65535, 65535, 65535, 61277, 6339, 0, 0, 0, 0, 8804, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19785, 4450, 0, 0, 0, 0, 19049, 65535, 65535, 65535, 65535, 0, 65535, 
    65535, 0, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 50712, 32, 0, 0, 0, 0, 8836, 19817, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 17673, 4418, 0, 0, 0, 0, 8452, 61309, 65535, 65535, 65535, 65535, 0, 65535, 
    65535, 0, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 42292, 0, 0, 0, 0, 0, 4483, 17705, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 17576, 2273, 0, 0, 0, 0, 4258, 57083, 65535, 65535, 65535, 65535, 65535, 0, 65535, 
    65535, 0, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 38066, 0, 0, 0, 0, 0, 2241, 15496, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 13190, 64, 0, 0, 0, 0, 4226, 52857, 65535, 65535, 65535, 65535, 65535, 65535, 0, 65535, 
    65535, 0, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 40147, 0, 0, 0, 0, 0, 32, 8836, 19753, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 17608, 4515, 0, 0, 0, 0, 0, 6371, 54938, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 0, 65535, 
    65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 46518, 2145, 0, 0, 0, 0, 0, 96, 11078, 19785, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 17641, 8836, 32, 0, 0, 0, 0, 0, 10597, 59164, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 0, 65535, 
    65535, 0, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 54970, 10597, 0, 0, 0, 0, 0, 0, 96, 8836, 17576, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 15431, 6692, 32, 0, 0, 0, 0, 0, 0, 25388, 63422, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 
    65535, 0, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 63422, 29582, 32, 0, 0, 0, 0, 0, 0, 0, 4386, 8836, 13255, 17673, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 19818, 17673, 13254, 8772, 2241, 0, 0, 0, 0, 0, 0, 0, 6339, 44373, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 
    65535, 0, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 52857, 14823, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2241, 4450, 6627, 6692, 6692, 6659, 6595, 4386, 96, 0, 0, 0, 0, 0, 0, 0, 0, 0, 32, 27501, 63390, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 
    65535, 0, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 46518, 14791, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 32, 25388, 57083, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 0, 65535, 
    65535, 0, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 52825, 25356, 4226, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8484, 33808, 59164, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 0, 65535, 
    65535, 0, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 63422, 46518, 27501, 10597, 32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2113, 16904, 33840, 52825, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 0, 65535, 
    65535, 0, 0, 65535, 0, 0, 0, 0, 0, 65535, 0, 0, 0, 0, 0, 0, 65535, 65535, 0, 0, 0, 0, 0, 0, 65535, 65535, 0, 0, 0, 0, 0, 0, 0, 0, 29582, 31727, 0, 0, 0, 0, 0, 0, 0, 0, 65535, 65535, 0, 0, 0, 0, 0, 0, 0, 0, 65535, 65535, 0, 0, 0, 0, 0, 0, 0, 65535, 
    65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 
};

const BITMAP_INFO enter_focus_bmp ={
    64,
    64,
    16,
    raw_data
};