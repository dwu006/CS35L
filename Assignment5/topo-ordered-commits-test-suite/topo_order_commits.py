#!/user/bin/env python3


"""
used this strace command to find the cmds being executed
strace -e execve -f python topo_order_commits.py
since i only saw "python" and "topo_order_commits.py" being executed,
i know that i havent used any other cmds
"""

import os
import sys
import zlib

class CommitNode:
    def __init__(self, commit_hash):
        """
        :type commit_hash: str
        """
        self.commit_hash = commit_hash
        self.parents = set()
        self.children = set()

"""
gets current directory then goes into a loop which looks for .git
if doesnt find goes up a directory and keeps looking.
returns path to git directory
"""
def find_git_directory():
    current_dir = os.getcwd()
    while True:
        git_dir = os.path.join(current_dir, '.git')
        if os.path.isdir(git_dir):
            return git_dir
        parent_dir = os.path.dirname(current_dir)
        if parent_dir == current_dir:
            sys.stderr.write('Not inside a Git repository\n')
            sys.exit(1)
        current_dir = parent_dir

"""
creates a dictionary to store branch and commits
finds refs/heads in the directories and loops thorugh all the files
gets the branch name and the commit and adds it to the dictionary
returns the dictionary
"""
def get_local_branches(git_dir):
    branches = {}
    heads_dir = os.path.join(git_dir, 'refs', 'heads')
    for root, dirs, files in os.walk(heads_dir):
        for file in files:
            branch_path = os.path.relpath(os.path.join(root, file), heads_dir)
            with open(os.path.join(root, file), 'r') as f:
                commit_hash = f.read().strip()

            if commit_hash not in branches:
                branches[commit_hash] = []
            branches[commit_hash].append(branch_path)
    return branches

"""
loops through commit history using DFS (stack)
then reads commit data and compresses it using zlib
extract parent commits from data 
stores child and parent data in CommitNode classes
return dictionary
"""
def build_commit_graph(git_dir, branches):
    nodes = {}
    for commit_hash in branches:
        stack = [commit_hash]
        while stack:
            current_hash = stack.pop()
            if current_hash in nodes:
                node = nodes[current_hash]
                if node.parents:
                    continue
            else:
                node = CommitNode(current_hash)
                nodes[current_hash] = node
            obj_dir = os.path.join(git_dir, 'objects', current_hash[:2])
            obj_file = os.path.join(obj_dir, current_hash[2:])
            if not os.path.exists(obj_file):
                sys.exit(1)
            with open(obj_file, 'rb') as f:
                try:
                    data = zlib.decompress(f.read())
                except zlib.error:
                    sys.exit(1)
            header, _, body = data.partition(b'\x00')
            if not header.startswith(b'commit '):
                sys.exit(1)
            parents = []
            for line in body.splitlines():
                if line.startswith(b'parent '):
                    parents.append(line[7:].decode('ascii'))
            node.parents = set(parents)
            for parent_hash in node.parents:
                if parent_hash not in nodes:
                    nodes[parent_hash] = CommitNode(parent_hash)
                parent_node = nodes[parent_hash]
                parent_node.children.add(current_hash)
            stack.extend(node.parents)
    return nodes

"""
used DFS (w/ stack) to sort commit history
returned in reverse order for topological sort
"""
def topological_sort(nodes):
    visited = set()
    order = []
    for commit_hash in sorted(nodes.keys()):
        if commit_hash in visited:
            continue
        stack = [(commit_hash, False)]
        while stack:
            current_hash, processed = stack.pop()
            if processed:
                order.append(current_hash)
                continue
            if current_hash in visited:
                continue
            visited.add(current_hash)
            node = nodes[current_hash]
            stack.append((current_hash, True))
            parents = sorted(node.parents, reverse=True)
            for parent_hash in parents:
                stack.append((parent_hash, False))
    return reversed(order)

"""
used to print commit history in topological order
"""
def print_commits(topological_order, nodes, branches):
    prev_commit = None
    for current_commit in topological_order:
        if prev_commit is not None:
            if current_commit not in nodes[prev_commit].parents and prev_commit not in nodes[current_commit].parents:
                parents = sorted(nodes[prev_commit].parents)
                print(f"{' '.join(parents)}=" if parents else '=')
                print()
                children = sorted(nodes[current_commit].children)
                print(f"={' '.join(children)}")
        
        branch_names = branches.get(current_commit, []) 
        branch_names = sorted(branch_names)
        print(current_commit + (' ' + ' '.join(branch_names) if branch_names else ''))
        prev_commit = current_commit

def topo_order_commits():
    git_dir = find_git_directory()
    branches = get_local_branches(git_dir)
    nodes = build_commit_graph(git_dir, branches)
    topological_order = topological_sort(nodes)
    print_commits(topological_order, nodes, branches)


if __name__ == '__main__':
    topo_order_commits()