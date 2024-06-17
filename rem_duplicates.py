import os
import hashlib
def hashfile(filename):
   with open(filename, 'rb') as f:
      hasher = hashlib.sha1()
      while True:
         data = f.read(1024)
         if not data:
            break
         hasher.update(data)
      return hasher.hexdigest()
 
def find_duplicates(dirname):
   files = os.listdir(dirname)
   if len(files) < 2:
      return
   hashes = {}
   for filename in files:
      path = os.path.join(dirname, filename)
      if not os.path.isfile(path):
         continue
      file_hash = hashfile(path)
      if file_hash not in hashes:
         hashes[file_hash] = path
      else:
         print(f'Duplicate found: {path} and {hashes[file_hash]}')
         os.remove(path);
            
if __name__ == '__main__':
   find_duplicates('./database_3')