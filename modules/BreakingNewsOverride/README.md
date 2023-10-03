<img align="left" width="220" height="250" style="margin: 0px 40px 0px 0px;" src="https://cdn.discordapp.com/attachments/814444289181351968/1077719446594002984/bkn.png">

# Breaking News Override
This module utilizes the Warden Payload Manager to enable the Breaking News frame on the left side of the character selection screen. You can feed it a html file defined the the config to use it.

<br><br><br>

# Setup
- Create a html file which contains the body for your breaking news frame.
- Set the path to the html file in the config.
- Enable the module and login to verify it works.

# Notes
There is a small amount of html tags that are actually compatible, follow this guide here: https://wowwiki-archive.fandom.com/wiki/UIOBJECT_SimpleHTML

You must escape the characters `[`, `]`, `'`, `\` in your title and html file because of the way the payload is sent.
Example:
`\'You think this hurts? Just wait.\'`

# Credits

Thanks to the AzerothCore community and Foe especially for getting this to work.