#pragma once
void print_colour(RTTXCB *rttxcb,int screen_x,int screen_y,const char *string, COLORREF colour,bool auto_kern=false,bool no_output=false);
void print_colour_right_justify(RTTXCB *rttxcb,int screen_x,int screen_y,const char *string, COLORREF colour,bool auto_kern=false);
void print_pretty_right_justify(RTTXCB *rttxcb,int screen_x,int screen_y,const char *string, COLORREF colour_alpha, COLORREF colour_number, COLORREF colour_operators, bool auto_kern=false);
void print_pretty(RTTXCB *rttxcb,int screen_x,int screen_y,const char *string, COLORREF colour_alpha, COLORREF colour_number, COLORREF colour_operators,bool auto_kern=false,bool no_output=false);
void print_colour_centered(RTTXCB *rttxcb,int screen_x,int screen_y,const char *string, COLORREF colour,bool auto_kern=false);
void print_pretty_centered(RTTXCB *rttxcb,int screen_x,int screen_y,const char *string, COLORREF colour_alpha, COLORREF colour_number, COLORREF colour_operators, bool auto_kern=false);
