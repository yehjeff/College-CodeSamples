"""                                                                                       
Delete files script, v1   
"""

import os
import sys

print("\n This will delete files from the specified folder in every computer in the lab \n\n")

folderPath = raw_input("Specify the path of the folder/file to delete (ex: /Users/Shared/HorsePower-TractorFiles/Opening deletes the Opening folder), include the folder/file name, no spaces please!: \n  ")

computer_list = ["akane", "arlet", "barry", "bonza", "braeburn", "crispin", "davey", "delicious", "dulcet", "empress", "fiesta", "firmgold", "freyburg", "fuji", "geneva", "gilpin", "grove", "haas", "hazen", "liberty", "mcintosh", "mio", "orin", "priam", "shay", "taylor", "vand", "viking", "winesap", "york"]

allOrNot = raw_input("Delete from ALL lab computers? (y/n, n = only ONE computer): \n  ")

if allOrNot == "n":
    print("\nList of computers in Sutardja Dai Apple Orchard:\n")
    print computer_list
    singleComp = raw_input("Delete files from which computer?: \n  ")
    computer_list = [singleComp]

for comp in computer_list:
    print("\n----------------------------------------------")
    print("\nDeleting folder " + folderPath + " on " + comp + "!\n[Ctrl+C if it stalls here, that means the lab computer needs to be restarted/turned on]\n")
    os.system("ssh -o StrictHostKeyChecking=no " + comp + " 'rm -rf " + folderPath + "; hostname;' & ")

