# My summary #
Pulm is Qt5 based application to be used as a client of [betaseries.com](http://www.betaseries.com). The idea is to have a simple Tray icon menu to check which episode you have seen. The second great feature will be the notification system, to alert you when an episode is released.

# To use me #
Download the executable for your platform, double-click, allow exception in the firewall if needed, use it.
* Pc
  * [Mega]()(unavaible)
  * [DropBox]()(unavaible)
* Mac
  * [Mega]()(unavaible)
  * [DropBox]()(unavaible)

# Whatch me I'm famous #
In progress

# What i'm doing ! (And what I'll do ...) #
What I did:
* Connect you with your betaseries account and display the unseens episodes of your favorite shows.
* Retrieve the summary of all apisodes already seen.
* Mark episode as seen direct on betaseries.com.

What I would do:
* A very nice notifications system to alert you when it's time to be hasty.
* Offline version with synchronisation when connected

What I hope to do:
* Local version
* Native OSX client

# To develop with me #
## Get base ##
Clone Pulm like:

    ssh  : git clone git@github.com:liomka/Pulm.git
    https: git clone https://github.com/liomka/qUpdater.git

## Get modules ##
Go into the Pulm folder, under ./src/Third-part/ and clone the qupdater project like:

    ssh  : git clone git@github.com:liomka/qUpdater.git
    https: git clone https://github.com/liomka/qUpdater.git

## Work with QtCreator ##
Open the Pulm.pro file avaible in Pulm/src folder.

Under the project tab, add build environment variable as BETASERIESAPIKEY with your betaseries api key avaible [here](http://www.betaseries.com/api/) (account needed).

Open-Ssh should be provided in your mac OSX system and is provided in this repo fro Windows users.

Other build system are not supported yet. CMake is in mind ...

----
