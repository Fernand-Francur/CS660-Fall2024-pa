# Timothy Borunov PA1 Writeup Submission

## (1) Design Decisions
I simply followed the exact methodology provided. All functions perform as expected and there were no specific 
datastructures I really needed to specify or use. I tried to make my code as simple and fast as possible, but I will 
be honest and say I did get a bit lazy with the aggregate code and it has some repetitive lines as a result of fully
separating the grouped and ungrouped aggregate operation which could possibly be combined to reduce the code base
while adding complexity to the internal conditional statements. Similarily I could simplify the predicate switch 
statements but since simplification is not a requirement, once I got it working I also left it as is where the code
performs quickly, but is still quite dense. Since we have to read the Tuples from the file, we are essentially having to
read in row by row and so it is pretty slow in the backend operation and in a real system, it might be possible to 
optimize it better for query workloads.

## (2) Missing or incomplete elements
For the assignment, I fulfill all requirements and anything that can be considered missing is outside of the scope of
the assignment. No elements are incomplete, but I did choose to specify what the final outputs would be for aggregate 
in terms of type so dBfiles for output would have to be correctly specified with int or double for the same way as the
test cases were. Essentially, tuples create in aggregate for count will always be ints, and averages will always be
double. This is not missing or incomplete, this is more just a bit of a specific situation that results of 
oversimplification of the problem.

## (3) Analytical questions
No analytical questions are listed as analytical questions in the pa1 directory so I will assume this means technical
questions and I will just copy my answers here as follows:

### Question 1:
The problem is that we could result in a lot of duplicates because the two predicates can cause a lot of overlap to the 
point that the resulting output file can at the worst have two copies of each row. The way to change this would be to 
simply use the existing filter function specification. We can modify the filter to have a particular op flag for the 
predicates so that we know whether or not we want to do AND, OR, NOT, etc. types of operations, and in so doing allow 
internal operation changes to allow for more predicates with specific combination in effect. If we want to do this 
externally without changing the internal filter function specifics, we can apply Morgan's Law and completely use
multiple filter operations to execute a simple OR using the form: A OR B = NOT ( NOT A AND NOT B ) which we can support
if we are allowed to use intermediate files. We could also do a join, then a filter, and then a projection to get
what we want because the join will act essentially as the first part of the OR and the projection and filter will pare
down the duplicates and use the second part of the OR, but again we will need intermediate files.


### Question 2:
The multiple group extension would not be too difficult. We would just need to add support for groups to have a variable
size because currently I used an unordered map that I create for the association between a unique group key and its 
related aggregate value, but I could simply make the key be a vector that can be appropriately hashed for the unordered
map type that can serve as a unique key for the aggregate values. As a result, I can then write in the tuples with the
aggregate value related to multiple groups. As for the having clause, it is essentially the usage of a filter
on the existing final aggregate values. This means once the tuple is created, I could add a helper filter function that
returns tuples after filtering rather than directly writing them to files so that I can only write the tuples having the 
specified values. This can come with a lot of different optimization like starting to ignore specific keys when a count
grows above a specified having value.

### Question 3:
My current join complexity is O(N x M) where N is number of tuples from left table and M is number of tuples from right
table if we ignore bufferpool optimization. If we use the bufferpool and are able to pin the left table's currently read
page to memory, we can get a complexity of O(a + N * b) I/O where a is the number of tuples in the outer page and
b is the number of tuples in a page on the inner table because we will be buffering both the 
left and right pages, but we still need to read in all pages of the outer table per tuple N. 
We can optimize this by the sequence of how we read in the pages. What we can do is specifically keep one page of the 
outer table in memory and compare it with all the pages of the inner table before we move to the next table. As a result
we can get an optimized complexity of O(a + a*b) because we would only have to read in pages of the inner table 
for each page of the outer table because after we compare tuples in the outer table page to all the tuples in the inner
table, we never have to read it again, and therefore we only have to perform b extra pages per outer table page.

## (4) Time and Challenges
I spent about 5 hours on this assignment as it felt on the easier side. No real challenges
because no difficult edge cases were present. I thought I would need to implement a lot of checks
for projections and joins having to do with NULL values but the problem description and tests
designate that we assume that we do not have null values, and we always assume we have correctly formatted
output files for all our functions. If we needed to add those, then it might have been more challenging.

## (5) Collaboration
I worked completely on my own and only used stack overflow and cppreference.com for some syntax and library information.