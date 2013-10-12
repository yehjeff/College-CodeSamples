"""                                                                                       
pull files script kind of.
v1, one computer at a time     
"""

import os
import sys

print("\n This will collect files from the specified computer(s) to the computer you are on now \n\n")
your_name = raw_input("UserName? (no spaces, will be used as folder name): \n  ")
print("\n----------------------------------------------\n")
print("Please make sure the folder /Users/Shared/Collect/" + your_name + "/ exists on your computer!!!")
print("\n----------------------------------------------\n")
computer_list = ["akane", "arlet", "barry", "bonza", "braeburn", "crispin", "davey", "delicious", "dulcet", "empress", "fiesta", "firmgold", "freyburg", "fuji", "geneva", "gilpin", "grove", "haas", "hazen", "liberty", "mcintosh", "mio", "orin", "priam", "shay", "taylor", "vand", "viking", "winesap", "york"]

allOrNot = raw_input("Copy from ALL lab computers? (y/n, n = only ONE computer): \n  ")

if allOrNot == "n":
    print("\nList of computers in Sutardja Dai Apple Orchard:\n")
    print computer_list
    singleComp = raw_input("Collect files from which computer?: \n  ")
    computer_list = [singleComp]

source = raw_input("Path of folder to copy: \n  ")

startNow = raw_input("\nPress Enter to Begin Copying Files\n\n")

for comp in computer_list:
    print("\n----------------------------------------------")
    print("\nCopying files to /Users/Shared/Collect/" + your_name + "/ from " + comp + "!\n[Ctrl+C if it stalls here, that means the lab computer needs to be restarted/turned on]\n")
    os.system("scp -r -p -o StrictHostKeyChecking=no " + comp + ":" + source + " /Users/Shared/Collect/" + your_name + "/.")
        
os.system("chmod -R 777 /Users/Shared/Collect/")
print "permissions set to 777, rwx for all"
