static unsigned char font_map[4096] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,126,129,165,129,129,189,153,129,129,126,0,0,0,0,0,0,126,255,219,255,255,195,231,255,255,126,0,0,0,0,0,0,102,255,255,255,255,126,126,60,60,24,24,0,0,0,0,24,24,60,60,126,126,255,126,126,60,60,24,24,0,0,0,0,24,60,60,24,102,255,255,102,24,60,126,0,0,0,0,0,24,24,60,60,126,255,255,255,126,24,60,126,0,0,0,0,0,0,0,24,60,60,60,24,0,0,0,0,0,0,255,255,255,255,255,231,195,195,195,231,255,255,255,255,255,255,0,0,0,0,60,102,66,66,66,102,60,0,0,0,0,0,255,255,255,255,195,153,189,189,189,153,195,255,255,255,255,255,0,30,14,26,50,120,204,204,204,204,120,0,0,0,0,0,0,60,102,102,102,102,60,24,126,24,24,0,0,0,0,0,0,0,48,56,44,38,34,34,32,32,96,224,192,0,0,0,3,7,13,27,55,45,57,49,35,39,38,32,96,224,192,0,0,24,24,219,60,231,231,60,219,24,24,0,0,0,0,0,0,0,128,192,224,248,254,254,248,224,192,128,0,0,0,0,0,0,1,3,7,31,127,127,31,7,3,1,0,0,0,0,24,60,126,24,24,24,24,24,24,24,24,24,24,126,60,24,0,102,102,102,102,102,102,102,102,102,0,102,102,0,0,0,0,127,219,219,219,219,219,123,27,27,27,27,27,0,0,0,0,60,102,112,56,124,206,230,124,56,28,204,120,0,0,0,0,0,0,0,0,0,0,0,254,254,254,254,254,0,0,0,0,24,60,126,24,24,24,24,24,126,60,24,126,0,0,0,24,60,126,255,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,255,126,60,24,0,0,0,0,8,12,14,255,14,12,8,0,0,0,0,0,0,0,0,0,16,48,112,255,112,48,16,0,0,0,0,0,0,0,0,0,192,192,192,192,254,254,0,0,0,0,0,0,0,0,0,0,0,36,102,255,102,36,0,0,0,0,0,0,0,0,0,0,0,0,0,0,24,24,60,60,126,126,255,255,255,255,126,126,60,60,24,24,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,24,60,60,60,60,24,24,24,0,24,24,0,0,0,0,0,102,102,102,204,0,0,0,0,0,0,0,0,0,0,0,0,108,108,108,254,254,108,108,254,254,108,108,108,0,0,0,24,24,126,219,216,216,126,27,27,219,219,126,24,24,0,0,0,0,230,172,236,24,24,48,48,110,106,206,0,0,0,0,0,56,108,108,108,56,118,220,220,204,204,118,0,0,0,0,0,24,24,24,48,0,0,0,0,0,0,0,0,0,0,0,12,24,24,48,48,48,48,48,48,48,24,24,12,0,0,0,48,24,24,12,12,12,12,12,12,12,24,24,48,0,0,0,0,0,0,0,0,102,60,255,60,102,0,0,0,0,0,0,0,0,0,0,0,24,24,126,24,24,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,24,24,24,48,0,0,0,0,0,0,0,0,0,254,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,48,48,0,0,0,0,0,6,12,12,24,24,48,48,96,96,192,192,0,0,0,0,0,56,108,206,222,222,214,246,246,230,108,56,0,0,0,0,0,24,56,120,24,24,24,24,24,24,24,126,0,0,0,0,0,56,108,198,198,6,12,24,48,96,198,254,0,0,0,0,0,252,140,24,48,56,12,6,6,198,108,56,0,0,0,0,0,28,28,60,60,108,108,204,254,12,12,30,0,0,0,0,0,254,192,192,248,204,6,6,6,198,108,56,0,0,0,0,0,56,108,192,192,248,236,198,198,198,108,56,0,0,0,0,0,254,198,6,12,12,24,24,48,48,48,48,0,0,0,0,0,56,108,198,198,108,56,108,198,198,108,56,0,0,0,0,0,56,108,198,198,198,110,62,6,6,108,56,0,0,0,0,0,0,0,0,0,24,24,0,0,0,24,24,0,0,0,0,0,0,0,0,0,24,24,0,0,0,24,24,48,96,0,0,0,0,0,6,12,24,48,96,48,24,12,6,0,0,0,0,0,0,0,0,0,0,254,0,0,254,0,0,0,0,0,0,0,0,0,96,48,24,12,6,12,24,48,96,0,0,0,0,0,60,102,195,3,6,12,24,24,0,24,24,0,0,0,0,0,0,0,124,198,198,222,222,222,220,192,124,0,0,0,0,0,16,56,124,238,198,198,198,254,198,198,198,0,0,0,0,0,252,102,102,102,124,102,102,102,102,102,252,0,0,0,0,0,124,198,198,192,192,192,192,192,198,198,124,0,0,0,0,0,248,108,102,102,102,102,102,102,102,108,248,0,0,0,0,0,254,102,98,96,104,120,104,96,98,102,254,0,0,0,0,0,254,102,98,96,104,120,104,96,96,96,240,0,0,0,0,0,124,198,198,198,192,192,206,198,198,206,122,0,0,0,0,0,198,198,198,198,198,254,198,198,198,198,198,0,0,0,0,0,60,24,24,24,24,24,24,24,24,24,60,0,0,0,0,0,30,12,12,12,12,12,12,12,204,204,120,0,0,0,0,0,230,102,108,108,120,120,120,108,108,102,230,0,0,0,0,0,240,96,96,96,96,96,96,96,98,102,254,0,0,0,0,0,130,198,238,254,254,214,214,198,198,198,198,0,0,0,0,0,198,198,230,230,246,246,222,206,206,198,198,0,0,0,0,0,124,198,198,198,198,198,198,198,198,198,124,0,0,0,0,0,252,102,102,102,102,102,124,96,96,96,240,0,0,0,0,0,124,198,198,198,198,198,198,198,222,254,124,14,6,0,0,0,252,102,102,102,102,108,120,108,102,102,230,0,0,0,0,0,124,198,198,192,96,56,12,6,198,198,124,0,0,0,0,0,126,90,24,24,24,24,24,24,24,24,60,0,0,0,0,0,198,198,198,198,198,198,198,198,198,198,124,0,0,0,0,0,198,198,198,198,198,198,198,198,124,56,16,0,0,0,0,0,195,195,195,195,219,219,219,219,255,102,102,0,0,0,0,0,198,198,108,108,56,56,56,108,108,198,198,0,0,0,0,0,102,102,102,102,102,60,24,24,24,24,60,0,0,0,0,0,254,198,140,12,24,16,48,96,98,198,254,0,0,0,0,0,60,48,48,48,48,48,48,48,48,48,60,0,0,0,0,0,192,192,96,96,48,48,24,24,12,12,6,0,0,0,0,0,60,12,12,12,12,12,12,12,12,12,60,0,0,0,0,0,16,56,108,198,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,0,0,0,48,48,48,24,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,120,12,124,204,204,204,118,0,0,0,0,0,224,96,96,96,124,102,102,102,102,102,220,0,0,0,0,0,0,0,0,0,124,198,192,192,192,198,124,0,0,0,0,0,28,12,12,12,124,204,204,204,204,204,118,0,0,0,0,0,0,0,0,0,124,198,198,252,192,198,124,0,0,0,0,0,60,102,102,96,240,96,96,96,96,96,240,0,0,0,0,0,0,0,0,0,118,204,204,204,204,204,124,12,12,204,120,0,224,96,96,96,124,102,102,102,102,102,230,0,0,0,0,0,0,24,24,0,56,24,24,24,24,24,60,0,0,0,0,0,0,12,12,0,28,12,12,12,12,12,12,12,204,204,120,0,224,96,96,96,102,102,108,120,108,102,230,0,0,0,0,0,56,24,24,24,24,24,24,24,24,24,60,0,0,0,0,0,0,0,0,0,236,254,214,214,214,198,198,0,0,0,0,0,0,0,0,0,220,102,102,102,102,102,102,0,0,0,0,0,0,0,0,0,124,198,198,198,198,198,124,0,0,0,0,0,0,0,0,0,220,102,102,102,102,102,124,96,96,96,240,0,0,0,0,0,118,204,204,204,204,204,124,12,12,12,30,0,0,0,0,0,220,118,102,96,96,96,240,0,0,0,0,0,0,0,0,0,124,198,192,124,6,198,124,0,0,0,0,0,16,48,48,48,252,48,48,48,48,54,28,0,0,0,0,0,0,0,0,0,198,198,198,198,198,198,123,0,0,0,0,0,0,0,0,0,198,198,198,238,124,56,16,0,0,0,0,0,0,0,0,0,198,198,214,214,214,254,108,0,0,0,0,0,0,0,0,0,198,238,124,56,124,238,198,0,0,0,0,0,0,0,0,0,198,198,198,198,198,198,126,6,6,124,0,0,0,0,0,0,254,198,12,56,96,198,254,0,0,0,0,14,24,24,24,24,48,224,48,24,24,24,24,14,0,0,0,0,24,24,24,24,24,0,0,24,24,24,24,24,0,0,0,112,24,24,24,24,12,7,12,24,24,24,24,112,0,0,0,0,0,118,220,0,0,0,0,0,0,0,0,0,0,0,0,0,0,24,24,60,60,102,102,195,195,195,255,0,0,0,0,0,255,247,231,247,247,247,227,255,255,255,255,255,255,255,0,0,255,239,215,247,239,223,199,255,255,255,255,255,255,255,0,0,255,231,251,231,251,251,231,255,255,255,255,255,255,255,0,0,255,247,231,215,195,247,247,255,255,255,255,255,255,255,0,0,255,227,239,231,251,251,231,255,255,255,255,255,255,255,0,0,255,243,239,231,235,235,247,255,255,255,255,255,255,255,0,0,255,227,251,251,247,239,239,255,255,255,255,255,255,255,0,0,255,247,235,247,235,235,247,255,255,255,255,255,255,255,0,0,255,247,235,235,243,251,231,255,255,255,255,255,255,255,0,0,255,219,149,213,213,213,139,255,255,255,255,255,255,255,0,0,255,219,147,219,219,219,129,255,255,255,255,255,255,255,0,0,255,219,149,221,219,215,129,255,255,255,255,255,255,255,0,0,255,209,157,219,221,213,139,255,255,255,255,255,255,255,0,0,255,219,147,203,193,219,129,255,255,255,255,255,255,255,0,0,255,209,151,211,221,221,131,255,255,255,255,255,255,255,0,0,255,217,151,211,213,213,139,255,255,255,255,255,255,255,0,0,255,209,157,221,219,215,135,255,255,255,255,255,255,255,0,0,255,219,149,219,213,213,139,255,255,255,255,255,255,255,0,0,255,219,149,213,217,221,131,255,255,255,255,255,255,255,0,0,255,219,165,229,213,181,139,255,255,255,255,255,255,255,0,0,255,219,163,235,219,187,129,255,255,255,255,255,255,255,0,0,255,219,165,237,219,183,129,255,255,255,255,255,255,255,0,0,255,209,173,235,221,181,139,255,255,255,255,255,255,255,0,0,255,219,163,235,193,187,129,255,255,255,255,255,255,255,0,0,255,209,167,227,221,189,131,255,255,255,255,255,255,255,0,0,255,217,167,227,213,181,139,255,255,255,255,255,255,255,0,0,255,209,173,237,219,183,135,255,255,255,255,255,255,255,0,0,255,219,165,235,213,181,139,255,255,255,255,255,255,255,0,0,255,219,165,229,217,189,131,255,255,255,255,255,255,255,0,0,255,139,229,213,229,165,219,255,255,255,255,255,255,255,0,0,255,139,227,219,235,171,209,255,255,255,255,255,255,255,0,0,255,139,229,221,235,167,209,255,255,255,255,255,255,255,0,0,0,0,0,24,100,148,186,82,76,48,0,0,0,0,0,0,0,0,0,48,76,82,186,148,100,24,0,0,0,0,0,0,120,204,224,112,120,220,204,236,120,56,28,204,120,0,0,0,0,0,0,0,48,120,48,0,0,48,120,48,0,0,0,0,96,48,56,28,28,28,28,28,28,56,48,96,0,0,0,0,12,24,56,112,112,112,112,112,112,56,24,12,0,0,0,0,0,0,0,0,0,108,54,27,27,54,108,0,0,0,0,0,0,0,0,0,0,54,108,216,216,108,54,0,0,0,0,0,0,0,0,0,0,254,254,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,48,120,48,0,0,0,0,96,48,24,12,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,24,60,24,48,0,0,0,0,0,0,0,0,0,124,124,124,0,0,0,0,0,0,0,0,0,0,0,0,0,194,230,124,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,146,146,146,0,0,0,0,0,3,118,220,128,0,0,0,0,0,0,0,0,0,0,0,0,12,24,48,96,0,0,0,0,0,0,0,0,0,0,0,0,60,70,150,140,0,0,0,0,0,0,0,0,0,0,0,0,204,204,204,204,204,204,204,220,222,207,123,0,0,0,0,0,0,0,0,0,214,214,214,214,214,214,106,0,0,0,0,0,120,204,204,204,204,204,192,192,254,192,192,0,0,0,0,0,0,0,0,0,248,204,204,204,192,192,254,192,192,192,0,0,120,204,204,204,204,204,204,127,12,12,12,0,0,0,0,0,0,0,0,0,124,204,204,204,204,204,127,12,12,12,0,0,120,204,204,204,204,204,204,207,12,12,12,0,0,0,0,0,0,0,0,0,248,204,204,204,204,204,207,12,12,12,12,0,192,192,254,192,192,204,204,204,204,204,120,0,0,0,0,0,192,192,192,192,254,192,192,204,204,204,124,0,0,0,0,0,60,102,102,102,102,102,62,6,12,216,254,195,0,0,0,0,0,0,0,0,204,204,204,204,204,204,124,12,12,15,0,0,192,192,192,254,192,192,192,192,248,12,12,120,0,0,0,0,192,192,192,192,248,192,192,192,192,248,12,56,0,0,0,0,120,204,204,204,204,204,204,192,192,192,254,0,0,0,0,0,0,0,0,0,248,204,204,204,204,204,192,192,192,254,0,0,124,198,198,198,207,214,214,214,214,214,204,0,0,0,0,0,0,0,0,0,252,198,207,214,214,214,204,192,192,192,0,0,12,12,12,127,204,204,204,204,204,204,120,0,0,0,0,0,12,12,12,12,127,204,204,204,204,204,124,0,0,0,0,0,192,192,192,248,204,204,204,204,204,192,192,0,0,0,0,0,192,192,192,192,248,204,204,204,204,204,204,192,192,192,0,0,192,192,192,192,192,192,192,192,192,192,252,6,0,0,0,0,0,0,0,0,96,96,96,96,96,96,96,96,96,126,0,0,192,192,192,246,214,214,214,214,214,214,204,0,0,0,0,0,192,192,192,192,246,214,214,214,214,214,206,192,192,192,0,0,192,126,251,204,204,204,204,204,204,204,120,0,0,0,0,0,192,120,12,12,127,204,204,204,204,204,120,0,0,0,0,0,192,192,204,204,204,204,204,204,124,12,12,0,0,0,0,0,192,192,192,192,204,204,204,204,204,204,124,12,12,12,0,0,24,48,24,12,24,48,96,192,240,60,14,12,0,0,0,0,192,192,192,192,248,204,204,204,204,204,204,0,0,0,0,0,60,102,102,102,102,6,6,6,108,216,110,0,0,0,0,0,48,126,12,24,48,112,216,216,204,206,118,0,0,0,0,0,120,204,204,204,204,204,204,204,12,12,15,0,0,0,0,0,0,0,0,0,248,204,204,204,204,204,204,12,12,15,0,0,14,219,112,48,120,204,204,204,204,220,108,0,0,0,0,0,28,54,96,96,248,108,102,102,102,110,58,0,0,0,0,0,207,204,204,204,204,204,204,204,204,204,120,0,0,0,0,0,15,12,12,12,204,204,204,204,204,204,124,0,0,0,0,0,120,204,12,28,56,108,12,12,204,204,120,0,0,0,0,0,0,0,0,0,12,12,12,12,12,12,12,108,220,112,0,0,224,96,96,102,102,102,102,102,102,102,60,0,0,0,0,0,224,96,96,96,102,102,102,102,102,102,62,0,0,0,0,0,248,12,124,204,204,192,192,192,204,204,120,0,0,0,0,0,0,0,0,0,120,204,204,204,12,24,48,96,192,126,0,0,120,204,204,204,204,204,204,204,204,204,204,0,0,0,0,0,0,0,0,0,248,204,204,204,204,204,204,0,0,0,0,0,120,204,204,204,204,12,12,12,216,112,48,24,0,0,0,0,0,0,0,0,24,48,24,12,24,48,96,192,192,126,0,0,126,219,219,219,219,219,219,27,27,3,3,0,0,0,0,0,0,0,0,0,219,219,219,219,219,219,127,3,3,3,0,0,96,240,216,204,198,102,54,22,28,216,254,195,0,0,0,0,0,0,0,0,112,216,204,204,108,56,48,96,192,126,0,0,120,204,204,204,204,204,207,204,204,204,204,0,0,0,0,0,0,0,0,0,248,204,204,204,204,204,207,0,0,0,0,0,204,204,204,204,204,204,204,204,204,204,120,0,0,0,0,0,0,0,0,0,204,204,204,204,204,204,124,0,0,0,0,0,12,12,204,204,204,204,204,204,124,12,15,0,0,0,0,0,12,12,12,12,204,204,204,204,204,204,124,12,12,15,0,0,120,204,204,192,96,48,24,12,204,204,120,0,0,0,0,0,0,0,0,0,220,214,214,214,214,214,118,0,0,0,0,0,120,204,204,204,204,204,204,204,192,192,192,0,0,0,0,0,0,0,0,0,248,204,204,204,204,204,204,192,192,192,0,0,120,204,204,204,120,204,12,12,204,204,120,0,0,0,0,0,0,0,0,0,204,204,204,204,204,204,124,12,204,120,0,0,192,192,240,216,216,216,216,216,206,192,192,0,0,0,0,0,0,0,0,0,96,96,96,96,96,96,124,0,0,0,0,0,16,16,124,214,214,214,214,214,124,16,16,0,0,0,0,0,0,16,16,16,220,214,214,214,214,214,118,16,16,16,0,0,60,102,102,102,102,124,96,96,254,96,96,0,0,0,0,0,0,0,0,0,124,102,102,102,102,124,96,254,96,96,0,0,120,204,204,204,204,204,204,204,204,204,120,0,0,0,0,0,0,0,0,0,120,204,204,204,204,204,120,0,0,0,0,0,112,208,208,208,124,22,22,214,214,214,124,0,0,0,0,0,112,208,208,208,124,22,22,214,214,214,124,16,16,16,0,0,56,56,24,24,48,0,0,0,0,0,0,0,0,0,0,0,24,24,8,24,16,0,0,0,0,0,0,0,0,0,0};
