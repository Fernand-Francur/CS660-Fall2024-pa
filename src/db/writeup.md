# Timothy Borunov PA4 Writeup Submission

## (1) Design Decisions
I simply followed the exact methodology provided. All functions perform as expected and there were no specific
datastructures I really needed to specify or use. I tried to make my code as simple and fast as possible. The code,
because I wrote it very quickly, might not be the most efficient but it gets the job done as that is not part of the
assignment. I simply used a vector to have a data structure that I can index that has a variable size based on
arguments provided, and a few spare integer values for math scratch space. I initially tried to optimize the code
as best as I could in the manner of not reusing code segments, but due to the test cases being very explicit with
how they wanted computation done, I had to match it and so could not optimize in the manner I thought I could due 
to always having to round at the very end for accuracy sake. Otherwise, very simple code without any fancy 
implementations.

## (2) Missing or incomplete elements
For the assignment, I fulfill all requirements and anything that can be considered missing is outside of the scope of
the assignment. No elements are incomplete.

## (3) Analytical questions
No analytical questions are listed as analytical questions in the pa4 directory so I will assume this means technical
questions and I will just copy my answers here as follows:

### Question 1:
We can estimate the IO cost of the query since we have all the information of how our system
does accesses to particular pages and tuples, along with the estimated resultant page cardinality. We need
to consider what type of indices we have access to, which in this case we have an clustered b+ tree index
as well as how many buffer pages we have to actually process our queries.
### Question 2:
We can expand the predicate operation for EQ to run for an array of values, or we can just
run the estimate_cardinality function in a for loop for all the values we wish to join on
for the join operation. Since the estimate_cardinality function has no I/O accesses since it
does its estimation during the initial addition of elements to the files themselves, we can
comfortably do all these computations without much overhead since we can use the histogram
of the outer table with the histogram of the inner table to compute the size of the final table.
The way we can do this is in essence by taking each bucket we have for both, comparing bucket
sizes, and then multiplying the coinciding bucket sizes to get the cardinality of the final
table for that range. IT is still an estimation since our histogram does not store all the actual
values for the join, but in this manner we can get an estimation on the upper limit of I/O operations
given an assumption that if there is a certain amount of values in the same bucket, they are joined. As an
example: if outer table and inner table have buckets of the same size, if bucket range 1-10 on first have 5 elements
and bucket range 1-10 on second have 10 elements, the worst case cardinality is 5x10 = 50 possible cardinality if each
value is joined. If the buckets differ in size, such as we have an outer table with bucket range 1-2 of cardinality 5 
and outer has 1-10 with cardinality 10, we just divide by bucket range so that the cardinality multiplication is
5*2 = 10 in worst case. Of course, if one has no values in a particular bucket, the cardinality of the final table
for that bucket is 0. It is a decent estimate, but is still an estimate as the only way to be sure is to have all
the values we are actually measuring for. A better join estimation cardinality is for range queries, as we can simply
take the cardinalities of both initial tables with a particular set of predicate ops and multiply them.
### Question 3:
Since a predicate is sequential, we can utilize the sequential nature of our b+ tree structure to make computation
efficient. So we first need to locate what is the first page we need to read and then just read them in sequence
which in our case, we need to traverse the tree once, and then read 1000/50 = 20 pages in sequence: so 3 I/O for 
traversal and then 20 for reading each page so it would take us 23 I/O in total with a predicate of size 1000. If we
use a heapfile instead, we have to scan ALL pages meaning we would have to do 150000 I/O to scan through every page
to find the values fitting our predicate.

## (4) Time and Challenges
This was essentially a quick algorithm. The total code took me about 30 minutes. No challenges whatsoever because the
assignment is very straightforward. Might be more challenging if there were more specific edgecases because I am sure
I missed some that are very specific or user error based, but since we are dealing with more simplistic testcases, 
there were no issues.

## (5) Collaboration
I worked completely on my own and only used stack overflow and cppreference.com for some syntax and library information.