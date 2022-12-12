import argparse
from collections import OrderedDict
from itertools import accumulate
import yaml
from yaml.loader import SafeLoader

class BinpackException(Exception):
    pass

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
        return OrderedDict([(b,[]) for b in bins.keys()])
    items = items.copy()
    # Next item to place
    (name, size) = items.pop()

    # Find a suitable bin
    for b, cap in bins.items():
        if size > cap:
            continue

        # Construct and finds solution of sub-problem
        bins_copy = bins.copy()
        bins_copy[b] = cap - size
        solution = binpack(items, bins_copy)

        if solution is not None:
            # We have found a solution to the sub-problem,
            # thus a solution for our problem
            solution[b].append(name)
            return solution
        
    # No solution exists
    return None

class Capability:
    def __init__(self, name, fields, with_assert=False):
        bins = OrderedDict([('word0', 56), ('word1', 64 )])
        words = binpack(fields, bins)
        if not words:
            raise BinpackException(f"Could not find allocation for {name}")
        alloc = dict(fields)
        for word, l in words.items():
            offset = 0
            for field in l:
                size = alloc[field]
                alloc[field] = (word, size, offset)
                offset += size
        self.__name = name
        self.__fields = [name for name, size in fields]
        self.__alloc = alloc
        self.__words = words
        self.__with_assert = with_assert

    def get_name(self):
        return self.__name

    def get_fields(self):
        return self.__fields

    def get_fields_of_word(self, word):
        return self.__words[word]

    def get_word(self, field):
        return self.__alloc[field][0]

    def get_size(self, field):
        return self.__alloc[field][1]

    def get_offset(self, field):
        return self.__alloc[field][2]

    def def_maker(self):
        lines = list()
        name = self.get_name()
        params = ", ".join([f"uint64_t {field}" for field in self.__fields])
        lines.append(f"static inline void s3k_{name}Make(cap_t *cap, {params})")
        lines.append("{")
        if self.__with_assert:
            for field in self.get_fields():
                mask = hex((1 << self.get_size(field)) - 1)
                lines.append(f"\tassert(({field} & {mask}ull) == {field});")
        words = dict([('word0', []), ('word1', [])])
        for field in self.get_fields():
            word = self.get_word(field)
            offset = self.get_offset(field)
            if offset:
                words[word].append(f"({field} << {offset})")
            else:
                words[word].append(f"{field}")
        word0 = ' | '.join(words['word0']) if words['word0'] else '0'
        word1 = ' | '.join(words['word1']) if words['word1'] else '0'
        lines.append(f"\tcap->word0 = {word0};")
        lines.append(f"\tcap->word1 = {word1};")
        lines.append("}\n")
        return "\n".join(lines)

    def def_getter(self, field):
        if field not in self.__fields:
            raise Exception
        lines = list()
        name = self.get_name()
        lines.append(f"static inline uint64_t s3k_{name}Get{field.capitalize()}(cap_t c)")
        lines.append("{")
        word = self.get_word(field)
        size_mask = (1 << self.get_size(field)) - 1
        offset = self.get_offset(field)
        if offset:
            lines.append(f"\treturn (c.{word} >> {offset}) & {hex(size_mask)}ull;")
        else:
            lines.append(f"\treturn c.{word} & {hex(size_mask)}ull;")
        lines.append("}\n")
        return "\n".join(lines)

    def def_setter(self, field):
        if field not in self.__fields:
            raise Exception
        lines = list()
        name = self.get_name()
        lines.append(f"static inline void s3k_{name}Set{field.capitalize()}(cap_t *c, uint64_t val)")
        lines.append("{")
        if self.__with_assert:
            mask = hex((1 << self.get_size(field)) - 1)
            lines.append(f"\tassert((val & {mask}ull) == val);")
        word = self.get_word(field)
        offset = self.get_offset(field)
        mask = hex(((1 << self.get_size(field)) - 1) << offset)
        if offset:
            lines.append(f"\tcap->{word} = (cap->{word} & ~{mask}ull) | (val << {offset});")
        else:
            lines.append(f"\tcap->{word} = (cap->{word} & ~{mask}ull) | val;")
        lines.append("}\n")
        return "\n".join(lines)

def main(caps_description):
    caps = []
    for d in caps_description:
        name = d['name']
        fields = [tuple(f.split()) for f in d['fields']]
        fields = [(f, int(size)) for f, size in fields]
        caps.append(Capability(name, fields))

    statements = list()

    statements.append('/*** Capability definitions ***/')
    for cap in caps:
        statements.append(cap.def_maker())
        for f in cap.get_fields():
            statements.append(cap.def_getter(f))
        for f in cap.get_fields():
            statements.append(cap.def_setter(f))

    return "\n".join(statements)

if __name__ == "__main__":
    parser = argparse.ArgumentParser(
            prog = "gen_cap",
            description = "Generate capability header file from yaml description")
    parser.add_argument("yaml")
    parser.add_argument("-o", "--output")
    args = parser.parse_args()
    with open(args.yaml, 'r') as f:
        data = yaml.safe_load(f)
    output = main(data['caps'])
    if not args.output or args.output == "-":
        print(output)
    else:
        with open(args.output, "w") as f:
            print(output, file=f)
