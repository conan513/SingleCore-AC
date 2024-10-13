# ![logo](https://raw.githubusercontent.com/azerothcore/azerothcore.github.io/master/images/logo-github.png) AzerothCore

# Breaking News Override

- Latest build status with azerothcore:

[![Build Status](https://github.com/azerothcore/mod-breaking-news-override/workflows/core-build/badge.svg?branch=master&event=push)](https://github.com/azerothcore/mod-breaking-news-override)

![WoWScrnShot_031224_001040](https://github.com/azerothcore/mod-breaking-news-override/assets/2810187/9b7a8b74-4a55-4b00-9732-39695179bddd)

![WoWScrnShot_031224_001046](https://github.com/azerothcore/mod-breaking-news-override/assets/2810187/4aed246d-c954-417e-8b91-fa1dd442a700)

This module utilizes the Warden Payload Manager to enable the Breaking News frame on the left side of the character selection screen.

You can feed it a html file defined in the config to use it.

# Setup

- Create a html file which contains the body for your breaking news frame.
- Set the path to the html file in the config.
- Enable the module and login to verify it works.

# How to generate the image file

```html
<html>
<body>
    <p>|cffFF6F51 The AzerothCore Team and Owners DO NOT in any case sponsor nor support illegal public servers. If you use these projects to run an illegal public server and not for testing and learning it is your personal choice.|r</p>
    <br/>
    <p>We have an active community of developers, contributors, and users who collaborate, share knowledge, and provide support through forums, Discord channels, and other communication platforms. If you want to contribute, we\'d love to!</p>
    <br/>
    <p>Is designed to be highly modular, allowing developers to extend and customize the game to suit their preferences or create unique gameplay experiences. This flexibility enables the addition of custom features, content, and modifications.</p>
</body>
</html>
```

- `BreakingNews.Enable = 1`
- `BreakingNews.Title = "AzerothCore"`
- `BreakingNews.HtmlPath = "./breakingnews.html"`
- `BreakingNews.Cache = 1`
- `BreakingNews.Verbose = 0`

![image](https://github.com/azerothcore/mod-breaking-news-override/assets/2810187/b6a3c52d-79fd-4687-bbea-35a8ba43cbb5)

# Notes

- There is a small amount of html tags that are actually compatible, follow this guide here: https://wowwiki-archive.fandom.com/wiki/UIOBJECT_SimpleHTML
- You must escape the characters `[`, `]`, `'`, `\` in your title and html file because of the way the payload is sent.
- Example: `\'You think this hurts? Just wait.\'`

# Credits

Thanks to the AzerothCore community and Foe especially for getting this to work.
