#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include <sys/stat.h>

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

// string constants for configuration and command interpreting

const char POMODORO_LENGTH_SECONDS_STRING[] = "pomodoro_length_seconds";
const char POMODORO_SHORT_BREAK_LENGTH_SECONDS_STRING[] =
   "pomodoro_short_break_length_seconds";
const char POMODORO_LONG_BREAK_LENGTH_SECONDS_STRING[] =
   "pomodoro_long_break_length_seconds";
const char POMODORO_SET_LENGTH_STRING[] = "pomodoro_set_length";
const char POMODORO_DEFAULT_SESSION_GOAL_STRING[] =
   "pomodoro_default_session_goal";

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
const char INTERRUPT_CURRENT_ACTIVITY_STRING[] =
   "interrupt_current_activity_shortcut";

const char ALERT_WITH_AUDIO_STRING[] = "view_help_document_shortcut";
const char ALERT_AUDIO_FILE_STRING[] = "alert_audio_file";

/******************** DATA STRUCTURES *********************/

// AppConfigurationContainer
//
// this holds anything that get's set by a value read from the
// configuration file.
typedef struct AppConfigContainer
{
   // this is used as a buffer to store the contents of the configuration file.
   // It is assumed
   char app_config_file_contents[0xFFFF];
   /****************** Basic Settings **********************/
   int pomodoro_length_seconds;
   int pomodoro_short_break_length_seconds;
   int pomodoro_long_break_length_seconds;
   int pomodoro_set_length_seconds;
   int pomodoro_default_session_goal;

   /****************** Behavior Settings *******************/
   char continue_tracking_time_upon_completion;
   char auto_start_short_breaks;
   char auto_start_long_breaks;

   /****************** Display Settings ********************/
   char show_big_time;
   char show_small_time;
   char show_time_left;
   char show_command_prompt;
   char show_command_line;
   char show_instructions;
   char show_pomodoros_completed;
   char show_activity_log;

   /****************** Shortcuts ***************************/
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

   /****************** Sound Settings **********************/
   char alert_with_audio;
   char* alert_audio_file_path;

   /****************** Font Settings ***********************/

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

/*********************** GLOBALS **************************/

AppConfigContainer* app_config;
AppStateContainer* app_state;

/*********************** INITIALIZATION *******************/

/*** LoadBigFont ***
 *
 * this takes the name of a font file as a c string and loads it into the
 * font array. See the user manual for how to edit the font to your liking.
 *
 */
void LoadBigFont(char* file_name)
{
   return;
}

/*** LoadTermodoroConfigFile ***
 *
 * This function takes a file name as a c string and attempts to load the
 * app_config global structure with the data.
 *
 * returns 0 on success, nonzero otherwise.
 */
int LoadTermodoroConfigFile(char* file_name)
{
   // open the file
   FILE* config_file = fopen(file_name, "r");

   // load the file line by line into an array of lines.
   int check_size = fread(app_config->app_config_file_contents,
                          0xFFFF, 1, config_file);

   return 0;
}
/*** InitializeAppConfig ***
 *
 * This function looks for a .termodoro file for app configuration. If there
 * isn't one, it initializes the app_config global with the defaults given
 * in the user manual. (Section Default Configuration).
 *
 */
void InitalizeAppConfig()
{
   // attempt loading from
   return;
}

/*********************** COMMAND INTERPRETATION ***********/

void InterpretCommand(char* command)
{
   return;
}

int main(int argc, char** argv)
{
   // just make sure that I know how ot use scanf.

   app_config = malloc(sizeof(AppConfigContainer));

   FILE* file = fopen(".termodoro", "r");

   int numbytesread = fread(app_config->app_config_file_contents,
                            645,
                            1,
                            file);
   printf("%s: %d\n", "number of bytes read", numbytesread);

   printf("%s\n", app_config->app_config_file_contents);
   return 0;
}
