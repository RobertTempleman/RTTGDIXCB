/***********************************************************
Copyright 1987, 1994, 1998  The Open Group

Permission to use, copy, modify, distribute, and sell this software and its
documentation for any purpose is hereby granted without fee, provided that
the above copyright notice appear in all copies and that both that
copyright notice and this permission notice appear in supporting
documentation.

The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE OPEN GROUP BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

Except as contained in this notice, the name of The Open Group shall
not be used in advertising or otherwise to promote the sale, use or
other dealings in this Software without prior written authorization
from The Open Group.


Copyright 1987 by Digital Equipment Corporation, Maynard, Massachusetts

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its
documentation for any purpose and without fee is hereby granted,
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in
supporting documentation, and that the name of Digital not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.

DIGITAL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
DIGITAL BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

******************************************************************/

/*
 * The "X11 Window System Protocol" standard defines in Appendix A the
 * keysym codes. These 29-bit integer values identify characters or
 * functions associated with each key (e.g., via the visible
 * engraving) of a keyboard layout. This file assigns mnemonic macro
 * names for these keysyms.
 *
 * This file is also compiled (by src/util/makekeys.c in libX11) into
 * hash tables that can be accessed with X11 library functions such as
 * XStringToKeysym() and XKeysymToString().
 *
 * Where a keysym corresponds one-to-one to an ISO 10646 / Unicode
 * character, this is noted in a comment that provides both the U+xxxx
 * Unicode position, as well as the official Unicode name of the
 * character.
 *
 * Where the correspondence is either not one-to-one or semantically
 * unclear, the Unicode position and name are enclosed in
 * parentheses. Such legacy keysyms should be considered deprecated
 * and are not recommended for use in future keyboard mappings.
 *
 * For any future extension of the keysyms with characters already
 * found in ISO 10646 / Unicode, the following algorithm shall be
 * used. The new keysym code position will simply be the character's
 * Unicode number plus 0x01000000. The keysym values in the range
 * 0x01000100 to 0x0110ffff are reserved to represent Unicode
 * characters in the range U+0100 to U+10FFFF.
 * 
 * While most newer Unicode-based X11 clients do already accept
 * Unicode-mapped keysyms in the range 0x01000100 to 0x0110ffff, it
 * will remain necessary for clients -- in the interest of
 * compatibility with existing servers -- to also understand the
 * existing legacy keysym values in the range 0x0100 to 0x20ff.
 *
 * Where several mnemonic names are defined for the same keysym in this
 * file, all but the first one listed should be considered deprecated.
 *
 * Mnemonic names for keysyms are defined in this file with lines
 * that match one of these Perl regular expressions:
 *
 *    /^\#define RTT_XK_([a-zA-Z_0-9]+)\s+0x([0-9a-f]+)\s*\/\* U+([0-9A-F]{4,6}) (.*) \*\/\s*$/
 *    /^\#define RTT_XK_([a-zA-Z_0-9]+)\s+0x([0-9a-f]+)\s*\/\*\(U+([0-9A-F]{4,6}) (.*)\)\*\/\s*$/
 *    /^\#define RTT_XK_([a-zA-Z_0-9]+)\s+0x([0-9a-f]+)\s*(\/\*\s*(.*)\s*\*\/)?\s*$/
 *
 * Before adding new keysyms, please do consider the following: In
 * addition to the keysym names defined in this file, the
 * XStringToKeysym() and XKeysymToString() functions will also handle
 * any keysym string of the form "U0020" to "U007E" and "U00A0" to
 * "U10FFFF" for all possible Unicode characters. In other words,
 * every possible Unicode character has already a keysym string
 * defined algorithmically, even if it is not listed here. Therefore,
 * defining an additional keysym macro is only necessary where a
 * non-hexadecimal mnemonic name is needed, or where the new keysym
 * does not represent any existing Unicode character.
 *
 * When adding new keysyms to this file, do not forget to also update the
 * following as needed:
 *
 *   - the mappings in src/KeyBind.c in the repo
 *     git://anongit.freedesktop.org/xorg/lib/libX11.git
 *
 *   - the protocol specification in specs/keysyms.xml
 *     in the repo git://anongit.freedesktop.org/xorg/proto/x11proto.git
 *
 */

#define RTT_XK_VoidSymbol                  0xffffff  /* Void symbol */

//#ifdef RTT_XK_MISCELLANY
/*
 * TTY function keys, cleverly chosen to map to ASCII, for convenience of
 * programming, but could have been arbitrary (at the cost of lookup
 * tables in client code).
 */

#define RTT_XK_BackSpace                     0xff08  /* Back space, back char */
#define RTT_XK_Tab                           0xff09
#define RTT_XK_Linefeed                      0xff0a  /* Linefeed, LF */
#define RTT_XK_Clear                         0xff0b
#define RTT_XK_Return                        0xff0d  /* Return, enter */
#define RTT_XK_Pause                         0xff13  /* Pause, hold */
#define RTT_XK_Scroll_Lock                   0xff14
#define RTT_XK_Sys_Req                       0xff15
#define RTT_XK_Escape                        0xff1b
#define RTT_XK_Delete                        0xffff  /* Delete, rubout */

/* International & multi-key character composition */
#define RTT_XK_Multi_key                     0xff20  /* Multi-key character compose */
#define RTT_XK_Codeinput                     0xff37
#define RTT_XK_SingleCandidate               0xff3c
#define RTT_XK_MultipleCandidate             0xff3d
#define RTT_XK_PreviousCandidate             0xff3e

/* Cursor control & motion */
#define RTT_XK_Home                          0xff50
#define RTT_XK_Left                          0xff51  /* Move left, left arrow */
#define RTT_XK_Up                            0xff52  /* Move up, up arrow */
#define RTT_XK_Right                         0xff53  /* Move right, right arrow */
#define RTT_XK_Down                          0xff54  /* Move down, down arrow */
#define RTT_XK_Prior                         0xff55  /* Prior, previous */
#define RTT_XK_Page_Up                       0xff55
#define RTT_XK_Next                          0xff56  /* Next */
#define RTT_XK_Page_Down                     0xff56
#define RTT_XK_End                           0xff57  /* EOL */
#define RTT_XK_Begin                         0xff58  /* BOL */

/* Misc functions */
#define RTT_XK_Select                        0xff60  /* Select, mark */
#define RTT_XK_Print                         0xff61
#define RTT_XK_Execute                       0xff62  /* Execute, run, do */
#define RTT_XK_Insert                        0xff63  /* Insert, insert here */
#define RTT_XK_Undo                          0xff65
#define RTT_XK_Redo                          0xff66  /* Redo, again */
#define RTT_XK_Menu                          0xff67
#define RTT_XK_Find                          0xff68  /* Find, search */
#define RTT_XK_Cancel                        0xff69  /* Cancel, stop, abort, exit */
#define RTT_XK_Help                          0xff6a  /* Help */
#define RTT_XK_Break                         0xff6b
#define RTT_XK_Mode_switch                   0xff7e  /* Character set switch */
#define RTT_XK_script_switch                 0xff7e  /* Alias for mode_switch */
#define RTT_XK_Num_Lock                      0xff7f

/* Keypad functions, keypad numbers cleverly chosen to map to ASCII */
#define RTT_XK_KP_Space                      0xff80  /* Space */
#define RTT_XK_KP_Tab                        0xff89
#define RTT_XK_KP_Enter                      0xff8d  /* Enter */
#define RTT_XK_KP_F1                         0xff91  /* PF1, KP_A, ... */
#define RTT_XK_KP_F2                         0xff92
#define RTT_XK_KP_F3                         0xff93
#define RTT_XK_KP_F4                         0xff94
#define RTT_XK_KP_Home                       0xff95
#define RTT_XK_KP_Left                       0xff96
#define RTT_XK_KP_Up                         0xff97
#define RTT_XK_KP_Right                      0xff98
#define RTT_XK_KP_Down                       0xff99
#define RTT_XK_KP_Prior                      0xff9a
#define RTT_XK_KP_Page_Up                    0xff9a
#define RTT_XK_KP_Next                       0xff9b
#define RTT_XK_KP_Page_Down                  0xff9b
#define RTT_XK_KP_End                        0xff9c
#define RTT_XK_KP_Begin                      0xff9d
#define RTT_XK_KP_Insert                     0xff9e
#define RTT_XK_KP_Delete                     0xff9f
#define RTT_XK_KP_Equal                      0xffbd  /* Equals */
#define RTT_XK_KP_Multiply                   0xffaa
#define RTT_XK_KP_Add                        0xffab
#define RTT_XK_KP_Separator                  0xffac  /* Separator, often comma */
#define RTT_XK_KP_Subtract                   0xffad
#define RTT_XK_KP_Decimal                    0xffae
#define RTT_XK_KP_Divide                     0xffaf

#define RTT_XK_KP_0                          0xffb0
#define RTT_XK_KP_1                          0xffb1
#define RTT_XK_KP_2                          0xffb2
#define RTT_XK_KP_3                          0xffb3
#define RTT_XK_KP_4                          0xffb4
#define RTT_XK_KP_5                          0xffb5
#define RTT_XK_KP_6                          0xffb6
#define RTT_XK_KP_7                          0xffb7
#define RTT_XK_KP_8                          0xffb8
#define RTT_XK_KP_9                          0xffb9



/*
 * Auxiliary functions; note the duplicate definitions for left and right
 * function keys;  Sun keyboards and a few other manufacturers have such
 * function key groups on the left and/or right sides of the keyboard.
 * We've not found a keyboard with more than 35 function keys total.
 */

#define RTT_XK_F1                            0xffbe
#define RTT_XK_F2                            0xffbf
#define RTT_XK_F3                            0xffc0
#define RTT_XK_F4                            0xffc1
#define RTT_XK_F5                            0xffc2
#define RTT_XK_F6                            0xffc3
#define RTT_XK_F7                            0xffc4
#define RTT_XK_F8                            0xffc5
#define RTT_XK_F9                            0xffc6
#define RTT_XK_F10                           0xffc7
#define RTT_XK_F11                           0xffc8
#define RTT_XK_L1                            0xffc8
#define RTT_XK_F12                           0xffc9
#define RTT_XK_L2                            0xffc9
#define RTT_XK_F13                           0xffca
#define RTT_XK_L3                            0xffca
#define RTT_XK_F14                           0xffcb
#define RTT_XK_L4                            0xffcb
#define RTT_XK_F15                           0xffcc
#define RTT_XK_L5                            0xffcc
#define RTT_XK_F16                           0xffcd
#define RTT_XK_L6                            0xffcd
#define RTT_XK_F17                           0xffce
#define RTT_XK_L7                            0xffce
#define RTT_XK_F18                           0xffcf
#define RTT_XK_L8                            0xffcf
#define RTT_XK_F19                           0xffd0
#define RTT_XK_L9                            0xffd0
#define RTT_XK_F20                           0xffd1
#define RTT_XK_L10                           0xffd1
#define RTT_XK_F21                           0xffd2
#define RTT_XK_R1                            0xffd2
#define RTT_XK_F22                           0xffd3
#define RTT_XK_R2                            0xffd3
#define RTT_XK_F23                           0xffd4
#define RTT_XK_R3                            0xffd4
#define RTT_XK_F24                           0xffd5
#define RTT_XK_R4                            0xffd5
#define RTT_XK_F25                           0xffd6
#define RTT_XK_R5                            0xffd6
#define RTT_XK_F26                           0xffd7
#define RTT_XK_R6                            0xffd7
#define RTT_XK_F27                           0xffd8
#define RTT_XK_R7                            0xffd8
#define RTT_XK_F28                           0xffd9
#define RTT_XK_R8                            0xffd9
#define RTT_XK_F29                           0xffda
#define RTT_XK_R9                            0xffda
#define RTT_XK_F30                           0xffdb
#define RTT_XK_R10                           0xffdb
#define RTT_XK_F31                           0xffdc
#define RTT_XK_R11                           0xffdc
#define RTT_XK_F32                           0xffdd
#define RTT_XK_R12                           0xffdd
#define RTT_XK_F33                           0xffde
#define RTT_XK_R13                           0xffde
#define RTT_XK_F34                           0xffdf
#define RTT_XK_R14                           0xffdf
#define RTT_XK_F35                           0xffe0
#define RTT_XK_R15                           0xffe0

/* Modifiers */

#define RTT_XK_Shift_L                       0xffe1  /* Left shift */
#define RTT_XK_Shift_R                       0xffe2  /* Right shift */
#define RTT_XK_Control_L                     0xffe3  /* Left control */
#define RTT_XK_Control_R                     0xffe4  /* Right control */
#define RTT_XK_Caps_Lock                     0xffe5  /* Caps lock */
#define RTT_XK_Shift_Lock                    0xffe6  /* Shift lock */

#define RTT_XK_Meta_L                        0xffe7  /* Left meta */
#define RTT_XK_Meta_R                        0xffe8  /* Right meta */
#define RTT_XK_Alt_L                         0xffe9  /* Left alt */
#define RTT_XK_Alt_R                         0xffea  /* Right alt */
#define RTT_XK_Super_L                       0xffeb  /* Left super */
#define RTT_XK_Super_R                       0xffec  /* Right super */
#define RTT_XK_Hyper_L                       0xffed  /* Left hyper */
#define RTT_XK_Hyper_R                       0xffee  /* Right hyper */
//#endif /* RTT_XK_MISCELLANY */

/*
 * Keyboard (XKB) Extension function and modifier keys
 * (from Appendix C of "The X Keyboard Extension: Protocol Specification")
 * Byte 3 = 0xfe
 */

//#ifdef RTT_XK_XKB_KEYS
#define RTT_XK_ISO_Lock                      0xfe01
#define RTT_XK_ISO_Level2_Latch              0xfe02
#define RTT_XK_ISO_Level3_Shift              0xfe03
#define RTT_XK_ISO_Level3_Latch              0xfe04
#define RTT_XK_ISO_Level3_Lock               0xfe05
#define RTT_XK_ISO_Level5_Shift              0xfe11
#define RTT_XK_ISO_Level5_Latch              0xfe12
#define RTT_XK_ISO_Level5_Lock               0xfe13
#define RTT_XK_ISO_Group_Shift               0xff7e  /* Alias for mode_switch */
#define RTT_XK_ISO_Group_Latch               0xfe06
#define RTT_XK_ISO_Group_Lock                0xfe07
#define RTT_XK_ISO_Next_Group                0xfe08
#define RTT_XK_ISO_Next_Group_Lock           0xfe09
#define RTT_XK_ISO_Prev_Group                0xfe0a
#define RTT_XK_ISO_Prev_Group_Lock           0xfe0b
#define RTT_XK_ISO_First_Group               0xfe0c
#define RTT_XK_ISO_First_Group_Lock          0xfe0d
#define RTT_XK_ISO_Last_Group                0xfe0e
#define RTT_XK_ISO_Last_Group_Lock           0xfe0f

#define RTT_XK_ISO_Left_Tab                  0xfe20
#define RTT_XK_ISO_Move_Line_Up              0xfe21
#define RTT_XK_ISO_Move_Line_Down            0xfe22
#define RTT_XK_ISO_Partial_Line_Up           0xfe23
#define RTT_XK_ISO_Partial_Line_Down         0xfe24
#define RTT_XK_ISO_Partial_Space_Left        0xfe25
#define RTT_XK_ISO_Partial_Space_Right       0xfe26
#define RTT_XK_ISO_Set_Margin_Left           0xfe27
#define RTT_XK_ISO_Set_Margin_Right          0xfe28
#define RTT_XK_ISO_Release_Margin_Left       0xfe29
#define RTT_XK_ISO_Release_Margin_Right      0xfe2a
#define RTT_XK_ISO_Release_Both_Margins      0xfe2b
#define RTT_XK_ISO_Fast_Cursor_Left          0xfe2c
#define RTT_XK_ISO_Fast_Cursor_Right         0xfe2d
#define RTT_XK_ISO_Fast_Cursor_Up            0xfe2e
#define RTT_XK_ISO_Fast_Cursor_Down          0xfe2f
#define RTT_XK_ISO_Continuous_Underline      0xfe30
#define RTT_XK_ISO_Discontinuous_Underline   0xfe31
#define RTT_XK_ISO_Emphasize                 0xfe32
#define RTT_XK_ISO_Center_Object             0xfe33
#define RTT_XK_ISO_Enter                     0xfe34

#define RTT_XK_dead_grave                    0xfe50
#define RTT_XK_dead_acute                    0xfe51
#define RTT_XK_dead_circumflex               0xfe52
#define RTT_XK_dead_tilde                    0xfe53
#define RTT_XK_dead_perispomeni              0xfe53  /* alias for dead_tilde */
#define RTT_XK_dead_macron                   0xfe54
#define RTT_XK_dead_breve                    0xfe55
#define RTT_XK_dead_abovedot                 0xfe56
#define RTT_XK_dead_diaeresis                0xfe57
#define RTT_XK_dead_abovering                0xfe58
#define RTT_XK_dead_doubleacute              0xfe59
#define RTT_XK_dead_caron                    0xfe5a
#define RTT_XK_dead_cedilla                  0xfe5b
#define RTT_XK_dead_ogonek                   0xfe5c
#define RTT_XK_dead_iota                     0xfe5d
#define RTT_XK_dead_voiced_sound             0xfe5e
#define RTT_XK_dead_semivoiced_sound         0xfe5f
#define RTT_XK_dead_belowdot                 0xfe60
#define RTT_XK_dead_hook                     0xfe61
#define RTT_XK_dead_horn                     0xfe62
#define RTT_XK_dead_stroke                   0xfe63
#define RTT_XK_dead_abovecomma               0xfe64
#define RTT_XK_dead_psili                    0xfe64  /* alias for dead_abovecomma */
#define RTT_XK_dead_abovereversedcomma       0xfe65
#define RTT_XK_dead_dasia                    0xfe65  /* alias for dead_abovereversedcomma */
#define RTT_XK_dead_doublegrave              0xfe66
#define RTT_XK_dead_belowring                0xfe67
#define RTT_XK_dead_belowmacron              0xfe68
#define RTT_XK_dead_belowcircumflex          0xfe69
#define RTT_XK_dead_belowtilde               0xfe6a
#define RTT_XK_dead_belowbreve               0xfe6b
#define RTT_XK_dead_belowdiaeresis           0xfe6c
#define RTT_XK_dead_invertedbreve            0xfe6d
#define RTT_XK_dead_belowcomma               0xfe6e
#define RTT_XK_dead_currency                 0xfe6f

/* extra dead elements for German T3 layout */
#define RTT_XK_dead_lowline                  0xfe90
#define RTT_XK_dead_aboveverticalline        0xfe91
#define RTT_XK_dead_belowverticalline        0xfe92
#define RTT_XK_dead_longsolidusoverlay       0xfe93

/* dead vowels for universal syllable entry */
#define RTT_XK_dead_a                        0xfe80
#define RTT_XK_dead_A                        0xfe81
#define RTT_XK_dead_e                        0xfe82
#define RTT_XK_dead_E                        0xfe83
#define RTT_XK_dead_i                        0xfe84
#define RTT_XK_dead_I                        0xfe85
#define RTT_XK_dead_o                        0xfe86
#define RTT_XK_dead_O                        0xfe87
#define RTT_XK_dead_u                        0xfe88
#define RTT_XK_dead_U                        0xfe89
#define RTT_XK_dead_small_schwa              0xfe8a
#define RTT_XK_dead_capital_schwa            0xfe8b

#define RTT_XK_dead_greek                    0xfe8c

#define RTT_XK_First_Virtual_Screen          0xfed0
#define RTT_XK_Prev_Virtual_Screen           0xfed1
#define RTT_XK_Next_Virtual_Screen           0xfed2
#define RTT_XK_Last_Virtual_Screen           0xfed4
#define RTT_XK_Terminate_Server              0xfed5

#define RTT_XK_AccessX_Enable                0xfe70
#define RTT_XK_AccessX_Feedback_Enable       0xfe71
#define RTT_XK_RepeatKeys_Enable             0xfe72
#define RTT_XK_SlowKeys_Enable               0xfe73
#define RTT_XK_BounceKeys_Enable             0xfe74
#define RTT_XK_StickyKeys_Enable             0xfe75
#define RTT_XK_MouseKeys_Enable              0xfe76
#define RTT_XK_MouseKeys_Accel_Enable        0xfe77
#define RTT_XK_Overlay1_Enable               0xfe78
#define RTT_XK_Overlay2_Enable               0xfe79
#define RTT_XK_AudibleBell_Enable            0xfe7a

#define RTT_XK_Pointer_Left                  0xfee0
#define RTT_XK_Pointer_Right                 0xfee1
#define RTT_XK_Pointer_Up                    0xfee2
#define RTT_XK_Pointer_Down                  0xfee3
#define RTT_XK_Pointer_UpLeft                0xfee4
#define RTT_XK_Pointer_UpRight               0xfee5
#define RTT_XK_Pointer_DownLeft              0xfee6
#define RTT_XK_Pointer_DownRight             0xfee7
#define RTT_XK_Pointer_Button_Dflt           0xfee8
#define RTT_XK_Pointer_Button1               0xfee9
#define RTT_XK_Pointer_Button2               0xfeea
#define RTT_XK_Pointer_Button3               0xfeeb
#define RTT_XK_Pointer_Button4               0xfeec
#define RTT_XK_Pointer_Button5               0xfeed
#define RTT_XK_Pointer_DblClick_Dflt         0xfeee
#define RTT_XK_Pointer_DblClick1             0xfeef
#define RTT_XK_Pointer_DblClick2             0xfef0
#define RTT_XK_Pointer_DblClick3             0xfef1
#define RTT_XK_Pointer_DblClick4             0xfef2
#define RTT_XK_Pointer_DblClick5             0xfef3
#define RTT_XK_Pointer_Drag_Dflt             0xfef4
#define RTT_XK_Pointer_Drag1                 0xfef5
#define RTT_XK_Pointer_Drag2                 0xfef6
#define RTT_XK_Pointer_Drag3                 0xfef7
#define RTT_XK_Pointer_Drag4                 0xfef8
#define RTT_XK_Pointer_Drag5                 0xfefd

#define RTT_XK_Pointer_EnableKeys            0xfef9
#define RTT_XK_Pointer_Accelerate            0xfefa
#define RTT_XK_Pointer_DfltBtnNext           0xfefb
#define RTT_XK_Pointer_DfltBtnPrev           0xfefc

/* Single-Stroke Multiple-Character N-Graph Keysyms For The X Input Method */

#define RTT_XK_ch                            0xfea0
#define RTT_XK_Ch                            0xfea1
#define RTT_XK_CH                            0xfea2
#define RTT_XK_c_h                           0xfea3
#define RTT_XK_C_h                           0xfea4
#define RTT_XK_C_H                           0xfea5

//#endif /* RTT_XK_XKB_KEYS */

/*
 * 3270 Terminal Keys
 * Byte 3 = 0xfd
 */

#ifdef RTT_XK_3270
#define RTT_XK_3270_Duplicate                0xfd01
#define RTT_XK_3270_FieldMark                0xfd02
#define RTT_XK_3270_Right2                   0xfd03
#define RTT_XK_3270_Left2                    0xfd04
#define RTT_XK_3270_BackTab                  0xfd05
#define RTT_XK_3270_EraseEOF                 0xfd06
#define RTT_XK_3270_EraseInput               0xfd07
#define RTT_XK_3270_Reset                    0xfd08
#define RTT_XK_3270_Quit                     0xfd09
#define RTT_XK_3270_PA1                      0xfd0a
#define RTT_XK_3270_PA2                      0xfd0b
#define RTT_XK_3270_PA3                      0xfd0c
#define RTT_XK_3270_Test                     0xfd0d
#define RTT_XK_3270_Attn                     0xfd0e
#define RTT_XK_3270_CursorBlink              0xfd0f
#define RTT_XK_3270_AltCursor                0xfd10
#define RTT_XK_3270_KeyClick                 0xfd11
#define RTT_XK_3270_Jump                     0xfd12
#define RTT_XK_3270_Ident                    0xfd13
#define RTT_XK_3270_Rule                     0xfd14
#define RTT_XK_3270_Copy                     0xfd15
#define RTT_XK_3270_Play                     0xfd16
#define RTT_XK_3270_Setup                    0xfd17
#define RTT_XK_3270_Record                   0xfd18
#define RTT_XK_3270_ChangeScreen             0xfd19
#define RTT_XK_3270_DeleteWord               0xfd1a
#define RTT_XK_3270_ExSelect                 0xfd1b
#define RTT_XK_3270_CursorSelect             0xfd1c
#define RTT_XK_3270_PrintScreen              0xfd1d
#define RTT_XK_3270_Enter                    0xfd1e
#endif /* RTT_XK_3270 */

/*
 * Latin 1
 * (ISO/IEC 8859-1 = Unicode U+0020..U+00FF)
 * Byte 3 = 0
 */
//#ifdef RTT_XK_LATIN1
#define RTT_XK_space                         0x0020  /* U+0020 SPACE */
#define RTT_XK_exclam                        0x0021  /* U+0021 EXCLAMATION MARK */
#define RTT_XK_quotedbl                      0x0022  /* U+0022 QUOTATION MARK */
#define RTT_XK_numbersign                    0x0023  /* U+0023 NUMBER SIGN */
#define RTT_XK_dollar                        0x0024  /* U+0024 DOLLAR SIGN */
#define RTT_XK_percent                       0x0025  /* U+0025 PERCENT SIGN */
#define RTT_XK_ampersand                     0x0026  /* U+0026 AMPERSAND */
#define RTT_XK_apostrophe                    0x0027  /* U+0027 APOSTROPHE */
#define RTT_XK_quoteright                    0x0027  /* deprecated */
#define RTT_XK_parenleft                     0x0028  /* U+0028 LEFT PARENTHESIS */
#define RTT_XK_parenright                    0x0029  /* U+0029 RIGHT PARENTHESIS */
#define RTT_XK_asterisk                      0x002a  /* U+002A ASTERISK */
#define RTT_XK_plus                          0x002b  /* U+002B PLUS SIGN */
#define RTT_XK_comma                         0x002c  /* U+002C COMMA */
#define RTT_XK_minus                         0x002d  /* U+002D HYPHEN-MINUS */
#define RTT_XK_period                        0x002e  /* U+002E FULL STOP */
#define RTT_XK_slash                         0x002f  /* U+002F SOLIDUS */
#define RTT_XK_0                             0x0030  /* U+0030 DIGIT ZERO */
#define RTT_XK_1                             0x0031  /* U+0031 DIGIT ONE */
#define RTT_XK_2                             0x0032  /* U+0032 DIGIT TWO */
#define RTT_XK_3                             0x0033  /* U+0033 DIGIT THREE */
#define RTT_XK_4                             0x0034  /* U+0034 DIGIT FOUR */
#define RTT_XK_5                             0x0035  /* U+0035 DIGIT FIVE */
#define RTT_XK_6                             0x0036  /* U+0036 DIGIT SIX */
#define RTT_XK_7                             0x0037  /* U+0037 DIGIT SEVEN */
#define RTT_XK_8                             0x0038  /* U+0038 DIGIT EIGHT */
#define RTT_XK_9                             0x0039  /* U+0039 DIGIT NINE */
#define RTT_XK_colon                         0x003a  /* U+003A COLON */
#define RTT_XK_semicolon                     0x003b  /* U+003B SEMICOLON */
#define RTT_XK_less                          0x003c  /* U+003C LESS-THAN SIGN */
#define RTT_XK_equal                         0x003d  /* U+003D EQUALS SIGN */
#define RTT_XK_greater                       0x003e  /* U+003E GREATER-THAN SIGN */
#define RTT_XK_question                      0x003f  /* U+003F QUESTION MARK */
#define RTT_XK_at                            0x0040  /* U+0040 COMMERCIAL AT */
#define RTT_XK_A                             0x0041  /* U+0041 LATIN CAPITAL LETTER A */
#define RTT_XK_B                             0x0042  /* U+0042 LATIN CAPITAL LETTER B */
#define RTT_XK_C                             0x0043  /* U+0043 LATIN CAPITAL LETTER C */
#define RTT_XK_D                             0x0044  /* U+0044 LATIN CAPITAL LETTER D */
#define RTT_XK_E                             0x0045  /* U+0045 LATIN CAPITAL LETTER E */
#define RTT_XK_F                             0x0046  /* U+0046 LATIN CAPITAL LETTER F */
#define RTT_XK_G                             0x0047  /* U+0047 LATIN CAPITAL LETTER G */
#define RTT_XK_H                             0x0048  /* U+0048 LATIN CAPITAL LETTER H */
#define RTT_XK_I                             0x0049  /* U+0049 LATIN CAPITAL LETTER I */
#define RTT_XK_J                             0x004a  /* U+004A LATIN CAPITAL LETTER J */
#define RTT_XK_K                             0x004b  /* U+004B LATIN CAPITAL LETTER K */
#define RTT_XK_L                             0x004c  /* U+004C LATIN CAPITAL LETTER L */
#define RTT_XK_M                             0x004d  /* U+004D LATIN CAPITAL LETTER M */
#define RTT_XK_N                             0x004e  /* U+004E LATIN CAPITAL LETTER N */
#define RTT_XK_O                             0x004f  /* U+004F LATIN CAPITAL LETTER O */
#define RTT_XK_P                             0x0050  /* U+0050 LATIN CAPITAL LETTER P */
#define RTT_XK_Q                             0x0051  /* U+0051 LATIN CAPITAL LETTER Q */
#define RTT_XK_R                             0x0052  /* U+0052 LATIN CAPITAL LETTER R */
#define RTT_XK_S                             0x0053  /* U+0053 LATIN CAPITAL LETTER S */
#define RTT_XK_T                             0x0054  /* U+0054 LATIN CAPITAL LETTER T */
#define RTT_XK_U                             0x0055  /* U+0055 LATIN CAPITAL LETTER U */
#define RTT_XK_V                             0x0056  /* U+0056 LATIN CAPITAL LETTER V */
#define RTT_XK_W                             0x0057  /* U+0057 LATIN CAPITAL LETTER W */
#define RTT_XK_X                             0x0058  /* U+0058 LATIN CAPITAL LETTER X */
#define RTT_XK_Y                             0x0059  /* U+0059 LATIN CAPITAL LETTER Y */
#define RTT_XK_Z                             0x005a  /* U+005A LATIN CAPITAL LETTER Z */
#define RTT_XK_bracketleft                   0x005b  /* U+005B LEFT SQUARE BRACKET */
#define RTT_XK_backslash                     0x005c  /* U+005C REVERSE SOLIDUS */
#define RTT_XK_bracketright                  0x005d  /* U+005D RIGHT SQUARE BRACKET */
#define RTT_XK_asciicircum                   0x005e  /* U+005E CIRCUMFLEX ACCENT */
#define RTT_XK_underscore                    0x005f  /* U+005F LOW LINE */
#define RTT_XK_grave                         0x0060  /* U+0060 GRAVE ACCENT */
#define RTT_XK_quoteleft                     0x0060  /* deprecated */
#define RTT_XK_a                             0x0061  /* U+0061 LATIN SMALL LETTER A */
#define RTT_XK_b                             0x0062  /* U+0062 LATIN SMALL LETTER B */
#define RTT_XK_c                             0x0063  /* U+0063 LATIN SMALL LETTER C */
#define RTT_XK_d                             0x0064  /* U+0064 LATIN SMALL LETTER D */
#define RTT_XK_e                             0x0065  /* U+0065 LATIN SMALL LETTER E */
#define RTT_XK_f                             0x0066  /* U+0066 LATIN SMALL LETTER F */
#define RTT_XK_g                             0x0067  /* U+0067 LATIN SMALL LETTER G */
#define RTT_XK_h                             0x0068  /* U+0068 LATIN SMALL LETTER H */
#define RTT_XK_i                             0x0069  /* U+0069 LATIN SMALL LETTER I */
#define RTT_XK_j                             0x006a  /* U+006A LATIN SMALL LETTER J */
#define RTT_XK_k                             0x006b  /* U+006B LATIN SMALL LETTER K */
#define RTT_XK_l                             0x006c  /* U+006C LATIN SMALL LETTER L */
#define RTT_XK_m                             0x006d  /* U+006D LATIN SMALL LETTER M */
#define RTT_XK_n                             0x006e  /* U+006E LATIN SMALL LETTER N */
#define RTT_XK_o                             0x006f  /* U+006F LATIN SMALL LETTER O */
#define RTT_XK_p                             0x0070  /* U+0070 LATIN SMALL LETTER P */
#define RTT_XK_q                             0x0071  /* U+0071 LATIN SMALL LETTER Q */
#define RTT_XK_r                             0x0072  /* U+0072 LATIN SMALL LETTER R */
#define RTT_XK_s                             0x0073  /* U+0073 LATIN SMALL LETTER S */
#define RTT_XK_t                             0x0074  /* U+0074 LATIN SMALL LETTER T */
#define RTT_XK_u                             0x0075  /* U+0075 LATIN SMALL LETTER U */
#define RTT_XK_v                             0x0076  /* U+0076 LATIN SMALL LETTER V */
#define RTT_XK_w                             0x0077  /* U+0077 LATIN SMALL LETTER W */
#define RTT_XK_x                             0x0078  /* U+0078 LATIN SMALL LETTER X */
#define RTT_XK_y                             0x0079  /* U+0079 LATIN SMALL LETTER Y */
#define RTT_XK_z                             0x007a  /* U+007A LATIN SMALL LETTER Z */
#define RTT_XK_braceleft                     0x007b  /* U+007B LEFT CURLY BRACKET */
#define RTT_XK_bar                           0x007c  /* U+007C VERTICAL LINE */
#define RTT_XK_braceright                    0x007d  /* U+007D RIGHT CURLY BRACKET */
#define RTT_XK_asciitilde                    0x007e  /* U+007E TILDE */

#define RTT_XK_nobreakspace                  0x00a0  /* U+00A0 NO-BREAK SPACE */
#define RTT_XK_exclamdown                    0x00a1  /* U+00A1 INVERTED EXCLAMATION MARK */
#define RTT_XK_cent                          0x00a2  /* U+00A2 CENT SIGN */
#define RTT_XK_sterling                      0x00a3  /* U+00A3 POUND SIGN */
#define RTT_XK_currency                      0x00a4  /* U+00A4 CURRENCY SIGN */
#define RTT_XK_yen                           0x00a5  /* U+00A5 YEN SIGN */
#define RTT_XK_brokenbar                     0x00a6  /* U+00A6 BROKEN BAR */
#define RTT_XK_section                       0x00a7  /* U+00A7 SECTION SIGN */
#define RTT_XK_diaeresis                     0x00a8  /* U+00A8 DIAERESIS */
#define RTT_XK_copyright                     0x00a9  /* U+00A9 COPYRIGHT SIGN */
#define RTT_XK_ordfeminine                   0x00aa  /* U+00AA FEMININE ORDINAL INDICATOR */
#define RTT_XK_guillemotleft                 0x00ab  /* U+00AB LEFT-POINTING DOUBLE ANGLE QUOTATION MARK */
#define RTT_XK_notsign                       0x00ac  /* U+00AC NOT SIGN */
#define RTT_XK_hyphen                        0x00ad  /* U+00AD SOFT HYPHEN */
#define RTT_XK_registered                    0x00ae  /* U+00AE REGISTERED SIGN */
#define RTT_XK_macron                        0x00af  /* U+00AF MACRON */
#define RTT_XK_degree                        0x00b0  /* U+00B0 DEGREE SIGN */
#define RTT_XK_plusminus                     0x00b1  /* U+00B1 PLUS-MINUS SIGN */
#define RTT_XK_twosuperior                   0x00b2  /* U+00B2 SUPERSCRIPT TWO */
#define RTT_XK_threesuperior                 0x00b3  /* U+00B3 SUPERSCRIPT THREE */
#define RTT_XK_acute                         0x00b4  /* U+00B4 ACUTE ACCENT */
#define RTT_XK_mu                            0x00b5  /* U+00B5 MICRO SIGN */
#define RTT_XK_paragraph                     0x00b6  /* U+00B6 PILCROW SIGN */
#define RTT_XK_periodcentered                0x00b7  /* U+00B7 MIDDLE DOT */
#define RTT_XK_cedilla                       0x00b8  /* U+00B8 CEDILLA */
#define RTT_XK_onesuperior                   0x00b9  /* U+00B9 SUPERSCRIPT ONE */
#define RTT_XK_masculine                     0x00ba  /* U+00BA MASCULINE ORDINAL INDICATOR */
#define RTT_XK_guillemotright                0x00bb  /* U+00BB RIGHT-POINTING DOUBLE ANGLE QUOTATION MARK */
#define RTT_XK_onequarter                    0x00bc  /* U+00BC VULGAR FRACTION ONE QUARTER */
#define RTT_XK_onehalf                       0x00bd  /* U+00BD VULGAR FRACTION ONE HALF */
#define RTT_XK_threequarters                 0x00be  /* U+00BE VULGAR FRACTION THREE QUARTERS */
#define RTT_XK_questiondown                  0x00bf  /* U+00BF INVERTED QUESTION MARK */
#define RTT_XK_Agrave                        0x00c0  /* U+00C0 LATIN CAPITAL LETTER A WITH GRAVE */
#define RTT_XK_Aacute                        0x00c1  /* U+00C1 LATIN CAPITAL LETTER A WITH ACUTE */
#define RTT_XK_Acircumflex                   0x00c2  /* U+00C2 LATIN CAPITAL LETTER A WITH CIRCUMFLEX */
#define RTT_XK_Atilde                        0x00c3  /* U+00C3 LATIN CAPITAL LETTER A WITH TILDE */
#define RTT_XK_Adiaeresis                    0x00c4  /* U+00C4 LATIN CAPITAL LETTER A WITH DIAERESIS */
#define RTT_XK_Aring                         0x00c5  /* U+00C5 LATIN CAPITAL LETTER A WITH RING ABOVE */
#define RTT_XK_AE                            0x00c6  /* U+00C6 LATIN CAPITAL LETTER AE */
#define RTT_XK_Ccedilla                      0x00c7  /* U+00C7 LATIN CAPITAL LETTER C WITH CEDILLA */
#define RTT_XK_Egrave                        0x00c8  /* U+00C8 LATIN CAPITAL LETTER E WITH GRAVE */
#define RTT_XK_Eacute                        0x00c9  /* U+00C9 LATIN CAPITAL LETTER E WITH ACUTE */
#define RTT_XK_Ecircumflex                   0x00ca  /* U+00CA LATIN CAPITAL LETTER E WITH CIRCUMFLEX */
#define RTT_XK_Ediaeresis                    0x00cb  /* U+00CB LATIN CAPITAL LETTER E WITH DIAERESIS */
#define RTT_XK_Igrave                        0x00cc  /* U+00CC LATIN CAPITAL LETTER I WITH GRAVE */
#define RTT_XK_Iacute                        0x00cd  /* U+00CD LATIN CAPITAL LETTER I WITH ACUTE */
#define RTT_XK_Icircumflex                   0x00ce  /* U+00CE LATIN CAPITAL LETTER I WITH CIRCUMFLEX */
#define RTT_XK_Idiaeresis                    0x00cf  /* U+00CF LATIN CAPITAL LETTER I WITH DIAERESIS */
#define RTT_XK_ETH                           0x00d0  /* U+00D0 LATIN CAPITAL LETTER ETH */
#define RTT_XK_Eth                           0x00d0  /* deprecated */
#define RTT_XK_Ntilde                        0x00d1  /* U+00D1 LATIN CAPITAL LETTER N WITH TILDE */
#define RTT_XK_Ograve                        0x00d2  /* U+00D2 LATIN CAPITAL LETTER O WITH GRAVE */
#define RTT_XK_Oacute                        0x00d3  /* U+00D3 LATIN CAPITAL LETTER O WITH ACUTE */
#define RTT_XK_Ocircumflex                   0x00d4  /* U+00D4 LATIN CAPITAL LETTER O WITH CIRCUMFLEX */
#define RTT_XK_Otilde                        0x00d5  /* U+00D5 LATIN CAPITAL LETTER O WITH TILDE */
#define RTT_XK_Odiaeresis                    0x00d6  /* U+00D6 LATIN CAPITAL LETTER O WITH DIAERESIS */
#define RTT_XK_multiply                      0x00d7  /* U+00D7 MULTIPLICATION SIGN */
#define RTT_XK_Oslash                        0x00d8  /* U+00D8 LATIN CAPITAL LETTER O WITH STROKE */
#define RTT_XK_Ooblique                      0x00d8  /* U+00D8 LATIN CAPITAL LETTER O WITH STROKE */
#define RTT_XK_Ugrave                        0x00d9  /* U+00D9 LATIN CAPITAL LETTER U WITH GRAVE */
#define RTT_XK_Uacute                        0x00da  /* U+00DA LATIN CAPITAL LETTER U WITH ACUTE */
#define RTT_XK_Ucircumflex                   0x00db  /* U+00DB LATIN CAPITAL LETTER U WITH CIRCUMFLEX */
#define RTT_XK_Udiaeresis                    0x00dc  /* U+00DC LATIN CAPITAL LETTER U WITH DIAERESIS */
#define RTT_XK_Yacute                        0x00dd  /* U+00DD LATIN CAPITAL LETTER Y WITH ACUTE */
#define RTT_XK_THORN                         0x00de  /* U+00DE LATIN CAPITAL LETTER THORN */
#define RTT_XK_Thorn                         0x00de  /* deprecated */
#define RTT_XK_ssharp                        0x00df  /* U+00DF LATIN SMALL LETTER SHARP S */
#define RTT_XK_agrave                        0x00e0  /* U+00E0 LATIN SMALL LETTER A WITH GRAVE */
#define RTT_XK_aacute                        0x00e1  /* U+00E1 LATIN SMALL LETTER A WITH ACUTE */
#define RTT_XK_acircumflex                   0x00e2  /* U+00E2 LATIN SMALL LETTER A WITH CIRCUMFLEX */
#define RTT_XK_atilde                        0x00e3  /* U+00E3 LATIN SMALL LETTER A WITH TILDE */
#define RTT_XK_adiaeresis                    0x00e4  /* U+00E4 LATIN SMALL LETTER A WITH DIAERESIS */
#define RTT_XK_aring                         0x00e5  /* U+00E5 LATIN SMALL LETTER A WITH RING ABOVE */
#define RTT_XK_ae                            0x00e6  /* U+00E6 LATIN SMALL LETTER AE */
#define RTT_XK_ccedilla                      0x00e7  /* U+00E7 LATIN SMALL LETTER C WITH CEDILLA */
#define RTT_XK_egrave                        0x00e8  /* U+00E8 LATIN SMALL LETTER E WITH GRAVE */
#define RTT_XK_eacute                        0x00e9  /* U+00E9 LATIN SMALL LETTER E WITH ACUTE */
#define RTT_XK_ecircumflex                   0x00ea  /* U+00EA LATIN SMALL LETTER E WITH CIRCUMFLEX */
#define RTT_XK_ediaeresis                    0x00eb  /* U+00EB LATIN SMALL LETTER E WITH DIAERESIS */
#define RTT_XK_igrave                        0x00ec  /* U+00EC LATIN SMALL LETTER I WITH GRAVE */
#define RTT_XK_iacute                        0x00ed  /* U+00ED LATIN SMALL LETTER I WITH ACUTE */
#define RTT_XK_icircumflex                   0x00ee  /* U+00EE LATIN SMALL LETTER I WITH CIRCUMFLEX */
#define RTT_XK_idiaeresis                    0x00ef  /* U+00EF LATIN SMALL LETTER I WITH DIAERESIS */
#define RTT_XK_eth                           0x00f0  /* U+00F0 LATIN SMALL LETTER ETH */
#define RTT_XK_ntilde                        0x00f1  /* U+00F1 LATIN SMALL LETTER N WITH TILDE */
#define RTT_XK_ograve                        0x00f2  /* U+00F2 LATIN SMALL LETTER O WITH GRAVE */
#define RTT_XK_oacute                        0x00f3  /* U+00F3 LATIN SMALL LETTER O WITH ACUTE */
#define RTT_XK_ocircumflex                   0x00f4  /* U+00F4 LATIN SMALL LETTER O WITH CIRCUMFLEX */
#define RTT_XK_otilde                        0x00f5  /* U+00F5 LATIN SMALL LETTER O WITH TILDE */
#define RTT_XK_odiaeresis                    0x00f6  /* U+00F6 LATIN SMALL LETTER O WITH DIAERESIS */
#define RTT_XK_division                      0x00f7  /* U+00F7 DIVISION SIGN */
#define RTT_XK_oslash                        0x00f8  /* U+00F8 LATIN SMALL LETTER O WITH STROKE */
#define RTT_XK_ooblique                      0x00f8  /* U+00F8 LATIN SMALL LETTER O WITH STROKE */
#define RTT_XK_ugrave                        0x00f9  /* U+00F9 LATIN SMALL LETTER U WITH GRAVE */
#define RTT_XK_uacute                        0x00fa  /* U+00FA LATIN SMALL LETTER U WITH ACUTE */
#define RTT_XK_ucircumflex                   0x00fb  /* U+00FB LATIN SMALL LETTER U WITH CIRCUMFLEX */
#define RTT_XK_udiaeresis                    0x00fc  /* U+00FC LATIN SMALL LETTER U WITH DIAERESIS */
#define RTT_XK_yacute                        0x00fd  /* U+00FD LATIN SMALL LETTER Y WITH ACUTE */
#define RTT_XK_thorn                         0x00fe  /* U+00FE LATIN SMALL LETTER THORN */
#define RTT_XK_ydiaeresis                    0x00ff  /* U+00FF LATIN SMALL LETTER Y WITH DIAERESIS */
//#endif /* RTT_XK_LATIN1 */
