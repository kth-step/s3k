#!/usr/bin/env python3

def binpack(items, bins):
    """
    Binpacking items into bins of variable size

    Input:
        - items: list of tuples of identifier and size, e.g., [('a', 3), ('b', 2), ('c', 2)].
        - bins: dict of bin identifier and capacity, e.g., {'b0':4, 'b1':3'}.
    Output:
        If binpacking not possible, return None, else return a solution:
        {'b0': [('c', 2), ('b', 2)], 'b1': [('a', 3)] }
    """
    if not items:
        # Trivial case 
        return dict([(b,[]) for b in bins.keys()])

    # Next item to place
    (name, size) = items.pop()

    # Find a suitable bin
    for b, cap in bins.items():
        if size > cap:
            continue

        # Construct and finds solution of sub-problem
        items_copy = items.copy()
        bins_copy = bins.copy()
        bins_copy[b] = cap - size
        solution = binpack(items, bins_copy)

        if solution is not None:
            # We have found a solution to the sub-problem,
            # thus a solution for our problem
            solution[b].append((name, size))
            return res
        
    # No solution exists
    return None
