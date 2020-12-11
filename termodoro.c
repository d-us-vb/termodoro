
/*
 * I dislike that I feel the need to write this in C. But I don't think that D or Rust have what I want. I want portability to ARM, and I think that right now, only C has that.
 *
 * Termodoro: a VT100 based pomodoro timer app that works mostly on macOS. Eventual port to linux. It's also one component of the TimeTracker app that I intend to write.
 */

/* episode 4
   "Up here at the top of the file is where we usually put lines of code called 'includes'. These lines are technically called /preprocessor directives/. I guess I should explain to you how C programs get run. C is readable by humans, but computers can't really understand it. We have another program called a compiler that translates the C code into machine code, otherwise known as a /compiled binary/ or /executable/. But before the compiler gets at it, there's a sort of side program that adds things to the code as a labor saving device. In this case, the #include means 'find this file and paste its contents right here.'. This means we don't have to write every single function in a program in the same file."
   "Oh, that's neat."
   "Yeah, it makes the code much cleaner and easier to read in the long run. The preprocessor can do a lot lot lot of stuff. #include is just one of several dozen directives, and some directives do more than just one thing."
 */

#include <stdio.h>

/* episode 5
 * "Why do we have to put angle brackets around the file name. And where is that file anyway?"
 * "Yeah, that's a confusing thing too. The angle brackets mean that the file is /guarded/, which is a fancy term that just means that the compiler is told where to find it outside of the source file. We can #include our own files as long as we give it a relative path from the directory where the source file lives. We'll do that later."
 * "Oh okay."
 */

/*
 * this program is an experiment. Donald Knuth talked about literate programming. Well, this is another step even stranger: novel programming. Iam going to write a short story in comments. As such, feel free to read this file much like a novel. It's about two high schoolers. One is the captain of the competitive programming team, and the other is someone who just joined. He's teaching him how to program.
 *
 * Each comment block is an episode. Episodes should be read in order to understand the design of the program. Occasionally episodes will be found in a separate file, but most of them will be here. Use the search feature of your text editor to find the next episode.
 */

/* Episode 1
 * "Alright, are you ready," Malakai said, looking somewhat excited at the prospect of teach someone how to program.
 * "I guess as ready as I'll ever be," Justin responded.
 * "Alright. I'll be teaching you a language called C. It's usually thought of as being sort of difficult for newcomers to programming, but really, you get to learn a lot of stuff about how computers work because it was designed back when being a programmer meant you were practically an electrical engineer. It was just a way of writing programs faster by automating annoying tasks like writing in the computers native machine code."
 * Justin considered this carefully. What was he talking about?
 * Malakai looked worried suddenly. "Oh, I guess I'm getting ahead of myself. We'll start at the beginning." He began typing.
 * "The first thing we need is what we call an 'entry point'. It's where the program begins. In C, we do this with a special function called 'main'."
 */

int main(int argc, char* argv[])
{
    /* Episode 2
     * Justin looked somewhat confused. "Is that like a function in math?"
     * Malakai beamed at him. "Sort of. In the really old days, they called them subroutines, but function got the point across better, I guess. The idea is that you give it some inputs, and then it produces an output."
     * "That sounds exactly like a function in math," Justin said, somewhat annoyed.
     * "Yeah, that's the confusing part. The problem is that functions in math don't usually affect other functions. You can write down a function, but unless you wave your hands and say 'by some mathemiracle this function depends on this other function' they'll be by themselves. You see, in math, a function has to pass the vertical line test, and every unique input always results in the same output. But in programming, that doesn't have to be true. Sometimes other parts of the program will make the same function produce different results depending on when you run it."
     * "I see. So what do those words mean?" Justin said, pointing at the main function.
     * "The first one is the type. int stands for integer, which means that this function can only produce an integer. It can't create a decimal number or a word, or some other crazy thing. This makes it easier to find bugs in our code when we force a function to only produce one type of thing.
     * "The next word is the name of the function. We usually try to make them as descriptive as possible. Then we have a left parenthesis which begins a list of paramaters, which are like the inputs to the function. In math this is usually x, and we usually only deal with one parameter, but in programming, we usually have two or even three paramaters."
     * "Cool! That probably lets them do more stuff."
     * "You got it!"
     * "And then I see int again. Does that mean that that parameter can only be an integer?"
     * "You catch on fast. That's exactly right. The next one is a char which is short for character. When you read it, some people say 'care', and some people say 'car', but most people just say 'char' like when you burn something. That's how I always say it. Now this is where things get a little confusing. That star symbol means that the variable is a pointer." Malakai started looking nervous.
     * "Pointer?"
     * "Yeah, basically it's a variable that points to something in the computer's memory. You see, in a computer there are a few components that we get to deal with: the most important one is the CPU, or central processing unit that processes all of the information that we give it by following a program. But the second most important part is the memory. It's where the program is stored and it's also like its scratch paper and source of information to process. Memory is sort of like a library. There are millions of books, but you need want to read one of them at a time. But your friend who wants you to read a certain book gave you the catalog number that tells you where to find it on the shelves. Instead of having to search through all the books to find the book, you just use the catalog number to point you to where to look."
     * "Oh, that's ... cool. Why do we need to have a separate type for them, though?"
     * "That's the confusing part. It's so confusing that a lot of people get discouraged because it doesn't click right away, so we'll put that off until we actually need it desperately. The next is the name of the variable parameter, and the square brackets means that the variable is an array."
     * Justin's expression turned into another shade of confusion. "What's an array?"
     * "I'm glad you asked. An array is actually like a list. Instead of just one value, it has a bunch of values all at once, and you access them by putting a number between square brackets. We won't be using those just yet."
     */


    /* episode 3
     * "Alright. The first thing we'll do is print a message to the terminal that tells the user what the program is and how to use it."
     * "Cool. How do we do that," justin inquired.
     * "We use another function."
     * "Like main?"
     * "Exactly. But this function is found in what's called a library. We have to include some way for the program to find where it's written."
     */

    /* episode 6
       "I should probably explain what stdio.h is. std is a very common programming abbreviation that means 'standard'. io is another very common abbreviation that means input-output. I usually say ee-oh, but others say it like an initialism. It's a library that contains functions for dealing with inputting data to the program and outputting data from the program. In this case, we want to write back to the terminal. The function that does that is called printf. It's usually pronounced 'print-eff'".
       "So if it's a function, does it take paramaters as well?"
       "It sure does. printf is a slightly unusual function, though, because it can take different numbers of arguments. I won't go into detail about that at the moment though. In this case, we're only going to give it one argument: a string."
     */

    printf("Termodoro 0.1 \nTermodoro is a command line tool that helps you focus on\na single task at a time. To get started with the default settings,\npress\n\t- 'p' then enter to begin a pomodoro session.\n\t- 's' then enter to begin a short break.\n\t- 'l' then enter to begin a long break.\n\nTo list the current default settings, enter 'defaults'.\n\nTo change defaults, edit the .termodoro file in your home folder.\n\nFor help on how to change the behavior of termodoro, type 'help' then enter.\n");

    /* episode 7
     * 
     */
}
