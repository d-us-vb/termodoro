#include <termodoro.h>


/****** INITIALIZATION *********************************************************/

void InitStatusLog(char* file_name, int verbosity
{
  return;
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

/****** COMMAND INTERPRETATION *************************************************/

void InterpretCommand(char* command)
{
   return;
}

/****** MAIN *******************************************************************/

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
