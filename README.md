# Termodoro
version 0.01

Termodoro is a tool I'm creating to help me keep track of work time vs break
time with the [pomodoro
technique](https://en.wikipedia.org/wiki/Pomodoro_Technique). It is
intended to be a lead up to another project that will be a more
comprehensive tool that helps manage my time.

My internship last summer (2020) saw me keeping close track of how I spent
my time at the office, and it felt sort of clunky using an excel
spreadsheet that gradually became longer and longer. That app is going to
be much more complex than this, so I'm starting off with a simple(r)
pomodoro timer.

## Why not just use Emacs (org-mode or something similar)?

I don't like the emacs solution to this. I tried it for several weeks and
found it more difficult to use than it was worth.


# Notes on the files

Originally, I was going to use this as an opportunity to test the notion of
teaching how to program with this file by writing a story in the comments.
But this the scope of this project outgrew a simple programming tutorial.

- termodoro.c contains the beginnings of a story.
- termodoronostory.c contains the first attempt at the full app. It's a
  hacky mess that I wrote in 2 days.
- termodoro2.c contains a better thought out version. 
- termodoro.org contains the design description as an Emacs org document.
  It's just plain text file with lines wrapped, so it should be no issue
  viewing it in a standard text editor.


## Why C?

Because it's what I've been doing a lot of right now. Also, I'm using this
as a simple way of practicing with multithreaded programming in C. Maybe I
won't use C for multithreading in practice, but it's a good exercise (I
hope).


## Where's ncurses?

I'm learning how terminals work with this project, so I'm not using
ncurses. I'll revise it later to use ncurses.


# Compiling

Termodoro requires `clang`. 

use ```clang -o termodoro termodoro2.c -I.```
and run with ```./termodoro```

# Installation

We're not even close. Stay tuned folks. (2020-12-12)

# Usage

For now, you should look at the org document for this part.

