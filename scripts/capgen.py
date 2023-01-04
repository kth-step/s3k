#!/usr/bin/env python3
# See LICENSE file for copyright and license details.
"""
asd
"""
import argparse
import re
import yaml


def binpack(bin_capacities, items):
    """
    Solves the binpacking problem optimally
    """
    if not items:
        return {i: [] for i in bin_capacities.keys()}
    name, size = items[0]
    for i, capacity in bin_capacities.items():
        if size > capacity:
            continue
        new_bins = bin_capacities.copy()
        new_bins[i] -= size
        packing = binpack(new_bins, items[1:])
        if packing is not None:
            packing[i].append(name)
            return packing
    return None


class Capability:
    def __init__(self, name, fields):
        self._name = name
        self._fields = fields.keys()
        self._sizes = fields.copy()
        items = list(fields.items())
        self._alloc = binpack({'word0': 56, 'word1': 64}, items)
        if not self._alloc:
            raise Exception(f"No allocation for {name} capability")
        self._offsets = {}
        for word, word_fields in self._alloc.items():
            offset = 0
            for field in word_fields:
                self._offsets[field] = offset
                offset += self._sizes[field]

    def type(self):
        return f"CAP_{self._name.upper()}"

    def name(self):
        return self._name

    def size(self, field):
        return self._sizes[field]

    def offset(self, field):
        return self._offsets[field]

    def word(self, field):
        for word, fields in self._alloc.items():
            if field in fields:
                return word
        return None

    def fields(self, word=None):
        if word:
            return self._alloc[word]
        return self._fields

    def constr_fun(self):
        output = []
        parameters = [f"uint64_t {name}" for name in self.fields()]
        output.append(f"static inline struct cap cap_{self.name()}"
                      f"({', '.join(parameters)})\n{{")
        for field in self.fields():
            mask = (1 << self.size(field)) - 1
            output.append(f"\tASSERT(({field} & {hex(mask)}ull) == {field});")
        output.append(f"\tstruct cap cap = (struct cap) {{{self.type()}, 0}};")
        for field in self.fields('word0'):
            offset = self.offset(field)
            output.append(f"\tcap.word0 |= {field} << {offset};")
        for field in self.fields('word1'):
            offset = self.offset(field)
            output.append(f"\tcap.word1 |= {field} << {offset};"
                          if offset else f"\tcap.word1 |= {field};")
        output.append("\treturn cap;")
        output.append("}")
        return "\n".join(output)

    def getter_fun(self, field):
        name = self.name()
        word = self.word(field)
        offset = self.offset(field)
        mask = (1 << self.size(field)) - 1
        output = []
        output.append(f"static inline uint64_t cap_{name}_get_{field}"
                      "(const struct cap *cap)\n{")
        output.append(f"\tASSERT(cap_get_type(cap) == {self.type()});")
        if offset:
            output.append(f"\treturn (cap->{word} >> {offset}) & {hex(mask)}ull;")
        else:
            output.append(f"\treturn cap->{word} & {hex(mask)}ull;")
        output.append("}")
        return "\n".join(output)

    def setter_fun(self, field):
        name = self.name()
        word = self.word(field)
        offset = self.offset(field)
        mask = (1 << self.size(field)) - 1
        output = []
        output.append(f"static inline void cap_{name}_set_{field}"
                      f"(struct cap *cap, uint64_t {field})\n{{")
        output.append(f"\tASSERT(cap_get_type(cap) == {self.type()});")
        output.append(f"\tASSERT(({field} & {hex(mask)}ull) == {field});")
        if offset:
            mask = mask << offset
            output.append(f"\tcap->{word} = (cap->{word} & ~{hex(mask)}ull)"
                          f" | ({field} << {offset});")
        else:
            output.append(f"\tcap->{word} = (cap->{word} & ~{hex(mask)}ull)"
                          f" | {field};")
        output.append("}")
        return "\n".join(output)


def make_condition(parent, child, conditions):
    condition = "(" + ") &&\n\t\t       (".join(conditions) + ")"
    pairs = re.findall(r'(p|c)\.([a-z]+)', condition)
    pairs = list(set(pairs))
    for var, field in pairs:
        abbr = f"{var}.{field}"
        unabbr = f"cap_{parent}_get_{field}(p)" \
                 if var == "p" else f"cap_{child}_get_{field}(c)"
        condition = condition.replace(abbr, unabbr)
    return condition


def make_pred_case(parent, child, conditions):
    output = []
    parent_type = f"CAP_{parent.upper()}"
    child_type = f"CAP_{child.upper()}"
    condition = make_condition(parent, child, conditions)
    output.append(f"\tif (cap_get_type(p) == {parent_type}"
                  f" && cap_get_type(c) == {child_type})")
    output.append(f"\t\treturn {condition};")
    return "\n".join(output)


def make_pred(name, cases):
    output = []
    output.append(f"static inline bool cap_{name}"
                  "(const struct cap *const p, const struct cap * c)\n{")
    for case in cases:
        output.append(make_pred_case(**case))
    output.append("\treturn false;")
    output.append("}")
    return "\n".join(output)


# Open the file and load the file
def main(capabilities, predicates):
    enums = ',\n\t'.join([f"CAP_{cap['name'].upper()}"
                          for cap in capabilities])
    output = []
    output.append(f"""\
#ifndef __CAP_H__
#define __CAP_H__
/* See LICENSE file for copyright and license details. */
#include <stdbool.h>
#include <stdint.h>

#include "common.h"

enum cap_type {{
        CAP_EMPTY,
        {enums}
}};

struct cap {{
        uint64_t word0, word1;
}};

static inline uint64_t cap_get_type(const struct cap *cap)
{{
        return (cap->word0 >> 56) & 0xFFull;
}}\
""")
    for cap in capabilities:
        capability = Capability(**cap)
        output.append(capability.constr_fun())
        for field in capability.fields():
            output.append(capability.getter_fun(field))
        for field in capability.fields():
            output.append(capability.setter_fun(field))

    for pred in predicates:
        output.append(make_pred(**pred))

    output.append("#endif /* __CAP_H__ */")
    output = "\n\n".join(output)
    output = output.replace("\t", " "*8)
    return output


if __name__ == "__main__":
    parser = argparse.ArgumentParser(
            prog="gen_cap",
            description="Generate capability header "
                        "file from yaml description")
    parser.add_argument("yaml", type=argparse.FileType('r'))
    parser.add_argument("output", type=argparse.FileType('w'))
    args = parser.parse_args()
    data = yaml.safe_load(args.yaml)
    print(main(**data), file=args.output)
