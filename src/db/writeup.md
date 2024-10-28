# Timothy Borunov PA1 Writeup Submission

## (1) Design Decisions
I simply followed the exact methodology provide. My only unique things that I changed was that I
in the BTreeFile.h I added an atomic counter for the page numbers in order to guarentee that I would
have a unique page number for each new page that was added. For me, index and leaf pages were within the
same numbering scheme and so on disk in the file you would find leaf and index pages next to each other.
Realistically, you would want these separate probably, or compressed in some way because a lot of pages would
have holes in them, especially index pages, but for simplicity sake, I chose this implementation. As for
index and leaf pages, nothing extra was added and they pass all tests for their respective test cases easily.

## (2) Missing or incomplete elements
For the assignment, I fulfill all requirements and anything that can be considered missing is outside of the scope of
the assignment. Currently however the btree tests seem to have some bugs in them and I have yet to find them and have
not had time due to unforseen time complications.

## (3) Analytical questions
No analytical questions are listed as analytical questions in the pa1 directory so I will assume this means technical
questions and I will just copy my answers here as follows:

### Question 1:
We can utilize the strategy we used in class of having a separate memory bound buffer which stores that latest leaf 
or leaves that were written to. What we do as a result is instead of writing to disk immediately, we can store these
pages in memory and access them directly if a key value comes in that is in the range for the already loaded pages. What
we do is when insertions or lookups occur, they only happen in memory and only when the pages are flushed from the
buffer are they written to disk. This would streamline both bulk insertions and deletions, as well as all for hot
sequential lookups. The changes necessary would entail that in my code I would have to examine every time I write a 
leaf page, and instead save its page to a memory location and implement some sort of buffer freeing scheduler in order
to determine what pages should be freed and when.

### Question 2:
This is essentially the lighter version of what I said about. Instead of saving pages in memory, instead indices are
stored in a buffer pool. This is a good a idea because indices take up much less space than data and so if you have
enough memory, it makes it possible to maintain internal nodes without having to make I/O lookups, thus speeding up
the system significantly. In essence, if only the leaves are written to disk with their tuples, and internal nodes are
kept in memory, the appropriate id for the leaf node can be acquired by looking through just the internal nodes in
memory without making any I/O requests except for the discovered leaf, meaning any point query takes only O(1) I/O
instead of O(\log(N)) I/O.

### Question 3:
If by entries we mean tuples, then we can employ the bulk loading strategy we discussed in class. Of course, we do not
know just how many entries we will have, but what we can do is if we are getting multiple sequential values greater
than the largest value inserted prior to the sequence, we can simply begin building a new leaf node and populate it
so that we can maintain the most dense leaf nodes possible. In other words, we can implement an algorithm in our code
where we can still have point inserts, but if a value comes in that is greater than our largest key, we can simply
start writing it to a fresh page until we get a lower value insert or until it is full, at which point we can simply 
attach it to the rightmost internal node and add the new leaf's lowest value as a new key for the indexNode. 

## (4) Time and Challenges
I spent about 12 hours on this assignment. It is hard to estimate as I worked mostly in 30 minute chunks across several
days and then spent a few hours debugging on Sunday. The bulk of the time came from just clearing up stupid offset
errors and page linkage problems which I made when coding late at night and had forgotten about. 
Nothing was confusing if the description of the pa1 assignment was followed to the letter. 
Iterators felt off to me, but testcases cleared up all issues I had with initial understanding, but
I had a lot of weird bugs implementing them initially.

## (5) Collaboration
I worked completely on my own and only used stack overflow and cppreference.com for some syntax and library information.

