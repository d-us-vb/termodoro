#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#define CTRL_KEY(k) ((k) & 0x1f)
#define CRLF "\r\n"
#define ABUF_INIT {NULL, 0}

/* Time Font
   the time consists of 8 x 5 numbers composed of any character the user
   specifies. I'll be using bit fields of 5 for data efficiency.
*/

char time_font[12][5]; // 12 characters (0 - 9 + ':' and '.')

// a buffer that holds one 13 character string ready to be printed to the
// terminal line by line.
char display_line[8][80]; 

char instructions[] = "Termodoro: the macOS terminal pomodoro technique tool.\nShortcuts: \n\tC-p ..... begin pomodoro\n\tC-s ..... begin short break\n\tC-l ..... begin long break\n\tC-c ..... enter a command\n\tC-h ..... open help document with less";

char command_prompt[] = "enter a command >>> ";
char command_line[80];

struct terminalConfig
{
  int cursor_pos_x;
  int screenRows;
  int screenCols;
  struct termios orig_termios;    
};

struct terminalConfig global_state;

struct abuf {
  char *b;
  int len;
};

void die(const char* const s)
{
  perror(s);
  exit(1);
}

char terminalReadKey() 
{
  char c;

  if(read(STDIN_FILENO, &c, 1) == -1 && errno != EAGAIN)
  {
    perror("read");
  }
  return c;
}

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

}

/*
  I can't believe that except for syntax errors I got this right on the second revision.

*/


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

void printSingleDigit(int digit, char ink)
{
  char digit_matrix[8][6];
  for(int i = 0; i < 5; i++)
  {
    for(int j = 0; j < 8; j++)
    {
      //
      if((time_font[digit][i] >> j) & (0b00000001))
      {
        digit_matrix[j][i] = ink;
      }
      else
      {
        digit_matrix[j][i] = ' ';
      }
    }

  }
  // add null terminators so these are actual strings
  for(int i = 0; i < 6; i++)
  {
    digit_matrix[i][5] = '\0';
  }
  for(int i = 0; i < 8; i++)
  {
    printf("%s\n", digit_matrix[i]);
  }
  printf("\n\n");
}

// TODO write function that instead builds a character matrix that goes
// across the <80 character> screen.

void initDisplayLine()
{
  memset(display_line, '\0', sizeof(display_line));
}

// TODO TEST
void printDisplayLine()
{
  for(int i = 0; i < 8; i++)
  {
    printf("%s", display_line[i]);
  }
}

// since only a small subset of ascii is used, when writing a line of big
// numbers, we need some way to convert that string to an array of integers
// that are properly encoded.
// int array must already be allocated.

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
        int_array[pos] = 10;
        break;
      case '.':
        int_array[pos] = 11;
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
  // two options: draw line by line, or character by character.
  // basically, column by column or row by row.
  // since we're reading a string char by char, I think that
  // column by column is a better idea.

  // we can use a very similar algorithm to the one we used above.

  /* for(int i = 0; i < 5; i++) */
  /* { */
  /*     for(int j = 0; j < 8; j++) */
  /*     { */
  /*         // */
  /*         if((time_font[digit][i] >> j) & (0b00000001)) */
  /*         { */
  /*             digit_matrix[j][i] = ink; */
  /*         } */
  /*         else */
  /*         { */
  /*             digit_matrix[j][i] = ' '; */
  /*         } */
  /*     } */
  /* } */

  /* Unfortunately, this implies that we'll be using a three nested for
   * loop.
   */

  // this variable is used so that we can put an offset for which character
  // we're currently drawing.
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

// TODO write a thread function that handles all keyboard input asynchronously
// This would be like a producer

// TODO write a thread function that handles the actual timer.

// this way, the main thread can handle exiting when the keyboard

/*
 * timeToDisplayLine
 * take an integer number of seconds and convert it to a mm:ss format and write
 * that to the display line.
 */
void timeToDisplayLine(int seconds)
{
    char time[6];

    int ten_minutes = ((seconds / 60) % 60 - ((seconds / 60) % 10)) / 10;
    int unit_minutes = (seconds / 60) % 10;
    int ten_seconds = (seconds / 10) % 6;
    int unit_seconds = seconds % 10;

//    printf("%d %d : %d %d \r\n", ten_minutes, unit_minutes, ten_seconds, unit_seconds); // for debugging
    sprintf(time, "%d%d:%d%d",
            ten_minutes,
            unit_minutes,
            ten_seconds,
            unit_seconds);

    stringToDisplayLine(time, '$');

    write(1, "\x1b[5;1H", 6);

    for(int i = 0; i < 8; i++)
    {
        write(1, display_line[i], 80);
        write(1, "\r\n", 2);
    }
}
h


int main(int argc, char** argv) {

  initTimeFont();
  initDisplayLine();

  enableRawMode();

  int seconds;
  if(argc != 2) {
    printf("Usage ./simpleTimer mm:ss\n");
    exit(-1);
  } else if(strlen(argv[1]) != 5) {
    printf("usage ./simpleTimer mm:ss\n");
    exit(-1);
  } else {
    int minutes;
    sscanf(argv[1], "%d:%d", &minutes, &seconds);
    seconds += 60 * minutes;
  }

  write(1, "\x1b[5;1yH", 3);
  write(1, "\x1b[2K", 4);


  write(1, "\x1b[2J", 4);
  while(1)
  {
      timeToDisplayLine(seconds);

//    write(1, "\r\n", 2);
    seconds -= 1;
    if(seconds >= 0) sleep(1);
    else break;
  }

  //system("say \"short break\"");

}
