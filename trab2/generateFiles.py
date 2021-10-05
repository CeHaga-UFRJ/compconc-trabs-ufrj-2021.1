import sys, random

def main(lines, blockSize, minN, maxN):
  N = lines * blockSize
  with open('in.txt','w') as f:
    f.write(f'{N}\n')
    for k in range(lines):
      for i in range(blockSize):
        f.write(f'{random.randint(minN, maxN)} ')
      f.write('\n')

if __name__ == "__main__":
  if(len(sys.argv) < 5):
    print(f'Formato:\n{sys.argv[0]} <Total de linhas> <Tamanho do bloco> <Numero minimo> <Numero maximo>')
    exit(1)
  lines = int(sys.argv[1])
  blockSize = int(sys.argv[2])
  minN = int(sys.argv[3])
  maxN = int(sys.argv[4])
  main(lines, blockSize, minN, maxN)