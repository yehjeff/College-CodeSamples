"""                                                                                       
Do-For-All file script, v1   
"""

import os
import sys

print("\n This will perform the specified cmd line command on every computer in the lab \n\n")

command = raw_input("Specify the command (example: rm -rf /Users/Shared/Yeff; mkdir /Users/Shared/Yeff-mf): \n  ")

computer_list = ["akane", "arlet", "barry", "bonza", "braeburn", "crispin", "davey", "delicious", "dulcet", "empress", "fiesta", "firmgold", "freyburg", "fuji", "geneva", "gilpin", "grove", "haas", "hazen", "liberty", "mcintosh", "mio", "orin", "priam", "shay", "taylor", "vand", "viking", "winesap", "york"]

allOrNot = raw_input("Do command on ALL lab computers? (y/n, n = only ONE computer): \n  ")

if allOrNot == "n":
    print("\nList of computers in Sutardja Dai Apple Orchard:\n")
    print computer_list
    singleComp = raw_input("Perform the command on which computer?: \n  ")
    computer_list = [singleComp]

for comp in computer_list:
    print("\n----------------------------------------------")
    print("\n Performing Command on " + comp + "!!!\n")
    os.system("ssh -o StrictHostKeyChecking=no " + comp + " ' " + command + "; hostname;' & ")

