import os
import random


files = os.listdir("./database_3");
#print(files);
lis = []
index = 0;
os.chdir("./database_3");
while len(lis) < 6000:
     num = random.randint(1, 999000);
     while lis.count(num) == 1:
         num = random.randint(1, len(files));
     lis.append(num);
     try:
         os.rename(f"file{index}.bmp" , f"img{num + 9123000}.bmp");
     except:
        print("except " + f"file{index}.bmp   ---> " + f"img{num + 9123000}.bmp");
     index += 1;
     