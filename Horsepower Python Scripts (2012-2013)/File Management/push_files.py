"""                                                                                       
Push files script, v2 - All computers copy from host   
"""

import os
import sys
from socket import gethostname

print("\n This will push files from the specified computer's /Users/Shared folder to all other computer's /Users/Shared folders in the lab \n\n")
print("This script assumes that the folder you are copying TO (i.e. HorsePower-TractorFiles) already exists on every lab computer!\n")
#from_path = raw_input("Full path to (/Users/Shared/HorsePower-TractorFiles/. for all inside, no /. for entire folder) the folder you want to copy from: \n  ")
folderPath = raw_input("Specify the path of the folder to copy in /Users/Shared/ (ex: HorsePower-TractorFiles/Opening), not the full path, include the folder name, no spaces please!: \n  ")
folderName = raw_input("Specify the name of folder to copy without the full path (ex: Opening), yes i am too lazy to parse: \n  ")


computer_list = ["akane", "arlet", "barry", "bonza", "braeburn", "crispin", "davey", "delicious", "dulcet", "empress", "fiesta", "firmgold", "freyburg", "fuji", "geneva", "gilpin", "grove", "haas", "hazen", "liberty", "mcintosh", "mio", "orin", "priam", "shay", "taylor", "vand", "viking", "winesap", "york"]

allOrNot = raw_input("Copy to ALL lab computers? (y/n, n = only ONE computer): \n  ")

if allOrNot == "n":
    print("\nList of computers in Sutardja Dai Apple Orchard:\n")
    print computer_list
    singleComp = raw_input("Put files onto which computer?: \n  ")
    computer_list = [singleComp]

startNow = raw_input("\nPress Enter to Begin Copying Files\n\n")

print("Tarballing Entire Folder")
os.system("cd /Users/Shared/" + folderPath + "; cd ..; rm -f " + folderName + ".tgz; tar -zcf " + folderName + ".tgz " + folderName + "; chmod -R 777 /Users/Shared/" + folderPath + ".tgz")

for comp in computer_list:
    print("\n----------------------------------------------")
    #os.system("scp -r -p " + from_path + " " + comp + ":/Users/Shared/" + folderName)
    if comp not in gethostname():
        print("\nCopying folder /Users/Shared/" + folderPath + " on " + comp + "!\n[Ctrl+C if it stalls here, that means the lab computer needs to be restarted/turned on]\n")
        os.system("ssh -o StrictHostKeyChecking=no " + comp + " 'cd " + os.path.dirname("/Users/Shared/"+folderPath) + "; scp -r -p -o StrictHostKeyChecking=no " + gethostname() + ":/Users/Shared/" + folderPath + ".tgz .; tar -zxf " + folderName + ".tgz; chmod -R 777 /Users/Shared/" + folderPath + "; rm -f " + folderName + ".tgz; hostname;' & ")

print("COMPLETED. Removing tgz file from host computer")
#os.system("cd /Users/Shared/" + folderPath + "; cd ..; rm -f " + folderName + ".tgz")

