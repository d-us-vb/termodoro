#include <termodoro.h>


/****** INITIALIZATION *********************************************************/

int InitStatusLog(StatusLogConfigContainer* status_log_config_local,
                  char* file_name,
                  int verbosity_level)
{

   // Allocate memory for status log structure
   int ret;
   static int first_run = 1;
   if(first_run)
   {
      first_run = 0;
      status_log_config_local = malloc(sizeof(StatusLogConfigContainer));

      status_log_config_local->status_log_file_path = malloc(1024);

      if(file_name == NULL)
      {
         strcpy(status_log_config_local->status_log_file_path,
                DEFAULT_STATUS_LOG_FILE_PATH);
      }
      else
      {
         strcpy(status_log_config_local->status_log_file_path, file_name);
      }

      status_log_config_local->verbosity_level = verbosity_level;

      status_log_config_local->status_log_file_handle =
         fopen(status_log_config_local->status_log_file_path, "a+");

      if(status_log_config_local->status_log_file_handle != NULL)
      {
         char message_string[] =
            "InitStatusLog: Status Log successfully Initialized.\n";

         fwrite(message_string,
                strlen(message_string),
                1,
                status_log_config_local->status_log_file_handle);

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

int WriteToLog(const StatusLogConfigContainer* const status_log_config_local,
               functionname_t function_name[],
               char* message,
               int verbosity_level_local)
{
   // for debugging, this depends on the verbosity level.
   // for now, I'm just going to write what I need for my specific logging
   // and calls to this as needed, since it really isn't necessary.

   int err_ret;
   if(status_log_config_local->verbosity_level == verbosity_level_local &&
      verbosity_level_local == VERBOSITY_NORMAL)
   {

      err_ret = (int)fprintf(status_log_config_local->status_log_file_handle,
                            "%s: %s\n",
                            function_name,
                            message);
   }

   return err_ret;
}


void LoadBigFont(char* file_name)
{
   return;
}
int LoadTermodoroConfigFile(char* file_name)
{

   // open the file
   FILE* config_file = fopen(file_name, "r");

   // load the file into the config structure
   int check_size = fread(app_config->app_config_file_contents,
                          MAX_CONFIG_FILE_SIZE,
                          1,
                          config_file);

   // we'll use an incremental search mechanism.
//   while()


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

   int init_status_log_res = InitStatusLog(status_log_config, NULL, 0);
   if(init_status_log_res == 0)
   {
      printf("it worked");
   }

   if(WriteToLog())
// just make sure that I know how ot use scanf.

   app_config = malloc(sizeof(AppConfigContainer));

   /* FILE* file = fopen(".termodoro", "r"); */

   /* int numbytesread = fread(app_config->app_config_file_contentsbranv, */
   /*                          645, */
   /*                          1, */
   /*                          file); */
   /* printf("%s: %d\n", "number of bytes read", numbytesread); */

   /* printf("%s\n", app_config->app_config_file_contents); */
   return 0;
}
