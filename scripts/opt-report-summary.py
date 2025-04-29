#!/usr/bin/env python3

import yaml
import sys
from os import path
from dataclasses import dataclass


@dataclass
class Remark:
    type: str
    was_successful: bool
    file: str
    line: int
    args: dict


class IgnoreUnknownTagsLoader(yaml.SafeLoader):
    pass


def unknown_tag_constructor(loader, tag_suffix, node):
    return loader.construct_mapping(node)


IgnoreUnknownTagsLoader.add_multi_constructor("", unknown_tag_constructor)


def transform_clang_optreport(fs):
    for entry in yaml.load_all(fs, Loader=IgnoreUnknownTagsLoader):
        type = entry.get("Pass", "")
        if not type:
            continue
        name = entry.get("Name", "")
        if not name:
            continue
        was_successful = "Missed" not in name and "Not" not in name

        loc = entry.get("DebugLoc")
        if not loc:
            continue
        if "File" not in loc:
            continue
        if "Line" not in loc:
            continue
        file = path.basename(loc["File"])
        line = int(loc["Line"])

        args = {k: v for d in entry.get("Args", []) for k, v in d.items()}
        # Remove overly-verbose, non-structured form arguments
        for arg in ("String", "DebugLoc"):
            args.pop(arg, None)

        if type == "loop-vectorize":
            # Beware: LLVM loop-vectorize pass is marked as "passed" even if
            # it was unable to vectorize but still able to interleave.
            # Make sure we consider it missed in such cases.
            vectorization_factor = int(args.setdefault("VectorizationFactor", 0))
            was_successful = vectorization_factor > 0

        yield Remark(
            type=type, was_successful=was_successful, file=file, line=line, args=args
        )


if __name__ == "__main__":
    for r in transform_clang_optreport(sys.stdin):
        status = "PASSED" if r.was_successful else "MISSED"
        args = " ".join(f"{key}={value}" for key, value in r.args.items())
        print(f"{status}({r.type}) {r.file}:{r.line} {args}")
