# mod-auctionator

This mod is meant to keep a healthy auction house stocked on a low-pop server. It's in it's early phases of building/testing/configuration but keeps a LOT of stuff in the AH. Code is also a little rough right now but I haven't done c++ in 20+ years so I am getting there.

## Installation and Setup

1. Download the code into your source code `modules/` folder
2. Do the Compilation Things as best you can.
3. Run the database script in `mod-auctionator/data/sql/db-world/base/2023-09-18.sql` against your `acore_world` database.
4. **NOTE** you may not need to do this, looks like server runs this automatically? Run the database script in `modules/mod-auctionator/data/sql/db-characters/updates/2023_11_12_00_marketprice.sql` against your `acore_characters` database.
5. If you want a fresh start clear you auction house with these SQL commands. Do this ONLY when your server is stopped. The AH is cached in memory and if you do this while your server is running then likely ... things won't go well. Run this against your character database (default of `acore_characters`).

```
DELETE FROM `item_instance` WHERE `guid` IN (SELECT `itemguid` FROM `auctionhouse`);
DELETE FROM `auctionhouse`;
```

## GM commands

### auctionator add <houseid> <itemid> <price>

ItemID can be looked up in the database table `item_template`.

Add an Iridescent Pearl to the Neutral AH for 1 gold buyout.

```
.auctionator add 7 5500 10000
```

### auctionator auctionspercycle <value>

Set the number of auctions per cycle that get added to each auction house.
This setting is shared by all houses. You should keep this number lower
than query limit or else ... I don't know, try it and see.

Also this has a direct impact on how hard each cycle hits your worldserver
and your mysql server so if you are running on smaller hardware you
might want to tune this and the query limit down to lower numbers.

```
.auctionator auctionspercycle 45
```

### auctionator bidonown <value>

Enable bidding on the auctions created by the auctionator. There is
really no reason to do this other than to test the buyer and make
sure it's working. You could use this to potentially have more turnover
in your auction houses but I am not sure how well that would work.

Valid values are `1` to enable and `0` to disable.

```
.auctionator bidonown 1
```

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

### auctionator expireall <houseid>

Expire all auctions for the specified house on the next tick.

```
.auctionator expireall 7
```

### auctionator multiplier <typetext> <qualitytext> <multiplier>

Typetext is either `seller` or `bidder` depending on which you want to set.

Set the sell and buy multiplier for a quality. Qualities are:

* `poor`
* `normal`
* `uncommon`
* `rare`
* `epic`
* `legendary`

Multiplier is a decimal value. Defaults are set in the config file.

```
.auctionator multiplier bidder epic 10
```

### auctionator status

Shows the status of the in memory configs as well as the current auction
houses. Useful for checking config values that may have changed or checking
on auction counts. Example output below.

```
.auctionator status
```

Output:
```
[Auctionator] Status:

 Enabled: 1

 CharacterGuid: 2
 Horde:
    Seller Enabled: 1
        Max Auctions: 20000
        Auctions: 9319
    Bidder Enabled: 1
        Cycle Time: 2
        Per Cycle: 20
 Alliance:
    Seller Enabled: 1
        Max Auctions: 20000
        Auctions: 8959
    Bidder Enabled: 1
        Cycle Time: 1
        Per Cycle: 20
 Neutral:
    Seller Enabled: 1
        Max Auctions: 20000
        Auctions: 9379
    Bidder Enabled: 1
        Cycle Time: 3
        Per Cycle: 30
 Seller Multipliers:
    Poor: 1.000000
    Normal: 1.000000
    Uncommon: 1.500000
    Rare: 2.000000
    Epic: 6.000000
    Legendary: 10.000000
 Bidder Multipliers:
    Poor: 1.000000
    Normal: 1.000000
    Uncommon: 1.500000
    Rare: 2.000000
    Epic: 6.000000
    Legendary: 10.000000
 Seller settings:
    Auctions per run: 100
    Query Limit: 1000
    Default Price: 10000000
```

## Importing Marketplace Data (v0.6)

There is a helper script in `mod-auctionator/apps/marketprices` for creating the sql to import the market data from a csv. You need to start the server first to make sure that the `mod_auctionator_market_price` table has been created. Using a newer version of Node (18+ should do it) you can generate insert queries that you can pipe to mysql something like this:

```
cd apps/marketprice
cp ~/mypricedata.csv .

# only need to do this the first time
npm install
node index.js mypricedata.csv | mysql -u <dbuser> -p <character_database_name>
```

The script skips the first line and expects the following columns:

```
scan_datetime,item_entry,avg_price,minimum_buyout,minimum_bid,item_count
```

minimum_buyout, minimum_bid, and item_count are currently not used for anything and can be set to 0.

## Current limitations

1. ~~Stacks are acting weird, especially things like enchanter rods.~~ This is now fixed BUT many stacks with the multipler are maxed out now to 20 and way to expensive. Whoopsie.
2. No control on stack size, it is hard coded to 20 (you can edit and recompile).
3. No control over item ranges (you can change the query).
