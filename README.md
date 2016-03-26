# Event-counter-
Implemented a event counter using Red black tree.
Event counter stores key value pair in ordered manner. 

Complexity:
increase(theID, m)          O(log n)
reduce(theID, m)            O(log n)
count(theID)                O(log n)
inRange(ID1, ID2)           O(log n + s) where s is the number of IDs in the range.
next(theID)                 O(log n)
previous(theID)             O(log n)
