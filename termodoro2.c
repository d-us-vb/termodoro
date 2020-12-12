#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>

#define CTRL_KEY(k) ((k) & 0x1f)
#define CRLF "\r\n"
#define ABUF_INIT {NULL, 0}

// display line font
#define FONT_COLON_INDEX 10
#define FONT_DECPT_INDEX 11

#define INPUT_QUEUE_SIZE 10


// display constants
#define DISPLAY_LINE_LENGTH 80
#define COMMAND_LINE_LENGTH 80

#define WINDOW_HEIGHT 24

// string constants for configuration and
const char BEGIN_SHORT_BREAK_SHORTCUT_STRING[] = "begin_short_break_shortcut";
const char POMODORO_LENGTH_SECONDS_STRING[] = "pomodoro_length_seconds";
const char POMODORO_SHORT_BREAK_LENGTH_SECONDS_STRING[] =
  "pomodoro_short_break_length_seconds";
const char POMODORO_LONG_BREAK_LENGTH_SECONDS_STRING[] =
  "pomodoro_long_break_length_seconds";
const char POMODORO_SET_LENGTH_STRING[] = "pomodoro_set_length";
const char DEFAULT_SESSION_POMODORO_GOAL_STRING[] = "default_session_pomodoro_goal";

const char CONTINUE_TRACKING_TIME_UPON_COMPLETION[] =
  "continue_tracking_time_upon_completion";
const char AUTO_START_SHORT_BREAKS_STRING[] = "auto_start_short_breaks";
const char AUTO_START_LONG_BREAKS_STRING[] = "auto_start_long_breaks";

const char SHOW_BIG_TIME_STRING[] = "show_big_time";
const char SHOW_SMALL_TIME_STRING[] = "show_small_time";
const char SHOW_TIME_LOGGED_SO_FAR_STRING[] = "show_time_logged_so_far";
const char SHOW_TIME_LEFT_STRING[] = "show_time_left";

const char EXIT_SHORTCUT_STRING[] = "exit_shortcut";
const char BEGIN_POMODORO_SHORTCUT_STRING[] = "begin_pomodoro_shortcut";
const char BEGIN_SHORT_BREAK_SHORTCUT_STRING[] = "begin_short_break_shortcut";
const char BEGIN_LONG_BREAK_SHORTCUT_STRING[] = "begin_long_break_shortcut";
const char ENTER_COMMAND_SHORTCUT_STRING[] = "enter_command_shortcut";
const char PREVIOUS_CHAR_SHORTCUT_STRING[] = "previous_char_shortcut";
const char NEXT_CHAR_SHORTCUT_STRING[] = "next_char_shortcut";
const char VIEW_HELP_DOCUMENT_SHORTCUT_STRING[] = "view_help_document_shortcut";

const char ALERT_WITH_AUDIO_STRING[] = "view_help_document_shortcut";
const char ALERT_AUDIO_FILE_STRING[] = "alert_audio_file";

