
const char MAIN_page[] PROGMEM = R"=====(
<html>
<head>
<meta name="viewport" content="initial-scale=1">
<style>
body {font-family: helvetica,arial,sans-serif;}
table {border-collapse: collapse; border: 1px solid black;}
td {padding: 0.25em;}
title { font-size: 2em; font-weight: bold;}
name {padding: 0.5em;}
heading {font-weight: bold; background: #c0c0c0; padding: 0.5em;}
update {color: #dd3333; font-size: 0.75em;}
</style>
</head>
<div class=title>Word Clock</div>
<div class=name>Location: @@CLOCKNAME@@</div>
<form method="post" action="/form">
<table>
<tr><td colspan=2 class=heading>Status</td></tr>
<tr><td>Current Date:</td><td><b>@@YEAR@@-@@MONTH@@-@@DAY@@</b></td></tr>
<tr><td>Current Time:</td><td><b>@@HOUR@@:@@MIN@@:@@SECOND@@</b></td></tr>
<tr><td>Last Movement:</td><td><b>@@LASTMOVEMENT@@</b></td></tr>
<tr><td>NTP Sync Status:</td><td>@@SYNCSTATUS@@</td></tr>
<tr><td>Name:</td><td><input type=text name="clockname" value="@@CLOCKNAME@@"></td></tr>

<tr><td colspan=2 class=heading>WiFi Setup</td></tr>
<tr><td>SSID:</td><td><input type=text name="ssid" value="@@SSID@@"></td></tr>
<tr><td>PSK:</td><td><input type=password name="psk" value="@@PSK@@"></td></tr>
<tr><td colspan=2>Update Wifi config:<input type=checkbox name=update_wifi value=1 ></td></tr>

<tr><td colspan=2 class=heading>Time Setup</td></tr>
<tr><td>NTP Server:</td><td><input type=text name="ntpsrv" value="@@NTPSRV@@"></td></tr>
<tr><td>Sync Interval:</td><td><input type=text name="ntpint" value="@@NTPINT@@">s</td></tr>
<tr><td>Timezone:</td><td><input type=text name="timezone" value="@@TZ@@">h</td></tr>

<tr><td colspan=2 class=heading>Screen Setup</td></tr>
<tr><td colspan=2>Show "It Is":<input type=checkbox id="showitis" name="showitis" value=@@ITIS@@></td></tr>
<tr><td>LED Dim level:</td><td><input type=text name="dimlvl" value="@@DIMLVL@@"></td></tr>
<tr><td>LED On-Time (Minutes):</td><td><input type=text name="ledTime" value="@@LEDTIME@@"></td></tr>
</table>
<input type="submit" value="Update">
</form>
<div class="update">@@UPDATERESPONSE@@</div>
</html>
)=====";

