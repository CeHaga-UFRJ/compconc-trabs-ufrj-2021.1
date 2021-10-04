blocksInput = []
blocksOutput = []

with open('in.txt') as f:
  f.readline()
  blocksInput = [sorted(list(map(int,line.split()))) for line in f.readlines()]

with open('out.txt') as f:
  blocksOutput = [sorted(list(map(int,line.split()))) for line in f.readlines()]

valid = len(blocksInput) == len(blocksOutput)
if(valid):
  for b in blocksOutput:
    if b in blocksInput:
      blocksInput.remove(b)
    else:
      valid = False
      break

print(valid)