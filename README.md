# WZRY-GamePatch
a simple patch for wzry or other games
# how to use?
0.kill the sign auth (use xphook or epichook)<br>
1.patch classesPatch.dex to original apk's dex<br>
2.put onCreateCode in MainClass onCreate<br>
or put ServiceCode in AndroidMainfest.xml Service.<br>
3.compile the cpp and put the copiled file in the lib/armxxx<br>
4.edit the dex code (the lib loader) to the lib-filename<br>
5.sign the apk and install to test. the lib/armxxx<br>
4.edit the dex code (the lib loader) to the lib-filename<br>
5.sign the apk and install to test the game.
# how to use epichook to kill sign auth
1.rename the apk-backup to 'Arm_Epic'
2.merge the androidmainfest.xml in apk<br>
3.patch epichook.dex to apk's dex<br>
4.put libs in apk's lib<br>
5.put 'Arm_Epic' to apk's 'assets/'<br>
6.sign the apk<br>
