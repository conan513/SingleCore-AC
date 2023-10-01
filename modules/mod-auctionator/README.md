# mod-auctionator

This mod is meant to keep a healthy auction house stocked on a low-pop server. It's in it's early phases of building/testing/configuration but keeps a LOT of stuff in the AH. Code is also a little rough right now but I haven't done c++ in 20+ years so I am getting there.

## Installation and Setup

1. Download the code into your source code `modules/` folder
2. Do the Compilation Things as best you can.
3. Run the database script in `mod-auctionator/data/sql/db-world/base/2023-09-18.sql` against your `acore_world` database.
4. If you want a fresh start clear you auction house with these SQL commands. Do this ONLY when your server is stopped. The AH is cached in memory and if you do this while your server is running then likely ... things won't go well. 

```
DELETE FROM acore_world.mod_acore_characters.item_instance ii WHERE ii.guid IN (
	SELECT ah.itemguid
	FROM acore_world.mod_acore_characters.auctionhouse ah
);

DELETE FROM acore_world.mod_acore_characters.auctionhouse;
```

## GM commands

### auctionator disable <faction>seller

Disable the seller for a particular faction.

```
.auctionator disable hordeseller
```

### auctionator enable <faction>seller

Enable the seller for a particular faction.

```
.auctionator enable hordeseller
```

### auctionator add <houseid> <itemid> <price>

Add an Iridescent Pearl to the Neutral AH for 1 gold buyout.

```
.auctionator add 7 5500 10000
```

### auctionator expireall <houseid>

Expire all auctions for the specified house on the next tick.

```
.auctionator expireall 7
```

## Current limitations

1. Caps at a hardcoded 10000 items in the AH (you can edit and recompile).
2. No control on stack size, it is hard coded to 20 (you can edit and recompile).
3. No control over item ranges (you can change the query).