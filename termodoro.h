#ifndef TERMODORO_H
#define TERMODORO_H

#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>

/*** Typedefs ******************************************************************/

typedef const char functionname_t;

/*** DATA STRUCTURES ***********************************************************/

typedef struct StatusLogConfigContainer
{
  // contains the file path, if different from the default.
  char* status_log_file_path;

  // may hold 0, 1, 2, 3
  int verbosity_level;

  // the actual file has to go somewher.
  FILE* status_log_file_handle;
} StatusLogConfigContainer;

const int MAX_CONFIG_FILE_SIZE = 0xFFFF;
// AppConfigContainer
//
// this holds anything that get's set by a value read from the
// configuration file.
typedef struct AppConfigContainer
{
   // this is used as a buffer to store the contents of the configuration file.
   // It is assumed that the size of the configuration file will not
   // exceed 64 KiB
   char app_config_file_contents[MAX_CONFIG_FILE_SIZE];

  /****** Basic Settings ****************************************/

   int pomodoro_length_seconds;

   int pomodoro_short_break_length_seconds;

   int pomodoro_long_break_length_seconds;

   int pomodoro_set_length_seconds;

   int pomodoro_default_session_goal;


  /****** Behavior Settings *************************************/

   char continue_tracking_time_upon_completion;

   char auto_start_short_breaks;

   char auto_start_long_breaks;


  /****** Display Settings **************************************/

   char show_big_time;

   char show_small_time;

   char show_time_left;

   char show_command_prompt;

   char show_command_line;

   char show_instructions;

   char show_pomodoros_completed;

   char show_activity_log;


  /****** Shortcuts *********************************************/

   // these should all be set to the number associated with that control
   // number (c => 3, i => 9)
   char begin_pomodoro_shortcut;

   char begin_short_break_shortcut;

   char begin_long_break_shortcut;

   char enter_command_shortcut;

   char previous_char_shortcut;

   char next_char_shortcut;

   char view_help_document_shortcut;

   char interrupt_current_activity_shortcut;

   char exit_shortcut;

  /****** Sound Settings ****************************************/
   char alert_with_audio;
   char* alert_audio_file_path;

  /****** Font Settings *****************************************/

   // this will hold character arrays as bit fields.
   // not every slot will be occupied, but in theory, all of ascii could be
   // employed. this is so that it can avoid needing a translation function
   // that would need to be updated every time a new character gets added,
   // not to mention would potentially waste system resources.
  char* display_line_font[128];

} AppConfigContainer;

// AppStateContainer
//
// This holds anything that changes dynamically in the program

typedef struct AppStateContainer
{
   int current_time_on_clock;
   char command_line_contents[80];
   char display_line_contents[8][80];
} AppStateContainer;

/*** FUNCTION HEADERS **********************************************************/

/****** INITIALIZATION *********************************************************/

/********* InitStatusLog *******************************************************
 *
 * The Status log keeps track of what the app is doing. There are three
 * verbosity levels of logging:
 * 1. Standard: only main events are logged.
 * 2. Verbose: all events are logged.
 * 3. Debug: all events are logged and the entire program state is
 *    recorded as well.
 *
 * returns 0 on success, -1 if a problem occurred while opening the log file.
 */
int InitStatusLog(StatusLogConfigContainer* status_log_config_local,
                  char* file_path,
                  int verbosity_level);

/********* LoadBigFont ********************************************************
 *
 * This function takes the name of a font file as a c string and loads it
 * into the font array. See the user manual for how to edit the font to
 * your liking.
 *
 */
void LoadBigFont(char* file_name);

/********* LoadTermodoroConfigFile ********************************************
 *
 * This function takes a file name as a c string and attempts to load the
 * app_config global structure with the data.
 *
 * returns 0 on success, nonzero otherwise.
 */
int LoadTermodoroConfigFile(char* file_name);

/********* InitAppConfig ******************************************************
 *
 * This function looks for a .termodoro file for app configuration. If there
 * isn't one, it initializes the app_config global with the defaults given
 * in the user manual. (Section Default Configuration).
 *
 */
void InitAppConfig();

/****** LOGGING & ERROR HANDLING ***********************************************/

/********* WriteToLog **********************************************************
 *
 * Takes a pointer to a StatusLogConfigContainer and writes the the
 * function name and a message to the status log depending on the verbosity
 * level.
 */
int WriteToLog(const StatusLogConfigContainer* const status_log_config_local,
               functionname_t function_name[],
               char* message,
               int verbosoty_level_local);

/****** COMMAND INTERPRETATION *************************************************/

/********* InterpretCommand ****************************************************
 *
 * This function accepts a string to be interpreted as a command and
 * executes it.
 */
void InterpretCommand(char* command);


/*** GLOBAL VARIABLES **********************************************************/

/****** GENERAL SETTINGS *******************************************************/

const int  INPUT_QUEUE_SIZE = 10;

const int COMMAND_LINE_LENGTH = 80;

const int WINDOW_HEIGHT = 24;


/****** DISPLAY LINE ***********************************************************/

const int FONT_COLON_INDEX = 10;

const int FONT_DECPT_INDEX = 11;

const int DISPLAY_LINE_LENGTH = 80;


/****** COMMAND INTERPRETATION *************************************************/

// when setting one of these via command, these need to be in string form.

typedef const char commandstr_t;

commandstr_t ALERT_AUDIO_FILE_STRING[] = "alert_audio_file";

commandstr_t ALERT_WITH_AUDIO_STRING[] = "alert_with_audio_string";

commandstr_t AUTO_START_LONG_BREAKS_STRING[] = "auto_start_long_breaks";

commandstr_t AUTO_START_SHORT_BREAKS_STRING[] = "auto_start_short_breaks";

commandstr_t BEGIN_LONG_BREAK_SHORTCUT_STRING[] = "begin_long_break_shortcut";

commandstr_t BEGIN_POMODORO_SHORTCUT_STRING[] = "begin_pomodoro_shortcut";

commandstr_t BEGIN_SHORT_BREAK_SHORTCUT_STRING[] = "begin_short_break_shortcut";

commandstr_t CONTINUE_TRACKING_TIME_UPON_COMPLETION[] =
   "continue_tracking_time_upon_completion";

commandstr_t ENTER_COMMAND_SHORTCUT_STRING[] = "enter_command_shortcut";

commandstr_t EXIT_SHORTCUT_STRING[] = "exit_shortcut";

commandstr_t INTERRUPT_CURRENT_ACTIVITY_STRING[] =
   "interrupt_current_activity_shortcut";

commandstr_t NEXT_CHAR_SHORTCUT_STRING[] = "next_char_shortcut";

commandstr_t POMODORO_DEFAULT_SESSION_GOAL_STRING[] =
   "pomodoro_default_session_goal";

commandstr_t POMODORO_LENGTH_SECONDS_STRING[] = "pomodoro_length_seconds";

commandstr_t POMODORO_LONG_BREAK_LENGTH_SECONDS_STRING[] =
   "pomodoro_long_break_length_seconds";

commandstr_t POMODORO_SET_LENGTH_STRING[] = "pomodoro_set_length";

commandstr_t POMODORO_SHORT_BREAK_LENGTH_SECONDS_STRING[] =
   "pomodoro_short_break_length_seconds";

commandstr_t PREVIOUS_CHAR_SHORTCUT_STRING[] = "previous_char_shortcut";

commandstr_t SHOW_BIG_TIME_STRING[] = "show_big_time";

commandstr_t SHOW_SMALL_TIME_STRING[] = "show_small_time";

commandstr_t SHOW_TIME_LEFT_STRING[] = "show_time_left";

commandstr_t SHOW_TIME_LOGGED_SO_FAR_STRING[] = "show_time_logged_so_far";

commandstr_t SHOW_COMPLETED_POMODOROS_STRING[] = "show_completed_pomodoros";

commandstr_t VIEW_HELP_DOCUMENT_SHORTCUT_STRING[] = "view_help_document_shortcut";

commandstr_t* SORTED_COMMAND_STRINGS[] =
{
   ALERT_AUDIO_FILE_STRING,
   ALERT_WITH_AUDIO_STRING,
   AUTO_START_LONG_BREAKS_STRING,
   AUTO_START_SHORT_BREAKS_STRING,
   BEGIN_LONG_BREAK_SHORTCUT_STRING,
   BEGIN_POMODORO_SHORTCUT_STRING,
   BEGIN_SHORT_BREAK_SHORTCUT_STRING,
   CONTINUE_TRACKING_TIME_UPON_COMPLETION,
   ENTER_COMMAND_SHORTCUT_STRING,
   EXIT_SHORTCUT_STRING,
   INTERRUPT_CURRENT_ACTIVITY_STRING,
   NEXT_CHAR_SHORTCUT_STRING,
   POMODORO_DEFAULT_SESSION_GOAL_STRING,
   POMODORO_LENGTH_SECONDS_STRING,
   POMODORO_LONG_BREAK_LENGTH_SECONDS_STRING,
   POMODORO_SET_LENGTH_STRING,
   POMODORO_SHORT_BREAK_LENGTH_SECONDS_STRING,
   POMODORO_SET_LENGTH_STRING,
   POMODORO_SHORT_BREAK_LENGTH_SECONDS_STRING,
   PREVIOUS_CHAR_SHORTCUT_STRING,
   SHOW_BIG_TIME_STRING,
   SHOW_SMALL_TIME_STRING,
   SHOW_TIME_LEFT_STRING,
   SHOW_TIME_LOGGED_SO_FAR_STRING,
   VIEW_HELP_DOCUMENT_SHORTCUT_STRING
};


/****** LOGGING & ERROR HANDLING ***********************************************/

const int VERBOSITY_NORMAL = 1;

const int VERBOSITY_VERBOSE = 2;

const int VERBOSITY_DEBUG = 3;

const int VERBOSITY_DEBUG_ADVANCED = 4;

// this typedef is to make it more clear.

// here for completeness. Dosen't get used. It would constantly call itself.

/********* Function names ******************************************************/

// this typeduf should be used in function headers.


char LOG_FUNCTION_CALL[] = "LogFunctionCall";

char LOAD_TERMODORO_CONFIG_FILE[] = "LoadTermodoroConfigFile";

char LOAD_BIG_FONT[] = "LoadBigFont";

char INIT_APP_CONFIG[] = "InitAppConfig";

char INTERPRET_COMMAND[] = "InterpretCommand";





/********* Command Interpretation **********************************************/




/****** CONFIGURATION **********************************************************/

const char DEFAULT_CONFIG_FILE_PATH[] = "../config/.termodoro";

const char DEFAULT_STATUS_LOG_FILE_PATH[] = "../logs/termodoro_log.txt";

/****** STATE MANAGEMENT *******************************************************/

StatusLogConfigContainer* status_log_config;

AppConfigContainer* app_config;

AppStateContainer* app_state;


#endif
