# plcc

Procedure parameter branch is kind of broken. I rushed trying to get the procedures working and it has become a mess.

The main issue is that I have lost any kind of separation between the idea of the language and the implementation of that using the Interpreter class. The way the language works should be separate from the code genaration if I want to make it possible to target different back ends. The syntax tree is in a way a kind of intermediate representation like intermediate code. So it needs to be a solid construct that represents the language appropriately.

I need to take some time and actually do some language design. Get a syntax that I can be happy with and pin down how the structures should and shouldn't work semanticaly before moving on to code generation.

I did get some cool stuff to work and I am pretty happy overall, but there are definately some things that need work on the language. Working on these things will break everything and be a significant re-write. Funny as this was a significant rewrite and improvement on the class project this semester. It has been a very good learning experience, but I went too fast. Time to slow down and do one thing at a time. Like get some testing and documentation going earlier, rather than just putting it off trying to finish before my job starts, which is in a few days. I won't make it, so time to adjust.
