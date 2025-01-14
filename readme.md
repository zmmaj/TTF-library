This small library I was implement in SrBinOS, but AND in HelenOS
Here is Implementacion in HelenOS
It will be cool, if someone this lib turn on usefull stuff, to actually draw gluph on screen.
This Library was able to read almost all elements of TTF file
And, as you will see and to PRINT it out on screen
 So, let start.
 
             INSTALLATION
 
 Download complete folder 'ttf' 
 Paste it in '/uspace/lib/'
 open now 'meson.build' ( last stuff in folder 'lib' and put inside list 'ttf', as is below

 	'termui',
	'trackmod',
	'ttf',    <- here it is....
	'untar',
	'uri',

 Ok.. save all
 Compile HelenOs with new library inside.
 that is all


        TEST

For testing purpose, I was make simple test
to print out Gluphs outline coordinates as example.
and YES, you can try and this app called same as lib 'ttf'.

      INSTALLATION
Download complete folder 'test'.
Inside is another file with name 'ttf'
Copy/paste folder 'ttf' to '/uspace/app/'.
OK... now on the bottom of 'app' folder find and open 'meson.build' file
Insert in that file line 'ttf,'   as is shown below, offcourse without ' <-here it is' stuff :)

	'top',
	'trace',
	'ttf',   <-here it is
	'uidemo',
	'untar',
	'usbinfo',
save all, compile HelenOs with new app
START HelenOS

How it's work?

Well this app test ttf library, and here will 
-open ttf file
-read ttf file
-extract all data related to gluph outline coordinates
-Print that data...

Since Terminal have small window, without possibilities to scrool back entires
I decide to , beside print on screen , and write to 'FontTest.txt' file.

Let's try
Enter in terminal just ttf... 
you will get warning, to enter ttf filename... as is below
![start wrong](https://github.com/user-attachments/assets/cad15571-fcc2-4b34-bf12-2e447e2904ee)

Do that
Enter now 
 ttf envy.ttf
![envy](https://github.com/user-attachments/assets/d2bfb150-3f96-4c93-8f2b-1fd400dbff46)

Whola... here it is, something interesting now. All coordinates was on screen. as is below
Ok, not all..
to see all extracted data type in terminal next...
edit FontTest.txt
You will get as below
![envy edit txt](https://github.com/user-attachments/assets/799e5c54-492d-4525-b24a-8964d76d4038)

OK how to compare data of envyttf and envy_bold.ttf?
Do not close editor window!
Open new terminal and now enter
ttf envy_bold
and you will get data as below 
![envy_bold](https://github.com/user-attachments/assets/b0845fbb-5b9b-4814-b346-575e8c6db9a0)

Continu, just as before, tupe in editor
edit FontTest.txt
And wditor will open same file but with new data inside..

now you can compare it.. 
![both txt](https://github.com/user-attachments/assets/44008347-3da2-4595-a270-b51efd4f0f3a)


Why only this 2 ttf files???
Only that 2 I was upload for testing purpose...
Try ttf lib with any availabe...
Note that that file must be on root folder.
I did not try to load file, with some wierd path eg '../pictures/envy.ttf'.






 
