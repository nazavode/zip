#!/usr/bin/env python3
import yaml
import sys
from os import path


class IgnoreUnknownTagsLoader(yaml.SafeLoader):
    pass


def unknown_tag_constructor(loader, tag_suffix, node):
    return loader.construct_mapping(node)


IgnoreUnknownTagsLoader.add_multi_constructor("", unknown_tag_constructor)


def transform_optreport(fs):
    for entry in yaml.load_all(fs, Loader=IgnoreUnknownTagsLoader):
        remark_type = entry.get("Pass", "")
        name = entry.get("Name", "")
        if remark_type and ("Missed" in name or "Not" in name):
            status = "MISSED"
        elif remark_type:
            status = "PASSED"
        else:
            continue

        loc = entry.get("DebugLoc")
        if not (loc and "File" in loc and "Line" in loc):
            continue

        file = path.basename(loc["File"])
        line = loc["Line"]
        message = f"{status}({remark_type}) {file}:{line}"

        # Special case: extract vectorization details
        if remark_type == "loop-vectorize":
            args = {k: v for d in entry.get("Args", []) for k, v in d.items()}
            vf = args["VectorizationFactor"]
            ic = args["InterleaveCount"]
            message += f" VectorizationFactor={vf} InterleaveCount={ic}"

        print(message)


if __name__ == "__main__":
    transform_optreport(sys.stdin)
