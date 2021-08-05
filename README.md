# WZRY-GamePatch
a simple patch for wzry or other games
# how to use?
0.kill the sign auth (use xphook or epichook)
1.patch classesPatch.dex in original apk dex
2.put onCreateCode in MainClass onCreate
or put ServiceCode in AndroidMainfest.xml Service.
3.compile the cpp and put the copiled file in the lib/armxxx
4.edit the dex code (the lib loader) to the lib-filename
5.sign the apk and install to test. the lib/armxxx
4.edit the dex code (the lib loader) to the lib-filename
5.sign the apk and install to test the game.
# how to use epichook to kill sign auth
