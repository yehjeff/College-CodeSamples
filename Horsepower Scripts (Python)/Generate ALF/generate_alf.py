"""                                                                                       
here we goooooo
by Yeff   
"""
'''
import os
import sys
'''
import datetime

def main():
   #ask for user inputs regarding the maya file
   job_name = raw_input("\n Name of this render job? \n  ( ex: 'Scene 1 - Opening' ) : \n    ")
   project = raw_input("\n Project path? \n  ( i.e. /Users/Shared/HorsePower-TractorFiles/CNM190-HorsePower ) : \n    ")
   render_file = raw_input("\n File path w/ project path? \n  ( ex: /Users/Shared/???/CNM190-HorsePower/scenes/camera/00-main/camera-main-current.ma ) : \n    ")
   camera = raw_input("\n What is the exact name of the camera? \n  ( don't forget to check if cameras are referenced! ) : \n    ")
   image_name = raw_input("\n What to name your image files? ( ex: 'Toyhorse', NO SPACES PLZ ) : \n    ")
   start_frame = raw_input("\n Starting frame number: \n    ")
   end_frame = raw_input("\n Ending frame number: \n    ")
   renderer = raw_input("\n Which renderer? \n  ( mr=mentalray, rman=renderman ) : \n    ")
   additional_flags = raw_input("\n Any additional render flags? ( ex: -rl lightsCam2_current1:Time12PM,lightsCam2_current1:Time02PM or shading rate stuff. Hit enter if nothing else ) : \n    ")

   if renderer == "mr":
      filetype = "png"
   elif renderer == "rman":
      filetype = "OpenEXR"
   else:
      filetype = raw_input("\n File Type? \n  ( You didn't choose mr or rman! ): \n    ")
         
   alf_file = open(image_name+".alf", "w")
   
   '''
   now = datetime.datetime.now()
   _year = str(now.year)
   _month = str(now.month)
   _day = str(now.day)
   _hour = str(now.hour)
   _minute = str(now.minute)
   '''
   
   alf_file.write("Job -title {Render " + job_name + " " + start_frame + "-" + end_frame + "} -serialsubtasks 1 -subtasks {\n")
   alf_file.write("  Task -title {Render} -subtasks {\n")
   for i in range(int(start_frame), int(end_frame)+1):
      alf_file.write("    Task -title {Frame " + str(i) + "} -serialsubtasks 1 -subtasks {\n")
      alf_file.write("      Task -title {Render} -cmds {\n")
      alf_file.write("        RemoteCmd -service {MacOSX} {/Applications/Autodesk/maya2013/Maya.app/Contents/bin/Render -r " + renderer + " -pad 4 -cam " + camera + " -proj " + project + " -im " + image_name + " -rd /Users/Shared/CNM_Farm/horsepower/renders/" + image_name + "/ -fnc name.#.ext -s " + str(i) + " -e " + str(i) + " -of " + filetype + " " + additional_flags + " " + render_file + "}")
      alf_file.write("      }\n")
      alf_file.write("    }\n")
   
   alf_file.write("  }\n")
   alf_file.write("}\n")
   
   alf_file.close()


main()
