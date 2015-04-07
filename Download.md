# Introduction #

Because this code is in constant development, we strongly encourage everyone to obtain the source code
through the Google Codes subversion server. We are not posting tar.gz source distributions, because we only want
user questions about the current version. (Please contact john.shumwayjr@gmail.com if you need a current
pi-qmc.tar.gz source tarball.) Once you have a subversion checkout, you can always make a tarball using
“make dist”.

# Obtaining the code through subversion #

Instructions from google for downloading the source code can be found under the
[“Source”](http://code.google.com/p/pi-qmc/source/checkout) tab, above.

If you are not a registered developer, just use an anonymous checkout:

```sh

svn checkout http://pi-qmc.googlecode.com/svn/trunk/ pi
```

Registered developers should follow the instructions on  the
[“Source”](http://code.google.com/p/pi-qmc/source/checkout) tab.

For help with subversion, see the online <a href='http://svnbook.red-bean.com/'><em>Subversion Red Book</em></a>.

Next, read the [Build/Install](BuildInstall.md) page.