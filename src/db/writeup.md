# Timothy Borunov PA1 Writeup Submission

## (1) Design Decisions
All of the code was implemented in the way that was specified to be done. In this case, files were the underlying
structure for our databases so I left it at that and used the recommend preads and pwrites in order to open files.
An alternative would have been to use fstreams but I felt that since we were passing file descriptors it would be easier
to stick with the base C-like implementation. Since we are not going for efficiency, however, I did not implement a form
of Bufferpool for this version of our implementation. I understand that our last week's version could be compiled with 
this with little adjustments but since it was not in the assignment that has not yet been done but might in the future.
As such, alas, I do perform a read and write per modification of any tuple which becomes costly over many subsequent
writes. Also, since it was mentioned in the assignment, I do not clean tuples when they are deleted, 
but simply remove them from the header and then replace them with a clean tuple when a new one is inserted into its
slot. In a real database, this could lead to security vulnerabilities since data from prior tuples sticks around in
database, but since this was not in the assignment's graded criteria, it has been overlooked for the sake of simplicity.
All other design decisions I believe are generally simple enough to see from the code. I implemented two helper 
functions in order to view and modify bits in big endian notation for the header, again, so that it appears like 
a bitmap and is easier to view.

## (2) Missing or incomplete elements
For the assignment, I fulfill all requirements and anything that can be considered missing is outside of the scope of
the assignment as described above.

## (3) Analytical questions
No analytical questions are listed as analytical questions in the pa1 directory so I will assume this means technical
questions and I will just copy my answers here as follows:

### Question 1:
If we assume that files are to be considered as hard disk storage, then unfortunately each page might be contiguous
within the file, so if there are empty pages between filled pages, we could have gaps in our disk space where a file
simply has nothing on a page. There are two possible solutions I can think of. If the OS supports file fragmentation
then it might be possible to free that page and simply re-label the existing pages to account for one page deleted and 
decrement the count of pages. This means that very little extra operation needs to be done by our system and we force
the OS to handle how to find the write pages, however, we lose contiguity of our data. The other solution is to fully
copy all the data down a page in order to "release" the empty space. Realistically, it might even be simpler to just
write all the existing full pages to another created file, and then free the previous file's space, thus essentially
removing all gaps inside the file but at the cost of fully rewriting most or all existing data. But this way we do not 
lose contiguity of our data and reads still occur at fastest speed.

### Question 2:
In the tuple we define each type as a particular type and then use a size. We can simply allow the user to declare the
type varchar, and have them input the size of the variable type. This places some more pressure on the user to provide 
correct information, but it also allows for a more generalized system. This way, instead of type_t, we could instead 
store just an integer size that we can store in the TupleDesc so that we know how much we need to read and write in the 
serialization and deserialization. In essence, we could store any data of type varchar with user provided size, but 
once the user gets the tuples with those sizes, if what they stored were structures, they can simply take the retrieved
data and fit them to the structure architecture in order to view their data as their structure again.

Another possibility is instead of using type_t, we can simply create an interface which takes an inputted type, does
sizeof and stores the size that way. This means that they would need to types that are defined by used libraries or
created structs, so we would lose some flexibility of being able to simply write in any kind of data since it would
require some existing type, but generally this would allow for easier reading out of data on receiving a tuple without
having to deserialize the data in the tuple to fit the original structure.

## (4) Time and Challenges
I spent about 8 hours on this assignment. It is hard to estimate as I worked mostly in 30 minute chunks across several 
days and then spent a few hours debugging on friday. The bulk of the time came from just clearing up stupid bit 
arithmetic errors which I made when coding late at night and had forgotten about. Nothing was confusing if the
description of the pa1 assignment was followed to the letter. Iterators felt off to me, but testcases cleared up
all issues I had with initial understanding.

## (5) Collaboration
I worked completely on my own and only used stack overflow and cppreference.com for some syntax and library information.

