#include <termodoro.h>


/****** INITIALIZATION *********************************************************/

int InitStatusLog(char* file_name, int verbosity_level)
{

   // Configuration for the status log cannot be done by loading something from
   // the user configuration file because the status log has to log what happens
   // while that file is being loaded.

   // Status log settings can be changed by the user during runtime, but if
   // they need to start up with something other than the default, it
   // requires an argument to be passed into it from the command line.

   // Defaults can be found in the user manual.

   // Allocate memory for status log structure
   int ret;
   static int first_run = 1;
   if(first_run)
   {
      first_run -= 1;
      status_log_config = malloc(sizeof(StatusLogConfigContainer));

      status_log_config->status_log_file_path = malloc(1024);

      if(file_name == NULL)
      {
         strcpy(status_log_config->status_log_file_path,
                DEFAULT_STATUS_LOG_FILE_PATH);
      }
      else
      {
         strcpy(status_log_config->status_log_file_path, file_name);
      }

      status_log_config->verbosity_level = verbosity_level;

      status_log_config->status_log_file_handle =
         fopen(status_log_config->status_log_file_path, "a+");

      if(status_log_config->status_log_file_handle != NULL)
      {
         char message_string[] =
            "InitStatusLog: Status Log successfully Initialized.\n";

         fwrite(message_string,
                strlen(message_string),
                1,
                status_log_config->status_log_file_handle);

         ret = 0;
      }
      else
      {
         // an error occurred. too bad.
         ret = -1;
      }
   }
   else
   {
      ret = -1;
   }
   return ret;
}

int LogFunctionCall(functionname_t function_name, char* message)
{
  
}


void LoadBigFont(char* file_name)
{
   return;
}

int LoadTermodoroConfigFile(char* file_name)
{
   // open the file
   FILE* config_file = fopen(file_name, "r");

   // load the file line by line into an array of lines.
   int check_size = fread(app_config->app_config_file_contents,
                          0xFFFF, 1, config_file);

   return 0;
}

void InitAppConfig()
{
   // attempt loading from
   return;
}

/****** STATUS LOG *************************************************************/




/****** COMMAND INTERPRETATION *************************************************/

void InterpretCommand(char* command)
{
   return;
}

/****** MAIN *******************************************************************/

int main(int argc, char** argv)
{

   int init_status_log_res = InitStatusLog(NULL, 0);
   if(init_status_log_res == 0)
   {
      printf("it worked");
   }

   init_status_log_res = InitStatusLog(NULL, 1);

   if(init_status_log_res != 0)
   {
      printf("something wrong happened with initializing the file");
   }
// just make sure that I know how ot use scanf.

   app_config = malloc(sizeof(AppConfigContainer));

   /* FILE* file = fopen(".termodoro", "r"); */

   /* int numbytesread = fread(app_config->app_config_file_contents, */
   /*                          645, */
   /*                          1, */
   /*                          file); */
   /* printf("%s: %d\n", "number of bytes read", numbytesread); */

   /* printf("%s\n", app_config->app_config_file_contents); */
   return 0;
}
