SELECT
	it.entry
	, it.name
	, aic.name as classname
	, aic2.name as subclassname
	, aiq.name
	, it.RequiredLevel 
	, it.BuyPrice 
FROM
	item_template it
LEFT JOIN auctionator_item_class aic ON it.class = aic.class AND aic.subclass IS NULL
LEFT JOIN auctionator_item_class aic2 on it.class = aic2.class AND it.subclass = aic2.subclass
LEFT JOIN auctionator_item_quality aiq ON it.Quality = aiq.quality
WHERE
	1
--	AND aic2.name LIKE '%container%'
	AND it.name like '%turalyon%'
--	AND it.class = 15
--	AND it.subclass = 3
LIMIT 1000;



SELECT
	it.name
	, aic.name 
	, aic2.name
	, aiq.name
	, it.RequiredLevel
	, ah.buyoutprice
FROM acore_characters.auctionhouse ah
LEFT JOIN acore_characters.item_instance ii ON ah.itemguid = ii.guid 
LEFT JOIN acore_world.item_template it on ii.itemEntry = it.entry
LEFT JOIN acore_world.auctionator_item_class aic ON it.class = aic.class AND aic.subclass IS NULL
LEFT JOIN acore_world.auctionator_item_class aic2 on it.class = aic2.class AND it.subclass = aic2.subclass
LEFT JOIN acore_world.auctionator_item_quality aiq ON it.Quality = aiq.quality
LIMIT 10000;

-- Initial query for loading our tiesm from the database to start inserting. This was before we had the config in the DB.
SELECT
    it.entry
    , it.BuyPrice
    , it.bonding
    , it.name
    , aic.class
    , aic.name as classname
	, aic2.subclass
    , aic2.name as subclassname
    , aiq.name
    , it.RequiredLevel 
FROM
    item_template it
LEFT JOIN auctionator_item_class aic ON it.class = aic.class AND aic.subclass IS NULL
LEFT JOIN auctionator_item_class aic2 on it.class = aic2.class AND it.subclass = aic2.subclass
LEFT JOIN auctionator_item_quality aiq ON it.Quality = aiq.quality
WHERE
    1
    AND aic.class = 1
    AND aic2.subclass = 0
    AND it.bonding = 1
    AND it.Flags != 16
    AND it.BuyPrice > 0
    AND it.Quality >= 1
ORDER BY RAND();

SELECT li.item, it.name FROM (
    SELECT item FROM creature_loot_template UNION
    SELECT item FROM reference_loot_template UNION
    SELECT item FROM disenchant_loot_template UNION
    SELECT item FROM fishing_loot_template UNION
    SELECT item FROM gameobject_loot_template UNION
    SELECT item FROM item_loot_template UNION
    SELECT item FROM milling_loot_template UNION
    SELECT item FROM pickpocketing_loot_template UNION
    SELECT item FROM prospecting_loot_template UNION 
    SELECT item FROM skinning_loot_template
) li
INNER JOIN acore_world.item_template it ON li.item = it.entry 
;

-- Initial query using the itemclass_config table as the starting point for items.
SELECT
	it.entry
	, it.name
	, it.BuyPrice 
	, it. stackable
	, aicconf.stack_count
--	, ic.itemCount
--	, aicconf.max_count
--	, it.bonding 
--	, it.Quality 
FROM
	acore_world.mod_auctionator_itemclass_config aicconf
    LEFT JOIN acore_world.item_template it ON 
        aicconf.class = it.class 
        AND aicconf.subclass = it.subclass 
        AND it.bonding != 1 -- skip BoP
        AND (
                it.bonding >= aicconf.bonding
                OR it.bonding = 0
            )

        -- this union based query doesn't work because it only looks at LOOT items and ignores crafted
--     LEFT JOIN (
--     		-- This union is kinda bad because it relies on a temp table
--     		-- to do the inner join. Probably need to do something better
--     		-- here.
-- 	    	SELECT itt.entry, itt.name, itt.class, itt.subclass, itt.bonding, itt.BuyPrice, itt.quality, itt.stackable FROM (
-- 			    SELECT item FROM creature_loot_template UNION
-- 			    SELECT item FROM reference_loot_template UNION
-- 			    SELECT item FROM disenchant_loot_template UNION
-- 			    SELECT item FROM fishing_loot_template UNION
-- 			    SELECT item FROM gameobject_loot_template UNION
-- 			    SELECT item FROM item_loot_template UNION
-- 			    SELECT item FROM milling_loot_template UNION
-- 			    SELECT item FROM pickpocketing_loot_template UNION
-- 			    SELECT item FROM prospecting_loot_template UNION 
-- 			    SELECT item FROM skinning_loot_template
-- 			) li
-- 			INNER JOIN acore_world.item_template itt ON li.item = itt.entry 
-- 		) it
        ON aicconf.class = it.class
        AND aicconf.subclass = it.subclass
        AND it.bonding != 1
        AND (
            it.bonding >= aicconf.bonding
            OR it.bonding = 0
        )
    LEFT JOIN acore_world.mod_auctionator_disabled_items dis on it.Entry = dis.item
	LEFT JOIN (
		-- this sub query lets us get the current count of each item already in the AH
		-- so that we can filter out any items where itemCount >= max_count and not add
		-- anymore of them.
		SELECT
			count(ii.itemEntry) as itemCount
			, ii.itemEntry AS itemEntry
		FROM
			acore_characters.item_instance ii 
			INNER JOIN acore_characters.auctionhouse ah ON ii.guid = ah.itemguid 
			LEFT JOIN acore_world.item_template it ON ii.itemEntry = it.entry 
		GROUP BY ii.itemEntry, it.name
	) ic ON ic.itemEntry = it.entry
WHERE
    -- filter out items from the disabled table
    dis.item IS NULL
    -- filter out items with 'depreacted' anywhere in the name
    AND it.name NOT LIKE '%deprecated%'
    -- filter out items that start with 'Test'
    AND it.name NOT LIKE 'Test%'
    AND it.name NOT LIKE 'NPC %'
    -- filter out items where we are already at or above max_count for uniques in this class to limit dups
    AND (ic.itemCount IS NULL OR ic.itemCount < aicconf.max_count)
ORDER BY RAND()
;

-- attempt to filter out things in the AH already but this didn't let us filter based on max_count like the subquery did
	AND it.entry NOT IN (
		SELECT
			ii.itemEntry  
		FROM
			acore_characters.item_instance ii 
			INNER JOIN acore_characters.auctionhouse ah ON ii.guid = ah.itemguid
	)



-- query to get the current unique items in the AH and how many of them there are
SELECT
	count(ii.itemEntry) as itemCount
	, ii.itemEntry AS itemEntry
FROM
	acore_characters.item_instance ii 
	INNER JOIN acore_characters.auctionhouse ah ON ii.guid = ah.itemguid 
	LEFT JOIN acore_world.item_template it ON ii.itemEntry = it.entry 
GROUP BY ii.itemEntry, it.name ;



SELECT
	it.name
	, it.class
	, it.subclass 
	, it.bonding 
	, it.Quality 
FROM 
	acore_characters.auctionhouse ah
LEFT JOIN
	acore_characters.item_instance ii on ah.itemguid = ii.guid 
LEFT JOIN
	acore_world.item_template it ON ii.itemEntry = it.entry

;

# get a count of auctions by subclass
SELECT
	it.class
	, it.subclass 
	, count(it.name)
FROM 
	acore_characters.auctionhouse ah
LEFT JOIN
	acore_characters.item_instance ii on ah.itemguid = ii.guid 
LEFT JOIN
	acore_world.item_template it ON ii.itemEntry = it.entry
GROUP BY it.class, it.subclass ;
;

# get a list of all class/subclass combos to be able to loop over and create auctions
SELECT
	*
FROM 
	acore_world.auctionator_item_class aic 
WHERE
	subclass IS NOT NULL
;

# get a count of items in the AH by class/subclass
SELECT
	aic.class 
	, aic.subclass 
	, count(it.name)
FROM
	acore_characters.auctionhouse ah 
LEFT JOIN acore_characters.item_instance ii on ah.itemguid = ii.guid
LEFT JOIN acore_world.item_template it ON ii.itemEntry = it.entry 
RIGHT JOIN acore_world.auctionator_item_class aic ON it.class = aic.class AND it.subclass = aic.subclass 
WHERE
	aic.subclass IS NOT NULL
GROUP BY
	aic.class, aic.subclass 
;


DROP TABLE acore_world.auctionator_itemclass_config;
CREATE TABLE acore_world.auctionator_itemclass_config (  
    class int NOT NULL COMMENT 'item class',
    subclass int NOT NULL COMMENT 'item subclass',
    bonding int NOT NULL COMMENT 'bonding level that is the minimum for this class. 2 means greens (for gear), 1 means whites (for bags).',
    max_count int NOT NULL DEFAULT 1 COMMENT 'The maximum number of unique versions of these items to add to the house. Low numbers keep items like weapons from having lots of dups. High nubmers are useful for crafting mats.',
    PRIMARY KEY(class, subclass)
) COMMENT '';


SELECT DISTINCT class FROM item_template;

DELETE  FROM acore_world.auctionator_itemclass_config ;

-- empty the auction house (2 queries)
DELETE FROM acore_characters.item_instance ii WHERE ii.guid IN (
	SELECT ah.itemguid
	FROM acore_characters.auctionhouse ah
);

DELETE FROM acore_characters.auctionhouse;


-- clear mailing for cancelled auctions (2 queries)
DELETE FROM acore_characters.mail_items mi WHERE mi.receiver = 2;
DELETE FROM acore_characters.mail m WHERE receiver = 2;

-- take a look at what is in the mail system
SELECT
	m.sender
	, m.receiver 
	, m.messageType 
	, m.subject 
	, it.name 
	, ii.count 
	, m.deliver_time 
FROM
	acore_characters.mail m 
	INNER JOIN acore_characters.mail_items mi ON mi.mail_id = m.id 
	INNER JOIN acore_characters.item_instance ii ON mi.item_guid = ii.guid 
	INNER JOIN acore_world.item_template it ON ii.itemEntry = it.entry 
;