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

#define FONT_COLON_INDEX 10
#define FONT_DECPT_INDEX 11

#define INPUT_QUEUE_SIZE 10

#define DISPLAY_LINE_LENGTH 80
#define COMMAND_LINE_LENGTH 80


/****************************** ERROR ********************************/

FILE* errorLog;

/* Time Font
   the time consists of 8 x 5 numbers composed of any character the user
   specifies. I'll be using bit fields of 5 for data efficiency.
*/

char time_font[13][5]; // 12 characters (0 - 9 + ':' and '.')

// a buffer that holds one 13 character string ready to be printed to the
// terminal line by line.
char display_line[8][COMMAND_LINE_LENGTH];

char instructions[] = "Termodoro: the macOS terminal pomodoro technique tool.\r\nShortcuts: \n\r\tC-p ..... begin pomodoro\r\n\tC-s ..... begin short break\r\n\tC-l ..... begin long break\r\n\tC-c ..... enter a command\r\n\tC-h ..... open help document with less";

char command_prompt[] = "enter a command >>> ";
char command_line[COMMAND_LINE_LENGTH];

/***************** INPUT QUEUE ****************/

// a circular buffer of char
char input_queue[INPUT_QUEUE_SIZE];

// indexes for the queue
int input_queue_back;
int input_queue_front;


/* initQueue
   set queue variables to initial values. After running, inputQueueIsEmpty
   must return true.
 */
void initInputQueue()
{
  for(int i = 0; i < INPUT_QUEUE_SIZE; i++)
  {
    input_queue[i] = '\0';
  }
  input_queue_back = 0;
  input_queue_front = 0;
}

int inputQueueIsFull()
{
  return (input_queue_back - input_queue_front) == INPUT_QUEUE_SIZE;
}

int inputQueueIsEmpty()
{
  return input_queue_back == input_queue_front;
}

int inputEnqueue(char new_value)
{
  if(inputQueueIsFull())
  {
    return -1;
  }
  else
  {
    input_queue[input_queue_back % INPUT_QUEUE_SIZE] = new_value;
    input_queue_back += 1;
    return input_queue_back % INPUT_QUEUE_SIZE;
  }
}

int inputDequeue(char* output_val)
{
  if(inputQueueIsEmpty())
  {
    return -1;
  }
  else
  {
    *output_val = input_queue[input_queue_front % INPUT_QUEUE_SIZE];
    input_queue_front += 1;
    return (input_queue_front - 1) % INPUT_QUEUE_SIZE;
  }
}

/* printInputQueue
   print the current state of the input queue to the terminal
 */
void printInputQueue()
{
  printf("[");
  for(int i = 0; i < INPUT_QUEUE_SIZE - 1; i++) {
    if(input_queue[i] == 0)
    {
      printf("0, ");
      continue;
    }
    printf("%c, ", input_queue[i]);
  }
  if(input_queue[INPUT_QUEUE_SIZE - 1] == 0)
  {
    printf("0]\r\n");
    printf("\tfront: %d\r\n\tback: %d\r\n", input_queue_front, input_queue_back);
    printf("\tisFull: %d\r\n", inputQueueIsFull());
    printf("\tisEmpty: %d\r\n", inputQueueIsEmpty());
    return;
  }
  printf("%c]\r\n", input_queue[INPUT_QUEUE_SIZE -1]);
  printf("\tfront: %d\r\n\tback: %d\r\n", input_queue_front, input_queue_back);
  printf("\tisFull: %d\r\n", inputQueueIsFull());
  printf("\tisEmpty: %d\r\n", inputQueueIsEmpty());
}

/****************** THREADING *****************/

pthread_t keyboard_listener_thread;
pthread_t command_processor_thread;
pthread_t terminal_output_controller_thread;

pthread_mutex_t input_queue_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t display_line_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t command_line_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t stdout_mutex = PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t input_queue_full_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t input_queue_empty_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t terminal_output_controller_done = PTHREAD_COND_INITIALIZER;

pthread_cond_t redraw_screen_cond = PTHREAD_COND_INITIALIZER;


struct terminalConfig
{
  // this is used for setting the position the cursor has on the command line.
  int cursor_pos_x;


  int screenRows;
  int screenCols;

  // line number on which the command line appears
  int commandLineNumber;

  int commandPromptShouldBeVisible;
  int commandLineShouldBeVisible;
  int displayLineShouldBeVisible;

  // linne number on which the first line of the display line appears
  int displayLineNumber;

  // shortcut key numbers e.g. command entry default is C-c => 3.
  char forceKill;
  char commandEntry;
  char commandCancel;

  struct termios orig_termios;
};

struct terminalConfig global_state;

void termodoroInit()
{
  // TODO Add code to read an init file and if one isn't present, create one

  global_state.cursor_pos_x = 0;
  global_state.forceKill = CTRL_KEY('D');

  // default behavior is to toggle the command line using the C-c.
  global_state.commandEntry = CTRL_KEY('C');
  global_state.commandCancel = CTRL_KEY('C');

  memset(command_line, '\0', sizeof(command_line));

  // prepare the display
  write(1, "\x1B[2J", 4);
  write(1, "\x1B[0;0H", 6);
  write(1, instructions, sizeof(instructions));
  write(1, "\x1B[8;0H", 6);
  global_state.commandLineNumber = 8;
  global_state.displayLineNumber = 9;

  global_state.commandLineShouldBeVisible = 0;
  global_state.commandPromptShouldBeVisible = 0;
  global_state.displayLineShouldBeVisible = 0;
}

void logProgramState(char* label)
{
  fprintf(errorLog,
          "program state report: %s\n"
          "\tcursor_pos_x = %d\n"
          "\tforceKill = %d\n"
          "\tcommandEntry = %d\n"
          "\tcommandCancel = %d\n"
          "\tcommandLineNumber = %d\n"
          "\tdisplayLineNumber = %d\n"
          "\tcommandLine = %s\n"
          "\tcommandLineShouldBeVisible = %d\n"
          "\tcommandPromptShouldBeVisible = %d\n"
          "\tdisplayLineShouldBeVisible = %d\n",
          label,
          global_state.cursor_pos_x,
          global_state.forceKill,
          global_state.commandEntry,
          global_state.commandCancel,
          global_state.commandLineNumber,
          global_state.displayLineNumber,
          command_line,
          global_state.commandLineShouldBeVisible,
          global_state.commandPromptShouldBeVisible,
          global_state.displayLineShouldBeVisible);
  fflush(errorLog);
}

struct abuf {
  char *b;
  int len;
};

void die(const char* const s)
{
  perror(s);
  exit(1);
}

// this function might not be necessory.
void terminalMoveCursor(char key)
{
  switch(key) 
  {
  case CTRL_KEY('b'):
    global_state.cursor_pos_x -= 1;
    break;
  case CTRL_KEY('f'):
    global_state.cursor_pos_x += 1;
    break;
  }
}

void initTimeFont()
{
  // 0
  time_font[0][0] = 0b01111110;
  time_font[0][1] = 0b11000011;
  time_font[0][2] = 0b10011001;
  time_font[0][3] = 0b11000011;
  time_font[0][4] = 0b01111110;

  // 1
  time_font[1][0] = 0b11000110;
  time_font[1][1] = 0b11111111;
  time_font[1][2] = 0b11111111;
  time_font[1][3] = 0b11000000;
  time_font[1][4] = 0b00000000;

  // 2
  time_font[2][0] = 0b11000010;
  time_font[2][1] = 0b11100011;
  time_font[2][2] = 0b10110001;
  time_font[2][3] = 0b10011011;
  time_font[2][4] = 0b10001110;

  // 3
  time_font[3][0] = 0b01000010;
  time_font[3][1] = 0b10000001;
  time_font[3][2] = 0b10001001;
  time_font[3][3] = 0b11011011;
  time_font[3][4] = 0b01110110;

  // 4
  time_font[4][0] = 0b00111000;
  time_font[4][1] = 0b00100100;
  time_font[4][2] = 0b00100010;
  time_font[4][3] = 0b11111111;
  time_font[4][4] = 0b00100000;

  // 5
  time_font[5][0] = 0b01011111;
  time_font[5][1] = 0b11001001;
  time_font[5][2] = 0b10001001;
  time_font[5][3] = 0b11011001;
  time_font[5][4] = 0b01110011;

  // 6
  time_font[6][0] = 0b01111110;
  time_font[6][1] = 0b11010011;
  time_font[6][2] = 0b10001001;
  time_font[6][3] = 0b11011011;
  time_font[6][4] = 0b01110010;

  // 7
  time_font[7][0] = 0b11000011;
  time_font[7][1] = 0b01110001; 
  time_font[7][2] = 0b00011001;
  time_font[7][3] = 0b00001111;
  time_font[7][4] = 0b00000011;

  // 8
  time_font[8][0] = 0b01110110;
  time_font[8][1] = 0b11011011;
  time_font[8][2] = 0b10001001;
  time_font[8][3] = 0b11011011;
  time_font[8][4] = 0b01110110;

  // 9
  time_font[9][0] = 0b01001110;
  time_font[9][1] = 0b11011011;
  time_font[9][2] = 0b10010001;
  time_font[9][3] = 0b11011011;
  time_font[9][4] = 0b01111110;

  // :
  time_font[10][0] = 0b00000000;
  time_font[10][1] = 0b01100110;
  time_font[10][2] = 0b01100110;
  time_font[10][3] = 0;
  time_font[10][4] = 0;

  // .
  time_font[11][0] = 0b00000000;
  time_font[11][1] = 0b11000000;
  time_font[11][2] = 0b11000000;
  time_font[11][3] = 0b00000000;
  time_font[11][4] = 0b00000000;

  // char not in font

  time_font[12][0] = 0b00000000;
  time_font[12][1] = 0b01111110;
  time_font[12][2] = 0b01000010;
  time_font[12][3] = 0b01111110;
  time_font[12][4] = 0b00000000;

}

void disableRawMode()
{
  write(STDOUT_FILENO, "\x1b[2J", 4);
  write(STDOUT_FILENO, "\x1b[H", 3);
  if(tcsetattr(STDIN_FILENO, TCSAFLUSH, &(global_state.orig_termios)) == -1)
  {
    perror("tcsetattr");
  }
}

void enableRawMode()
{
  if(tcgetattr(STDIN_FILENO, &(global_state.orig_termios)) == -1)
  {
    perror("tcsetattr");
  }
  atexit(disableRawMode);

  struct termios raw = global_state.orig_termios;
  raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
  raw.c_oflag &= ~(OPOST);
  raw.c_cflag |= (CS8);
  raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);

  tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

void initDisplayLine()
{
  memset(display_line, '\0', sizeof(display_line));
}

/* stringToFontArray
   since only a small subset of ascii is used, when writing a line of big
   numbers, we need some way to convert that string to an array of integers
   that are properly encoded.
   int array must already be allocated.

   conv_string : the string to be converted
   int_array   : output paramater where the string of integers will be put.
*/
// TESTED 11-20-2020
void stringToFontArray(char* const conv_string, int* int_array)
{
  int conv_string_length = strlen(conv_string);
  for(int pos = 0; pos < conv_string_length; pos ++)
  {
    // numbers are easy. just subtract 48.
    if(conv_string[pos] >= '0' && conv_string[pos] <= '9')
    {
      int_array[pos] = conv_string[pos] - 48;
    }
    else
    {
      // otherwise
      // TODO make these constants. 10 and 11 are magic numbers here.
      switch(conv_string[pos])
      {
      case ':':
        int_array[pos] = FONT_COLON_INDEX;
        break;
      case '.':
        int_array[pos] = FONT_DECPT_INDEX;
        break;
      default:
        int_array[pos] = 12; // I'll add a "char not in character set"
        // character later.
      }
    }
  }
}

/*
 * Print a maximum of 13 characters to the display line
 * return -1 on error (string is too long)
 */
int stringToDisplayLine(char* const display_string, char ink)
{
  int display_string_length = strlen(display_string);
  if(display_string_length > 13)
  {
    return -1;
  }

  initDisplayLine();

  int past_char_column_offset = 0;

  int int_string[13];
  stringToFontArray(display_string, int_string);

  for(int current_char = 0;
      current_char < display_string_length;
      current_char++)
  {
    for(int current_column = 0; current_column < 5; current_column++)
    {
      for(int current_row_char = 0; current_row_char < 8; current_row_char++)
      {
        if((time_font[int_string[current_char]][current_column] >> current_row_char) &
           0b00000001)
        {
          display_line[current_row_char]
            [past_char_column_offset + current_column] = ink;
        }
        else
        {
          display_line[current_row_char]
            [past_char_column_offset + current_column] = ' ';
        }
      }
    }
    // insert spaces
    for(int row_char = 0; row_char < 8; row_char++)
    {
      display_line[row_char][past_char_column_offset + 5] = ' ';
    }

    past_char_column_offset += 6;
  }
  return 0;
}

// TODO write a thread function that handles all screen output

// this was actually a bad idea. We'll make this a normal function
void* terminalOutputController(void* arg)
{
  /* this takes all of the elements that need to be drawn to the screen and
     figures out how to do that.
     elements:

     instructions - a static buffer. this won't be drawn each time. It'll be left
     at the top of the screen.

     command_line - this is positioned at the line specified in global_state and
     the length of the command prompt. command

     whether not the command prompt is seen is determined by a variable in
     terminalConfig
     global_state.commandPromptShouldBeVisible

     displayLine - the display line contains the actual time remaining on the clock.
  */

  logProgramState("\nterminalOutputController pre execution");
  char term_command[8];

  if(global_state.displayLineShouldBeVisible)
  {
    sprintf(term_command, "\x1B[%d;%dH",
            global_state.displayLineNumber,
            0);
    write(1, term_command, strlen(term_command));
    for(int i = 0; i < 8; i++)
    {
      write(1, display_line[i], 80);
      write(1, "\r\n", 2);
    }
  }
  else
  {
    sprintf(term_command, "\x1B[%d;%dH",
            global_state.displayLineNumber,
            0);
    write(1, term_command, strlen(term_command));
    for(int i = 0; i < 8; i++)
    {
      write(1, "\x1B[2K", 4);
      write(1, "\n\r", 2);
    }
  }

  if(global_state.commandPromptShouldBeVisible)
  {
    sprintf(term_command, "\x1B[%d;1H", global_state.commandLineNumber);
    write(1, term_command, strlen(term_command));
    write(1, command_prompt, strlen(command_prompt));
  }
  else
  {
    // hide the command prompt
    char* cp_mask = (char*) malloc(strlen(command_prompt));
    memset(cp_mask, ' ', strlen(command_prompt));
    sprintf(term_command, "\x1B[%d;1H", global_state.commandLineNumber);
    write(1, term_command, strlen(term_command));
    write(1, cp_mask, strlen(command_prompt));
    free(cp_mask);
  }

  if(global_state.commandLineShouldBeVisible)
  {
    if(global_state.commandPromptShouldBeVisible)
    {
      sprintf(term_command, "\x1B[%d;%dH",
              global_state.commandLineNumber,
              (int)strlen(command_prompt));

      write(1, term_command, strlen(term_command));
    }
    else
    {
      sprintf(term_command, "\x1B[%d;%dH",
              global_state.commandLineNumber,
              1);
    }
    // we assume that the command line is a null term string
    write(1, command_line, strlen(command_line));
  }
  else
  {
    write(1, "\x1B[2K", 4);
    if(global_state.commandPromptShouldBeVisible)
    {
      sprintf(term_command, "\x1B[%d;%dH",
              global_state.commandLineNumber,
              0);

      write(1, term_command, strlen(term_command));
      write(1, command_prompt, strlen(command_prompt));
    }

    char* cl_mask = (char*) malloc(strlen(command_line));
    memset(cl_mask, ' ', strlen(command_line));

    sprintf(term_command, "\x1B[%d;%dH",
            global_state.commandLineNumber,
            (int)strlen(command_prompt));

    write(1, term_command, strlen(term_command));
    write(1, "\x1B[H", 3);
  }
  return NULL;
}

/* captureLine
 * given a char buffer and starting index, append or insert the current input
 * into the char buffer until a cancel or return is received.
 */
void captureLine(char* line, int index, int should_update_screen, int line_max)
{
  // we basically have the same kind of loop that's in processInput, but
  // this function encapsulates interaction with a given buffer
  while(1)
  {
    int debug_line = 10;
    // first get appending correct
    //
    pthread_mutex_lock(&input_queue_mutex);
    while(inputQueueIsEmpty())
    {
      pthread_cond_wait(&input_queue_empty_cond, &input_queue_mutex);
    }
    char c;
    inputDequeue(&c);
    pthread_mutex_unlock(&input_queue_mutex);

    if(c == global_state.commandEntry)
    {
      // cancel the command by placing a null pointer in line
      line = NULL;
      return;
    }
    else if(c == '\r')
    {
      // line is ready to be submitted.
      write(1, "\r\n", 2);
      return;
    }
    else if(c == 127 && index > 0)
    {
      // move write index back
      index -= 1;
      // erase character
      line[index] = '\0';
      // check if screen update is turned on
      if(should_update_screen)
      {
        /* pthread_mutex_lock(&command_line_mutex); */
        // kill the command line
        global_state.commandLineShouldBeVisible = 0;
        // redraw the screen
        terminalOutputController(NULL);
        /* pthread_cond_signal(&redraw_screen_cond); */
        /* pthread_cond_wait(&terminal_output_controller_done, &command_line_mutex); */
        // turn command line back on
        global_state.commandLineShouldBeVisible = 1;
        // redraw screen
        terminalOutputController(NULL);
        /* pthread_cond_signal(&redraw_screen_cond); */
        /* pthread_mutex_unlock(&command_line_mutex); */
      }
    }
    else if(index < line_max){
      line[index] = c;
      line[index + 1] = '\0';
      index += 1;
    }
    if(should_update_screen)
    {
      /* pthread_cond_signal(&redraw_screen_cond); */
      terminalOutputController(NULL);
    }
  }
}

void executeCommand(char* command_buffer, int command_buffer_size)
{
  logProgramState("executeCommand pre-execution");
  if(strcmp(command_buffer, "setDisplayLine") == 0)
  {
    // clear command line
    global_state.commandLineShouldBeVisible = 0;
    terminalOutputController(NULL);
    memset(command_buffer, '\0', command_buffer_size);

    // capture input
    global_state.commandLineShouldBeVisible = 1;
    global_state.commandPromptShouldBeVisible = 0;
    terminalOutputController(NULL);
    captureLine(command_buffer, 0, 1, command_buffer_size);

    fprintf(errorLog, "executeCommand:setDisplayLine 2\n\tstrlen(command_buffer) = %d,\n\tcommand_buffer = %s\n", (int)strlen(command_buffer), command_buffer);
    logProgramState("executeCommand > if(strcmp(command_buffer, \"setDisplayLine\") == 0);");
    // TODO change this magic number to a macro
    if(strlen(command_buffer) > 13)
    {
      fprintf(errorLog, "executeCommand: setDisplayLine 3\n\tstrlen(command_buffer) = %d,\n\tcommand_buffer = %s\n", (int)strlen(command_buffer), command_buffer);
      logProgramState("executeCommand > if(strcmp(command_buffer, \"setDisplayLine\") == 0) > if(strlen(command_buffer) > 13) <1>");

      global_state.commandPromptShouldBeVisible = 0;
      global_state.commandLineShouldBeVisible = 1;
      char message[] = "That string was too long";
      strcpy(command_buffer, message);
      terminalOutputController(NULL);

      fprintf(errorLog, "executeCommand: setDisplayLine 4\n\tstrlen(command_buffer) = %d,\n\tcommand_buffer = %s\n", (int)strlen(command_buffer), command_buffer);
      logProgramState("executeCommand > if(strcmp(command_buffer, \"setDisplayLine\") == 0) > if(strlen(command_buffer) > 13) <2>");
    }
    else
    {
      stringToDisplayLine(command_line, '$');
    }
  }
  else if(strcmp(command_buffer, "showDisplayLine") == 0)
  {
    logProgramState("executeCommand: else if(strcmp(command_buffer, \"showDisplayLine\") == 0)");
    terminalOutputController(NULL);

  }
  else
  {
    global_state.commandLineShouldBeVisible = 1;

    fprintf(errorLog, "executeCommand: Command Not Recognized\n");
    memset(command_line, '\0', sizeof(command_line));
    strcpy(command_line, "ERROR: Command Not Recognized");
    terminalOutputController(NULL);
  }
}

/************************* THREAD FUNCTIONS ***********************/

/* Emergency shutdown: this thread is started when keyboard input is about
   to be blocked (because of a full input queue, most likely). In case a bug
   is causing the input pipeline to block, this thread will be able to terminate
   the program and save all data that was being worked on
*/
void* emergencyShutdown(void* arg)
{
  while(1)
  {
    char c;
    if(read(STDIN_FILENO, &c, 1) == -1 && errno!= EAGAIN)
    {
      perror("emergencyShutdown");
      exit(1);
    }

    if(c == 4)
    {
      disableRawMode();
      printf("You are seeing this because you shut down termodoro in an emergency.\n");
      exit(1);
    }
  }
}

void* keyboardListener(void* arg)
{
  char c;

  // this is an event loop. This thread blocks while it waits for input.
  // we can't allow this since we need another thread to act as a timer,
  // if we also want to issue commands asynchronously while the timer is still
  // running.
  while(1) {
    if(read(STDIN_FILENO, &c, 1) == -1 && errno != EAGAIN)
    {
    }

    if(c == global_state.forceKill)
    {
      disableRawMode();
      exit(0);
    }

    pthread_mutex_lock(&input_queue_mutex);
    if(inputEnqueue(c) == -1)
    {
      pthread_t emergencyThread;
      pthread_create(&emergencyThread, NULL, emergencyShutdown, NULL);
      pthread_cond_wait(&input_queue_full_cond, &input_queue_mutex);
      inputEnqueue(c);
    }
    pthread_cond_signal(&input_queue_empty_cond);
    pthread_mutex_unlock(&input_queue_mutex);
  }
  return NULL;
}

// this doesn't necessarily need to be a separate thread, but I think it makes
// some sense to do it this way. 
void* processInput(void* arg)
{
  int processing_command = 0;
  while(1)
  {
    pthread_mutex_lock(&input_queue_mutex);
    while(inputQueueIsEmpty())
    {
      pthread_cond_wait(&input_queue_empty_cond, &input_queue_mutex);
    }

    char c;
    inputDequeue(&c);
    pthread_mutex_unlock(&input_queue_mutex);

    // handle a command
    if(c == global_state.commandEntry)
    {

      global_state.commandPromptShouldBeVisible = 1;
      global_state.commandLineShouldBeVisible = 1;
      /* pthread_cond_signal(&redraw_screen_cond); */

      terminalOutputController(NULL);


      captureLine(command_line, 0, 1, COMMAND_LINE_LENGTH);

      executeCommand(command_line, COMMAND_LINE_LENGTH);

      global_state.commandPromptShouldBeVisible = 0;
//      global_state.commandLineShouldBeVisible = 0;
      /* pthread_cond_signal(&redraw_screen_cond); */
      terminalOutputController(NULL);

      memset(command_line, '\0', sizeof(command_line));
    }
  }
  return NULL;
}


// TODO write a thread function that handles the actual timer.

void* timerManager(void* arg)
{
  return NULL;
}

/* starting at the current cursor location, write the display line to the screen. */
int timeToDisplayLine(int seconds)
{
    char time[6];

    if(seconds > 5999)
    {
      // we shouldn't allow more seconds to be converted to this format than
      // will fit. 99:59 = 5999 seconds. sprintf will attempt to write 7
      // characters which would not be correct, and might cause a segfault.
      return -1;
    }
    // extract from seconds each position in the number.
    int ten_minutes = ((seconds / 60) % 60 - ((seconds / 60) % 10)) / 10;
    int unit_minutes = (seconds / 60) % 10;
    int ten_seconds = (seconds / 10) % 6;
    int unit_seconds = seconds % 10;

    sprintf(time, "%d%d:%d%d",
            ten_minutes,
            unit_minutes,
            ten_seconds,
            unit_seconds);

    stringToDisplayLine(time, '$');

    for(int i = 0; i < 8; i++)
    {
        write(1, display_line[i], 80);
        write(1, "\r\n", 2);
    }
    return seconds;
}


/************************* UNIT TESTS *************************/

#ifdef UNIT_TESTS

void UnitTests()
{
  initInputQueue();
  printInputQueue();

  inputEnqueue('-');
  printInputQueue();
  inputEnqueue('1');
  printInputQueue();
  inputEnqueue('2');
  printInputQueue();
  inputEnqueue('3');
  printInputQueue();
  inputEnqueue('4');
  printInputQueue();
  inputEnqueue('5');
  printInputQueue();
  inputEnqueue('6');
  printInputQueue();
  inputEnqueue('7');
  printInputQueue();
  inputEnqueue('8');
  printInputQueue();
  inputEnqueue('9');
  printInputQueue();

  char output;
  
  inputDequeue(&output);
  printInputQueue();
  printf("\toutput: %c\n", output);

  inputDequeue(&output);
  printInputQueue();
  printf("\toutput: %c\n", output);

  inputDequeue(&output);
  printInputQueue();
  printf("\toutput: %c\n", output);

  inputDequeue(&output);
  printInputQueue();
  printf("\toutput: %c\n", output);

  inputDequeue(&output);
  printInputQueue();
  printf("\toutput: %c\n", output);

  inputDequeue(&output);
  printInputQueue();
  printf("\toutput: %c\n", output);

  inputDequeue(&output);
  printInputQueue();
  printf("\toutput: %c\n", output);

  inputDequeue(&output);
  printInputQueue();
  printf("\toutput: %c\n", output);

  inputDequeue(&output);
  printInputQueue();
  printf("\toutput: %c\n", output);

  inputDequeue(&output);
  printInputQueue();
  printf("\toutput: %c\n", output);


  inputEnqueue('A');
  printInputQueue();
  inputEnqueue('B');
  printInputQueue();
  inputEnqueue('C');
  printInputQueue();

  inputDequeue(&output);
  printInputQueue();
  printf("\toutput: %c\n", output);

  inputDequeue(&output);
  printInputQueue();
  printf("\toutput: %c\n", output);

  inputDequeue(&output);
  printInputQueue();
  printf("\toutput: %c\n", output);
}

#endif

int main(int argc, char** argv) {

  errorLog = fopen("termodoroerror.txt", "a");
  fprintf(errorLog, "%s", "=====================================================\n");
  #ifdef UNIT_TESTS

  UnitTests();

  #endif
  #ifndef UNIT_TESTS
  termodoroInit();
  initTimeFont();
  initDisplayLine();

  enableRawMode();

  pthread_create(&keyboard_listener_thread, NULL, keyboardListener, NULL);
  pthread_create(&command_processor_thread, NULL, processInput, NULL);
  /* pthread_create(&terminal_output_controller_thread, NULL, */
  /*                terminalOutputController, NULL); */


  while(1) sleep(1);
/*   int seconds; */
/*   if(argc != 2) { */
/*     printf("Usage ./simpleTimer mm:ss\n"); */
/*     exit(-1); */
/*   } else if(strlen(argv[1]) != 5) { */
/*     printf("usage ./simpleTimer mm:ss\n"); */
/*     exit(-1); */
/*   } else { */
/*     int minutes; */
/*     sscanf(argv[1], "%d:%d", &minutes, &seconds); */
/*     seconds += 60 * minutes; */
/*   } */

/*   write(1, "\x1b[5;1yH", 3); */
/*   write(1, "\x1b[2K", 4); */


/*   write(1, "\x1b[2J", 4); */
/*   while(1) */
/*   { */
/*       timeToDisplayLine(seconds); */

/* //    write(1, "\r\n", 2); */
/*     seconds -= 1; */
/*     if(seconds >= 0) sleep(1); */
/*     else break; */
/*   } */

  //system("say \"short break\"");

  #endif
}
